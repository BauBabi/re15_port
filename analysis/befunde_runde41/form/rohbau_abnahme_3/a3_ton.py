# Blender: Tonbilder aus netz/clipN.obj, Workbench mit Tiefenpuffer.
# Aufruf: blender -b -P a3_ton.py -- clip "az,el;az,el" ausprefix [farbig] [nah:bone]
# az 0 = Kamera vor dem Kopf (+X Blender = +x PSX), az 90 = Kamera auf der linken Seite der Figur (+Y Blender = +z PSX)
import bpy, sys, math, os, json
from mathutils import Vector
a = sys.argv[sys.argv.index('--') + 1:]
clip = int(a[0]); blicke = [tuple(float(v) for v in t.split(',')) for t in a[1].split(';')]; aus = a[2]
farbig = 'farbig' in a[3:]
tag = ''.join('_' + t[4:] for t in a[3:] if t.startswith('tag:'))
HIER = os.path.dirname(os.path.abspath(sys.argv[sys.argv.index('-P') + 1]))
bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.wm.obj_import(filepath=os.path.join(HIER, ([t[5:] for t in a[3:] if t.startswith('netz:')] or ['netz'])[0], 'clip%d.obj' % clip), forward_axis='Y', up_axis='Z')
ob = [o for o in bpy.data.objects if o.type == 'MESH']
GR = {(0, 7, 10): (.80, .55, .50), (21, 22, 23): (.90, .80, .35), (1, 2, 3, 4, 5, 6): (.45, .78, .50),
      (16, 17, 18, 19, 20, 29, 31): (.82, .35, .82), (11, 12, 13, 14, 15, 28, 30): (.30, .70, .30),
      (8, 24, 25): (.95, .55, .20), (9, 26, 27): (.25, .70, .90)}
FARBE = {k: f for ks, f in GR.items() for k in ks}
for o in ob:
    b = int(o.name[1:3])
    for p in o.data.polygons:
        p.use_smooth = False
    m = bpy.data.materials.new('m%d' % b)
    f = FARBE.get(b, (.6, .6, .6)) if farbig else (.75, .73, .70)
    m.diffuse_color = (f[0], f[1], f[2], 1)
    o.data.materials.append(m)
s = bpy.context.scene
s.render.engine = 'BLENDER_WORKBENCH'
sh = s.display.shading
sh.light = 'STUDIO'
sh.color_type = 'MATERIAL'
sh.show_cavity = True
sh.cavity_type = 'WORLD'
sh.show_shadows = True
s.display.shadow_focus = 0.2
s.render.resolution_x = 640; s.render.resolution_y = 480
s.render.film_transparent = False
s.render.image_settings.file_format = 'PNG'
w = bpy.data.worlds.new('w'); s.world = w; w.color = (.18, .19, .21)
bpy.ops.mesh.primitive_plane_add(size=80, location=(0, 0, -0.003))
pl = bpy.context.active_object
mb = bpy.data.materials.new('boden'); mb.diffuse_color = (.30, .30, .32, 1); pl.data.materials.append(mb)
mn = [min((o.matrix_world @ v.co)[i] for o in ob for v in o.data.vertices) for i in range(3)]
mx = [max((o.matrix_world @ v.co)[i] for o in ob for v in o.data.vertices) for i in range(3)]
mitte = Vector([(mn[i] + mx[i]) / 2 for i in range(3)])
gross = max(mx[i] - mn[i] for i in range(3))
for t in a[3:]:
    if t.startswith('nah:'):
        ks, dd = t[4:].split(':')
        ks = [int(x) for x in ks.split(',')]
        pts = [o.matrix_world @ v.co for o in ob if int(o.name[1:3]) in ks for v in o.data.vertices]
        mitte = sum(pts, Vector()) / len(pts)
        gross = float(dd) / 1.55
cam = bpy.data.cameras.new('c'); cam.lens = 50
co = bpy.data.objects.new('c', cam); s.collection.objects.link(co); s.camera = co
for i, (az, el) in enumerate(blicke):
    d = gross * 1.55
    r = math.radians(az); e = math.radians(el)
    pos = mitte + Vector((math.cos(r) * math.cos(e), math.sin(r) * math.cos(e), math.sin(e))) * d
    co.location = pos
    rich = (mitte - pos).normalized()
    co.rotation_euler = rich.to_track_quat('-Z', 'Y').to_euler()
    s.render.filepath = '%s_c%d_az%03d_el%02d%s.png' % (aus, clip, int(az), int(el), tag)
    bpy.ops.render.render(write_still=True)
print('FERTIG', len(blicke))
