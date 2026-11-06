# Abnahme 2: Szenenbild n -> Zeit im echten RE2-Mitschnitt (Lauf 1, re2_lauf1.mp4), eigene Ankerung
# Anker (eigene Messung re2_kanten.py / Standbild-Suche): Leon laeuft los (Plc_motion @0x01906 = Bild 12) bei 63,87 s -> Bild 0 = 63,47 s;
# Standbild 67,806..68,258 (Xa_on 0x0B @0x01920, Bild 132), Standbild 78,539..79,643 vor Schnitt 10 (Bild 437/438),
# 92,030..92,766 vor Schnitt 5 (Bild 807), 104,585..104,869 vor Schnitt 6 (Bild 1162). Spieltakt 59,8/2 = 29,9 B/s.
FPS = 29.9
def re2_t(n):
    if n <= 132: return 63.47 + n / FPS
    if n <= 437: return 68.258 + (n - 133) / FPS
    if n <= 807: return 79.643 + (n - 438) / FPS
    return 92.766 + (n - 807) / FPS
if __name__ == "__main__":
    import sys
    for a in sys.argv[1:]: print(a, "%.3f" % re2_t(int(a)))
