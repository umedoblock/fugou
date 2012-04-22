import sys
import os
import argparse

def make_streams_binary():
    sys.stdin = sys.stdin.detach()
    sys.stdout = sys.stdout.detach()

parser = argparse.ArgumentParser(description='generate random data.')
parser.add_argument('--octets', metavar='N', dest='octets',
                   type=int, nargs='?', default=2048,
                   help='octetss length (default: 2048)')
args = parser.parse_args()
octets = args.octets

make_streams_binary()

random_data = os.urandom(octets)
sys.stdout.write(random_data)
