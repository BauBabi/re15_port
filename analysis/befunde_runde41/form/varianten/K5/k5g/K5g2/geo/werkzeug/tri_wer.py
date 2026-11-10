import re, sys
def sig(z):
    return (z[z.find("clut="):z.find("clut=") + 11],) + tuple(re.findall(r"uv\((\d+),(\d+)\)", z))
def letzte(pfad, n):
    with open(pfad, "r", errors="replace") as f:
        L = f.readlines()
    return L[-n:]
a = letzte(sys.argv[1], 5190 * 2)          # 5090: zwei Bilder
sp = set(sig(z) for z in a if "slot=11 " in z)
fo = set(sig(z) for z in a if "slot=12 " in z)
b = letzte(sys.argv[2], 4417)              # 5080: ein Bild
nur_sp = sum(1 for z in b if sig(z) in sp and sig(z) not in fo)
nur_fo = sum(1 for z in b if sig(z) in fo and sig(z) not in sp)
print("Kennungen (CLUT + UV) Spielerin %d, Form %d, gemeinsam %d" % (len(sp), len(fo), len(sp & fo)))
print("Bild in 5080: %d Zeilen, davon nur-Spielerin-Kennung %d, nur-Form-Kennung %d, weder noch %d" % (len(b), nur_sp, nur_fo, len(b) - nur_sp - nur_fo))
