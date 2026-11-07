"""ram_reihe.py SAV... - je Savestate: Schnitt, Claire, Annette (Slot 0), Birkin (Slot 1), SCD-Zeiger.
Adressen (SLES_109.72, Belege in zeitachse.md):
  Schnitt u16 0x800cc3b4   (Cut_chg-Kern @0x80050e08 `3c 02 80 0d` / @0x80050e0c `24 42 c3 b4` / @0x80050e18 `a4 44 00 00` sh a0)
  Spieler 0x800c79a8, Gegner 0x800c79b0[i] (Work_set @0x80051cc0.. / @0x80051ce4..)
  Lage +0x38/+0x3C/+0x40 (s32), Richtung +0x74/+0x76/+0x78 (s16)  (Member_set-Tabelle 0x80010f00 [0x0b..0x10])
  Routine +4..+7 (u8; Plc_motion schreibt +4 = (p1<<8)|4), Plc_motion-Nr +0x14C (u8), +0x1CC (u16), Member 0x17 = +0x1D4 (u16)
  +0x14D = Bildzaehler der laufenden Bewegung (Plc_motion setzt ihn auf 0; laeuft in den Staenden hoch und springt zurueck)
  SCD-Zeiger der Ereignis-Aufgabe: u32 0x800cf644 (= Aufgabe+0x1C; die Befehlsroutinen lesen ihn mit `lw v0,0x1c(a0)`),
  Ruecksprung (gosub) u32 0x800cf76c; RDT-Lage im RAM per Muster MAIN sub02 (Datei 0x00E32, 24 Byte) gesucht.
"""
import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from re2c_ss import SS

def ent(s, p):
    if not p or p == 0x800cbde0:
        return None
    return dict(id=s.u8(p + 8), x=s.s32(p + 0x38), y=s.s32(p + 0x3c), z=s.s32(p + 0x40),
                dx=s.s16(p + 0x74), dy=s.s16(p + 0x76) & 0xfff, dz=s.s16(p + 0x78),
                r=s.bytes(p + 4, 4).hex(), mo=s.u8(p + 0x14c), mo2=s.u8(p + 0x14d), f1cc=s.u16(p + 0x1cc),
                m17=s.u16(p + 0x1d4), f1d6=s.u16(p + 0x1d6), p=p)

def fmt(e):
    if not e:
        return "-"
    return "id%02x (%6d,%5d,%6d) R%04d r%s mo%02x/%02x cc%04x m17=%d" % (
        e['id'], e['x'], e['y'], e['z'], e['dy'], e['r'], e['mo'], e['mo2'], e['f1cc'], e['m17'])

RDT = r"C:/workspace/git/reAi_v2/info/re2clair/PL1/RDT/ROOM6141.RDT"
SUBS = [("sub00", 0xC40, 0xDFA), ("sub01", 0xDFA, 0xE32), ("sub02", 0xE32, 0x1192), ("sub03", 0x1192, 0x14D4),
        ("sub08", 0x151C, 0x152C), ("sub09", 0x152C, 0x156C), ("sub10", 0x156C, 0x157A), ("sub11", 0x157A, 0x158C)]
_rdt = open(RDT, "rb").read()

def scd(s):
    ram = s.ram(); i = ram.find(_rdt[0xE32:0xE32 + 24])
    if i < 0:
        return "?"
    base = 0x80000000 + i - 0xE32
    out = []
    for a in (0x800cf644, 0x800cf76c):
        o = s.u32(a) - base
        nm = [n for n, lo, hi in SUBS if lo <= o < hi]
        out.append("@0x%05X%s" % (o, "(" + nm[0] + ")" if nm else "") if 0 <= o < len(_rdt) else "-")
    return " / ".join(out)

if __name__ == "__main__":
    for f in sys.argv[1:]:
        s = SS(f)
        cut = s.u16(0x800cc3b4)
        pl = ent(s, s.u32(0x800c79a8))
        a = ent(s, s.u32(0x800c79b0))
        b = ent(s, s.u32(0x800c79b4))
        print("%-8s cut %2d | SCD %s | C %s | A %s | B %s" % (os.path.basename(f)[:-4], cut, scd(s), fmt(pl), fmt(a), fmt(b)))
