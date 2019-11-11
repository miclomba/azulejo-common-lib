import unittest

from ResourcesModule import *

def qc_matrix(resource, val):
    matrix = resource.data()
    assert(len(matrix) == 1)
    assert(len(matrix[0]) == 1)
    assert(matrix[0][0] == val)

class Test_ResourcesModule(unittest.TestCase):

    def test_resources_matrix_double(self):
        matrix = MatrixDouble()
        assert(len(matrix) == 0)

        val = 7
        vec = VecDouble()
        vec.append(val)
        matrix.append(vec)

        resource = ResourceDouble(matrix)
        #qc_matrix(resource, val)

    def test_resources_matrix_float(self):
        matrix = MatrixFloat()
        assert(len(matrix) == 0)

        val = 7
        vec = VecFloat()
        vec.append(val)
        matrix.append(vec)

        resource = ResourceFloat(matrix)
        #qc_matrix(resource, val)

    def test_resources_matrix_int(self):
        matrix = MatrixInt()
        assert(len(matrix) == 0)

        val = 7
        vec = VecInt()
        vec.append(val)
        matrix.append(vec)

        resource = ResourceInt(matrix)
        #qc_matrix(resource, val)

    def test_resources_matrix_uint(self):
        matrix = MatrixUInt()
        assert(len(matrix) == 0)

        val = 7
        vec = VecUInt()
        vec.append(val)
        matrix.append(vec)

        resource = ResourceUInt(matrix)
        #qc_matrix(resource, val)
