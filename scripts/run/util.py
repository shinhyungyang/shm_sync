import sys
import subprocess
import os
import fnmatch
import re

def fail(msg):
    print('ERROR: ' + msg)
    sys.exit(1)

def shell(cmd):
    ret = subprocess.run(cmd, shell=True, stderr=subprocess.STDOUT, stdout=subprocess.PIPE, executable='/bin/bash')
    return ret.stdout.decode('utf-8').strip()

def shell_with_returncode(cmd):
    ret = subprocess.run(cmd, shell=True, stderr=subprocess.STDOUT, stdout=subprocess.PIPE, executable='/bin/bash')
    myRet = {
            'output': ret.stdout.decode('utf-8').strip(),
            'returncode': ret.returncode,
    }
    return myRet

def fshell(pool, cmd):
    return pool.submit(subprocess.run, cmd, shell=True, stderr=subprocess.STDOUT, stdout=subprocess.PIPE, executable='/bin/bash')

def fshell_nopipe(pool, cmd):
    return pool.submit(subprocess.run, cmd, shell=True, executable='/bin/bash')

def log(msg):
    return None

def find(pattern, path):
    result = []
    for root, dirs, files in os.walk(path):
        for name in files:
            if fnmatch.fnmatch(name, pattern):
                result.append(os.path.join(root, name))
    return result

def wc_l(fname):
    i = 0
    with open(fname) as f:
        for i, l in enumerate(f, 1):
            pass
    return i

def convertA2I(text):
    SCALES = {
        'K': 1000,
        'M': 1000000,
        'B': 1000000000,
        'T': 1000000000000,
        'KB': 1000,
        'MB': 1000000,
        'GB': 1000000000,
        'TB': 1000000000000,
        'KiB': 1024,
        'MiB': 1048576,
        'GiB': 1073741824,
        'TiB': 1099511627776,
    }
    num_parsed = re.findall(r"\d+", text)
    scale_parsed = re.findall(r"[a-zA-Z]+", text)

    ret = 0
    factor = 0

    if len(num_parsed) is 0:
        fail('convertA2I \"{}\": invalid number!'.format(text))
    else:
        ret = int(num_parsed[0])

    if len(scale_parsed) is 0:
        factor = 1
    elif scale_parsed[0] not in SCALES:
        fail("convertA2I: \"{}\": no matching scale found!".format(scale_parsed[0]))
    else:
        factor = int(SCALES[scale_parsed[0]])

    return ret * factor
