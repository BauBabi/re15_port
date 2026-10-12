#!/usr/bin/env python3
"""Runde 37 Spur O: Auswertung eines 11C0-Laufs (echte exe, tools/r36_k/kette_lauf.sh) Bild fuer Bild.

Eingaben im Laufordner (der Lauf muss IN ROOM11C0 enden, RE15_EXIT_AT=<n>#11C0, sonst ist der letzte Abschnitt des
Kopf-Protokolls nicht 11C0):
  s11c0.log       RE15_S11C0_LOG (engine/src/szene11c0_r37.c): je Bild Untertitel + je Figur Ort, Gier r, Clip,
                  Kopf-Flags nf (+0x1b8), Kopf-Zielpunkt nt (+0x160/+0x164), Kopf-Akku acc, Gier zum Partner gp,
                  Kopf-Ort h (head_world)
  neck_trace.log  RE15_NECK_TRACE (skeleton_common.c): je gezeichnetem Bild und Kopf tgt (lokales, GEKLEMMTES Ziel),
                  res, acc, kf (Kopf-Keyframe) - der letzte Raumbesuch (Bildzaehler faellt zurueck) = 11C0
Kopf-Blick (Welt-Gier) wie die Kopf-FSM ihn baut (skeleton_common.c): lokales Ziel tgt = wYaw - Gier - Wurzel-Keyframe,
gezeichnet wird Kopf-Keyframe + Akku. Also Blick = Gier + Wurzel-Keyframe + kf + acc. Der Wurzel-Keyframe steht nicht
im Protokoll; er folgt aus jedem Bild mit ungeklemmtem Ziel: wurzel = wYaw(Kopf-Ort -> Zielpunkt) - Gier - tgt
(fuer geklemmte Bilder der letzte so bestimmte Wert der Figur).
"Kopf auf X" = Zielpunkt X, Akku eingeschwungen (|res - acc| <= 2) und Ziel nicht an der Klemme (|tgt| < 0x2c8).
Ausgaben:
  (1) Drehungen je Figur: zusammenhaengende Abschnitte mit Gier-Aenderung (Start-/Endbild, Start-/Endgier, Summe in Grad)
  (2) je Untertitel-Abschnitt (Sprecher: 10 Ada, 11/12/13 Marvin, 14 Ada): Koerper -> Partner (max Abweichung),
      Kopf-Ziel, "Kopf auf Partner" (eingeschwungen + ungeklemmt), Kopf-Blick -> Partner (max Abweichung)
  (3) --bilder: eine Zeile je Bild
Aufruf: python szene_mess.py <laufordner> [--bilder]
"""
import re, sys, os, math

ADA = (-8965, -14347)
TUER = (-25100, 17250)
KLEMME = 0x2c8   # NPC-Kopfklemme Gier (STAGE1 @0x8011c79c/@0x8011c7a0)
KEGEL = 0x60     # Ausrichtungs-Kegel des Drehens fuer Typ 0x40/0x42 (Tabelle @0x80076c41) = 8,4 Grad
SPRECHER = {10: 'A', 11: 'M', 12: 'M', 13: 'M', 14: 'A'}
TEXT = {10: 'Ada: What was this noise? Did you hear that?', 11: 'Marvin: Yes!....', 12: 'Marvin: Oh, no, Leon!',
        13: 'Marvin: I have to help him, sorry!', 14: 'Ada: Marvin!...'}


def gier(vx, vz, zx, zz):
    """Peilung wie re15_s11c0_gier (FUN_8001a6d4-Rohwert: 0 = +X, 1024 = -Z). Gleitkomma-Naeherung (+-1)."""
    a = math.degrees(math.atan2(zz - vz, zx - vx))
    return int(round((-a) / 360.0 * 4096)) & 0xfff


def wdiff(a, b):
    d = (b - a) & 0xfff
    return d - 0x1000 if d > 0x800 else d


def grad(u):
    return u * 360.0 / 4096.0


RX = re.compile(r'([AM])\(p=\((-?\d+),(-?\d+)\) r=(\d+) mo=(\d+) s=(\d+) nf=([0-9a-f]+) nt=\((-?\d+),(-?\d+)\) '
                r'acc=(-?\d+) gp=(-?\d+)(?: h=\((-?\d+),(-?\d+)\))?\)')


