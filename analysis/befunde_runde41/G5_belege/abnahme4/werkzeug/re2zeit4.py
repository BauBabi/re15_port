# Abnahme 4: RE2-Szenenbild n -> Videozeit (Mitschnitt Lauf 1, 59,8 B/s). Segmentanker = Enden der selbst gemessenen CD-Standbilder
# (re2_kanten4.py: 67,809..68,261 / 78,545..79,649 / 92,040..92,776 s); Bild 0 = 63,47 s (Abnahme 2/3; Segment 1 endet 67,809 s = Bild 129..132).
FPS = 29.9
def re2_t(n):
    if n <= 132: return 63.47 + n / FPS
    if n <= 437: return 68.261 + (n - 133) / FPS
    if n <= 807: return 79.649 + (n - 438) / FPS
    return 92.776 + (n - 807) / FPS
