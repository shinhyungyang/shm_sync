import argparse
import datetime
import shutil
import os
from settings import BENCHMARKS
from settings import PHYSICAL_CORES
from util import shell
from util import shell_with_returncode
from util import convertA2I
from util import fail
from _variables import BINDIR 
from _variables import DATADIR
from _variables import BUILDDIR
from _variables import PROJECTDIR
from _variables import LIKWID_PIN


#############################
# Read command-line arguments
#############################
b_name = ''
executions = 1
iterations = 10000000

lkwd_root = ''
num_thread_range = range(PHYSICAL_CORES, PHYSICAL_CORES+1)

parser = argparse.ArgumentParser(description='Run shm_sync benchmark')
parser.add_argument("-e", "--executions", metavar='N', type=int,
                    help="number of benchmark excutions (default: "+str(executions)+")",
                    default=executions)
# mutually exclusive argument group
group = parser.add_mutually_exclusive_group(required=False)
group.add_argument("-t", "--threads", type=int,
                    help="number of worker threads (E.g.,: "+str(PHYSICAL_CORES)+")")
group.add_argument("-m", "--max_threads", type=int,
                    help="number of worker threads increased by 1 at each time to run benchmark with repeatedly until it reaches the given value (default: "+str(0)+")")
parser.add_argument("-i", "--iterations", metavar='N', type=str,
                    help="number of workload iterations (default: "+str(iterations)+")")
parser.add_argument("-r", "--runs", action="store_true",
                    help="Scaling runs, 1..t threads (default: "+str(False)+")")
parser.add_argument("-o", "--overview", action="store_true",
                    help="Overview statistics only (default: "+str(False)+")")
parser.add_argument("-u", "--useful_work_in", metavar='N', type=int,
                    help="Amount of nsec to perform useful work(busy loop) inside critical section of lock (default: "+str(0)+")")
parser.add_argument("-y", "--useful_work_out", metavar='N', type=int, 
                    help="Amount of nsec to perform useful work(busy loop) outside critical section of lock (default: "+str(0)+")")
parser.add_argument("-l", "--l_root", metavar='STR', type=str,
                    help="root path of Likwid (default: NONE)",
                    default=lkwd_root)
parser.add_argument("benchmark",
                    help="name of benchmark to run")
args = parser.parse_args()

b_name = args.benchmark
if args.executions is not None:
    executions = args.executions
if args.iterations is not None:
    iterations = convertA2I(args.iterations)

if args.max_threads is None and args.threads is None:
    num_thread_range = range(PHYSICAL_CORES, PHYSICAL_CORES + 1)
elif args.max_threads is not None:
    num_thread_range = range(1, args.max_threads + 1)
else:
    num_thread_range = range(args.threads, args.threads + 1)

if args.useful_work_out is not None:
    uouts = args.useful_work_out
if args.useful_work_in is not None:
    uins = args.useful_work_in

##########################
# Setup likwid-pin command
##########################
lkwd_pin = ''
lkwd_cmd = ''
if LIKWID_PIN is not "LIKWID_PIN-NOTFOUND":
    lkwd_pin = LIKWID_PIN
else:
    lkwd_root = os.path.normpath(args.l_root)
    lkwd_pin = lkwd_root + '/bin/likwid-pin'
    lkwd_cmd = lkwd_pin + ' -q -c {} -s 0x1 '.format('0,0-' + str(PHYSICAL_CORES-1))

lkwd_valid = False

if (os.path.isfile(lkwd_pin) and os.access(lkwd_pin, os.X_OK)):
    lkwd_valid = True

if lkwd_valid is False:
    print('WARNING: BENCHMARK IS GOING TO RUN WITHOUT PROCESSOR AFFINITY!')
    print('Please refer to README.md on how to install Likwid in your system.')
    print('For detailed usage, type ``python3 ./run.py --help\'\'\n')
    lkwd_cmd = ''


#####################
# Prepare data folder
#####################
data_dir = '{}/{}/{}'.format(DATADIR, datetime.datetime.now().strftime('%B%d_%Y_%H:%M:%S'), b_name)
#data_dir = DATADIR + '/' + datetime.datetime.now().strftime('%B%d_%Y_%H:%M:%S')
os.makedirs(data_dir)


######################
# Run target benchmark
######################
if (args.useful_work_out is not None or args.useful_work_in is not None) and BENCHMARKS[b_name]['useful'] == False:
    fail('The BENCHMARK cannot perform useful_works because it isn\'t lock benchmark.')
