import struct, sys
def scan(path, base, off0, size=None):
    d = open(path, 'rb').read()
    text = d[off0:off0 + size] if size else d[off0:]
    out = []
    for i in range(0, len(text) - 3, 4):
        ins = struct.unpack_from('<I', text, i)[0]
        op = ins >> 26
        if op in (0x28, 0x29) and (ins & 0xffff) == 130:   # sb / sh  rt,130(rs)
            out.append("0x%08x %s r%d,130(r%d)" % (base + i, "sb" if op == 0x28 else "sh", (ins >> 16) & 31, (ins >> 21) & 31))
    return out
exe = sys.argv[1]
d = open(exe, 'rb').read()
t_size = struct.unpack_from('<I', d, 0x1c)[0]
r = scan(exe, 0x80010000, 0x800, t_size)
print("PSX.EXE:", len(r)); print("\n".join(r))
for b in sys.argv[2:]:
    r = scan(b, 0x80100000, 0)
    print(b, len(r)); print("\n".join(r))
