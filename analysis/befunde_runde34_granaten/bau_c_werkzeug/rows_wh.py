import sys, struct
sys.path.insert(0, '.')
import esp_rows as E
b = open(E.P,'rb').read()
effs = E.parse(b)
for e in effs:
    print("id %d hdr_clut=0x%04x hdr_tpage=0x%04x" % (e['id'], e['hdr_clut'], e['hdr_tpage']))
    blk = e['end']
    for sub in range(8):
        sub_off = E.u16(b, blk + sub*2)
        base = blk + sub_off*4
        if base + 4 > len(b): continue
        streams = E.u16(b, base)
        if streams == 0 or streams > 16: continue
        p = base + 4
        for s in range(streams):
            nr = E.u16(b, p)
            rows = []
            if p + 4 + nr*40 > len(b): print("   sub%d st%d nrows=%d ausserhalb" % (sub,s,nr)); break
            for r in range(nr):
                o = p + 4 + r*40
                A = E.u16(b,o); B=E.u16(b,o+2); w=E.u16(b,o+4); h=E.u16(b,o+6)
                fl = E.u16(b,o+0x0e)
                rows.append("A%d B%d w%04x h%04x fl%02x t16=%04x t1e=%04x" % (A,B,w,h,fl,E.u16(b,o+0x16),E.u16(b,o+0x1e)))
            print("   sub%d st%d nrows=%d: %s" % (sub, s, nr, " | ".join(rows)))
            p += 4 + nr*40
