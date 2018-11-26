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

if __name__ == "__main__":
    args = parse_args()

    ecc = ECC(19, 77, 307, 331)
    points = ecc.collect_all_points()

    view_all_points = False
    if view_all_points:
        L = list(points)
        L.sort()
        print("\n".join([str(x) for x in L]))
        raise()

    uniq = uniq_by_x(points)
    points = list(uniq)
    points.sort()

    points_on_ec = []
    if args.primes:
        for point in points:
            if point.isinf():
                raise ValueError(f"doesn't collect {args.n_points} primes, we have {len(points_on_ec)} primes.")
            if lib.is_prime(point.x):
                points_on_ec.append(point)
            if len(points_on_ec) >= args.n_points:
                break
    else:
        points_on_ec = points[:args.n_points]
        if points_on_ec[-1].isinf():
            len_available_points = len(points) - 1
            # "len(points) - 1" means delete infinity point in points.
            raise ValueError(f"doesn't collect available {args.n_points} points, we have {len_available_points} points.")

    print("points =")
    if points_on_ec:
        points_on_ec.sort()
        str_points = [str(point) for point in points_on_ec]
        print("\n".join(str_points))
    else:
        print("nothing")
