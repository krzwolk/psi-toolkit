# -*- coding: utf-8 -*-
"""
Description: Converts each word in text sentence by sentence
Usage example:
    $ python words_to_lem -i [input file] -o [output file] -t 5
"""

from __future__ import division
from __future__ import absolute_import
from __future__ import print_function

import subprocess as sp
import os
import logging
from argparse import ArgumentParser, RawTextHelpFormatter
from multiprocessing import Pool

psi_cmd = [
    'framework/psi-pipe',
    'lamerlemma',
    '--lang',
    'pl',
    '!',
    'simple-writer',
    '--tags',
    'lemma'
]
p_mark = [',', '.', '!', '?']

def get_p_mark_map(text):
    """@todo: Docstring for get_p_mark_map.

    :text: @todo
    :returns: @todo

    """
    tokens = text.split()
    p_mark_map = {}
    for i, token in list(enumerate(tokens))[:-1]:
        if not token:
            continue
        if token[-1] in p_mark:
            p_mark_map[i] = token[-1]
    if text and text[-1] in p_mark:
        p_mark_map[-1] = text[-1]
    return p_mark_map

def lemarize(text):
    """@todo: Docstring for lemarize.

    :text: @todo
    :returns: @todo

    """
    text = text.strip()
    p_mark_map = get_p_mark_map(text)
    lemarizer = sp.Popen(psi_cmd, stdin=sp.PIPE, stdout=sp.PIPE)
    lemarizer.stdin.write(text)
    result, _ = lemarizer.communicate()
    result = result.strip()
    if result:
        tokens = []
        for line in result.split('\n'):
            first_lem = line.split('|', 1)[0].strip()
            if first_lem:
                tokens.append(first_lem)
        if tokens:
            p_tokens = []
            for i, token in enumerate(tokens[:-1]):
                if i in p_mark_map:
                    p_tokens.append(token + p_mark_map[i])
                else:
                    p_tokens.append(token)
            if -1 in p_mark_map:
                p_tokens.append(tokens[-1] + p_mark_map[-1])
            else:
                p_tokens.append(tokens[-1])
            return ' '.join(p_tokens).capitalize()
    return text.capitalize()

if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')
    log = logging.getLogger()
    parser = ArgumentParser(description=__doc__, formatter_class=RawTextHelpFormatter)
    parser.add_argument('-i', '--in', dest='in_path', required=True, help='Input text file')
    parser.add_argument('-o', '--out', dest='out_path', help='Output file with processed data')
    parser.add_argument('-t', '--threads', type=int, default=1, help='Number of threads to process data')
    args = parser.parse_args()

    if not args.out_path:
        args.out_path = os.path.join(
            os.path.dirname(args.in_path), 'lem_' + os.path.basename(args.in_path))

    log.info('Input file: %s', args.in_path)
    log.info('Output file: %s', args.out_path)
    pool = Pool(args.threads)
    with open(args.in_path) as in_file, open(args.out_path, 'w') as out_file:
        for lem in pool.imap(lemarize, in_file):
            out_file.write(lem + '\n')
