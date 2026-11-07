# ST80 Abnahme 2: prueft JEDES Programm in gen/story80_szenen.inc gegen die RDT-Bytes.
# - Kommentarblock direkt vor der Deklaration (nicht der erste Treffer des Raum/Sub-Namens)
# - Bausteine lueckenlos (+Offset aufsteigend, Summe = Programmlaenge)
# - "orig" bytegleich mit der RDT, "op" gleich der Vorlage bis auf die genannten Bytes
# - Signatur = erste 16 Bytes des Original-Subs (Sub-Tabelle der RDT)
# - Programm laeuft mit der SCD-Laengentabelle (scd_dump_room.py) ohne Versatz bis Evt_end; Liste Message_on/Set/Ck
# Nur lesen. Aufruf: python -I programm_pruef.py <baum>
import re, sys, struct, os
BAUM = sys.argv[1]
PORT = BAUM + "/re15_port"
INC = PORT + "/engine/src/gen/story80_szenen.inc"
RDT = {"30C1": PORT + "/shared_assets/PSX/STAGE3/ROOM30C1.RDT", "6030": PORT + "/shared_assets/PSX/STAGE6/ROOM6030.RDT"}
ns = {}
src = open(PORT + "/tools/scd_dump_room.py", encoding="utf-8").read()
exec(compile(src[:src.index("\ndef main(")], "scd_dump_room.py", "exec"), ns)
SIZES, NAMES = ns["SIZES"], ns["NAMES"]
def oplen(b, o):
    op = b[o]
    if op == 0x2C: return 28 if (b[o + 3] & 0x80) else 20   # wie scd_dump_room.py (pc[3]&0x80)
    if op == 0x3B: return 40 if (b[o + 3] & 0x80) else 32
    if op == 0x50: return 30 if (b[o + 3] & 0x80) else 22
    return SIZES.get(op)
txt = open(INC, encoding="utf-8").read()
fehler = 0
for m in re.finditer(r"static const uint8_t (k_s80_\w+?)\[(\d+)\] = \{(.*?)\};", txt, re.S):
    name, n, body = m.group(1), int(m.group(2)), m.group(3)
    if name.endswith("_sig") or name.endswith("_folge"): continue
    data = bytes(int(x, 16) for x in re.findall(r"0x([0-9a-f]{2})", body))
    vor = txt[:m.start()]
    k0 = vor.rindex("/* ROOM")
    kopf = vor[k0:]
    raum, sub = re.match(r"/\* ROOM(\w{4}) sub(\d\d)", kopf).groups()
    rdt = open(RDT[raum], "rb").read()
    print("== %s ROOM%s sub%s %d B (Deklaration %d)" % (name, raum, sub, len(data), n))
    if len(data) != n: print("  FEHLER Laenge"); fehler += 1
    segs = []
    for s in re.finditer(r"\+0x([0-9A-F]{4}) orig @0x([0-9A-F]{5})\.\.0x([0-9A-F]{5}) \((\d+) B\)", kopf):
        p, a, b, nb = int(s.group(1), 16), int(s.group(2), 16), int(s.group(3), 16), int(s.group(4))
        ok = data[p:p + nb] == rdt[a:b] and (b - a) == nb
        segs.append((p, nb)); fehler += not ok
        print("  +%04X orig @%05X..%05X %3d B %s" % (p, a, b, nb, "bytegleich" if ok else "ABWEICHUNG"))
    for s in re.finditer(r"\+0x([0-9A-F]{4}) op   ((?:[0-9a-f]{2} )+) \(Vorlage @0x([0-9A-F]{5}) `([0-9a-f ]+)`\)", kopf):
        p = int(s.group(1), 16); ob = bytes(int(x, 16) for x in s.group(2).split()); v = int(s.group(3), 16)
        vb = bytes(int(x, 16) for x in s.group(4).split())
        okp = data[p:p + len(ob)] == ob; okv = rdt[v:v + len(vb)] == vb
        segs.append((p, len(ob))); fehler += not (okp and okv)
        diff = [i for i in range(len(ob)) if ob[i] != vb[i]]
        print("  +%04X op   %s Vorlage @%05X %s %s, im Programm %s, getauscht Byte %s" % (p, ob.hex(" "), v, vb.hex(" "), "= RDT" if okv else "!= RDT", "ja" if okp else "NEIN", diff))
    segs.sort(); pos = 0; luecke = 0
    for p, nb in segs:
        if p != pos: luecke += 1
        pos = p + nb
    if pos != len(data): luecke += 1
    fehler += luecke
    print("  Bausteine lueckenlos bis Programmende:", "ja" if luecke == 0 else "NEIN (%d)" % luecke)
    # Signatur
    st = {"30C1": 0x0BD8, "6030": 0x0D14}[raum]
    so = struct.unpack_from("<H", rdt, st + 2 * int(sub))[0] + st
    sig = re.search(r"static const uint8_t %s_sig\[16\] = \{(.*?)\};" % name, txt).group(1)
    sigb = bytes(int(x, 16) for x in re.findall(r"0x([0-9a-f]{2})", sig))
    oks = sigb == rdt[so:so + 16]; fehler += not oks
    print("  Original-sub%s @0x%05X, Signatur = erste 16 Bytes: %s" % (sub, so, "ja" if oks else "NEIN"))
    # Lauf ueber das Programm
    o = 0; msgs = []; flags = []; ende = False
    while o < len(data):
        L = oplen(data, o)
        if L is None: print("  FEHLER unbekannter Opcode %02x @+%04X" % (data[o], o)); fehler += 1; break
        op = data[o]
        if op == 0x2B: msgs.append(data[o + 1])
        if op in (0x21, 0x22): flags.append("%s(%d,%d)=%d@+%04X" % (NAMES[op], data[o + 1], data[o + 2], data[o + 3], o))
        if op == 0x01 and o + L == len(data): ende = True
        o += L
    fehler += (o != len(data)) or not ende
    print("  SCD-Lauf: %s, Ende Evt_end: %s" % ("ohne Versatz" if o == len(data) else "VERSATZ", ende))
    print("  Message_on:", msgs)
    print("  Ck/Set:", " ".join(flags))
    folge = re.search(r"static const uint8_t %s_folge\[\d+\] = \{(.*?)\};" % name, txt)
    if folge:
        fl = [int(x) for x in folge.group(1).replace(" ", "").split(",")]
        okf = fl == msgs; fehler += not okf
        print("  Folge-Tabelle %s = Message_on-Folge: %s" % (fl, "ja" if okf else "NEIN"))
print("FEHLER", fehler)
