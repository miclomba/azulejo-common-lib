import unittest

from ResourcesModule import *

def qc_vec(resource):
    vec = resource.data()

    assert(len(vec) == 0)
    val = 7
    vec.append(val)
    assert(len(vec) == 1)
    assert(vec[0] == val)

class Test_ResourcesModule(unittest.TestCase):

    def test_resources_vec_double(self):
        vec = VecDouble()
        resource = ResourceVecDouble(vec)
        qc_vec(resource)

    def test_resources_vec_float(self):
        vec = VecFloat()
        resource = ResourceVecFloat(vec)
        qc_vec(resource)

    def test_resources_vec_int(self):
        vec = VecInt()
        resource = ResourceVecInt(vec)
        qc_vec(resource)

    def test_resources_vec_uint(self):
        vec = VecUInt()
        resource = ResourceVecUInt(vec)
        qc_vec(resource)

