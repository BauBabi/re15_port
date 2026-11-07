# Prueft jedes Programm in gen/story80_szenen.inc gegen die RDT-Bytes: "orig @0xA..0xB" muss bytegleich sein,
# "op <bytes> (Vorlage @0xV `...`)" muss der Vorlage bis auf die getauschte Id/Ziel gleichen. Nur lesen.
import re, sys
INC = r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_integration/re15_port/engine/src/gen/story80_szenen.inc"
RDT = {"30C1": r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_integration/re15_port/shared_assets/PSX/STAGE3/ROOM30C1.RDT",
       "6030": r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_integration/re15_port/shared_assets/PSX/STAGE6/ROOM6030.RDT"}
txt = open(INC, encoding="utf-8").read()
progs = re.findall(r"/\* ROOM(\w{4}) (sub\d\d) - .*?\*/\s*static const uint8_t (k_s80_\w+)\[(\d+)\] = \{(.*?)\};", txt, re.S)
fehler = 0
for raum, sub, name, n, body in progs:
    data = bytes(int(x, 16) for x in re.findall(r"0x([0-9a-f]{2})", body))
    rdt = open(RDT[raum], "rb").read()
    kopf = txt[txt.index("/* ROOM%s %s" % (raum, sub)):txt.index("static const uint8_t %s[" % name)]
    print("==", raum, sub, name, len(data), "B (soll", n, ")")
    for m in re.finditer(r"\+0x([0-9A-F]{4}) orig @0x([0-9A-F]{5})\.\.0x([0-9A-F]{5}) \((\d+) B\)", kopf):
        p, a, b, nb = int(m.group(1), 16), int(m.group(2), 16), int(m.group(3), 16), int(m.group(4))
        ok = data[p:p + nb] == rdt[a:b] and (b - a) == nb
        fehler += not ok
        print("  +%04X orig @%05X..%05X %3d B %s" % (p, a, b, nb, "gleich" if ok else "ABWEICHUNG"))
    for m in re.finditer(r"\+0x([0-9A-F]{4}) op   ((?:[0-9a-f]{2} )+) \(Vorlage @0x([0-9A-F]{5}) `([0-9a-f ]+)`\)", kopf):
        p = int(m.group(1), 16); ob = bytes(int(x, 16) for x in m.group(2).split()); v = int(m.group(3), 16)
        vb = bytes(int(x, 16) for x in m.group(4).split())
        okp = data[p:p + len(ob)] == ob; okv = rdt[v:v + len(vb)] == vb
        diff = [i for i in range(len(ob)) if ob[i] != vb[i]]
        fehler += not (okp and okv)
        print("  +%04X op %s Vorlage @%05X %s, im Programm %s, abweichende Bytes %s" % (p, ob.hex(" "), v, "gleich" if okv else "ABWEICHUNG", "ja" if okp else "NEIN", diff))
    # Ende muss Evt_end sein
    print("  Ende:", data[-2:].hex(" "))
print("FEHLER", fehler)
