import os

from cse_util import t1, t2
import sympy as sym
import numpy as np
import time
from sympy import dotprint
import sha256_matrici.util_logical_vec as lv

n_blok = 16
n_cycle = int(64 / n_blok)
print(n_cycle)
file = 'out/compress.hpp'

file2 = open(file, 'w')
# file3 = open(file_call, 'w')
file2.writelines('#ifndef _compresshpp\n')
file2.writelines('#define _compresshpp\n\n')
file2.writelines('#include "sha256_util.hpp"\n\n')


def line_prepender(filename, line):
    with open(filename, 'r+') as f:
        content = f.read()
        f.seek(0, 0)
        f.write(line.rstrip('\r\n') + '\n' + content)


def write(expr, blk, n_cycle):
    args = "hls::stream<uint> w[], hls::stream<uint> s_in[], hls::stream<uint> s_out[],ap_uint<BIT_WCH> &ch"
    file2.writelines("void " + "compress_" + str(n_cycle) + "(" + str(args) + ")\n{\n")

    file2.writelines("#pragma HLS INLINE off\n")

    for i in range(0, n_cycle):
        file2.writelines("uint w" + str(i) + "=w.channel[ch++].read();\n")

    file2.writelines("uint a=s_in.channel[0].read();\n")
    file2.writelines("uint b=s_in.channel[1].read();;\n")
    file2.writelines("uint c=s_in.channel[2].read();;\n")
    file2.writelines("uint d=s_in.channel[3].read();;\n")
    file2.writelines("uint e=s_in.channel[4].read();;\n")
    file2.writelines("uint f=s_in.channel[5].read();;\n")
    file2.writelines("uint g=s_in.channel[6].read();;\n")
    file2.writelines("uint h=s_in.channel[7].read();;\n")

    for s in expr[0]:
        # print(str(s[0]) + "=" + str(s[1]))
        line = "uint {}={}".format(s[0], s[1])
        file2.writelines(line + ";\n")

    for i, ex in enumerate(expr[1]):
        line = "{}<<{}".format("s_out.channel[" + str(7 - i) + "]", ex)
        file2.writelines(line + ";\n")


    file2.writelines("if(bc<48){\n")
    for i in range(0, n_cycle):
        bc_pos = "bc" + ("+" + str(i + 16))
        res = "w[bc" + (("+" + str(i)) if i != 0 else "") + "]+" \
                "SIG0(w[bc+" + str(1 + i) + "])+" + \
              "w[bc+" + str(9 + i) + "]+" + \
              "SIG1(w[bc+" + str(14 + i) + "])"

        line = "{}={}".format("w[" + bc_pos + "]", res)
        file2.writelines(line + ";\n")
    file2.writelines("}\n")

    file2.writelines("}\n")
    args = "w, s"
    return "blk_" + str(blk) + "(" + str(args) + ");\n"


h0 = np.uint32(0x6a09e667)
h1 = np.uint32(0xbb67ae85)
h2 = np.uint32(0x3c6ef372)
h3 = np.uint32(0xa54ff53a)
h4 = np.uint32(0x510e527f)
h5 = np.uint32(0x9b05688c)
h6 = np.uint32(0x1f83d9ab)
h7 = np.uint32(0x5be0cd19)


k = np.uint32(
    [0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
     0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
     0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
     0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
     0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
     0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
     0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
     0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2])

a = h0
b = h1
c = h2
d = h3
e = h4
f = h5
g = h6
h = h7

func_call = "void iter_64(uint w[], uint s[])\n{\n"
pos = 0
for blk in range(0, 20):  # n_blok
    start = time.time()
    if False:
        a = h0
        b = h1
        c = h2
        d = h3
        e = h4
        f = h5
        g = h6
        h = h7
    else:
        a = sym.Symbol('a')
        b = sym.Symbol('b')
        c = sym.Symbol('c')
        d = sym.Symbol('d')
        e = sym.Symbol('e')
        f = sym.Symbol('f')
        g = sym.Symbol('g')
        h = sym.Symbol('h')

    a_ = a
    b_ = b
    c_ = c
    d_ = d
    e_ = e
    f_ = f
    g_ = g
    h_ = h

    # if blk==0:
    #    n_cycle=20
    # else:
    n_cycle = blk + 1
    for i in range(0, n_cycle):  # n_cycle
        bc_pos = "bc" + (("+" + str(i)) if i != 0 else "")
        t1_ = t1(e_, f_, g_, h_) + sym.Symbol(
            'w' + str(i) + '')  # +w[i+64-4]# #+ k[n_cycle * blk + i]#+sym.Symbol('Ki['+str(bc_pos)+']')

        t2_ = t2(a_, b_, c_)

        h_ = g_
        g_ = f_
        f_ = e_
        e_ = d_ + t1_
        d_ = c_
        c_ = b_
        b_ = a_
        a_ = t1_ + t2_
        pos += 1


    expr_sym = sym.cse([h_, g_, f_, e_, d_, c_, b_, a_])

    func_call += write(expr_sym, blk, n_cycle)
    file2.flush()

    a = sym.Symbol('a')
    b = sym.Symbol('b')
    c = sym.Symbol('c')
    d = sym.Symbol('d')
    e = sym.Symbol('e')
    f = sym.Symbol('f')
    g = sym.Symbol('g')
    h = sym.Symbol('h')
    done = time.time()
    print(str(n_cycle) + "\t" + '{:,}'.format(done - start).replace('.', ','))

# expr_sym = sym.cse(h_, symbols=sym.numbered_symbols(sym.Symbol("h_")))
func_call += "}\n"

file2.writelines('\n#endif\n')
file2.close()
