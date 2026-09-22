# -*- coding: utf-8 -*-
"""rueckbau.py <wurzel> <name>  — genau EINEN Rueckbau anwenden (ein Variablenwechsel).

Jeder Rueckbau ist ein exakter Textersatz in genau einer Datei. Er wird angewendet,
gebaut/gemessen und danach mit `git checkout --` zurueckgenommen.
"""
import io, os, sys

SRC = "re15_port/engine/src/item_discard_common.c"
TST = "re15_port/tests/unit/r21_discard_wegwerfen.c"

R = {}

# ---------------------------------------------------------------- R-A: fail-closed AUS
R["A"] = (SRC, """    if (!s_vor.gilt || s_vor.room != room_id || s_vor.msg != msg_id) { s_vor.gilt = 0; return; }
    uint8_t item = s_vor.item;
    int     slot = s_vor.slot;
    s_vor.gilt = 0;
""", """    uint8_t item = 0;          /* RUECKBAU R-A: Kopplung AUS, Stelle selbst nachsehen */
    for (int ri = 0; ri < RE15_DISCARD_SITE_COUNT; ri++)
        if (re15_discard_sites[ri].room == (uint16_t)room_id
            && re15_discard_sites[ri].msg == msg_id) { item = re15_discard_sites[ri].item; break; }
    if (!item) { s_vor.gilt = 0; return; }
    int slot = re15_inv_find_item(item);
    s_vor.gilt = 0;
""")

# ------------------------------------- R-B1: die erzwungene Aufnahme im 2. M-Lauf weg
R["B1"] = (TST, """            scharf = 1; g_re15_voice_laeuft = 1; g_re15_voice_restbilder = 600;""",
                """            scharf = 1; g_re15_voice_laeuft = mit_besitz; g_re15_voice_restbilder = mit_besitz ? 600 : 0;  /* R-B1 */""")

# ------------------------------------------- R-B2: das Nachhall-Ende beim Aufgehen weg
R["B2"] = (SRC, """        re15_msg_nachhall_beenden();
        s_reveal       = 0;""",
                """        /* RUECKBAU R-B2: re15_msg_nachhall_beenden() weg */
        s_reveal       = 0;""")

# --------------------------------------------------- R-C: restore belebt wieder (R.23)
R["C"] = (SRC, """void re15_discard_restore(uint8_t item)
{
    (void)item;
    re15_discard_reset();
}""", """void re15_discard_restore(uint8_t item)
{
    re15_discard_reset();                       /* RUECKBAU R-C: Runde-23-Form */
    if (item) {
        int slot = re15_inv_find_item(item);
        if (slot >= 0 && slot < RE15_INV_MAX_SLOTS && g_inv.slots[slot].qty > 0) {
            s_item = item; s_slot = slot; s_choice = 0; s_zustand = D_WARTET;
        }
    }
}""")

# --------------------- R-D1: der Pruefstand erreicht in TEIL L nur noch EINE Stelle
R["D1"] = (TST, """    int erreicht = 0, abfrage = 0;
    for (long fr = 0; fr < 6000; fr++) {
        const unsigned char *r; int l, id;
        g_scd_pad_edge = (fr > 2 && (fr % 4) == 0) ? 0x4000u : 0u;
        g_scd_pad_held = 0;
        if (!re15_discard_frozen()) scd_vm_tick();
        re15_msg_tick(&r, &l, &id);
        g_scd_pad_edge = 0;
        if (re15_pauseflags_belegt() && g_scd.message_id == msg) erreicht = 1;
        if (re15_discard_active()) { abfrage = 1; re15_discard_tick(0, 0); }
        if (re15_discard_prompt(NULL, NULL)) break;
    }
    scd_register_current_rdt(NULL);
    if (out_erreicht) *out_erreicht = erreicht;
    return abfrage;""",
                """    int erreicht = 0, abfrage = 0;
    long kappe = (room == 0x4000) ? 6000 : 3;      /* RUECKBAU R-D1 */
    for (long fr = 0; fr < kappe; fr++) {
        const unsigned char *r; int l, id;
        g_scd_pad_edge = (fr > 2 && (fr % 4) == 0) ? 0x4000u : 0u;
        g_scd_pad_held = 0;
        if (!re15_discard_frozen()) scd_vm_tick();
        re15_msg_tick(&r, &l, &id);
        g_scd_pad_edge = 0;
        if (re15_pauseflags_belegt() && g_scd.message_id == msg) erreicht = 1;
        if (re15_discard_active()) { abfrage = 1; re15_discard_tick(0, 0); }
        if (re15_discard_prompt(NULL, NULL)) break;
    }
    scd_register_current_rdt(NULL);
    if (out_erreicht) *out_erreicht = erreicht;
    return abfrage;""")

