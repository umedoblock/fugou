import math, argparse

from ecc import *
from ecc import lib

def parse_args():
    parser = argparse.ArgumentParser(description=("look for primes on ec of x axis."))
    parser.add_argument("--primes", dest="primes",
                       action='store_true', default=False,
                       help="primes option default: False")
    parser.add_argument("--n_points", metavar="N", dest="n_points",
                       type=int, default=53,
                       help="collect n_points on ec default: 53")
  # parser.add_argument("--top", metavar="N", dest="top",
  #                    type=int, nargs="?", default=10,
  #                    help="ranking default: 10")
    args = parser.parse_args()
    return args

def uniq_by_x(points):
    uniq = {}
    for point in points:
        if point.x in uniq.keys() and \
           point > uniq[point.x]:
            # point.x == uniq[point.x].x and
            # point.y > uniq[point.x].y
            continue
        uniq[point.x] = point

    return uniq.values()

def get_mazekoze_candidates(points):
    mazekoze_candidates = []
    for point in points:
        if point.isinf():
            continue
        mazekoze_candidates.append(point)
    return mazekoze_candidates

if __name__ == "__main__":
    args = parse_args()

    ecc = ECC(19, 77, 307, 331)
    points_on_ecc = ecc.collect_all_points()

    view_all_points = False
    if view_all_points:
        L = list(points_on_ecc)
        L.sort()
        print("\n".join([str(x) for x in L]))
        raise()

    x_is_uniq = uniq_by_x(points_on_ecc)
    mazekoze_candidates = get_mazekoze_candidates(x_is_uniq)
    mazekoze_candidates = list(mazekoze_candidates)
    mazekoze_candidates.sort()

    candidates = []
    x_is_prime = []
    if args.primes:
        for point in mazekoze_candidates:
            if lib.is_prime(point.x):
                x_is_prime.append(point)
            if len(x_is_prime) >= args.n_points:
                break
        if len(x_is_prime) < args.n_points:
            raise ValueError(f"doesn't collect {args.n_points} primes, {ecc} has {len(x_is_prime)} primes of uniq x.")
        candidates = x_is_prime
    else:
        if len(mazekoze_candidates) < args.n_points:
            raise ValueError(f"doesn't collect available {args.n_points} points, {ecc} has {len(mazekoze_candidates)} points of uniq x.")
        candidates = mazekoze_candidates[:args.n_points]

    print("points =")
    if candidates:
        candidates.sort()
        str_points = [str(point) for point in candidates]
        print("\n".join(str_points))
    else:
        print("nothing")
