# -*- coding: utf-8 -*-
"""Runde 30 / irons-diary-welt: MESS-RIG.

Patcht eine KOPIE der Port-Quellen (build/r30_irons-diary-welt/rig/), NIE das Repo.
Zweck: die beiden neuen Props im ECHTEN Renderer messen (Masken, Beleuchtung,
Zeichenreihenfolge) - der Python-Abzug kennt die Vordergrundmasken nicht.

Alles ist env-gesteuert, damit EIN Rig-Binary alle Varianten faehrt:
   RIG_DIARY="x,y,z,ry"   RIG_DIARY_MD1=<datei> RIG_DIARY_TIM=<datei>     -> obj_id 5
   RIG_KARTE="x,y,z,ry"   RIG_KARTE_MD1=<datei> RIG_KARTE_TIM=<datei>     -> obj_id 6
   RIG_OTMAX=<bucket>     Tiefen-Klemme fuer obj_id 5/6 (0/unset = aus)

    python r30_idw_rig_patch.py <rig-verzeichnis>
"""
import io, os, sys

RIG = sys.argv[1]


def lesen(p):
    return io.open(os.path.join(RIG, p), encoding='utf-8', errors='surrogateescape', newline='').read()


def schreiben(p, s):
    io.open(os.path.join(RIG, p), 'w', encoding='utf-8', errors='surrogateescape', newline='').write(s)


def ersetze(p, alt, neu, anzahl=1):
    s = lesen(p)
    nl = '\r\n' if '\r\n' in s else '\n'
    alt = alt.replace('\n', nl); neu = neu.replace('\n', nl)
    n = s.count(alt)
    assert n == anzahl, '%s: Anker %d-mal statt %d-mal gefunden: %r' % (p, n, anzahl, alt[:70])
    schreiben(p, s.replace(alt, neu))
    print('gepatcht: %s (%d Stelle(n))' % (p, n))


# (1) neues Engine-Modul: legt die zwei Props an
schreiben('engine/src/r30_rig.c', r'''/* MESS-RIG Runde 30 (irons-diary-welt) - NICHT Teil des Ports. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "re15_scd.h"

void r30_rig_install(unsigned room_id)
{
    if (room_id != 0x1150 && room_id != 0x1151) return;
    static const char *namen[2] = { "RIG_DIARY", "RIG_KARTE" };
    for (int k = 0; k < 2; k++) {
        const char *e = getenv(namen[k]);
        int x, y, z, ry;
        if (!e || sscanf(e, "%d,%d,%d,%d", &x, &y, &z, &ry) != 4) continue;
        if (g_scd.prop_count >= RE15_SCD_MAX_PROPS) return;
        int i = (int)g_scd.prop_count++;
        memset(&g_scd.props[i], 0, sizeof g_scd.props[i]);
        g_scd.props[i].active     = 1;
        g_scd.props[i].obj_id     = (unsigned char)(5 + k);
        g_scd.props[i].obj_type   = 0;
        g_scd.props[i].band       = 1;
        g_scd.props[i].parent_obj = -1;
        g_scd.props[i].x = x; g_scd.props[i].y = y; g_scd.props[i].z = z;
        g_scd.props[i].rot_y = (short)ry;
        g_scd.props[i].flags = 0x0001;
        fprintf(stderr, "[rig] %s obj_id=%d slot=%d pos=(%d,%d,%d) ry=%d\n",
                namen[k], 5 + k, i, x, y, z, ry);
    }
}
''')
print('neu: engine/src/r30_rig.c')

# (2) Aufruf nach dem Init-Lauf, direkt hinter der Sicherung
ersetze('engine/src/scd_room_setup.c',
        '    re15_sicherung_install((uint16_t)g_current_room_id);\n',
        '    re15_sicherung_install((uint16_t)g_current_room_id);\n'
        '    { extern void r30_rig_install(unsigned room_id);\n'
        '      r30_rig_install((unsigned)g_current_room_id); }   /* MESS-RIG */\n')

