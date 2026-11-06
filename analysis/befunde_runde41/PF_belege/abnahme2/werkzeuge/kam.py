"""Eigene Kamera aus der RDT (Kopf +0x24 -> Tabelle, je Satz 32 Bytes: u16 Kennung, u16 fov, i32 Auge xyz, i32 Ziel xyz, u32).
Achsen wie re15_camera_build_view (FUN_80053ca4), in Gleitkomma: Zeile0 (cy,0,-sy), Zeile1 (sp*sy, cp, sp*cy), Zeile2 (cp*sy, -sp, cp*cy);
sx = 160 + H*X/Z, sy = 120 + H*Y/Z, H = fov >> 7."""
import struct, math
import numpy as np

PSX = r'C:/workspace/git/reAi_v2/.claude/worktrees/r41_panelfix/re15_port/shared_assets/PSX'


def kamera(raum, cut):
    b = open('%s/STAGE%s/ROOM%s.RDT' % (PSX, raum[0], raum), 'rb').read()
    off = struct.unpack_from('<I', b, 0x24)[0] + 32 * cut
    kenn, fov, ex, ey, ez, tx, ty, tz, _ = struct.unpack_from('<HHiiiiiiI', b, off)
    return Kamera((ex, ey, ez), (tx, ty, tz), fov >> 7, off, b[off:off + 32])


class Kamera:
    def __init__(self, auge, ziel, H, off=None, roh=None):
        self.auge = np.array(auge, float)
        self.H = H
        self.off, self.roh = off, roh
        dx, dy, dz = np.array(ziel, float) - self.auge
        dist = math.sqrt(dx * dx + dy * dy + dz * dz)
        hor = math.sqrt(dx * dx + dz * dz)
        sp, cp = -dy / dist, hor / dist
        sy, cy = dx / hor, dz / hor
        self.R = np.array([[cy, 0, -sy], [sp * sy, cp, sp * cy], [cp * sy, -sp, cp * cy]])

    def proj(self, P):
        P = np.asarray(P, float)
        c = (P - self.auge) @ self.R.T
        return np.stack([160 + self.H * c[..., 0] / c[..., 2], 120 + self.H * c[..., 1] / c[..., 2], c[..., 2]], -1)

    def strahl(self, sx, sy):
        """Richtung (Welt) durch den Bildpunkt (sx, sy)."""
        d = np.stack([np.asarray(sx, float) - 160, np.asarray(sy, float) - 120, np.full(np.shape(sx), float(self.H))], -1)
        return d @ self.R

    def schnitt_ebene(self, sx, sy, p0, n):
        """Bildpunkt -> Weltpunkt auf der Ebene (p0, n)."""
        d = self.strahl(sx, sy)
        t = ((np.asarray(p0, float) - self.auge) @ np.asarray(n, float)) / (d @ np.asarray(n, float))
        return self.auge + d * t[..., None]


if __name__ == '__main__':
    import sys
    # Port-Geometrie (tools/r41_nachtrag30/schrank_bild.py X_WAND 5400, Z -2000..-3300, Y -3650..-50; Fluegel offen = rot_y 1024 um die Angel (5400, -2000))
    u = np.linspace(0, 1, 9)
    v = np.linspace(0, 1, 9)
    U, V = np.meshgrid(u, v)
    innen = np.stack([np.full_like(U, 5400), -3650 + 3600 * V, -2000 - 1300 * U], -1)
    fluegel = np.stack([5400 - 1300 * U, -3650 + 3600 * V, np.full_like(U, -2000.)], -1)
    for raum in ('2010', '2011'):
        for c in range(6):
            k = kamera(raum, c)
            for name, F, n in (('innen', innen, (-1, 0, 0)), ('fluegel', fluegel, (0, 0, -1))):
                p = k.proj(F).reshape(-1, 3)
                im = (p[:, 2] > 0) & (p[:, 0] >= 0) & (p[:, 0] < 320) & (p[:, 1] >= 0) & (p[:, 1] < 240)
                vorne = (np.mean(F.reshape(-1, 3), 0) - k.auge) @ np.array(n) < 0
                print(raum, 'cut', c, '@0x%x' % k.off, name, '%2d/81 im Bild' % im.sum(), 'Vorderseite' if vorne else 'Rueckseite',
                      'x %.0f..%.0f y %.0f..%.0f' % (p[im, 0].min(), p[im, 0].max(), p[im, 1].min(), p[im, 1].max()) if im.any() else '')
