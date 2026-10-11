import struct, sys
exe = open(sys.argv[1], 'rb').read()
t_addr = struct.unpack_from('<I', exe, 0x18)[0]
for tgt in sys.argv[2:]:
    t = int(tgt, 16)
    word = 0x0C000000 | ((t >> 2) & 0x03FFFFFF)
    hits = []
    for off in range(0x800, len(exe) - 3, 4):
        if struct.unpack_from('<I', exe, off)[0] == word:
            hits.append(off - 0x800 + t_addr)
    print(tgt, 'jal-Aufrufer:', ' '.join('%08x' % h for h in hits))
