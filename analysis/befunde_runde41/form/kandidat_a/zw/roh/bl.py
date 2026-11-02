
import bpy, bmesh, json, sys, os, math
args = sys.argv[sys.argv.index('--') + 1:]
ordner, ziel_json = args[0], args[1]
ziele = json.load(open(ziel_json))
bpy.ops.wm.read_factory_settings(use_empty=True)
aus = {}
for name, ziel in ziele.items():
    pfad = os.path.join(ordner, name + '.obj')
    bpy.ops.wm.obj_import(filepath=pfad, forward_axis='Y', up_axis='Z')
    ob = bpy.context.selected_objects[0]
    bpy.context.view_layer.objects.active = ob
    me = ob.data
    n0 = len(me.polygons)
    m = ob.modifiers.new('d', 'DECIMATE')
    m.decimate_type = 'COLLAPSE'
    m.ratio = min(1.0, ziel / max(n0, 1))
    bpy.ops.object.modifier_apply(modifier=m.name)
    bm = bmesh.new()
    bm.from_mesh(me)
    bmesh.ops.join_triangles(bm, faces=bm.faces, angle_face_threshold=math.radians(30), angle_shape_threshold=math.radians(40), cmp_uvs=False, cmp_vcols=False, cmp_seam=False, cmp_sharp=False)
    bm.to_mesh(me)
    bm.free()
    bpy.ops.object.mode_set(mode='EDIT')
    bpy.ops.mesh.select_all(action='SELECT')
    bpy.ops.uv.smart_project(angle_limit=math.radians(70), island_margin=0.0, area_weight=0.0, correct_aspect=False, scale_to_bounds=False)
    bpy.ops.object.mode_set(mode='OBJECT')
    uvl = me.uv_layers.active.data
    V = [list(v.co) for v in me.vertices]
    F = []
    for p in me.polygons:
        F.append(dict(v=list(p.vertices), uv=[list(uvl[i].uv) for i in p.loop_indices]))
    aus[name] = dict(v=V, f=F)
    print('TEIL', name, n0, '->', len(me.polygons), flush=True)
    bpy.ops.object.delete()
json.dump(aus, open(os.path.join(ordner, 'netze.json'), 'w'))
