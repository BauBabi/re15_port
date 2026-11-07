"""Texturiertes Probebild: Hintergrund (BSS) + Form + Annette durch einen 5090-Schnitt (form_buehne-Kette)."""
from szmass import *
from PIL import Image, ImageDraw
import struct
def frau_tex():
    b = FRAU.blob; do, n = struct.unpack_from("<II", b, 0); d = list(struct.unpack_from("<%dI" % n, b, do))
    return FB.Textur(b[d[8]:do])
FT = FB.Textur(open(RE2 + "BAHNSTEIG_FORM.TIM", "rb").read())
AT = frau_tex()
def ansicht(cut, objs, titel=""):
    k = kam(cut)
    try: hg = FB.bild_laden(FB.hintergrund_bss("5090", cut))
    except Exception as ex: hg = None; print("kein BSS", ex)
    ziel = np.array(hg, copy=True) if hg is not None else np.full((480, 640, 3), 40, np.uint8)
    tris = []
    for mo, clip, bild, lage, gier, oid in objs:
        licht = RAUM.licht(cut)
        tr = FB.dreiecke(mo, kf(mo, clip, bild), lage, gier, k, licht)
        tris += [(t, FT if oid == 1 else AT) for t in tr]
    # gemeinsam fern zuerst
    for t, tex in sorted(tris, key=lambda q: -q[0][0]):
        FB.rastern([t], tex, ziel, k.abbildung)
    ziel[:48] //= 4; ziel[432:] //= 4
    im = Image.fromarray(ziel); ImageDraw.Draw(im).text((6, 6), titel, fill=(255, 255, 0))
    return im
if __name__ == "__main__":
    import sys
    A = tuple(map(int, sys.argv[1].split(","))); F = tuple(map(int, sys.argv[2].split(","))); aus = sys.argv[3]
    gf = blick(*F, *A); ga = blick(*A, *F)
    bilder = [ansicht(15, [(FORM,10,0,(F[0],0,F[1]),gf,1),(FRAU,1,0,(A[0],0,A[1]),(ga+2048)&4095,2)], "C15 Landung"),
              ansicht(11, [(FORM,10,40,(F[0],0,F[1]),gf,1),(FRAU,1,0,(A[0],0,A[1]),ga,2)], "C11 Drohen"),
              ansicht(15, [(FORM,2,0,(F[0],0,F[1]),gf,1),(FRAU,1,0,(A[0],0,A[1]),ga,2)], "C15 Satz"),
              ansicht(11, [(FORM,2,0,(F[0],0,F[1]),gf,1),(FRAU,1,0,(A[0],0,A[1]),ga,2)], "C11 Satz")]
    b = Image.new("RGB", (1290, 970)); [b.paste(im, ((i % 2) * 645, (i // 2) * 485)) for i, im in enumerate(bilder)]; b.save(aus)
