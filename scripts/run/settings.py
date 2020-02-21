"""
Benchmark configurations
"""

import sys
from util import shell
from util import fail

if sys.version_info[0] < 3:
    fail('Python 3 is required to run this code')

CORES=shell('lscpu | gawk \'/^CPU\(s\):/{print $2}\'')
THREADS_PER_CORE=shell('lscpu | gawk \'/^Thread\(s\) per core:/{print $4}\'')
PHYSICAL_CORES=int(CORES)//int(THREADS_PER_CORE)

BENCHMARKS = {
    'ArrayLock': {
        'type': 'blocking',
        'bins': ['arraylock_sc', 'arraylock_ar',],
        'useful' : True,
        'runType': 'cxxopts',
    },
    'CLHQueueLock': {
        'type': 'blocking',
        'bins': ['clhqueue_lock_sc','clhqueue_lock_ar',],
        'useful' : True,
        'runType': 'cxxopts',
    },
    'Ada_TAS_MPMC': {
        'type': 'blocking',
        'bins': ['driver_ada_tas_mpmc',],
        'useful' : False,
        'runType': 'cxxopts',
        'src': 'adatas.adb',
    },
    'PO_FIFO_Lock': {
        'type': 'blocking',
        'bins': ['driver_po_fifo_lock',],
        'useful' : True,
        'runType': 'cxxopts',
        'src': 'po_fifo_lock.adb',
    },
    'C++11MutexLock': {
        'type': 'blocking',
        'bins': ['cpp11_mutex_lock',],
        'useful' : True,
        'runType': 'cxxopts',
    },
    'C++11MutexStack': {
        'type': 'blocking',
        'bins': ['driver_cpp11_mutex_stack',],
        'useful' : False,
        'runType': 'cxxopts',
    },
    'POSIXMutexLock': {
        'type': 'blocking',
        'bins': ['posix_mutex_lock',],
        'useful' : True,
        'runType': 'cxxopts',
    },
    'PetersonLock': {
        'type': 'blocking',
        'bins': ['peterson_lock_sc', 'peterson_lock_ar',],
        'useful' : True,
        'runType': 'cxxopts',
    },
    'FilterLock': {
        'type': 'blocking',
        'bins': ['filter_lock_sc', 'filter_lock_ar',],
        'useful' : True,
        'runType': 'cxxopts',
    },
    'POLock': {
        'type': 'blocking',
        'bins': ['driver_polock',],
        'useful' : True,
        'runType': 'cxxopts',
        'src': 'po_lock.adb',
    },
    'Taubenfeld': {
        'type': 'blocking',
        'bins': ['fair_lock_sc','fair_lock_ar',],
        'useful' : True,
        'runType': 'cxxopts',
    },
    'TASLock': {
        'type': 'blocking',
        'bins': ['taslock_sc', 'taslock_ar',],
        'useful' : True,
        'runType': 'cxxopts',
    },
    'TATASLock': {
        'type': 'blocking',
        'bins': ['tatas_lock_sc', 'tatas_lock_ar',],
        'useful' : True,
        'runType': 'cxxopts',
    },
    'Ada': {
        'type': 'blocking/monitors',
        'bins': ['driver_ada_po_mpmc',],
        'useful' : False,
        'runType': 'cxxopts',
        'src': 'adapo.adb',
    },
#############################################################################
    'AdaLockfree': {
        'type': 'nonblocking',
        'bins': ['adalockfree',],
        'useful' : False,
        'runType': 'cxxopts',
        'src': 'driver_adalockfree.adb',
    },
    'BoostMPMC': {
        'type': 'nonblocking',
        'bins': ['boostmpmc',],
        'useful' : False,
        'runType': 'cxxopts',
    },
    'BoostSPSC': {
        'type': 'nonblocking',
        'bins': ['boostspsc',],
        'useful' : False,
        'runType': 'cxxopts',
    },
    'BQueue': {
        'type': 'nonblocking',
        'bins': ['bqueue',],
        'useful' : False,
        'runType': 'cxxopts',
    },
    'COstack': {
        'type': 'nonblocking',
        'bins': ['costack_sc', 'costack_ar',],
        'useful' : False,
        'runType': 'cxxopts',
    },
#############################################################################
    'DiningPhilosophers': {
        'type': 'tsx',
        'bins': ['driver_dp',],
        'useful' : False,
        'runType': 'CMD_Args',
    },
    'HashTable': {
        'type': 'tsx',
        'bins': ['driver_hashtable',],
        'useful' : False,
        'runType': 'CMD_Args',
    },
    'kmeans': {
        'type': 'tsx',
        'bins': ['driver_kmeans',],
        'useful' : False,
        'runType': 'CMD_Args',
    },
}
