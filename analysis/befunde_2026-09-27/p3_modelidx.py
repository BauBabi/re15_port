import sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from elza_ed import rep

M = "re15_port/platform/pc/main.c"
rep(M, [
(
"""    s_player_md1_ref = &md1;
    re15_scd_set_player_model_sync(pc_player_model_sync_cb);""",
"""    s_player_md1_ref = &md1;
    re15_scd_set_player_model_sync(pc_player_model_sync_cb);
    /* ⛔ SITZUNGSSTART: DER ANGEFORDERTE PL-INDEX MUSS AUF DEN CHARAKTER STEHEN.
     * Das Original fuellt 0x800b0ff0 (= work_vars[0x10]) beim Einstieg aus dem
     * Charakter-Byte: `lbu a0,DAT_800aca5c` @0x8001d51c -> `sh a0,4080(at)` @0x8001d558.
     * Der Raumlader vergleicht danach die untere Nibble von aca5c gegen genau diesen
     * Wert und laedt bei Abweichung neu:
     *     8003976c  andi v0,a0,0xf
     *     80039770  beq  v0,v1,0x80039790     ; gleich -> nichts tun
     *     80039788  jal  0x800314b0           ; sonst Spielermodell NEU LADEN
     * Ohne den Gleichstand haette der erste Raumwechsel Elzas eben geladenes PL04 gegen
     * work_vars[0x10] == 0 = PL00 zurueckgetauscht. Der Westen-Spiegel wird mitgezogen,
     * damit re15_vest_hp_on_model_reload den Start nicht als Modellwechsel liest. */
    s_player_model_idx    = g_gameflow.character & 0x0F;
    g_scd.work_vars[0x10] = (int16_t)(g_gameflow.character & 0x0F);
    re15_vest_model_mark((int16_t)(g_gameflow.character & 0x0F));"""
),
])

S = "re15_port/engine/src/re15_savedata.c"
rep(S, [
(
"""    {
        int16_t vm = re15_game_flag_get(3, 0x75) ? 1 : 0;
        g_scd.work_vars[0x10] = vm;
        re15_vest_model_mark(vm);
    }""",
"""    {
        /* ⛔ Der Modell-Index ist die untere NIBBLE von DAT_800ACA5C (@0x8003976c
         * `andi v0,a0,0xf`), und die traegt den CHARAKTER — Leon 0, Elza 4
         * (@0x801024c0 / @0x801024d4). Die Weste ist Leons Variante PL01 (Datei-Id
         * 0x3C+1, Tabelle 0x80073f70). Fuer Elza gibt es im Port keine ermittelte
         * Westen-Variante, also bleibt ihr Index unveraendert stehen; ihn hier auf 1
         * zu setzen wuerde sie in Leons rote Uniform stecken. */
        int16_t vm = (g_gameflow.character & 4)
                       ? (int16_t)(g_gameflow.character & 0x0F)
                       : (re15_game_flag_get(3, 0x75) ? 1 : 0);
        g_scd.work_vars[0x10] = vm;
        re15_vest_model_mark(vm);
    }"""
),
])

H = "re15_port/include/re15_savedata.h"
rep(H, [
(
"""    uint8_t  character;        /* g_gameflow.character (RE1.5 DAT_800aca5c bit0)  */""",
"""    uint8_t  character;        /* g_gameflow.character = DAT_800ACA5C: 0 = Leon (PL00),
                                * 4 = Elza (PL04). Das Original speichert genau dieses
                                * Byte: `lbu v0,0x800aca5c` @0x80026f4c -> `sb v0,4030(at)`
                                * = 0x800b0fbe @0x80026f6c, Rueckweg `lbu` @0x8001d4c8 ->
                                * `sb` @0x8001d4f4. Bis Runde 34 trug das Feld 0/1; ein
                                * Altstand mit 1 laedt jetzt als Leon (1 & 4 == 0) — was
                                * er de facto immer war, denn Elza hatte bis dahin weder
                                * eigenes Modell noch eigene Raumkette. */"""
),
])
