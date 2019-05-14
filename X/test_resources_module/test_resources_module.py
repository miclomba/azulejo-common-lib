
from ResourcesModule import *

def qc(resource):
    vec = resource.data()

    vec.append(0)
    vec.append(1)
    print("resource:")
    print("len=", len(vec), end="\n values=")
    for i in vec:
        print(i, end=" ")
    print()

if __name__ == "__main__":

    vec_d = VecDouble()
    vec_f = VecFloat()
    vec_i = VecInt()
    vec_ui = VecUInt()

    resource_d = ResourceVecDouble(vec_d)
    resource_f = ResourceVecFloat(vec_f)
    resource_i = ResourceVecInt(vec_i)
    resource_ui = ResourceVecUInt(vec_ui)

    qc(resource_d)
    qc(resource_f)
    qc(resource_i)
    qc(resource_ui)
