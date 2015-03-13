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
#   print(dir(par2))
#   print(dir(par2.py3))
#   print(dir(par2.py3.ReedSolomon))
#   rs = par2.py3.ReedSolomon.get_reed_solomon(4)
#   rs = par2.py3.ReedSolomon(4, 19, 1, 2)
#   print(rs)
#   rs
#   _make_vandermonde_matrix()
#   rs4 = par2.py3.Par2(4, 10)
   #print("/* bits=4, division=10) */")
   #print("expected_vm_of_rs4 = ", end="")
#  #help(pprint)
   #pprint.pprint(rs4.vandermonde_matrix, indent=2)
   #print(";")
#   L = extender(rs4.vandermonde_matrix)
#   s = pylist2Carray(str(L))
#   print(s)

#   rs8 = par2.py3.Par2(8, 100)
#   print("/* bits=8, division=100 */")
#   print("expected_vm_of_rs8[] = ", end="")
#   print(rs8.vandermonde_matrix, end="")
#   print(";")
#   print("")
# # pprint.pprint(rs8.vandermonde_matrix, indent=2)

    rs16 = par2.py3.Par2(16, 30)
#   print("/* bits=16, division=300 */")
#   print("expected_vm_of_rs16[] = ", end="")
  # pprint.pprint(rs16.vandermonde_matrix, indent=2)
  # print(rs16.vandermonde_matrix, end="")
  # print(";")
    for row in rs16.vandermonde_matrix:
      # print("row =")
      # print(row)
        fmt = " ".join(["{:4x}"] * len(row))
        print(fmt.format(*row))