def lese_s11c0(p):
    bilder = []
    for z in open(p):
        m = re.match(r'F(\d+) msg=(\d+):(\d+) cam=(\d+)', z)
        if not m:
            continue
        b = dict(f=int(m.group(1)), ma=int(m.group(2)), mid=int(m.group(3)), cam=int(m.group(4)))
        for g in RX.finditer(z):
            fig = dict(x=int(g.group(2)), z=int(g.group(3)), r=int(g.group(4)), mo=int(g.group(5)),
                       s=int(g.group(6)), nf=int(g.group(7), 16), nt=(int(g.group(8)), int(g.group(9))),
                       acc=int(g.group(10)), gp=int(g.group(11)))
            fig['h'] = (int(g.group(12)), int(g.group(13))) if g.group(12) is not None else (fig['x'], fig['z'])
            b[g.group(1)] = fig
        bilder.append(b)
    return bilder


RT = re.compile(r'F(\d+) slot=(\d+) fl=([0-9a-f]+) tgt=\((-?\d+),(-?\d+)\) res=\((-?\d+),(-?\d+)\) '
                r'acc=\((-?\d+),(-?\d+)\) kf=\((-?\d+),(-?\d+)\)')


def s12(v):
    v &= 0xfff
    return v - 0x1000 if v >= 0x800 else v


def lese_trace(p):
    """letzter Raumbesuch: Bildzaehler je Slot faellt zurueck -> neu beginnen."""
    tr = {1: {}, 4: {}}
    letzt = {1: -1, 4: -1}
    if not os.path.exists(p):
        return tr
    for z in open(p):
        m = RT.match(z)
        if not m:
            continue
        f, sl = int(m.group(1)), int(m.group(2))
        if sl not in tr:
            continue
        if f < letzt[sl]:
            tr[sl] = {}
        letzt[sl] = f
        tr[sl][f] = dict(fl=int(m.group(3), 16), tgt=s12(int(m.group(4))), res=s12(int(m.group(6))),
                         acc=s12(int(m.group(8))), kf=s12(int(m.group(10))))
    return tr


def kopf_ziel(fig, partner):
    nf, nt = fig['nf'], fig['nt']
    if nf & 0x04:
        if partner and abs(nt[0] - partner['x']) < 2 and abs(nt[1] - partner['z']) < 2:
            return 'Partner'
        if partner and abs(nt[0] - partner['x']) < 500 and abs(nt[1] - partner['z']) < 500:
            return 'Partner (folgt, Nachtrag 12)'
        if nt == TUER:
            return 'Tuer'
        if nt == ADA:
            return 'Ada-Platz'
        return 'Punkt(%d,%d)' % nt
    if (nf & 0x2a) == 0x2a:
        return 'Nicken'
    if nf & 0x02:
        return 'frei'
    return 'nf=%02x' % nf


def blicke(B, T):
    """je Bild und Figur: Kopf-Blick (Welt), eingeschwungen, geklemmt."""
    wurzel = {'A': None, 'M': None}
    for b in B:
        for k, sl in (('A', 1), ('M', 4)):
            if k not in b:
                continue
            fig = b[k]
            # Reihenfolge im Bild: der Mess-Haken laeuft im Irons-Takt VOR den SCD-Faeden (Zustand nach Bild F-1), die
            # Kopf-FSM beim Zeichnen NACH VM und KI -> die s11c0-Zeile F gehoert zum gezeichneten Kopf von Bild F-1.
            tr = T[sl].get(b['f'] - 1)
            fig['tr'] = tr
            if tr is None:
                fig['blick'] = None
                continue
            if (fig['nf'] & 0x04) and abs(tr['tgt']) < KLEMME:
                w = gier(fig['h'][0], fig['h'][1], fig['nt'][0], fig['nt'][1])
                wurzel[k] = wdiff(fig['r'], w) - tr['tgt']
            wz = wurzel[k] if wurzel[k] is not None else 0
            fig['blick'] = (fig['r'] + wz + tr['kf'] + tr['acc']) & 0xfff
            fig['ein'] = abs(tr['res'] - tr['acc']) <= 2
            fig['klemme'] = abs(tr['tgt']) >= KLEMME


def drehungen(B, k):
    seg = None
    segs = []
    prev = None
    for b in B:
        if k not in b:
            continue
        r = b[k]['r']
        if prev is not None:
            d = wdiff(prev[1], r)
            if d != 0:
                if seg is None:
                    seg = [prev[0], prev[1], b['f'], r, d]
                else:
                    seg[2], seg[3], seg[4] = b['f'], r, seg[4] + d
            elif seg is not None and b['f'] - seg[2] > 3:
                segs.append(seg)
                seg = None
        prev = (b['f'], r)
    if seg:
        segs.append(seg)
    return segs


