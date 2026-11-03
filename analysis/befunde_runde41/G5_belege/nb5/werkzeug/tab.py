import struct
b = open(r"C:\workspace\git\reAi_v2\info\re2leon\SLUS_007.48", "rb").read()
def adr2off(a): return a - 0x80010000 + 0x800
needle = struct.pack("<I", 0x80058028)
p = b.find(needle)
while p >= 0:
    a = p - 0x800 + 0x80010000
    print("Xa_on-Zeiger bei 0x%08x" % a)
    base = a - 0x59*4
    for i in range(0x8f):
        v = struct.unpack_from("<I", b, adr2off(base + 4*i))[0]
        print("  op %02x -> %08x" % (i, v))
    p = b.find(needle, p+1)
