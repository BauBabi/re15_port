# Blender: Tonbilder aus clip*.obj. Aufruf: blender -b -P a2_ton.py -- clip  az,el;az,el  ausdir  [farbig] [ziel:x,y,z,scale]
import bpy, sys, math, os
a = sys.argv[sys.argv.index('--') + 1:]
clip = int(a[0]); blicke = [tuple(float(v) for v in t.split(',')) for t in a[1].split(';')]; aus = a[2]
farbig = 'farbig' in a[3:]
ziel = None
for t in a[3:]:
    if t.startswith('ziel:'):
        ziel = [float(v) for v in t[5:].split(',')]
tag = 'x'
for t in a[3:]:
    if t.startswith('tag:'):
        tag = t[4:]
HIER = os.path.dirname(os.path.abspath(sys.argv[sys.argv.index('-P') + 1]))
bpy.ops.wm.read_factory_settings(use_empty=True)
pref = [t[4:] for t in a[3:] if t.startswith('obj:')]
bpy.ops.wm.obj_import(filepath=os.path.join(HIER, 'mess', (pref[0] if pref else 'clip') + '%d.obj' % clip), forward_axis='Y', up_axis='Z')
ob = [o for o in bpy.data.objects if o.type == 'MESH']
for o in ob:
    for p in o.data.polygons:
        p.use_smooth = True
FARBE = {}
GR = {'rumpf': ((0, 7, 10), (.78, .55, .50)), 'hals': ((21, 22, 23), (.85, .75, .35)), 'schwanz': ((1, 2, 3, 4, 5, 6), (.45, .75, .50)),
      'armr': ((16, 17, 18, 19, 20, 29, 31), (.80, .35, .80)), 'arml': ((11, 12, 13, 14, 15, 28, 30), (.35, .75, .35)),
      'beinl': ((8, 24, 25), (.90, .55, .20)), 'beinr': ((9, 26, 27), (.25, .70, .85))}
for n, (ks, f) in GR.items():
    for k in ks:
        FARBE[k] = f
for o in ob:
    b = int(o.name[1:3])
    m = bpy.data.materials.new('m%d' % b)
    m.use_nodes = True
    bs = m.node_tree.nodes.get('Principled BSDF')
    f = FARBE.get(b, (.6, .6, .6)) if farbig else (.72, .70, .66)
    bs.inputs['Base Color'].default_value = (f[0], f[1], f[2], 1)
    bs.inputs['Roughness'].default_value = .62
    o.data.materials.append(m)
s = bpy.context.scene
s.render.engine = 'CYCLES'
s.cycles.samples = 24
s.cycles.use_denoising = False
s.cycles.device = 'CPU'
s.render.resolution_x = s.render.resolution_y = 560
s.render.image_settings.file_format = 'PNG'
w = bpy.data.worlds.new('w'); s.world = w; w.use_nodes = True
w.node_tree.nodes['Background'].inputs[0].default_value = (.30, .32, .36, 1)
w.node_tree.nodes['Background'].inputs[1].default_value = .55
ohne = 'ohneboden' in a[3:]
bpy.ops.mesh.primitive_plane_add(size=60, location=(0, 0, -.002 if not ohne else -60))
pl = bpy.context.active_object
ma = bpy.data.materials.new('b'); ma.use_nodes = True
ma.node_tree.nodes['Principled BSDF'].inputs['Base Color'].default_value = (.34, .34, .36, 1)
pl.data.materials.append(ma)
sun = bpy.data.lights.new('s', 'SUN'); sun.energy = 3.0; sun.angle = math.radians(14)
so = bpy.data.objects.new('s', sun); s.collection.objects.link(so)
cam = bpy.data.cameras.new('c'); cam.type = 'ORTHO'
co = bpy.data.objects.new('c', cam); s.collection.objects.link(co); s.camera = co
mn = [min(v.co[i] for o in ob for v in o.data.vertices) for i in range(3)]
mx = [max(v.co[i] for o in ob for v in o.data.vertices) for i in range(3)]
mitte = [(mn[i] + mx[i]) / 2 for i in range(3)]
sk = max(mx[i] - mn[i] for i in range(3)) * 1.02
if ziel:
    mitte = ziel[:3]; sk = ziel[3]
cam.ortho_scale = sk
for az, el in blicke:
    ra, re = math.radians(az), math.radians(el)
    d = (math.cos(re) * math.cos(ra), math.cos(re) * math.sin(ra), math.sin(re))
    D = 30
    co.location = (mitte[0] + D * d[0], mitte[1] + D * d[1], mitte[2] + D * d[2])
    co.rotation_mode = 'XYZ'
    co.rotation_euler = (math.pi / 2 - re, 0, ra + math.pi / 2)
    # Licht schraeg ueber der linken Schulter des Betrachters
    so.rotation_euler = (math.radians(50), 0, ra + math.pi / 2 - math.radians(35))
    s.render.filepath = os.path.join(aus, '%s_c%d_az%03d_el%02d.png' % (tag, clip, int(az), int(el)))
    bpy.ops.render.render(write_still=True)
print('TON-FERTIG')
