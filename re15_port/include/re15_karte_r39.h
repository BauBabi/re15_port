/* =============================================================================
 * Runde 39 Spur G "karten" — Korrekturen der Kartenblaetter nach den Nutzer-Vorlagen
 * =============================================================================
 * Nutzer 2026-10-17 (analysis/befunde_runde39/AUFTRAG.md): "map3F_correct.png,
 * map2F_correct.png, map1F_correct.png, mapB2_correct.png. Dort ist immer links, wie es
 * bei uns aussieht und rechts wie es eigentlich aussehen sollte. Keine Doppelwaende, Tuer
 * immer genau auf Wand platziert, treppen nicht in der Wand platziert usw."
 *
 * NUTZER-VORGABE (Form belegt): die rechten Haelften der Vorlagen, auf 320x240
 * zurueckgerechnet (analysis/befunde_runde39/G_karten_bilder/soll_*.png). Die linke Haelfte
 * jeder Vorlage ist pixelgleich unser Stand 1038dc15 (Dossier G_karten.md M1).
 *
 * RE1.5 selbst zeichnet jedes Rechteck einer Seite unveraendert aus MAP0x.PIX (Schleife
 * FUN_80046fd8 @0x800472fc-0x800473e0, Zeichner FUN_800473f8 @0x80047608) - die Doppelwaende
 * entstehen dort genauso, wo zwei Kacheln Wand an Wand liegen. Die Korrektur ist eine
 * PORT-ERGAENZUNG auf Nutzerwunsch: einzelne Texel eines Rechtecks werden beim Zeichnen
 * durch Raumkoerper, Wandlinie oder "durchsichtig" ersetzt; die Kachelkunst selbst und die
 * Seiten-Tabelle @0x80076840 bleiben unberuehrt.
 * ========================================================================== */
#ifndef RE15_KARTE_R39_H
#define RE15_KARTE_R39_H

/* Ein Stueck einer Kachel: art 0 = Sprite-Ausschnitt der Kachel (u,v gueltig),
 * art 1 = Raumkoerper (Eintrag 1 der Zustandszeile, halbtransparent),
 * art 4 = Wandlinie (RE15_KARTE_WAND, deckend). "Durchsichtig" erzeugt kein Stueck. */
typedef struct { short x, y, w, h; unsigned char u, v, art; } re15_karte_r39_stueck_t;

/* Zerlegt das Rechteck <rect> von Blatt <page> (Schirmlage rx,ry,rw,rh, Kachelecke ru,rv),
 * beschnitten auf das Fenster (fx,fy,fw,fh), in Stuecke. Rueckgabe: Zahl der Stuecke,
 * oder -1, wenn fuer das Rechteck keine Korrektur gilt (Aufrufer zeichnet wie bisher).
 * inv_screen (re15_inv_screen_t*, Nachbesserung 1 / Abnahme-Mangel M3): eine Korrektur, die eine
 * Wandlinie loescht, gilt nur, wenn ihr TRAEGER - das Nachbar-Rechteck mit der Ersatzlinie - in
 * diesem Schirm gezeichnet wird; sonst bliebe der Raum offen. NULL = alles gezeichnet (Vorlagen-Zustand). */
int re15_karte_r39_kachel_stuecke(const void *inv_screen, unsigned page, unsigned rect,
                                  int rx, int ry, int rw, int rh, int ru, int rv,
                                  int fx, int fy, int fw, int fh,
                                  re15_karte_r39_stueck_t *out, int max);

/* Zeichnet die Kachel des Rechtecks <rect> von Blatt <page> (Ausschnitt x,y,w,h ab Kachelecke
 * u,v, Zustandszeile clut, Tint r,g,b) MIT den Korrekturen in die Op-Liste ops[*n..max).
 * Rueckgabe 0, wenn fuer das Rechteck keine Korrektur gilt - dann zeichnet der Aufrufer
 * unveraendert seinen sprt() (re15_inv_screen.c, vier Haken "Runde 39 Spur G"). */
