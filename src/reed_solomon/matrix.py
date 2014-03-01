import pprint

pp = pprint.PrettyPrinter(indent=4)

def mul_matrixes(mat1, mat2):
    mat3 = [None] * 4
    for i in range(4):
        mat3[i] = [None] * 4
#   print("mat3 = ")
#   pp.pprint(mat3)
#   for j in range(4):
#       for i in range(4):
#           mat3[j][i] = j * i
    for k in range(4):
        for j in range(4):
            sum = 0
            for i in range(4):
              # print("mat1[{0}][{1}] = {2}".format(j, i, mat1[j][i]))
              # print("mat2[{0}][{1}] = {2}".format(i, j, mat2[i][j]))
                sum += mat1[k][i] * mat2[i][j]
            mat3[k][j] = sum
#           print("mat3 = ")
#           pp.pprint(mat3)
#           input()
    print("mat3 = ")
    pp.pprint(mat3)

if __name__ == "__main__":
    mat1 = [
        [0, 1, 2, 3],
        [4, 5, 6, 7],
        [8, 9, 10, 11],
        [12, 13, 14, 15]
    ]
    mat2 = [
        [15, 14, 13, 12],
        [11, 10, 9, 8],
        [7, 6, 5, 4],
        [3, 2, 1, 0]
    ]
    matE = [
        [1, 0, 0, 0],
        [0, 1, 0, 0],
        [0, 0, 1, 0],
        [0, 0, 0, 1],
    ]
    mul_matrixes(matE, matE)
    mul_matrixes(matE, mat1)
    mul_matrixes(mat2, matE)
    mul_matrixes(mat1, mat2)
