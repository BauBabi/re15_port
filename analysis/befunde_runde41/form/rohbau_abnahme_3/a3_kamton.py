# Blender: Ton aus der Screenshot-Kamera. Aufruf: blender -b -P a3_kamton.py -- bildnr aus.png [farbig]
import bpy, sys, math, os, json
from mathutils import Vector
a = sys.argv[sys.argv.index('--') + 1:]
n = int(a[0]); aus = a[1]; farbig = 'farbig' in a[2:]
HIER = os.path.dirname(os.path.abspath(sys.argv[sys.argv.index('-P') + 1])); FORM = os.path.dirname(HIER)
kam = json.load(open(os.path.join(FORM, 'kamera_bild%d.json' % n)))
fj = json.load(open(os.path.join(FORM, 'rohbau_final', 'form.json')))
st = fj['bild%d' % n]; clip = st['clip']; lage = st['lage']; th = st['gier'] * 2 * math.pi / 4096
c, s = math.cos(th), math.sin(th)
def modell(p):
    dx, dy, dz = p[0] - lage[0], p[1] - lage[1], p[2] - lage[2]
    x = c * dx - s * dz; z = s * dx + c * dz
    return Vector((x / 1000, z / 1000, -dy / 1000))
bpy.ops.wm.read_factory_settings(use_empty=True)
bpy.ops.wm.obj_import(filepath=os.path.join(HIER, 'netz', 'clip%d.obj' % clip), forward_axis='Y', up_axis='Z')
ob = [o for o in bpy.data.objects if o.type == 'MESH']
GR = {(0, 7, 10): (.80, .55, .50), (21, 22, 23): (.90, .80, .35), (1, 2, 3, 4, 5, 6): (.45, .78, .50),
      (16, 17, 18, 19, 20, 29, 31): (.82, .35, .82), (11, 12, 13, 14, 15, 28, 30): (.30, .70, .30),
      (8, 24, 25): (.95, .55, .20), (9, 26, 27): (.25, .70, .90)}
FARBE = {k: f for ks, f in GR.items() for k in ks}
for o in ob:
    b = int(o.name[1:3]); m = bpy.data.materials.new('m%d' % b)
    f = FARBE.get(b, (.6, .6, .6)) if farbig else (.75, .73, .70)
    m.diffuse_color = (f[0], f[1], f[2], 1); o.data.materials.append(m)
sc = bpy.context.scene
sc.render.engine = 'BLENDER_WORKBENCH'
sh = sc.display.shading; sh.light = 'STUDIO'; sh.color_type = 'MATERIAL'; sh.show_cavity = True; sh.cavity_type = 'WORLD'
w = bpy.data.worlds.new('w'); sc.world = w; w.color = (.10, .10, .11)
sc.render.resolution_x = 640; sc.render.resolution_y = 480; sc.render.film_transparent = True
cam = bpy.data.cameras.new('c'); cam.sensor_fit = 'HORIZONTAL'; cam.angle = 2 * math.atan(160.0 / kam['H'])
co = bpy.data.objects.new('c', cam); sc.collection.objects.link(co); sc.camera = co
P = modell(kam['auge']); Z = modell(kam['ziel'])
co.location = P; co.rotation_euler = (Z - P).normalized().to_track_quat('-Z', 'Y').to_euler()
sc.render.filepath = aus
bpy.ops.render.render(write_still=True)
print('FERTIG', P, Z)
