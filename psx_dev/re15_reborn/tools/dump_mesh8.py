#!/usr/bin/env python3
"""Dump specific meshes from PL00.md1 to investigate mesh 8 identity."""
import struct
import sys

PATH = r"C:\workspace\git\reAi\psx_dev\re15_reborn\assets\test.md1"

with open(PATH, "rb") as f:
    data = f.read()

length, unknown, object_count = struct.unpack_from("<III", data, 0)
mesh_count = object_count // 2
print(f"File size: {len(data)}  length={length}  object_count={object_count}  mesh_count={mesh_count}")
print()

def parse_mesh(i):
    hdr = 12 + i * 56
    fields = struct.unpack_from("<14I", data, hdr)
    (tv_off, tv_cnt, tn_off, tn_cnt, tf_off, tf_cnt, tu_off,
     qv_off, qv_cnt, qn_off, qn_cnt, qf_off, qf_cnt, qu_off) = fields
    # Offsets are absolute from (file_start + 12)
    base = 12
    verts = []
    for k in range(tv_cnt):
        x, y, z, _ = struct.unpack_from("<hhhh", data, base + tv_off + k * 8)
        verts.append(("T", k, x, y, z))
    for k in range(qv_cnt):
        x, y, z, _ = struct.unpack_from("<hhhh", data, base + qv_off + k * 8)
        verts.append(("Q", k, x, y, z))
    return verts, (tv_cnt, qv_cnt, tf_cnt, qf_cnt)

def bbox(verts):
    if not verts:
        return None
    xs = [v[2] for v in verts]
    ys = [v[3] for v in verts]
    zs = [v[4] for v in verts]
    return (min(xs), max(xs), min(ys), max(ys), min(zs), max(zs))

def centroid(verts):
    if not verts:
        return (0, 0, 0)
    n = len(verts)
    return (sum(v[2] for v in verts)/n,
            sum(v[3] for v in verts)/n,
            sum(v[4] for v in verts)/n)

print("=== Mesh inventory ===")
print(f"{'i':>3} {'tV':>4} {'qV':>4} {'tF':>4} {'qF':>4}  {'xmin':>6} {'xmax':>6} {'ymin':>6} {'ymax':>6} {'zmin':>6} {'zmax':>6}  {'cx':>6} {'cy':>6} {'cz':>6}")
for i in range(mesh_count):
    verts, counts = parse_mesh(i)
    bb = bbox(verts)
    cx, cy, cz = centroid(verts)
    if bb is None:
        print(f"{i:>3} {counts[0]:>4} {counts[1]:>4} {counts[2]:>4} {counts[3]:>4}  (empty)")
    else:
        print(f"{i:>3} {counts[0]:>4} {counts[1]:>4} {counts[2]:>4} {counts[3]:>4}  "
              f"{bb[0]:>6} {bb[1]:>6} {bb[2]:>6} {bb[3]:>6} {bb[4]:>6} {bb[5]:>6}  "
              f"{cx:>6.0f} {cy:>6.0f} {cz:>6.0f}")

print()
print("=== Mesh 8 — full vertex dump ===")
verts8, _ = parse_mesh(8)
for entry in verts8:
    kind, k, x, y, z = entry
    print(f"  {kind}[{k:2}] ({x:>6}, {y:>6}, {z:>6})")

print()
print("=== Comparison: mesh 1 (torso?) vs mesh 4 (hand?) vs mesh 8 ===")
for i in (0, 1, 4, 8, 15, 16):
    if i >= mesh_count:
        continue
    verts, _ = parse_mesh(i)
    bb = bbox(verts)
    cx, cy, cz = centroid(verts)
    if bb:
        size_x = bb[1] - bb[0]
        size_y = bb[3] - bb[2]
        size_z = bb[5] - bb[4]
        print(f"  mesh {i:2}: nVerts={len(verts):3}  size=({size_x:>5},{size_y:>5},{size_z:>5})  centroid=({cx:>6.0f},{cy:>6.0f},{cz:>6.0f})")
