import collections
import copy
import pprint

pp = pprint.PrettyPrinter(indent=4, width=40)

class Matrix(object):
    def __init__(self, seq):
        if isinstance(seq, Matrix):
            seq = seq._matrix

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
        if self.len_columns != other.len_rows:
            raise ValueError("self.len_columns(={}) and other.len_rows(={}) must be equal.".format(self.len_columns, other.len_rows))

#       print("self = {}".format(self))
#       print("other = {}".format(other))
        mat = [None] * self.len_rows
        for i in range(self.len_rows):
      # for i in range(len(mat)):
            mat[i] = [None] * other.len_columns
#       print("mat = ")
#       pp.pprint(mat)
    #   for j in range(4):
    #       for i in range(4):
    #           mat[j][i] = j * i
        for k in range(self.len_rows):
            for j in range(other.len_columns):
                sum = 0
                for i in range(other.len_rows):
                  # print("self[{0}][{1}] = {2}".format(j, i, self[j][i]))
                  # print("other[{0}][{1}] = {2}".format(i, j, other[i][j]))
                  # print("(k, j, i)=({}, {}, {})".format(k, j, i))
                    sum += self._matrix[k][i] * other._matrix[i][j]
                mat[k][j] = sum
    #           print("mat = ")
    #           pp.pprint(mat)
    #           input()
        print("mat = ")
        pp.pprint(mat)

        return mat

    # see in par2/par2/py3.py
    def _make_square_matrix(self, value=None):
        square_matrix = [None] * self.len_rows
        for i in range(self.len_rows):
            square_matrix[i] = [value] * self.len_rows
        return square_matrix

    # see in par2/par2/py3.py
    def _make_e_matrix(self):
        e = self._make_square_matrix(0)
        for i in range(self.len_rows):
            e[i][i] = 1
        return e

    # see in par2/par2/py3.py
    def _solve_inverse_matrix(self):
        if self.len_columns != self.len_rows:
            raise ValueError("self.len_columns(={}) and self.len_rows(={}) must be equal.".format(self.len_columns, self.len_rows))
        matrix = copy.deepcopy(self._matrix)
        im = inverse_matrix = self._make_e_matrix()

      # self._at_a_glance(matrix, im)
      # print()
        for k in range(self.len_rows):
            # self._at_a_glance(matrix, im)
            if not matrix[k][k]:
                swap = False
                for j in range(k + 1, self.len_rows):
                    if matrix[j][k]:
                        matrix[k], matrix[j] = matrix[j], matrix[k]
                        im[k], im[j] = im[j], im[k]
                        swap = True
                        break
                if not swap:
                    message = 'cannot make inverse_matrix.'
                    raise ValueError(message)

            if matrix[k][k] != 1:
                tmp = matrix[k][k]
                for i in range(self.len_rows):
                    matrix[k][i] = matrix[k][i] / tmp
                    im[k][i] = im[k][i] / tmp

            for j in range(k + 1, self.len_rows):
                foo = matrix[j][k]
                if not foo:
                    continue
                for i in range(self.len_rows):
                    tmp1 = matrix[k][i]
                    tmp2 = foo * tmp1
                    tmp3 = matrix[j][i]
                    matrix[j][i] = tmp3 + tmp2

                    im1 = im[k][i]
                    im2 = foo * im1
                    im3 = im[j][i]
                    im[j][i] = im3 + im2
      # print('前進完了') # moving front done

        for k in range(self.len_rows - 1):
            for j in range(self.len_rows - 1 - k):
                z = self.len_rows - 1 - k
                x = self.len_rows - 1 - k
                y = self.len_rows - 1 - k - j - 1
                foo = matrix[y][x]

                for i in range(self.len_rows):
              #     tmp1 = matrix[z][i]
              #     tmp2 = self._mul(foo, tmp1)
              #     tmp3 = matrix[y][i]
              #     matrix[y][i] = self._add(tmp3, tmp2)

                    im1 = im[z][i]
                    im2 = foo * im1
                    im3 = im[y][i]
                    im[y][i] = im3 + im2
      # print()

        return inverse_matrix

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
    seq3 = [
        [4, 5, 6, 7],
    ]
    seqE = [
        [1, 0],
        [0, 1],
    ]
    seqA = [
        [0, 1],
        [4, 5],
    ]
    mmat1 = Matrix(seq1)
    mmat2 = Matrix(seq2)
    mmat3 = Matrix(seq3)
    mmatE = Matrix(seqE)
    print("mmat1 =")
    print(mmat1)
    print("mmat2 =")
    print(mmat2)
    print("mmatE =")
    print(mmatE)
#   mmat = mmat1 * mmat2
#   print("mmat1 * mmat2 =")
#   print(mmat._matrix)
    try:
        mmat1 * mmatE
    except ValueError as e:
        pass
    try:
        mmatE * mmat2
    except ValueError as e:
        pass
    Matrix(matE) * Matrix(matE)
    Matrix(matE) * Matrix(mat1)
    Matrix(mat2) * Matrix(matE)
    mmat3 * mmat2
    Matrix(mat1) * Matrix(mat2)
    print("seqA ^ -1 =")
    pp.pprint(Matrix(seqA)._solve_inverse_matrix())
    pp.pprint(Matrix(seqA) * Matrix(Matrix(seqA)._solve_inverse_matrix()))
    print("mat1 ^ -1 =")
    pp.pprint(Matrix(mat1)._solve_inverse_matrix())
    pp.pprint(Matrix(mat1) * Matrix(mat1)._solve_inverse_matrix())
