import struct

__all__ = [ \
    'matrix_to_bytes', 'bytes_to_matrix'
]

def matrix_to_bytes(matrix):
    elements = [None] * len(matrix)
    for i, m in enumerate(matrix):
        elements[i] = struct.pack('=' + 'H' * len(m), *m)
    return b''.join(elements)

def bytes_to_matrix(bys, redundancy, horizontal_size):
    matrix = [None] * redundancy
    for i in range(redundancy):
        horizontal_bytes = bys[i*horizontal_size:(i+1)*horizontal_size]
        vector = struct.unpack('=' + 'H' * redundancy, horizontal_bytes)
        matrix[i] = list(vector)
    return matrix
