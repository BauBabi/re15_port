# Kandidat B: Blender-Schritt. Aufruf:
#   blender.exe -b -P b_blender.py -- EIN_ORDNER AUS_ORDNER budget.json
# Je Teil: OBJ laden, Voxel-Remesh, QuadriFlow auf Flaechenziel (mehrere Saaten, Pruefung gegen die Ausdehnung des Eingangs,
# Rueckfall Decimate + Tris-zu-Quads), Smart UV, OBJ schreiben.
import bpy, json, math, os, sys

argv = sys.argv[sys.argv.index('--') + 1:]
EIN, AUS, BUDGET = argv[0], argv[1], json.load(open(argv[2]))
os.makedirs(AUS, exist_ok=True)


def leeren():
    if bpy.context.mode != 'OBJECT':
        bpy.ops.object.mode_set(mode='OBJECT')
    bpy.ops.object.select_all(action='SELECT')
    bpy.ops.object.delete()
    for m in list(bpy.data.meshes):
        bpy.data.meshes.remove(m)


def aktiv(o):
    bpy.ops.object.select_all(action='DESELECT')
    o.select_set(True)
    bpy.context.view_layer.objects.active = o


def ausdehnung(o):
    v = [p.co for p in o.data.vertices]
    return [max(c[i] for c in v) - min(c[i] for c in v) for i in range(3)]


def bearbeite(name, ziel):
    leeren()
    bpy.ops.wm.obj_import(filepath=os.path.join(EIN, name + '.obj'), forward_axis='Y', up_axis='Z')
    o = bpy.context.selected_objects[0]
    aktiv(o)
    m = o.modifiers.new('r', 'REMESH')
    m.mode = 'VOXEL'
    d0 = ausdehnung(o)
    m.voxel_size = max(9.0, min(30.0, math.sqrt(2.2 * (d0[0] * d0[1] + d0[1] * d0[2] + d0[0] * d0[2]) / ziel) / 5.0))
    m.use_smooth_shade = True
    bpy.ops.object.modifier_apply(modifier='r')
    soll = ausdehnung(o)
    print('Voxel', name, m_vox if False else round(max(9.0, min(30.0, math.sqrt(2.2 * (d0[0] * d0[1] + d0[1] * d0[2] + d0[0] * d0[2]) / ziel) / 5.0)), 1), len(o.data.polygons))
    gut = None
    for saat in (7, 3, 11, 19, 23, 31, 42, 57, 64, 71, 83, 97):
        k = o.copy()
        k.data = o.data.copy()
        bpy.context.collection.objects.link(k)
        aktiv(k)
        try:
            r = bpy.ops.object.quadriflow_remesh(use_mesh_symmetry=False, use_preserve_sharp=False, use_preserve_boundary=False,
                                                 preserve_attributes=False, smooth_normals=False, mode='FACES', target_faces=int(ziel), seed=saat)
            q = bpy.context.view_layer.objects.active
            n = len(q.data.polygons)
            ok = 'FINISHED' in r and n > ziel * 0.6 and n < ziel * 1.25
            if ok:
                ist = ausdehnung(q)
                ok = all(a >= b * 0.965 for a, b in zip(ist, soll)) and len({tuple(round(c, 1) for c in p.co) for p in q.data.vertices}) > 0.97 * len(q.data.vertices)
            if ok:
                gut = q
                print('Saat', name, saat, n)
                break
            print('verworfen', name, saat, n)
        except Exception as e:
            print('QuadriFlow Fehler', name, saat, str(e)[:60])
        bpy.data.objects.remove(bpy.context.view_layer.objects.active, do_unlink=True)
    if gut is None:
        print('Rueckfall Decimate', name)
        aktiv(o)
        m = o.modifiers.new('d', 'DECIMATE')
        m.decimate_type = 'COLLAPSE'
        dreiecke = sum(len(p.vertices) - 2 for p in o.data.polygons)
        m.ratio = min(1.0, 1.55 * ziel / max(dreiecke, 1))
        bpy.ops.object.modifier_apply(modifier='d')
        bpy.ops.object.mode_set(mode='EDIT')
        bpy.ops.mesh.select_all(action='SELECT')
        bpy.ops.mesh.tris_convert_to_quads()
        bpy.ops.object.mode_set(mode='OBJECT')
        gut = o
    else:
        bpy.data.objects.remove(o, do_unlink=True)
    aktiv(gut)
    bpy.ops.object.mode_set(mode='EDIT')
    bpy.ops.mesh.select_all(action='SELECT')
    bpy.ops.uv.smart_project(angle_limit=math.radians(70), island_margin=0.0, area_weight=0.0, correct_aspect=False, scale_to_bounds=False)
    bpy.ops.object.mode_set(mode='OBJECT')
    print('FLAECHEN', name, len(gut.data.polygons), sum(len(p.vertices) == 4 for p in gut.data.polygons))
    bpy.ops.wm.obj_export(filepath=os.path.join(AUS, name + '.obj'), export_selected_objects=True, forward_axis='Y', up_axis='Z',
                          export_normals=False, export_uv=True, export_materials=False, export_triangulated_mesh=False)


for n in sorted(BUDGET):
    if os.path.exists(os.path.join(EIN, n + '.obj')):
        bearbeite(n, BUDGET[n])
