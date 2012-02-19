import struct

__all__ = [ \
    'matrix_to_bytes', 'bytes_to_matrix'
]

def matrix_to_bytes(matrix, code_size):
    elements = [None] * len(matrix)
    for i, m in enumerate(matrix):
      # elements[i] = struct.pack('=' + 'H' * len(m), *m)
        ms = [int.to_bytes(mm, code_size, 'big') for mm in m]
        elements[i] = b''.join(ms)
#   print('elements =', elements)
    return b''.join(elements)

def bytes_to_matrix(bys, element_num, horizontal_size, code_size):
    matrix = [None] * element_num
    for i in range(element_num):
        horizontal_bytes = bys[i*horizontal_size:(i+1)*horizontal_size]
        nums = [None] * element_num
        for j in range(element_num):
            byts = horizontal_bytes[j*code_size:(j+1)*code_size]
            num = int.from_bytes(byts, 'big')
            nums[j] = num
        matrix[i] = nums
      # print('matrix[{}] = {}'.format(i, matrix[i]))
    return matrix
