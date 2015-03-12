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
   #rs4 = par2.py3.Par2(4, 10)
#  #print("dir(rs4) =", dir(rs4))
   #print("/* bits=4, division=10) */")
   #print("expected_vm_of_rs4 = ", end="")
#  #help(pprint)
   #pprint.pprint(rs4.vandermonde_matrix, indent=2)
   #print(";")
#  #print(rs4.vandermonde_matrix)
    print("")

    rs8 = par2.py3.Par2(8, 100)
    print("/* bits=8, division=100 */")
    print("expected_vm_of_rs8[][100] = ", end="")
    print(rs8.vandermonde_matrix, end="")
    print(";")
    print("")
  # pprint.pprint(rs8.vandermonde_matrix, indent=2)

    rs16 = par2.py3.Par2(16, 300)
    print("/* bits=16, division=300 */")
    print("expected_vm_of_rs16[][300] = ", end="")
  # pprint.pprint(rs16.vandermonde_matrix, indent=2)
    print(rs16.vandermonde_matrix, end="")
    print(";")
