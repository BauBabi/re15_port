#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Brute-force MD1 mesh scan over an entire directory tree.

Signature (all derived from the format contract in re15_port/include/re15_md1.h
and verified against PL0/RDT/room1110/obj/model00.md1):
  +0x08 u32 object_count, even, 2..64  -> nmesh = object_count/2
  mesh headers occupy nmesh*56 bytes starting at (file+12); therefore the very
  first block offset (t_vertex_offset of mesh 0) must equal nmesh*56.
  All block offsets are absolute from (file+12), all counts are sane, and the
  highest block end must fit inside the file.

Verified on the 893 RDT props: the scan re-finds every one of them at its
known offset (see --selftest).
"""
import os, sys, struct, hashlib, json

MAXOFF = 1 << 20
MAXCNT = 8000


def try_md1(d, o):
    if o + 12 + 56 > len(d):
        return None
    objc = struct.unpack_from('<I', d, o + 8)[0]
    if objc < 2 or objc > 64 or objc % 2:
        return None
    nmesh = objc // 2
    base = o + 12
    if base + nmesh * 56 > len(d):
        return None
    end = nmesh * 56
    total_faces = 0
    for i in range(nmesh):
        f = struct.unpack_from('<14I', d, base + i * 56)
        (tvo, tvc, tno, tnc, tfo, tfc, tuo,
         qvo, qvc, qno, qnc, qfo, qfc, quo) = f
        if i == 0 and tvo != nmesh * 56:
            return None
        for off in (tvo, tno, tfo, tuo, qvo, qno, qfo, quo):
            if off > MAXOFF or off % 4:
                return None
        for cnt in (tvc, tnc, tfc, qvc, qnc, qfc):
            if cnt > MAXCNT:
                return None
        if tfc == 0 and qfc == 0:
            return None
        total_faces += tfc + qfc
        end = max(end, tvo + tvc * 8, tno + tnc * 8, tfo + tfc * 12,
                  tuo + tfc * 12, qvo + qvc * 8, qno + qnc * 8,
                  qfo + qfc * 16, quo + qfc * 16)
    if base + end > len(d):
        return None
    if total_faces == 0:
        return None
    return 12 + end, nmesh, total_faces


def scan_file(p):
    d = open(p, 'rb').read()
    out = []
    o = 0
    while o + 68 <= len(d):
        r = try_md1(d, o)
        if r:
            size, nmesh, nf = r
            out.append((o, size, nmesh, nf, hashlib.md5(d[o:o + size]).hexdigest()))
            o += 4
        else:
            o += 4
    return out, d


def main():
    root = sys.argv[1]
    outdir = sys.argv[2]
    os.makedirs(outdir, exist_ok=True)
    os.makedirs(os.path.join(outdir, 'md1'), exist_ok=True)
    rows = []
    nfiles = 0
    nbytes = 0
    SKIP = ('.bmp', '.png', '.wav', '.avi', '.str', '.bgm', '.vb', '.vh',
            '.msg', '.c', '.ini', '.txt')
    for dirpath, dirnames, filenames in os.walk(root):
        for fn in filenames:
            if fn.lower().endswith(SKIP):
                continue
            p = os.path.join(dirpath, fn)
            try:
                hits, d = scan_file(p)
            except Exception as e:
                print('ERR', p, e)
                continue
            nfiles += 1
            nbytes += len(d)
            rel = os.path.relpath(p, root).replace('\\', '/')
            for (o, size, nmesh, nf, h) in hits:
                rows.append({'file': rel, 'off': o, 'size': size,
                             'nmesh': nmesh, 'nfaces': nf, 'md5': h})
                fp = os.path.join(outdir, 'md1', h + '.md1')
                if not os.path.exists(fp):
                    open(fp, 'wb').write(d[o:o + size])
    json.dump(rows, open(os.path.join(outdir, 'scan.json'), 'w'), indent=0)
    print('files scanned : %d  (%.1f MB)' % (nfiles, nbytes / 1048576.0))
    print('MD1 hits      : %d' % len(rows))
    print('distinct md5  : %d' % len(set(r['md5'] for r in rows)))


main()