# ------------- R-D2: der OHNE-Besitz-Lauf von TEIL N erreicht die Stelle nicht mehr
R["D2"] = (TST, """        int erreicht = 0;
        for (long fr = 0; fr < 6000; fr++) {
            const unsigned char *r; int l, id;
            g_scd_pad_edge = (fr > 2 && (fr % 4) == 0) ? 0x4000u : 0u;""",
                """        int erreicht = 0;
        for (long fr = 0; fr < (mit ? 6000 : 3); fr++) {   /* RUECKBAU R-D2 */
            const unsigned char *r; int l, id;
            g_scd_pad_edge = (fr > 2 && (fr % 4) == 0) ? 0x4000u : 0u;""")

# --------------------------- R-E: der JA-Zweig ohne anzahl_zurueck (Stand vor R.25)
R["E"] = (SRC, """        /* ⛔ DER PLATZ TRAEGT DEN GEGENSTAND NICHT MEHR — also wird NICHTS weggeworfen,""",
               """        s_zustand = D_AUS; s_item = 0; s_slot = -1;      /* RUECKBAU R-E */
        s_reveal = s_reveal_total = s_reveal_timer = 0; s_blink = 0;
        return;
        /* ⛔ DER PLATZ TRAEGT DEN GEGENSTAND NICHT MEHR — also wird NICHTS weggeworfen,""")

# ------------- R-F: die Abdeckungs-Schranken selbst weg (zeigt: sie sind der Riegel)
R["F"] = (TST, """    PRUEFE(gefahren >= 10,
           "ABDECKUNG: nur %d von %d Stellen erreicht (erwartet >= 10) — dann messen die"
           " Nullen darunter nichts", gefahren, RE15_DISCARD_SITE_COUNT);""",
               """    /* RUECKBAU R-F: Schranke weg */""")

R["F2"] = (TST, """            PRUEFE(erreicht, "ROOM%04X: die Benutzungsstelle wurde im OHNE-Lauf gar nicht"
                   " erreicht — dann prueft (a) nichts", room);""",
                """            /* RUECKBAU R-F2: Schranke weg */""")

R["F3"] = (TST, """    PRUEFE(nh2 > 0,
           "GEGENPROBE: der Aufbau erzeugt GAR KEINEN Nachhall (%ld Bilder im Lauf ohne"
           " Abfrage) — dann ist die 0 aus Lauf 1 wertlos", nh2);""",
                """    /* RUECKBAU R-F3: Schranke weg */""")


# ----- R-G: re15_discard_frozen() AUS (= Rueckbau R4 der Vorrunde, hier nachgemessen,
#            damit belegt ist, dass die ALTEN Riegel nach den Aenderungen weiter beissen)
R["G"] = (SRC, """    if (s_zustand == D_FRAGT) return 1;""",
               """    if (s_zustand == D_FRAGT) return 0;   /* RUECKBAU R-G */
    return 0;""")


def anwenden(wurzel, name):
    datei, alt, neu = R[name]
    p = os.path.join(wurzel, datei)
    src = io.open(p, encoding="utf-8").read()
    n = src.count(alt)
    if n != 1:
        print("MARKER %dx gefunden — Rueckbau %s NICHT angewendet" % (n, name))
        sys.exit(2)
    io.open(p, "w", encoding="utf-8", newline="").write(src.replace(alt, neu))
    print("RUECKBAU %s angewendet in %s" % (name, datei))


def main():
    # "A" = ein Rueckbau, "D1+F" = zwei zusammen (fuer die Frage "faengt die Schranke
    # den Defekt, oder faengt ihn schon eine andere Pruefung?").
    wurzel, namen = sys.argv[1], sys.argv[2]
    for name in namen.split("+"):
        anwenden(wurzel, name)


if __name__ == "__main__":
    main()