def abschnitte_von(B):
    ab = []
    for b in B:
        if not b['ma']:
            continue
        if ab and ab[-1][0] == b['mid'] and b['f'] - ab[-1][2] <= 1:
            ab[-1][2] = b['f']
        else:
            ab.append([b['mid'], b['f'], b['f']])
    return ab


def main():
    lauf = sys.argv[1]
    B = lese_s11c0(os.path.join(lauf, 's11c0.log'))
    T = lese_trace(os.path.join(lauf, 'neck_trace.log'))
    blicke(B, T)
    print('Bilder %d..%d' % (B[0]['f'], B[-1]['f']))
    for k in ('A', 'M'):
        segs = drehungen(B, k)
        print('Drehungen %s (%s):%s' % (k, 'Ada' if k == 'A' else 'Marvin', '' if segs else ' keine'))
        for s in segs:
            print('  F%d..F%d  Gier %4d -> %4d  Summe %+5d = %+6.1f Grad  (%d Bilder)' % (
                s[0], s[2], s[1], s[3], s[4], grad(s[4]), s[2] - s[0]))
        if segs:
            gr = max(segs, key=lambda s: abs(s[4]))
            print('  groesste Einzeldrehung: %+.1f Grad (F%d..F%d)' % (grad(gr[4]), gr[0], gr[2]))
    ab = abschnitte_von(B)
    print('Untertitel-Abschnitte (Id, Bilder):', ', '.join('%d F%d..F%d' % tuple(a) for a in ab))
    for mid, f0, f1 in ab:
        sp = SPRECHER.get(mid)
        if not sp:
            continue
        zu = 'M' if sp == 'A' else 'A'
        print('msg %d "%s" F%d..F%d:' % (mid, TEXT[mid], f0, f1))
        for wer in (sp, zu):
            rolle = 'Sprecher' if wer == sp else 'Zuhoerer'
            ander = zu if wer == sp else sp
            n = nb = bok = kauf = kl = 0
            kz = {}
            bmax = kmax = 0
            for b in B:
                if b['f'] < f0 or b['f'] > f1 or wer not in b or ander not in b:
                    continue
                fig = b[wer]
                par = b[ander]
                n += 1
                gp = gier(fig['x'], fig['z'], par['x'], par['z'])
                dk = abs(wdiff(fig['r'], gp))
                bmax = max(bmax, dk)
                bok += (dk <= KEGEL)
                z = kopf_ziel(fig, par)
                kz[z] = kz.get(z, 0) + 1
                if fig.get('blick') is not None:
                    nb += 1
                    gh = gier(fig['h'][0], fig['h'][1], par['x'], par['z'])
                    kmax = max(kmax, abs(wdiff(fig['blick'], gh)))
                    kl += fig['klemme']
                    kauf += (z == 'Partner' and fig['ein'] and not fig['klemme'])
            print('   %-8s %-6s: Koerper->Partner max %5.1f Grad (im Kegel 8,4 Grad: %d/%d) | Kopf-Ziel %s | '
                  'Kopf auf Partner (eingeschwungen, ungeklemmt): %d/%d | Kopf-Blick->Partner max %5.1f Grad | '
                  'Kopf an der Klemme: %d' % (rolle, 'Ada' if wer == 'A' else 'Marvin', grad(bmax), bok, n, kz,
                                              kauf, nb, grad(kmax), kl))
    if '--bilder' in sys.argv:
        for b in B:
            s = 'F%d msg=%s' % (b['f'], ('%d' % b['mid']) if b['ma'] else '-')
            for k in ('A', 'M'):
                if k not in b:
                    continue
                o = 'M' if k == 'A' else 'A'
                fig = b[k]
                par = b.get(o)
                zup = grad(wdiff(fig['r'], fig['gp'])) if fig['gp'] >= 0 else 0.0
                if fig.get('blick') is not None and par:
                    gh = gier(fig['h'][0], fig['h'][1], par['x'], par['z'])
                    bls = '%+.0f%s' % (grad(wdiff(gh, fig['blick'])), ' K' if fig['klemme'] else '')
                else:
                    bls = '-'
                s += ' | %s r=%d mo=%d Koerper->P=%+.0f Kopf=%s Blick->P=%s' % (k, fig['r'], fig['mo'], zup,
                                                                             kopf_ziel(fig, par), bls)
            print(s)


if __name__ == '__main__':
    main()
