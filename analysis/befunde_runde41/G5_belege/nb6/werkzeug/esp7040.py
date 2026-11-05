# ROOM7040 effect.esp + esp TIM aus dem RDT schneiden, Bankkopf/Skripte/Steps dumpen
import struct, sys, os, hashlib
R = r"C:\workspace\git\reAi_v2\info\re2leon\PL0\RDT\%s.RDT"
OUT = os.path.dirname(os.path.abspath(__file__))
def u16(b,o): return struct.unpack_from('<H',b,o)[0]
def s16(b,o): return struct.unpack_from('<h',b,o)[0]
def u32(b,o): return struct.unpack_from('<I',b,o)[0]
def esp_aus_rdt(room):
    b = open(R % room,'rb').read()
    offs = struct.unpack_from('<23I', b, 8)
    esp = b[offs[18]:offs[19]+4]
    return b, offs, esp
def tims(b, offs, esp):
    ids = [x for x in esp[:8] if x != 0xff]
    res = {}
    for n,i in enumerate(ids):
        o = u32(b, offs[21]-4-4*n)
        t0 = offs[20]+o
        csz = u32(b, t0+8)
        img = t0+8+csz
        isz = u32(b, img)
        res[i] = (t0, b[t0:img+isz], o)
    return res
def bank_dump(esp, name):
    al = (len(esp)+3)&~3
    ende = al-4
    ids = []
    for n in range(8):
        i = esp[n]
        if i == 0xff: break
        ids.append(i)
    print(name, 'Groesse', len(esp), 'Ids', ' '.join('%02X'%i for i in ids))
    for n,i in enumerate(ids):
        off = u32(esp, ende-4*n)
        w = u32(esp, off)
        n1, n2 = w & 0xffff, w >> 16
        tab = off + (n1*2 + n2 + 2)*4
        print(' Bank %02X off 0x%X n1(Anim) %d n2(Zellen) %d tpage/clut kopf: +4 0x%04X +6 0x%04X  Tabelle 0x%X' % (i, off, n1, n2, u16(esp,off+4), u16(esp,off+6), tab))
        print('  Kopf', esp[off:off+8].hex(' '))
        for a in range(n1):
            e = esp[off+8+a*8: off+16+a*8]
            print('  Anim[%d] cell %d nprim %d dauer %d size %d | %s' % (a, e[0], e[1], e[2], e[3], e.hex(' ')))
        uvb = off + (n1*4+4)*2
        for c in range(n2):
            e = esp[uvb+c*4: uvb+c*4+4]
            print('  Zelle[%d] u %d v %d dx %d dy %d' % (c, e[0], e[1], struct.unpack('b',e[2:3])[0], struct.unpack('b',e[3:4])[0]))
        # Skripttabelle: 8 u16 woff (sub&7)
        woffs = [u16(esp, tab+2*k) for k in range(8)]
        print('  Skript-woff', woffs)
        seen=set()
        for k,wo in enumerate(woffs):
            sk = tab + wo*4
            if wo < 4 or sk in seen or sk+4 > len(esp): continue
            seen.add(sk)
            nparts = u16(esp, sk)
            print('  Skript sub%d @0x%X nparts %d (kopf %s)' % (k, sk, nparts, esp[sk:sk+4].hex(' ')))
            p = sk+4
            for t in range(nparts):
                if p+4 > len(esp): break
                nst = u16(esp, p)
                print('   Teil %d @0x%X nsteps %d kopf %s' % (t, p, nst, esp[p:p+4].hex(' ')))
                for s in range(nst):
                    if p+4+s*24+24 > len(esp): break
                    st = esp[p+4+s*24: p+4+s*24+24]
                    print('    Step %d: OpA %d OpB %d s2 %d s3 %d asp %d/%d acc %d/%d/%d lb %d vel %d/%d/%d ph %d w14 0x%04X w16 0x%04X | %s' % (
                        s, st[0], st[1], st[2], st[3], u16(st,4), u16(st,6), *struct.unpack('bbb', st[8:11]), st[11],
                        s16(st,12), s16(st,14), s16(st,16), u16(st,18), u16(st,20), u16(st,22), st.hex(' ')))
                p += 4 + nst*24
    return ids
if __name__ == '__main__':
    room = sys.argv[1] if len(sys.argv) > 1 else 'ROOM7040'
    b, offs, esp = esp_aus_rdt(room)
    print(room, 'ESP = RDT[0x%X:0x%X]' % (offs[18], offs[19]+4), 'md5', hashlib.md5(esp).hexdigest())
    open(os.path.join(OUT, room+'_effect.esp'),'wb').write(esp)
    bank_dump(esp, room)
    for i,(t0, t, o) in tims(b, offs, esp).items():
        print('TIM Bank %02X: Tafel-Offset 0x%X -> RDT[0x%X:0x%X] %d B md5 %s' % (i, o, t0, t0+len(t), len(t), hashlib.md5(t).hexdigest()))
        csz = u32(t,8); print('  TIM-Kopf flags 0x%X clut x %d y %d w %d h %d; bild x %d y %d hw %d h %d' % (u32(t,4), u16(t,12),u16(t,14),u16(t,16),u16(t,18), u16(t,8+csz+4),u16(t,8+csz+6),u16(t,8+csz+8),u16(t,8+csz+10)))
        open(os.path.join(OUT, '%s_%02X.TIM' % (room, i)),'wb').write(t)
