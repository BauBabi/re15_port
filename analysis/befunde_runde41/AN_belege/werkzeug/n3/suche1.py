from szmass import *
import itertools, json
def frei(x, z, r):
    """grob: begehbar mit Abstand r (Waende 5090 Wagen C, SCA-Kaesten 21/23/25/28/29/30/31)."""
    kaesten = [(-5683,-21990,5180,5166),(-5673,-19870,33730,3006),(-5683,-29740,5220,4766),(4050,-21700,16175,2000),
               (5300,-27200,14950,2000),(-425,-27200,5725,2000),(2700,-21150,1325,1325),(-5673,-29750,33740,2866)]
    for kx,kz,w,d in kaesten:
        if kx - r < x < kx + w + r and kz - r < z < kz + d + r: return False
    return True
def _haupt():
  pass
if __name__ == "__main__":
    D = int(__import__('sys').argv[1]) if len(__import__('sys').argv) > 1 else 5157
    erg = []
    for ax in range(-200, 2701, 300):
      for az in range(-24800, -21099, 300):
        if not frei(ax, az, 350): continue
        for th in range(-30, 31, 6):
          t = math.radians(th); fx = ax + D*math.cos(t); fz = az + D*math.sin(t)
          if not frei(fx, fz, 1500): continue
          gf = blick(fx, fz, ax, az); ga = blick(ax, az, fx, fz)
          buf, al = idpuffer(11, [(FORM,2,0,(fx,0,fz),gf,1),(FRAU,1,0,(ax,0,az),ga,2)])
          m11 = mass(buf, al)
          if not m11['frau_ganz'] or m11['frau_sichtbar'] < 0.97 or m11['form'] > 0.40: continue
          buf, al = idpuffer(15, [(FORM,10,0,(fx,0,fz),gf,1),(FRAU,1,0,(ax,0,az),(ga+2048)&4095,2)])
          m15l = mass(buf, al)
          buf, al = idpuffer(15, [(FORM,10,40,(fx,0,fz),gf,1),(FRAU,1,0,(ax,0,az),ga,2)])
          m15s = mass(buf, al)
          erg.append(dict(A=(ax,az), F=(int(fx),int(fz)), th=th, c11=m11, c15land=m15l, c15starr=m15s))
          print(ax, az, th, int(fx), int(fz), 'c11 form %.2f frau %.2f %s' % (m11['form'], m11['frau_sichtbar'], m11['frau_bbox']),
                '| c15land form %.2f frau %.2f | c15starr form %.2f frau %.2f %s' % (m15l['form'], m15l['frau_sichtbar'], m15s['form'], m15s['frau_sichtbar'], m15s['frau_bbox']), flush=True)
    json.dump(erg, open('suche1_%d.json' % D, 'w'), indent=0)
