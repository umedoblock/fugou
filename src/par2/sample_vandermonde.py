import par2
import pprint

if __name__ == "__main__":
#   print(dir(par2))
#   print(dir(par2.py3))
#   print(dir(par2.py3.ReedSolomon))
#   rs = par2.py3.ReedSolomon.get_reed_solomon(4)
#   rs = par2.py3.ReedSolomon(4, 19, 1, 2)
#   print(rs)
#   rs
#   _make_vandermonde_matrix()
    rs4 = par2.py3.Par2(4, 10)
#   print("dir(rs4) =", dir(rs4))
    print("rs4.vandermonde_matrix =")
#   help(pprint)
    pprint.pprint(rs4.vandermonde_matrix, indent=2)
    print(rs4.vandermonde_matrix)
