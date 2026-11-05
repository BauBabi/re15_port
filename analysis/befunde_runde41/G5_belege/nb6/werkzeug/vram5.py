# NB6: RE2-VRAM aus DuckStation-Savestates (Tag 'GPU-VRAM' + 8, 1024x512 16 bpp) -> PNG beider Bildpuffer.
import sys, struct, zstandard
import numpy as np
from PIL import Image
SN = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/re2/snaps/s%02d.sav"
def vram(n):
    d = open(SN % n, "rb").read()
    k = d.find(b"\x28\xb5\x2f\xfd")
    b = zstandard.ZstdDecompressor().stream_reader(d[k:]).read()
    t = b.find(b"GPU-VRAM"); assert t > 0
    v = np.frombuffer(b[t + 8: t + 8 + 1024 * 512 * 2], np.uint16).reshape(512, 1024)
    return v
def rgb(v):
    r = (v & 31) << 3; g = ((v >> 5) & 31) << 3; bl = ((v >> 10) & 31) << 3
    return np.stack([r, g, bl], -1).astype(np.uint8)
if __name__ == "__main__":
    n = int(sys.argv[1])
    v = vram(n)
    Image.fromarray(rgb(v)).save("vram_s%02d.png" % n)
    print("vram_s%02d.png" % n)
