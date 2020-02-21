import csv
import os
import re
import statistics
import sys
import tempfile
import argparse
import subprocess

# Default Global variables

def Plotting(paths):
    for ls in paths:
        num_iter = ls.pop(0) # No use
        num_threads = ls.pop(0)
        interpath = ls.pop(0)
        path2rscript_file = os.path.dirname(os.path.realpath(sys.argv[0])) # Current directory
        rscript_file = "statistics.r"
        rscript = "/usr/bin/Rscript" # Way to find available Rscript automatically
        rpath = str(path2rscript_file + '/' + rscript_file)
        subprocess.run([rscript, rpath, interpath, str(num_threads)])
            
def Refine():
    num_iter = 10000000
    num_threads = 4
    combined = False
    parser = argparse.ArgumentParser(description='Plot R-diagram from the measurement')
    parser.add_argument("result", type=str, nargs='*', help="name of measuremnt file name including its path")
    parser.add_argument("-i", "--iterations", type=int, nargs='*',
            help="number of workload iterations (default: "+str(num_iter)+")")
    parser.add_argument("-t", "--threads", type=int, nargs='*',
            help="number of available threads (default: "+str(num_threads)+")")
    parser.add_argument("-c", "--combined", action="store_true",
            help="Combined benchmarks in arguments into a single R-plot (default: "+str(combined)+")")
    f_list = parser.parse_args()
    num_iter = f_list.iterations
    num_threads = f_list.threads
    benchmarks = f_list.result
    combined = f_list.combined

    if len(benchmarks) != len(num_iter) or len(benchmarks) != len(num_threads):
        if not combined:
            print('ERROR: Benchmarks({}) require num_iter(-i) and num_threads(-t) for each of them'.format(benchmarks))
            sys.exit(1)

    for f in benchmarks:
        if not os.access(f, os.F_OK):
            print('ERROR: File does not exist:\n\n\t{}\n'.format(f))
            sys.exit(1)

    t_list = []
    # Write temp files to prepare csv read
    for f in benchmarks:
        with open (f, 'r') as txt:
            bench_name = os.path.splitext(os.path.basename(f))[0]
            t_name = '/tmp/{}_{}.txt'.format(bench_name,
                     next(tempfile._get_candidate_names()))
            tmp = open (t_name, 'w')
            run = '-1'
            for line in txt:
                # find and update current RUN number
                m = re.search('^RUN (.+?):$', line) 
                if m:
                    run = m.group(1)
                # remove redundant whitespace delimiters
                new_line = re.sub("\s\s+", " ", line.strip())

                # Stop writing when Overview data at the end
                m = re.search('^Overview\sacross\sruns:$', new_line)
                if m:
                    break;
                # search for data and insert run number at the beginning
                m = re.search('^[0-9]+', new_line)
                if m and new_line.count(' ') > 2:
                    new_line = '{} {} {}'.format(bench_name, run, new_line)
                else:
                    continue # skip writing
                tmp.write(new_line + '\n')
            tmp.close()
            t_list.append(t_name)

    dl = []
    for f_name in t_list:
        d = dict()
        with open (f_name, 'r') as f:
            reader = csv.reader(f, delimiter= ' ')
            for row in reader:
                key = (row[0],int(row[1]))
                if key in d:
                    d[key].append(int(row[3]))
                else:
                    d[key] = [int(row[3])]
        dl.append(d)

    # Write 2nd temp files storing refined data
    rlist = []
    if combined:
        # When benchmarks are combined into a single R-plot, 
        # their iterations and threads are assumed to be same.
        rlist.append([num_iter[0], num_threads[0]])
        d3 = dict()
        for idx in range(len(dl)):
            for key,value in dl[idx].items():
                mean = statistics.mean(value)
                sd = statistics.pstdev(value) # stdev is sample-based, so use pstdev
                cv = sd / mean
                nano_in_sec = 1000000000 # 10^9
                value2 = [num_iter[0]/(x/nano_in_sec) for x in value] # throughput
                throughput_mean = statistics.mean(value2)
                throughput_sd = statistics.pstdev(value2)
                throughput_cv = throughput_sd / throughput_mean
                new_data= [ key[0],
                            (key[1] + 1),
                            statistics.median(value),
                            max(value),
                            mean,
                            sd,
                            cv,
                            statistics.median(value2),
                            max(value2),
                            throughput_mean,
                            throughput_sd,
                            throughput_cv
                          ]
                if key[0] in d3:
                    d3[key[0]].append(new_data)
                else:
                    d3[key[0]] = []
                    d3[key[0]].append(new_data)
        t_name = '/tmp/stats_combined.csv'
        rlist[0].append(t_name)
        f = open (t_name, 'w') 
        writer = csv.writer(f)
        header = ['Version', 'Threads',
                  'ExecutionTime',    'MAX',    'Mean',    'SD',    'CV',
                  'Throughput',    'TH_MAX', 'TH_Mean', 'TH_SD', 'TH_CV']
        writer.writerow(header)
        for runs in d3:
            for items in d3[runs]:
                writer.writerow(items)
        f.close()

    else:
        for idx in range(len(dl)):
            d3 = dict()
            for key,value in dl[idx].items():
                mean = statistics.mean(value)
                sd = statistics.pstdev(value) # stdev is sample-based, so use pstdev
                cv = sd / mean
                nano_in_sec = 1000000000 # 10^9
                value2 = [num_iter[idx]/(x/nano_in_sec) for x in value] # throughput
                throughput_mean = statistics.mean(value2)
                throughput_sd = statistics.pstdev(value2)
                throughput_cv = throughput_sd / throughput_mean
                new_data= [ key[0],
                            (key[1] + 1),
                            statistics.median(value),
                            max(value),
                            mean,
                            sd,
                            cv,
                            statistics.median(value2),
                            max(value2),
                            throughput_mean,
                            throughput_sd,
                            throughput_cv
                          ]
                if key[0] in d3:
                    d3[key[0]].append(new_data)
                else:
                    d3[key[0]] = []
                    d3[key[0]].append(new_data)

            rlist.append( [num_iter[idx], num_threads[idx]] )
            for bname in d3: 
                t_name = '/tmp/stats_{}.csv'.format(bname)
                rlist[idx].append(t_name)
                f = open (t_name, 'w') 
                writer = csv.writer(f)
                header = ['Version', 'Threads',
                          'ExecutionTime',    'MAX',    'Mean',    'SD',    'CV',
                          'Throughput',    'TH_MAX', 'TH_Mean', 'TH_SD', 'TH_CV']
                writer.writerow(header)
                for runs in d3[bname]:
                    writer.writerow(runs)
                f.close()
    # delete intermediate data txt files ( stats_{}.csv file)
    return rlist

if __name__ == '__main__':
    Plotting(Refine())