# (3) Modelle + Texturen aus Dateien in die Prop-Slots 5/6
ersetze('platform/pc/main.c',
        '                          &tt, RE15_TIM_SLOT_PROP(RE15_SICHERUNG_OBJ_ID)); }\n    }\n',
        '                          &tt, RE15_TIM_SLOT_PROP(RE15_SICHERUNG_OBJ_ID)); }\n    }\n'
        '    /* MESS-RIG: obj 5/6 aus Dateien */\n'
        '    if (g_current_room_id == 0x1150 || g_current_room_id == 0x1151) {\n'
        '        static const char *mn[2] = { "RIG_DIARY_MD1", "RIG_KARTE_MD1" };\n'
        '        static const char *tn[2] = { "RIG_DIARY_TIM", "RIG_KARTE_TIM" };\n'
        '        static uint8_t *mbuf[2], *tbuf[2];\n'
        '        static long     mlen[2],  tlen[2];\n'
        '        for (int k = 0; k < 2; k++) {\n'
        '            const char *mp = getenv(mn[k]), *tp = getenv(tn[k]);\n'
        '            if (!mp || !tp) continue;\n'
        '            if (!mbuf[k]) { FILE *f = fopen(mp, "rb");\n'
        '                if (f) { fseek(f, 0, SEEK_END); mlen[k] = ftell(f); fseek(f, 0, SEEK_SET);\n'
        '                         mbuf[k] = (uint8_t *)malloc((size_t)mlen[k] + 4);\n'
        '                         if (fread(mbuf[k], 1, (size_t)mlen[k], f) != (size_t)mlen[k]) mlen[k] = 0;\n'
        '                         fclose(f); } }\n'
        '            if (!tbuf[k]) { FILE *f = fopen(tp, "rb");\n'
        '                if (f) { fseek(f, 0, SEEK_END); tlen[k] = ftell(f); fseek(f, 0, SEEK_SET);\n'
        '                         tbuf[k] = (uint8_t *)malloc((size_t)tlen[k] + 4);\n'
        '                         if (fread(tbuf[k], 1, (size_t)tlen[k], f) != (size_t)tlen[k]) tlen[k] = 0;\n'
        '                         fclose(f); } }\n'
        '            int oid = 5 + k;\n'
        '            if (mbuf[k] && mlen[k] > 0 && re15_md1_parse(mbuf[k], (size_t)mlen[k], &md1[oid]) == 0)\n'
        '                ok[oid] = 1;\n'
        '            if (tbuf[k] && tlen[k] > 0) { re15_tim_t tt; if (re15_tim_parse(tbuf[k], (int)tlen[k], &tt) == 0)\n'
        '                re15_render_pc_upload_tim_slot(&tt, RE15_TIM_SLOT_PROP(oid)); }\n'
        '            fprintf(stderr, "[rig] obj %d: MD1 %ld B ok=%d, TIM %ld B -> Slot %d\\n",\n'
        '                    oid, mlen[k], ok[oid], tlen[k], RE15_TIM_SLOT_PROP(oid));\n'
        '        }\n'
        '    }\n')

# (4) Tiefen-Klemme im Prop-Zeichner: Dreiecke und Vierecke
ersetze('platform/pc/main.c',
        '                            wz_avg /= 3;\n                            int wz_for_sort = wz_avg;\n'
        '                            const re15_md1_tri_uv_t *uv = &hm->triangle_uvs[ti];\n',
        '                            wz_avg /= 3;\n                            int wz_for_sort = wz_avg;\n'
        '                            { static int rig_ot = -1; if (rig_ot < 0) { const char *e = getenv("RIG_OTMAX"); rig_ot = e ? atoi(e) : 0; }\n'
        '                              if (rig_ot > 0 && (oid == 5 || oid == 6)) { int k = (int)re15_pri_mask_camera_z(rig_ot) - 1;\n'
        '                                  if (wz_for_sort > k) wz_for_sort = k; } }\n'
        '                            const re15_md1_tri_uv_t *uv = &hm->triangle_uvs[ti];\n')
ersetze('platform/pc/main.c',
        '                            wz_avg /= 4;\n                            const re15_md1_quad_uv_t *uv = &hm->quad_uvs[qi];\n',
        '                            wz_avg /= 4;\n'
        '                            { static int rig_ot = -1; if (rig_ot < 0) { const char *e = getenv("RIG_OTMAX"); rig_ot = e ? atoi(e) : 0; }\n'
        '                              if (rig_ot > 0 && (oid == 5 || oid == 6)) { int k = (int)re15_pri_mask_camera_z(rig_ot) - 1;\n'
        '                                  if (wz_avg > k) wz_avg = k; } }\n'
        '                            const re15_md1_quad_uv_t *uv = &hm->quad_uvs[qi];\n')
print('fertig')
