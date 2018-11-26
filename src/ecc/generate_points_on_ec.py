import math, argparse

from ecc import *
from ecc import lib

def parse_args():
    parser = argparse.ArgumentParser(description=("look for primes on ec of x axis."))

    parser.add_argument("--max_prime", metavar="N", dest="max_prime",
                       type=int, default=100,
                       help="max_prime default: 100")
#   parser.add_argument("--n_points", metavar="N", dest="n_points",
#                      type=int, default=53,
#                      help="collect n_points on ec default: 53")
  # parser.add_argument("--top", metavar="N", dest="top",
  #                    type=int, nargs="?", default=10,
  #                    help="ranking default: 10")
    args = parser.parse_args()
    return args

if __name__ == "__main__":
    args = parse_args()

    ecc = ECC(19, 77, 307, 331)
    points = ecc.collect_all_points()

    max_prime = args.max_prime
    primes = lib.collect_primes(max_prime)

    points_on_x_axis = []
    for point in points:
        if point.x in primes:
            points_on_x_axis.append(point)

    print("points =")
    if points_on_x_axis:
        points_on_x_axis.sort()
        str_points = [str(point) for point in points_on_x_axis]
        print("\n".join(str_points))
    else:
        print("nothing")
