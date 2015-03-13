import par2
import pprint

def extender(L):
    L2 = []
    for x in L:
        L2.extend(x)
    return L2

def pylist2Carray(s):
    return s.replace("[", "{").replace("]", "}")

if __name__ == "__main__":
    rs4 = par2.py3.Par2(4, 10)
    print("dir(rs4) =", dir(rs4))
    print("dir(rs4.rds) =", dir(rs4.rds))

    print("rs4.rds.gf =")
    print(rs4.rds.gf)
    print("rs4.rds.gfi =")
    print(rs4.rds.gfi)

#   rs8 = par2.py3.Par2(8, 100)
#   print("rs8.rds.gf =")
#   pprint.pprint(rs8.rds.gf)
#   print("rs8.rds.gfi =")
#   pprint.pprint(rs8.rds.gfi)

#   rs16 = par2.py3.Par2(16, 300)
#   print("rs16.rds(poly, w, bits) =")
#   print(rs16.rds.poly, rs16.rds.w, rs16.rds.bits)
# # pprint.pprint(rs16.vandermonde_matrix, indent=2)
#   print(rs16.vandermonde_matrix, end="")
#   print(";")