int re15_karte_r39_kachel(const void *inv_screen, void *ops, int *n, int max, unsigned page, int rect,
                          int clut, int x, int y, int w, int h, int u, int v, int r, int g, int b);

/* 1, wenn die Seite <seite> (0 oben, 1 unten, 2 links, 3 rechts) der Schema-Zelle
 * (cx,cy,cw,ch) auf Blatt <page> NICHT gezeichnet wird (Doppelwand laut Vorlage) - nur, wenn die
 * Ersatzlinie des Nachbarn in diesem Schirm (inv_screen) gezeichnet wird (Nachbesserung 1, M3). */
int re15_karte_r39_kante_weg(const void *inv_screen, unsigned page, int cx, int cy, int cw, int ch, int seite);

/* ---- B1 als Schema-Zeichnung (Runde 39 Spur G): Kartenhinweis-Ziel auf einer Schema-Zeichnung ----
 * Das Hinweis-/Ziel-System (map_hint_common.c, RE2 FUN_8006F1C4) fuehrt sein Ziel als (Blatt, Rechteck) und
 * zeichnet die Kachel dieses Rechtecks blinkend. Seit B1 nur noch Schema-Zeichnungen traegt, liegt ein Ziel dort
 * (ROOM11C0 "PARKING LOT" nach der ROOM10F0-Szene) auf KEINEM Rechteck. Es wird deshalb als Pseudo-Rechteck
 * 128 + synth gefuehrt (gemalte Rechtecke sind < 64, 255 = keins) und vom Schema-Zeichner blinkend gemalt. */
#define RE15_KARTE_R39_SCHEMA_RECT 128
/* Rechteck einer Zielzone fuer die Hinweis-Tabelle: zn->rect, bei einer Schema-Zone 128 + synth. */
int re15_karte_r39_ziel_rect(int rect, int synth);
/* Ist die Schema-Zeichnung <synth> auf Blatt <page> gerade Hinweis-/Kartenziel? 0 nein, 1 rote Phase,
 * 2 Umriss-Phase (wie CLUT 502 / 498 der Kachel, re15_inv_screen.c Kartenhinweis). */
int re15_karte_r39_schema_ziel(const void *inv_screen, unsigned page, int synth);

/* Ist die Zeichnung der Zone <zone> (re15_map_zone_t*) auf Blatt <page> die der AKTUELLEN Etage? Haupt-Zeilen
 * immer; eine Gast-Zeile (Etagen-Zeichnung, z.B. ROOM10A0 auf B1) nur, wenn das Band des Spielers per
 * re15_map_floor_lookup auf dieses Blatt fuehrt - dieselbe Regel wie die Kacheln in re15_map_rect_state
 * ("Auf einer Etagen-Umschaltung ist NUR das Rechteck dieser Etage aktuell"). */
int re15_karte_r39_etage_hier(const void *zone, unsigned page);

/* Nachbesserung 2 (Abnahme 1, N1): Klemmfenster des Markers = GEMALTER Innenraum (samt Wandlinie) fuer einen
 * Weltbereich einer Zone (zone = re15_map_zone_t*, Weltlage wx/wz). Engt lo/hi des Kachel-Klemmers in
 * re15_inv_map_marker ein (Haken "Runde 39 Spur G" neben re15_karte_fahrstuhl_fenster); ausserhalb der Tabelle
 * keine Wirkung. Belege je Eintrag in karte_r39.c. */
void re15_karte_r39_fenster(const void *zone, int wx, int wz, int *lo_x, int *hi_x, int *lo_y, int *hi_y);

/* Riegel-Zugriff: Zahl der Texel-Korrekturen und ein Eintrag. traeger = Rechteck derselben Seite,
 * dessen Wandlinie die geloeschte ersetzt; 255 = gilt immer (Nachbesserung 1, M3). */
int re15_karte_r39_korrektur_zahl(void);
int re15_karte_r39_korrektur(int i, int *page, int *rect, int *c, int *r, int *w, int *h, int *neu, int *traeger);

#endif
