import struct, sys
sys.path.insert(0, r'C:\workspace\git\reAi_v2\.claude\worktrees\r41_annette\re15_port\tools\r34n_d')
import rbj_zensus as RZ
def re2_rbj(path, ptr=0x60):
    d = open(path,'rb').read()
    s = struct.unpack_from('<I', d, ptr)[0]
    # RZ.records liest 0x5C: fake-Kopf
    fake = bytearray(d); struct.pack_into('<I', fake, 0x5C, s)
    return d, s, RZ.records(bytes(fake))
if __name__ == '__main__':
    d, s, recs = re2_rbj(sys.argv[1])
    print('RBJ @0x%X' % s)
    for r in recs:
        if 'fehler' in r: print(r); continue
        print('rec', r['rec'], 'marker 0x%x' % r['marker'], 'bones', r['bones'], 'ksize', r['ksize'], 'kf', r['keyframes'],
              'clips', len(r['clips']), [c['bilder'] for c in r['clips']])
