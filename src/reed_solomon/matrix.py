import collections
import pprint

pp = pprint.PrettyPrinter(indent=4)

class Matrix(object):
    def __init__(self, seq):
        if not isinstance(seq, collections.Sequence) or \
           not isinstance(seq[0], collections.Sequence):
            raise ValueError("seq and seq[0] must be Sequence object")
        self.len_rows = len(seq)
        self.len_columns = len(seq[0])
        self._matrix = seq

        self._check_column_of_elements()

    def _check_column_of_elements(self):
        row_elements = 0
        for i, row in enumerate(self._matrix):
            len_row = len(row)
            if i == 0:
                row_elements = len_row
            elif len_row != row_elements:
                ValueError("row of length must be equal in all rows.")

    def __str__(self):
        return "(len_rows, len_columns)=({}, {})".format(self.len_rows, self.len_columns)

    def __repr__(self):
        return self

    def __mul__(self, other):
        print("self = {}".format(self))
        print("other = {}".format(other))
        mat = [None] * self.len_rows
        for i in range(self.len_rows):
      # for i in range(len(mat)):
            mat[i] = [None] * other.len_columns
        print("mat = ")
        pp.pprint(mat)
    #   for j in range(4):
    #       for i in range(4):
    #           mat[j][i] = j * i
        for k in range(self.len_rows):
            for j in range(other.len_columns):
                sum = 0
                for i in range(other.len_rows):
                  # print("self[{0}][{1}] = {2}".format(j, i, self[j][i]))
                  # print("other[{0}][{1}] = {2}".format(i, j, other[i][j]))
                    print("(k, j, i)=({}, {}, {})".format(k, j, i))
                    sum += self._matrix[k][i] * other._matrix[i][j]
                mat[k][j] = sum
    #           print("mat = ")
    #           pp.pprint(mat)
    #           input()
        print("mat = ")
        pp.pprint(mat)

        return Matrix(mat)

def mul_matrixes(mat1, mat2):
    return
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

    seq1 = [
        [0, 1, 2, 3],
        [4, 5, 6, 7],
    ]
    seq2 = [
        [15, 14],
        [11, 10],
        [7, 6, ],
        [3, 2, ]
    ]
    seqE = [
        [1, 0],
        [0, 1],
    ]
    mat1 = Matrix(seq1)
    mat2 = Matrix(seq2)
    matE = Matrix(seqE)
    print("mat1 =")
    print(mat1)
    print("mat2 =")
    print(mat2)
    print("matE =")
    print(matE)
    mat = mat1 * mat2
    print("mat1 * mat2 =")
    print(mat)
