#!/usr/bin/env python

from math import sqrt

def compute_stats(data):
    '''given a list of data, compute statistics and return as dict'''
    data_sum = sum(data)
    data_sum2 = sum([x**2 for x in data])
    n = len(data)
    stat_data = {
        'mean': data_sum/n,
        'min': min(data),
        'max': max(data),
        'stddev': sqrt((data_sum2 - data_sum**2/n)/(n - 1)),
    }
    return stat_data

if __name__ == '__main__':
    from argparse import ArgumentParser
    arg_parser = ArgumentParser(description='parse DiskFitness output file '
                                            'and write statistics')
    arg_parser.add_argument('file', help='DiskFitness output file')
    options = arg_parser.parse_args()
    with open(options.file, 'r') as diskfitness_file:
        state = 'init'
        modes = ['write', 'read', 'random read']
        data = {}
        for mode in modes:
            data[mode] = []
        for line in diskfitness_file:
            if line.startswith('# Running with'):
                state = 'collecting'
            elif line.startswith('# diskfitness finished'):
                state = 'done'
            elif line.startswith('#') or line.isspace():
                pass
            elif state != 'init' and state != 'done':
                items = line.split('\t')
                if len(items) == 3 and items[1] in modes:
                    data[items[1]].append(float(items[2]))
    stats = {}
    for mode in data:
        stats[mode] = compute_stats(data[mode])
    fmt_str = ('{mode:15s}\t{mean:-8.2f}\t{min:-8.2f}\t{max:-8.2f}'
               '\t{stddev:-8.2f}')
    for mode in sorted(stats):
        stat_data = stats[mode]
        print fmt_str.format(mode=mode, mean=stat_data['mean'],
                             min=stat_data['min'], max=stat_data['max'],
                             stddev=stat_data['stddev'])
