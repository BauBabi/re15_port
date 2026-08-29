# Nutzer-Batch 2026-08-29 — Ergebnis (v0.3.38)

10 Fix-/Feature-Commits + 4 beantwortete Untersuchungen. Jede Verhaltens-Konstante traegt
ihre `@0x…`-Adresse im Code und in der Commit-Message; jeder Fix hat einen Pin mit
gemessener GEGENPROBE (Pin faellt ohne den Fix). Suite: 253/253.
Einzel-Dossiers in diesem Verzeichnis.

## Fixes

| # | Report | Commit | Kern |
|---|---|---|---|
| 1 | Item-Stapel: 2. Item ohne Druck | 00bf5017 | Modal-FSM tickt jetzt NACH dem Step (Original-Ordnung @0x8001ce0c vor @0x8001ce34; Edge-Verbrauch `sw 0xffff` @0x80028588 dokumentiert) |
| 2 | Hund: 1. Treffer verzoegert | 796b9d42 | byte-true Hoehenband-Stempel (Tail @0x8010dd00-4c, R=0xfa0 @0x800129cc-f0) — Down-Schuesse treffen; IDLE nur mit Down |
| 3 | RE2-Hund: Todes-Quieken fehlt | aff65d11 | Todes-Zeile = WAFFEN-Row (+0x5-Stempel @0x80047324), SE 7 @0x801041C8-CC spielt bei jedem Schusswaffen-Kill |
| 4 | Sherry laeuft nach Cutscene rum | 1d609969 | NPC-Wand-Klemme in den Root-Tail (alle 6 NPC-Roots, z.B. @0x8011e318) — die geparkte Sherry (0x4B, Park @RDT 0x13AC) bleibt draussen |
| 5 | 1210: kein Arm-Treffer-Feedback | 193390f2 | Arm-Band LEVEL\|DOWN + byte-true Original-Flinch 0x8010d188 (Clip 2, Blend 3 @0x8010d220, Blut @0x8010d268-8c) |
| 6 | 1210: Griff zieht links in die Wand | c7951d45 | Halte-Anker auf die begehbare Kante geklemmt (West-Anker lag 287 IN der Wand, Ost 73 — gemessene Asymmetrie = das "links") |
| 7 | Weste ohne Effekt (Wunsch) | 70f72bc2 | +5 Max-HP = exakt 1 Biss (-5 @0x801027dc) mehr; Anlegen heilt voll (Original @0x80031710/18); Save/Load rekonstruiert das Modell |
| 8 | 1090: "Zeug ueber den Feuern" | 03b5a343 | Feuer-fx ADDITIV (Routine 17 @0x80017c00: ABE+ABR1), reiten auf dem Emitter (Follow @0x80019f44-f94, y=-2740), Groesse oszilliert (Routine 18 @0x80017c8c) |
| 9 | Affen treffen nie / Leon clippt | 97589546 | Original-Bone-QUADRAT statt Distanz+Arc (FUN_8001bff8 @0x8001c080-c0c0; Biss Bone 9 r=0x3e8 @0x801183c0-cc) + Wurf-Platzierung wandgeklemmt |

## Untersuchungen (beantwortet, kein Codebedarf)

- **Timer-Bomb (1110)**: WIRD verwendet — ROOM2040/2041 (Kanalisation): sprengt die
  verrostete Tuer (dahinter u.a. "Head of Akuma"). Reine Skript-Mechanik (Flag Bank3/0x92),
  kein Inventar-Item. → `timerbomb-verwendung.md`
- **10F0 kleine Tuer**: NIE oeffenbar — reiner Message-AOT (sce 1 @0x0FFE), kein
  Tuer-Record, kein Retype, kein Zielraum. → `10f0-kleine-tuer.md`
- **Cutscenes**: (A) Ada+Marvin zusammen EXISTIERT AKTIV — 11B0 sub06 (Manhole-Szene),
  Gate = beide 11C0-Bosse toeten (Flag 3:0x43); weitere gemeinsame Raeume 1260/2000/3070….
  (B) Chief-Todesszene existiert NICHT (game-weiter Sweep). (C) Die 1050-Zeilen
  ("I need a break"/"medical room") sind WAISEN; die Rahmen-Szene haengt an der
  1090-Ada-Rettung (Flag 3:0x6E). → `cutscenes-11c0-11b0-1050.md`
- **Schlafender Content**: 35 Waisen-Texte + verlorene Szenen — Highlights: 1150 sub04
  (komplette verlorene Szene, sofort per RE15_FORCE_EVENT=4), 6020 sub02 "Itembox is not
  available in this preview", 1030-Such-Pickup, 4090 T-Virus-Synthese-Maschine.
  → `schlafender-content.md`

## Offen / braucht Nutzer-Input

- **1210 "ganzer Koerper clippt durch die Wand"**: nicht reproduziert/verortet. Belegt:
  in RE1.5-Modus EXISTIERT kein Koerper (EM01A = 4 Meshes/4 Bones = nur ein Arm; HP-Bytes
  der 10 Spawns = 0), 0x1A ist nicht RE2-owned (kein Modell-Tausch), und ALLE 9
  1210-Kameras TRAGEN PRI-Masken (Records @0x60ff, letzte u32 = 0x4d4..0x1824).
  Brauche: KI-Modus + Screenshot/kurzes Video der Stelle.
- **1210 Wandgriff links**: Anker-Fix drin (oben #6) — bitte testen; die Sonde konnte den
  Live-Report nicht reproduzieren (stehende Griffe beidseitig 0 Wand-Bilder; ein GEHENDER
  Spieler wird nie gegriffen — eigener Nebenbefund in c7951d45).
- Affen: dedizierter Pin-Opfer-Handler 0x8011c118 unportiert (der geklemmte generische
  Wurf ist eine Naeherung); Spieler-vs-Maggot-Push @0x80116e40-44 nicht nachgezogen.
- RE2-Hund: HURT-Router traegt dieselbe prev_sub-Fehldeutung (eigener Batch);
  Todes-Varianten 0x801042B0/0x80104774/0x8010481C unportiert.
- Feuer: Strike-Routine 34 (FUN_80018784) + scale16-Live-Update offen; gdigrab-A/B in
  Cut 15 aussstehend.
- Item-Modal: der Original-Edge-Verbrauch blankt im Folgeframe ALLE Tasten
  (@0x80028588/@0x80028468/@0x800286b0) — Port hat kein persistentes prev/held/edge-Tripel
  (dokumentiert in main.c).
