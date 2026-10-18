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
 * oder -1, wenn fuer das Rechteck keine Korrektur gilt (Aufrufer zeichnet wie bisher). */
int re15_karte_r39_kachel_stuecke(unsigned page, unsigned rect,
                                  int rx, int ry, int rw, int rh, int ru, int rv,
                                  int fx, int fy, int fw, int fh,
                                  re15_karte_r39_stueck_t *out, int max);

/* Zeichnet die Kachel des Rechtecks <rect> von Blatt <page> (Ausschnitt x,y,w,h ab Kachelecke
 * u,v, Zustandszeile clut, Tint r,g,b) MIT den Korrekturen in die Op-Liste ops[*n..max).
 * Rueckgabe 0, wenn fuer das Rechteck keine Korrektur gilt - dann zeichnet der Aufrufer
 * unveraendert seinen sprt() (re15_inv_screen.c, vier Haken "Runde 39 Spur G"). */
int re15_karte_r39_kachel(void *ops, int *n, int max, unsigned page, int rect, int clut,
                          int x, int y, int w, int h, int u, int v, int r, int g, int b);

/* 1, wenn die Seite <seite> (0 oben, 1 unten, 2 links, 3 rechts) der Schema-Zelle
 * (cx,cy,cw,ch) auf Blatt <page> NICHT gezeichnet wird (Doppelwand laut Vorlage). */
int re15_karte_r39_kante_weg(unsigned page, int cx, int cy, int cw, int ch, int seite);

/* Riegel-Zugriff: Zahl der Texel-Korrekturen und ein Eintrag. */
int re15_karte_r39_korrektur_zahl(void);
int re15_karte_r39_korrektur(int i, int *page, int *rect, int *c, int *r, int *w, int *h, int *neu);

#endif