if BENCHMARKS[b_name]['runType'] is 'cxxopts':
    for binname in BENCHMARKS[b_name]['bins']:
        bin_dir = data_dir + '/' + binname
        os.mkdir(bin_dir)
        for t_count in num_thread_range:
            binExec = '{}/{}/{}/{}'.format(
                    BINDIR, BENCHMARKS[b_name]['type'],
                    b_name, binname)
            flags = list(" -t " + str(t_count) + \
                         " -i " + str(iterations) + \
                         " -e " + str(executions)) 
            if args.runs:
                flags.append(" -r ")
            if args.overview:
                flags.append(" -o ")
            if args.useful_work_in:
                flags.append(" -u " + str(uins))
            if args.useful_work_out:
                flags.append(" -y " + str(uouts))
                      
            ret=shell(lkwd_cmd + binExec + "".join(flags))
            log_file = '{}/{}.out'.format(bin_dir,binname)
                
            if BENCHMARKS[b_name]['useful']:
                with open(log_file, 'w') as f:
                    print(ret, file=f)
            else:
                current_path = os.path.dirname(os.path.realpath(__file__))
                currentFiles = os.listdir(current_path)
                for f in currentFiles:
                    if f.endswith("producers.txt") or f.endswith("consumers.txt") or f.endswith("pop.txt") or f.endswith("push.txt"):
                        shutil.move(f, bin_dir)


elif BENCHMARKS[b_name]['runType'] is 'CMD_Args':
    for e in range(executions):
        for binname in BENCHMARKS[b_name]['bins']:
            bin_dir = data_dir + '/' + binname
            if not os.path.isdir(bin_dir):
                os.mkdir(bin_dir)
            for t_count in num_thread_range:
                binPath = '{}/{}/{}/{}'.format(
                        BINDIR, BENCHMARKS[b_name]['type'],
                        b_name, binname)
                ret=shell(lkwd_cmd + binPath +
                          " -n " + str(t_count) +
                          " -m " + str(t_count) +
                          " -o " + str(iterations))
                log_file = '{}/{}_{}.out'.format(bin_dir, t_count, executions)
                with open(log_file, 'w') as f:
                    print(ret, file=f)

elif BENCHMARKS[b_name]['runType'] is 'sed_make':

    srcPath='{}/{}/{}/{}/{}'.format(
            PROJECTDIR, 'benchmark', BENCHMARKS[b_name]['type'],
            b_name, BENCHMARKS[b_name]['src'])

    for t_count in num_thread_range:
        ###################################################################
        # Ada source code files have to be built with different t_count and
        # iteration values
        ###################################################################

        ##################
        # Set thread count
        cmd = "sed -i 's/{0}/{1}/g' {2}".format(
                "\\bNo_Users: constant Positive := [0-9_]\\+\\b",
                "No_Users: constant Positive := " + str(t_count),
                srcPath)
        ret_sed = shell_with_returncode(cmd)
        if ret_sed['returncode'] is not 0:
            fail('Setting thread count in source failed:\n{}'.format(srcPath))

        #####################
        # Set iteration count
        cmd = "sed -i 's/{0}/{1}/g' {2}".format(
                "\\bIterations: constant Positive := [0-9_]\\+\\b",
                "Iterations: constant Positive := " + str(iterations),
                srcPath
                )
        ret_sed = shell_with_returncode(cmd)
        if ret_sed['returncode'] is not 0:
            fail('Setting iteration count in source failed:\n{}'.format(srcPath))

        for binname in BENCHMARKS[b_name]['bins']:
            bin_dir = data_dir + '/' + binname
            if not os.path.isdir(bin_dir):
                os.mkdir(bin_dir)
            #################################
            # re-build target Ada source code
            cmd = '{}; {};'.format(
                'cd ' + BUILDDIR,
                'make ' + binname + '/fast'
                )
            ret_make = shell_with_returncode(cmd)
            if ret_make['returncode'] is not 0:
                fail('Rebuilding source code failed:\n\n\t{}\n'.format(cmd))

            ret = ''
            for e in range(executions):
                binPath = '{}/{}/{}/{}'.format(
                        BINDIR, BENCHMARKS[b_name]['type'],
                        b_name, binname)
                ret += shell(lkwd_cmd + binPath)
            log_file = '{}/{}_{}.out'.format(bin_dir, t_count, executions)
            with open(log_file, 'w') as f:
                print(ret, file=f)


print('All benchmark data files are saved under:\n\n\t{}\n'.format(data_dir))
