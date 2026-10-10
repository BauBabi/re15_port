"""Nachbesserung 3 Spur C — Zensus aller `jal 0x8001bf10` in RE2 EMZ0.BIN MIT a0 (Id) und a3-Vektor.

Unterschied zu n1/zensus2.py: (1) a0 wird ausgewertet (addiu a0,zero,imm | lui a0 + ori/addiu), Id = a0 >> 24,
Sub = (a0 >> 16) & 0xFF; (2) das Register-Fenster beginnt hinter dem letzten jal (auch rand 0x80015fe8, a0..a3 sind
caller-saved), das Stapel-Fenster (sh ..,N(sp)) reicht ueber rand-Aufrufe hinweg bis zum letzten anderen jal — zensus2.py
brach dort ab und fand z.B. an 0x80102BBC keinen Vektor. Ausgabe je Stelle: Adresse, a0/Id/Sub, a2, a3, Stapel-Schreiber.
Aufruf: python -I zensus3.py <EMZ0.BIN> <re2_disasm.py>
"""
import re, struct, subprocess, sys

BIN, DIS = sys.argv[1], sys.argv[2]
data = open(BIN, "rb").read()
JAL_BF10 = 0x0C000000 | ((0x8001BF10 & 0x0FFFFFFF) >> 2)
RAND = "jal 0x80015fe8"
sites = [0x80100000 + i for i in range(0, len(data) - 3, 4) if struct.unpack_from("<I", data, i)[0] == JAL_BF10]

def dis(addr, n):
    out = subprocess.run([sys.executable, "-I", DIS, "--bin", "EMZ0.BIN", "dis", hex(addr), str(n)],
                         capture_output=True, text=True).stdout
    res = []
    for l in out.splitlines():
        m = re.match(r"\s*([0-9a-f]{8}):\s+(.*?)\s*$", l)
        if m:
            res.append((int(m.group(1), 16), m.group(2).split(";")[0].strip()))
    return res

def imm(s):
    s = s.strip()
    return int(s, 16) if s.lower().startswith(("0x", "-0x")) else int(s)

rows = []
for s in sites:
    L = dis(s - 4 * 90, 92)
    idx = [i for i, (a, _) in enumerate(L) if a == s][0]
    # Register-Fenster: hinter dem Delay-Slot des letzten jal/jalr/jr ra
    rs = 0
    for i in range(idx - 1, -1, -1):
        if re.match(r"(jal|jalr|jr) ", L[i][1]):
            rs = i + 2; break
    # Stapel-Fenster: rand-Aufrufe ueberspringen
    ss = 0
    for i in range(idx - 1, -1, -1):
        t = L[i][1]
        if re.match(r"(jal|jalr|jr) ", t) and not t.startswith(RAND):
            ss = i + 2; break
    win = L[rs: idx + 2]          # inkl. Delay-Slot der Stelle
    swin = L[ss: idx + 2]
    a0 = None; a0txt = []
    for a, t in win:
        m = re.match(r"addiu a0,zero,(-?\w+)$", t)
        if m: a0 = imm(m.group(1)) & 0xFFFFFFFF; a0txt.append(t); continue
        m = re.match(r"lui a0,(\w+)$", t)
        if m: a0 = (imm(m.group(1)) << 16) & 0xFFFFFFFF; a0txt.append(t); continue
        m = re.match(r"ori a0,a0,(\w+)$", t)
        if m and a0 is not None: a0 |= imm(m.group(1)); a0txt.append(t); continue
        m = re.match(r"addiu a0,a0,(-?\w+)$", t)
        if m and a0 is not None: a0 = (a0 + imm(m.group(1))) & 0xFFFFFFFF; a0txt.append(t); continue
        if re.match(r"\w+ a0,", t): a0 = "dyn"; a0txt.append(t)
    a2 = [t for a, t in win if re.match(r"\w+ a2,", t)]
    a3 = [t for a, t in win if re.match(r"\w+ a3,", t)]
    a3s = a3[-1] if a3 else "(a3 aus frueherem Fenster)"
    vec = ""
    m = re.match(r"addiu a3,sp,(\d+)$", a3s)
    if m:
        base = int(m.group(1))
        st = [(a, t) for a, t in swin if re.search(r"\b(sh|sw) \w+,(%d|%d|%d)\(sp\)" % (base, base + 2, base + 4), t)]
        vec = " ; ".join("%08x %s" % (a, t) for a, t in st[-6:])
    if isinstance(a0, int):
        ida = "a0=0x%08x Id=%d Sub=%d Skala=%d" % (a0, a0 >> 24, (a0 >> 16) & 0xFF, a0 & 0xFFFF)
    else:
        ida = "a0=%s (%s)" % (a0, " | ".join(a0txt[-3:]))
    rows.append((s, ida, a2[-1] if a2 else "-", a3s, vec))

for s, ida, a2, a3s, vec in rows:
    print("%08x  %-44s  a2[%s]  a3[%s]  vek[%s]" % (s, ida, a2, a3s, vec))
print("Summe", len(rows))
