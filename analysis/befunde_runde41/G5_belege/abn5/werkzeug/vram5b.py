# Abnahme 5: RE2-VRAM aus dem DuckStation-Savestate (Abschnitt 'GPU-VRAM' + 8, 1024x512 x 16 bpp), Bildpuffer 0 (y 0) / 1 (y 240)
import zstandard, numpy as np
SN = r"C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/re2/snaps/s%02d.sav"
def vram(n):
    d = open(SN % n, "rb").read(); k = d.find(b"\x28\xb5\x2f\xfd")
    b = zstandard.ZstdDecompressor().stream_reader(d[k:]).read()
    t = b.find(b"GPU-VRAM"); assert t > 0 and b.find(b"GPU-VRAM", t + 1) < 0
    return np.frombuffer(b[t + 8: t + 8 + 1024 * 512 * 2], np.uint16).reshape(512, 1024)
def rgb(v):
    return np.stack([(v & 31) << 3, ((v >> 5) & 31) << 3, ((v >> 10) & 31) << 3], -1).astype(np.uint8)
