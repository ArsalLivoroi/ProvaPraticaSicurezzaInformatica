
import sympy as sym
import numpy as np
from sympy.logic.boolalg import And, Or, Not, Xor


def op(x,d,c):
    res=x * ((1 / sym.Integer(2 ** d)) + (sym.Integer(2 ** (32 - d))))
    if type(res)==sym.Rational:
        return sym.Integer(int(res))
    else:
        return res
    return res


ror_a = lambda val, r_bits, max_bits: \
    ((val & (2**max_bits-1)) >> r_bits%max_bits) | \
    (val << (max_bits-(r_bits%max_bits)) & (2**max_bits-1))

ror=ror_a

def add(a,b):
    print(a)
    res=[sym.false for i in range(len(a))]
    r=sym.false
    for i in range(0,len(a)):
        #i=len(a)-j
        s=Xor(Xor(a[i], b[i]),r)
        r=And(a[i] , b[i],r)
        res[i]=Or(s+r)
    return res
    #return a+b

def t1(e,f,g,h):
    #print("{}{}{}{}".format(type(e), type(f), type(g), type(h)))
    if (type(e) is (np.uint32 or np.int64)) and \
           (type(f) is (np.uint32 or np.int64)) and\
            (type(g) is (np.uint32 or np.int64)):
        s1 = ror(e, 6, 32) ^ ror(e, 11, 32) ^ ror(e, 25, 32)
        ch = (e & f) ^ ((~e) & g)
    else:
        s1 = sym.Function('ep1')(e)
        ch = sym.Function('CH')(e,f,g)

    return h+s1+ch


def t2(a,b,c):
    #print("{}{}{}".format(type(a),type(b),type(c)))
    if (type(a) is (np.uint32 or np.int64)) and \
            (type(b) is (np.uint32 or np.int64)) and\
            (type(c) is (np.uint32 or np.int64)):
        s0 = ror(a, 2, 32) ^ ror(a, 13, 32) ^ ror(a, 22, 32)
        maj = (a & b) ^ (a & c) ^ (b & c)
    else:
        s0 = sym.Function('ep0')(a)
        maj = sym.Function('MAJ')(a,b,c)

    return s0+maj




def matrix(n):
    matrix= [[i^j for i in range(n)] for j in range(n)]
    print(np.matrix(matrix))


def sig1(x):
    if type(x) is np.uint32:
        return ror(x,7,32)^ror(x,18,32)^(x>>3)
    else:
        return sym.Function('sig1')(x)


def sig0(x):
    if type(x) is np.uint32:
        return ror(x,17,32)^ror(x,19,32)^(x>>10)
    else:
        return sym.Function('sig0')(x)