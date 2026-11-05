# Ein Skript (Bank, Sub) einer ESP-Datei dumpen: alle Teile/Steps mit Op A/B (Spawner-Rechnung wie FUN_8001cbe8 @0x8001cbfc-cc38)
import struct, sys, os
def u16(b,o): return struct.unpack_from('<H',b,o)[0]
def u32(b,o): return struct.unpack_from('<I',b,o)[0]
def skript(path, bank, sub):
    esp = open(path,'rb').read()
    al=(len(esp)+3)&~3; ende=al-4
    for n in range(8):
        if esp[n]==0xff: break
        if esp[n]!=bank: continue
        off=u32(esp,ende-4*n); w=u32(esp,off); n1,n2=w&0xffff,w>>16
        tab=off+(n1*2+n2+2)*4
        wo=u16(esp,tab+(sub&7)*2); sk=tab+wo*4; np_=u16(esp,sk); p=sk+4
        print('%s Bank %02X Sub %d -> Skript @0x%X (woff %d), %d Teil(e), CLUT-Zeile +%d' % (os.path.basename(path), bank, sub, sk, wo, np_, sub>>3))
        ops=set()
        for t in range(np_):
            ns=u16(esp,p)
            for s in range(ns):
                st=esp[p+4+s*24:p+28+s*24]
                ops.add(st[0]); ops.add(st[1])
                print('  Teil %d Step %d: OpA %d OpB %d s2 %d s3 %d asp %d/%d acc %s lb %d | %s'%(t,s,st[0],st[1],st[2],st[3],u16(st,4),u16(st,6),struct.unpack('bbb',st[8:11]),st[11],st.hex(' ')))
            p+=4+ns*24
        # Anim-Eintraege, die das Skript erreichen kann, sind per Op gesetzt; hier nur Op-Menge
        print('  Ops:', sorted(ops))
        return sorted(ops)
if __name__=='__main__':
    skript(sys.argv[1], int(sys.argv[2],16), int(sys.argv[3]))
