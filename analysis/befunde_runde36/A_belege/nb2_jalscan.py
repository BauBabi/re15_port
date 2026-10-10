import struct, sys
exe = open(sys.argv[1], 'rb').read()
t_addr = struct.unpack_from('<I', exe, 0x18)[0]
t_size = struct.unpack_from('<I', exe, 0x1c)[0]
text = exe[0x800:0x800 + t_size]
print("t_addr=0x%08x t_size=0x%x" % (t_addr, t_size))
def jal(target):
    return 0x0c000000 | ((target & 0x0fffffff) >> 2)
for name, tgt in [("FUN_8003ef6c", 0x8003ef6c), ("FUN_8003ecec", 0x8003ecec), ("FUN_8003ee3c", 0x8003ee3c), ("FUN_800396fc", 0x800396fc)]:
    w = jal(tgt)
    hits = [t_addr + i for i in range(0, len(text) - 3, 4) if struct.unpack_from('<I', text, i)[0] == w]
    print(name, "jal-word 0x%08x" % w, "callers:", " ".join("0x%08x" % h for h in hits))
# stores to 0x800b1028: lui at,0x800b (3c01800b) followed within 2 instr by sw ...,0x1028(at) (ac2?1028)
hits = []
for i in range(0, len(text) - 3, 4):
    ins = struct.unpack_from('<I', text, i)[0]
    op = ins >> 26
    if op in (0x2b, 0x29, 0x28) and (ins & 0xffff) == 0x1028 and ((ins >> 21) & 31) == 1:
        hits.append((t_addr + i, ins))
print("Stores sw/sh/sb ...,0x1028(at):", " ".join("0x%08x:%08x" % h for h in hits))
