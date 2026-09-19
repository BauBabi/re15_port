"""Gegenprobe 10D0/07_01: welche Tiefe bekaemen die Luecken-Pixel des Nutzer-Originals (zwischen
den Stuhlrohren) im neuen Modell, verglichen mit der Bodentiefe an denselben Bildpunkten?"""
import sys, struct, re
sys.path.insert(0, "re15_port/tools/maske")
import numpy as np
import geom, geometrie, maske_aus_png
from geom import load_rdt, cut_view
from abnahme import proj
CD = "re15_port/shared_assets/PSX"
room = "ROOM10D0"; cut = 7
rdt, _ = load_rdt(CD, room); cam = struct.unpack_from("<I", rdt, 0x24)[0]; R, t, H = cut_view(rdt, cam, cut)
r_orig = maske_aus_png.setze("build/p2/10D0_07_01_original.png", 0, 123, 1)
r_fix = maske_aus_png.setze("pri/STAGE1/10D0/07_01.png", 0, 123, 1)
luecke = r_orig & ~r_fix; neu = r_fix & ~r_orig
print("Original %d px, Fix-Fassung %d px, nur im Original (Luecken) %d px, nur im Fix %d px" % (r_orig.sum(), r_fix.sum(), luecke.sum(), neu.sum()))
ys, xs = np.nonzero(luecke); print("Luecken-Pixel: x %d..%d y %d..%d" % (xs.min(), xs.max(), ys.min(), ys.max()))
pos = []
for ln in open(sys.argv[1], errors="replace"):
    m = re.match(r"F(\d+)\s+R10D0 C7\s+hp=\S+\s+pos=\(\s*(-?\d+),\s*(-?\d+),\s*(-?\d+)\)", ln)
    if m: pos.append((int(m.group(2)), int(m.group(4))))
pos = sorted(set(pos))
for name, o in (("aufrecht (wie auswahl)", {"aufrecht": True}), ("Geometrie ohne Handschluessel", {})):
    ber = []; vz, info = geometrie.tiefe_geometrie(rdt, R, t, H, r_orig, 0, 0, o, None, ber)
    boden = np.array([geom.vz_at_floor(R, t, H, x + 0.5, y + 0.5, 0) or 0 for y, x in zip(ys, xs)])
    d = geometrie.rastern(vz[luecke]); db = geometrie.rastern(boden)
    print("%s: %s | Luecken-Pixel Maskentiefe %d..%d (median %d) | Bodentiefe dort %d..%d (median %d) | Maske NAEHER als der Boden: %.0f%%"
          % (name, info.get("quelle"), d.min(), d.max(), np.median(d), db.min(), db.max(), np.median(db), 100 * np.mean(d < db)))
    hits = 0; verd = 0; dd = geometrie.rastern(vz)
    for x, z in pos:
        p = proj(R, t, H, x, 0, z)
        if p is None: continue
        sx, sy, fvz = p; hw = 450 * H / fvz; x0 = int(max(0, sx - hw)); x1 = int(min(320, sx + hw))
        pk = proj(R, t, H, x, -1500, z)
        if pk is None: continue
        y0 = int(max(0, min(pk[1], sy))); y1 = int(min(240, max(pk[1], sy)))
        box = luecke[y0:y1, x0:x1]
        if box.any():
            hits += 1
            vzs, ok = geometrie.profil_spalte(R, t, H, x, z, np.arange(y0, y1)); vzs = np.where(ok, vzs, fvz)
            v = box & geometrie.verdeckt(dd[y0:y1, x0:x1], vzs[:, None])
            verd += int(v.sum() > 0)
    print("   befund.log R10D0 C7: %d Standorte, %d beruehren Luecken-Pixel, davon %d dort (teilweise) verdeckt" % (len(pos), hits, verd))
