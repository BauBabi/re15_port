# Bauwerkzeug fuer die Bahnsteig-Form (r41/form)

Die Figur selbst wird hier NICHT gestaltet. Commits auf r41/form: 93df3679a, 612f1453e, 40d9793e3, 84f88d080.

⛔ Stand seit Stufe 3 (STUFE3.md): das Spiel laedt `RE2/BAHNSTEIG_FORM.EMD/.TIM` (Riesenform J mit eigenen Clips), `BAHNSTEIG_G4.*`
ist aus dem Repo entfernt; `form_emd.py rundlauf`, `form_spiel.sh` und das Paket-Gate arbeiten mit dem neuen Namen. Die Messungen
unten (Abschnitt 2 und 4) sind am G4-Muster entstanden und bleiben als Formatbeleg stehen. Die Festwerte aus Abschnitt 3 bedient die
Form (Clipnummern, Laengen, Bit 16, Spur aus G4), die Tests pruefen das (`test_birkin_bahnsteig.c test_asset`).

Werkzeuge in `re15_port/tools/`: `form_emd.py` (Bibliothek + Befehle), `form_buehne.py` (Offline-Buehne),
`form_spiel.sh` + `form_fensterbild.ps1` (echtes Spiel), Test `re15_port/tests/unit/test_r41_form_emd.c` (`unit_r41_form_emd`).

## 1. Aufrufe

Bibliothek: Zwischenbeschreibung, Regeln und Beispiel stehen im Kopf von `form_emd.py`.

```bash
# Rundlauf-Beweis
python re15_port/tools/form_emd.py rundlauf
python re15_port/tools/form_emd.py rundlauf --datei re15_port/shared_assets/RE2/EM23.EMD --tim re15_port/shared_assets/RE2/EM23.TIM

# Probemodell (die Dateien fuer den C-Test liegen unter tests/unit/form_probe)
python re15_port/tools/form_emd.py probe --aus re15_port/tests/unit/form_probe
python re15_port/tools/form_emd.py info X.EMD --tim X.TIM

# Aus JSON bauen (Textur als PNG, Alpha < 128 = transparent)
python re15_port/tools/form_emd.py bau form.json --emd X.EMD --tim X.TIM --bild textur.png

# Buehne, Kamera aus dem RDT, Hintergrund aus der BSS, Licht aus dem RDT
python re15_port/tools/form_buehne.py --emd X.EMD --tim X.TIM --clip 5 --bild 0 --lage 1500,0,-23000,0 \
    --raum 5090 --schnitt 11 --original analysis/befunde_runde41/form/bild2.png --aus blatt.png

# Buehne gegen das echte Fenster
python re15_port/tools/form_buehne.py --emd X.EMD --tim X.TIM --clip 1 --bild 8 --lage 1500,0,-23000,0 \
    --raum 5090 --schnitt 11 --spiel spiel.png --aus blatt.png

# Echtes Spiel: x,z,gier,clip,bild,schnitt; bild < 0 spielt ab
bash re15_port/tools/form_spiel.sh --emd X.EMD --tim X.TIM --pose 1500,-23000,0,1,8,11 --aus C:/.../spiel.png
bash re15_port/tools/form_spiel.sh --zuruecksetzen     # nur nach --einsetzen noetig
```

Bau: `bash re15_port/tools/local_build.sh configure`, dann `build`. Dauer im Arbeitsbaum unter Last rund 6 Minuten.

## 2. Format (gemessen an BAHNSTEIG_G4.EMD und EM23.EMD)

- Datei: u32 dir_off, u32 8, Sektionen ab +8, am Ende 8 u32 Verzeichnis (`dir_off + 32` = Dateilaenge).
- dir[0] Morphblock, dir[1]/[2] EDD + EMR Paar 1, dir[3]/[4] Paar 2, dir[5]/[6] Paar 3, dir[7] MD1. Leere Paare = je 4 Nullbytes.
- EDD: je Clip (u16 Bilder, u16 Byte-Offset), dann u32-Worte, am Ende u32 = Laenge bis hierher (5880 / 852 / 0). Das letzte Wort war
  im Port-Parser nicht bekannt, es wird nicht gelesen; der Schreiber setzt es trotzdem, damit der Rundlauf byte-gleich bleibt.
- EDD-Wort: Bits 0-11 Keyframe, Bit 16 Treffer, Bit 27 Ton, Bits 28-31 Ton-Id. Andere Bits kommen im Muster nicht vor.
- EMR: Kopf (Tabelle, Keyframes, Knochen, Keyframe-Groesse), Bindversaetze ab +8, Tabelle auf 4 Byte (Kinderzahl, Offset relativ zur
  Tabelle), Kinderbytes, Keyframes auf 4 Byte. Keyframe: 6 s16 (Wurzel, Spur), dann 36 Bit je Knochen, auf 4 Byte gerundet.
- MD1: Tri- und Quad-Teil teilen je Mesh denselben Vertex- und Normalenblock. Wort 0 = 12 + Beginn der UV-Bloecke.
- TPage-Wort: 0x80 + Seite (Seite = Texel-x / 128), CLUT-Wort: (480 + Zeile) << 6.
- TIM: Flag 9, CLUT 256 x Zeilen ab (0, 480), Bild ab (0, 0). 0x0000 ist transparent, opakes Schwarz muss 0x8000 sein.

## 3. Was der Port voraussetzt

Stand gemessen im Quelltext dieses Baums. Spalte "Folge" nennt, was passiert, wenn die Regel verletzt wird. `FE.pruefe()` meldet alle
Punkte als Fehler (Datei wird nicht geschrieben) oder Warnung.

| Voraussetzung | Stelle | Folge |
|---|---|---|
| 8 Verzeichnis-Eintraege, dir[7] gesetzt | re2_ems.c re2_emd_parse_bank | Rueckgabe -2/-4, Lader meldet "fehlt oder defekt", Auslieferungsmodell bleibt |
| 1..32 Knochen | re15_emd.h RE15_EMD_MAX_BONES, emd_common.c parse_skeleton | Rueckgabe -3 |
| Eltern stehen VOR dem Kind | skeleton_common.c `parent < 0 \|\| parent >= b` | Knochen wird still zur Wurzel, sitzt auf der Keyframe-Wurzel |
| Mesh i haengt an Knochen i, Meshzahl >= Knochenzahl | emd_common.c `bone_mesh_index`, main.c `npc_bones` | Knochen ohne Mesh werden weggekappt |
| Ueberzaehlige Meshes zeichnet Typ 0x30 nicht | main.c `npc_zeichen_n` (`npc->type != 0x30u`) | Mesh fehlt im Bild |
| Keyframe-Groesse 12 + 36 Bit je Knochen, auf 4 Byte | emd_common.c `bit_offset + 36 > angles_byte_size*8` | Winkel 0 fuer die letzten Knochen |
| Mindestens 1, hoechstens 64 Clips, in Summe hoechstens 1664 Bilder, 4096 Keyframes | re15_emd.h, emd_common.c, EDD Bits 0-11 | Rueckgabe -4/-6, Paar unbenutzt |
| Hoechstens 256 Bilder je Clip | enemy_ai_common.c re15_birkin_anim `(uint8_t)` | Bildzaehler laeuft um |
| Paar 1 hat die meisten Clips | re2_ems.c "best" | sonst liest der Tick ein anderes Paar |
| Paar 2 und 3 leer oder gueltig | re2_emd_parse_bank | leer ist erlaubt (own_ok = victim_ok = 0) |
| Opfer-Bank kommt immer aus EM30 | main.c pc_enemy_load_bahnsteig | dir[5]/[6] des EMD werden ueberschrieben, die Datei braucht sie nicht |
| Morphblock Maske 0 | re2_ems.c | morph_ok = 0; nur 0x36 liest Morphs |
| TIM 8 bpp mit CLUT, Hoehe <= 256, x bis 15*128+255 | render_pc.c Upload, main.c `page_off` | Textur fehlt oder liest daneben |
| Windung: cross(b-a, c-a) gegen die Normale | nclip_r38.c re15_nclip_r41_weg, gilt fuer Typ 0x30 | Flaeche von aussen unsichtbar |
| Texel-Index 0x0000 = Loch, Schwarz = 0x8000 | re15_tim.h re15_tim_texel_argb | schwarze Stellen werden zu Loechern |
| Tiefster Punkt auf y = 0 je Pose | Aktor steht bei y = 0, Wurzel kommt nur aus dem Keyframe (skeleton_common.c) | Figur schwebt oder steckt im Boden; `FE.boden_nachfuehren()` |

Festwerte im Tick (engine/src/birkin_bahnsteig.c), die jede neue Figur bedienen muss oder die mit ihr geaendert werden muessen:

| Wert | Bedeutung | Heute |
|---|---|---|
| Teil 10 BB_TEIL_RUMPF | Biss, Rennen, Blut; Versatz (1400,0,0) im Teilraum | fehlt der Knochen, trifft nichts (re15_enemy_part_point Rueckgabe 0), Blut sitzt am Aktor |
| Teile 13 und 18 | Trefferpunkte Hieb Seite 1 / Seite 0 | wie oben |
| Clips 0, 3, 4, 5, 8, 9, 10, 26, 27 | Rennen, Hieb 1+2, Gehen, Zucken, Nachhieb, Drohen, Biss, Griff | hoehere Nummern laufen ueber Modulo in re15_compute_actor_kf |
| Clip 3 ab Bild 30, Clip 4 ab Bild 19 | k_doppel: Doppelschlag, Einstieg Clip 4 Bild 7, Clip 3 Bild 20 | Mindestlaenge 31 / 20 |
| Clip 8 ab Bild 5, Clip 27 Bild 8 | Zucken-Einstieg, Griffbild | Mindestlaenge 6 / 9 |
| EDD-Bit 16 in Clip 3, 4, 26 | Trefferbilder | ohne Bit trifft der Angriff nie |
| Spur Clip 5 | Vortrieb, Lenken erst ab Tempo 21 und 31 je Bild | zu langsam = Figur lenkt nur ausserhalb des 824er-Kegels |
| Spur Clip 0 + 10 je Bild | Ueberrennen ab Tempo 201 | zu langsam = Rennen trifft nie |
| EDD-Ton Bit 27 | bb_bild_ton, Bank 24 | Id = Wort >> 28 |
| hit_offset_x 800, Schatten 2300 x 1300 | bb_tick, main.c | Kasten und Schatten passen nicht von selbst zur neuen Figur |

Geaendert in diesem Baum, weil die Figur sonst nicht laedt oder falsch zeichnet:
- `re15_bb_bank_da` verlangte genau 27 Knochen. Jetzt reicht das neue Bank-Bit `bb_form`, das der Lader setzt. Die Zahl 27 bleibt fuer die Baenke,
  die Tests von Hand bauen.
- `bk_rohr` (main.c): Die Form erbt `birkin_rohr` aus dem Spawn, grid 0x33 hat Bit 0x10 (INIT Zeile 12140, gemessen in bb_pose.log). Jede Form mit
  mehr als 16 Meshes haette an Teil 15 die Geometrie von Mesh 16 gezeichnet. Das betraf auch die heutige Form. Tor `!re15_bb_bank_da(npc)` ergaenzt.

Beim Tausch des Assets muessen mit: `tests/unit/test_birkin_bahnsteig.c` (vergleicht gegen RE2 kind 0x34, 27 Knochen, 29 Clips),
`tests/unit/test_birkin_szenen.c`, `tests/integration/test_r41_birkin_pistole.cmake` (sucht "Bahnsteig-Form: 27 Meshes"), `release/make_package.sh` (Paket-Gate).

## 4. Beweise

**(a) Rundlauf `python re15_port/tools/form_emd.py rundlauf`**, Stand gemessen:
- Stufe 1 Leser -> Schreiber: alle acht Sektionen und die ganze Datei byte-gleich, BAHNSTEIG_G4.EMD (273812 Byte) und EM23.EMD (189208 Byte).
- Stufe 2 Keyframes je Bild ausgeschrieben, Schreiber legt zusammen: 1441 Bilder, 0 abweichend, Pool 1296 -> 1295. Das Muster fuehrt einen Keyframe
  doppelt, der Schreiber legt ihn zusammen. EM23: 1167 Bilder, 0 abweichend, Pool 1153 -> 1150.
- Stufe 3 UV ueber Texel und CLUT-Zeile neu bestimmt: MD1 byte-gleich.
- TIM byte-gleich (99872 / 133152 Byte).
- Gegenprobe Stufe 1 ueber alle 55 EMD in RE2 CDEMD0.EMS (Einmalskript, kein Teil des Werkzeugs): 55 byte-gleich, 0 abweichend.

**(b) Echter Parser und echtes Spiel.** `unit_r41_form_emd` liest FORM_PROBE.EMD/.TIM (3 Knochen, 22 Flaechen, 2 Clips) mit `re2_emd_parse_bank`,
`re15_tim_parse`, `re15_skel_compute_pose`: Eltern, Versaetze, Keyframe-Groesse 28, negative Winkel, EDD-Bits, Pose, TPage, CLUT-Zeile, Windung, Loch. Gruen,
ebenso `ctest -R "birkin|r41_form"` 17/17 (vor dem Nachtrag der Dreiecke; danach `unit_r41_form_emd` erneut gruen).
Im Spiel meldet debug.log `[enemy] Bahnsteig-Form: 3 Meshes, 3 Knochen, 2 Clips -> Slot 12`, bb_pose.log zeigt den 0x30 auf der Pose.
Bilder unter `form/werkzeug/belege/`: `probe_gdigrab_cut11.png`, `probe_spiel_cut12.png` (gdigrab), `probe_spiel_cut11.png`, `probe_eingesetzt.png` (PrintWindow, Weg `--einsetzen`).

**(c) Buehne gegen echtes Fenster** (`--spiel`), bester Versatz in +-6 Pixeln und mittlerer Kanalabstand ueber die Figur:

| Lauf | Verfahren | Pixel | Versatz | Abstand |
|---|---|---|---|---|
| Probe, Schnitt 12, Gier 700, Clip 1 Bild 24 | gdigrab | 4281 | 0,0 | 4,0 |
| Probe, Schnitt 11, Gier 1024, Clip 1 Bild 8 | gdigrab | 10693 | 0,-1 | 13,5 |
| Probe, Schnitt 11, Gier 0, Clip 1 Bild 8 | PrintWindow | 8127 | 0,0 | 6,9 |
| Probe, `--einsetzen`, Schnitt 11, Gier 1024, Clip 0 Bild 0 | PrintWindow | 10604 | 0,0 | 10,0 |
| Formatmuster 27 Knochen, Schnitt 11, Gier 2048, Clip 5 Bild 40 | PrintWindow | 25637 | 0,0 | 3,3 |

In Schnitt 11 stehen Flammen des Raums vor der Figur, die treiben den Abstand. Der Zeilenversatz -1 im zweiten Lauf aendert den Abstand kaum (ohne Versatz 14,1); er ist nicht weiter untersucht.
Die Buehne zeigt keine Raumeffekte und keine Vordergrundmasken.
Die Spielbilder des Formatmusters wurden nach der Messung geloescht, damit sie niemand als Vorlage nimmt; dort ging es nur um den Zeichenweg.

**Rohr-Tor:** gemessen im echten Ablauf ohne Messhaken (`RE15_ENEMY_DBG=1`, Pistolenlauf bis Bild 1400): `ZEICHNE Typ 0x30: 16 Teile (Bones 16, Meshes 17) grid 0x33 rohr 1`,
danach `Bahnsteig-Form: 27 Meshes`. `birkin_rohr` hat nur zwei Schreiber, beide setzen 1 (enemy_ai_common.c), die Form erbt es also.

## 5. Fensterbild und getrennte Sitzung

Waehrend dieser Arbeit war die Sitzung getrennt (`qwinsta`: mjoedicke, Status Getr.). Dann gilt, gemessen:
- `gdigrab -i desktop`: `Failed to capture image (error 5)`.
- `gdigrab -i hwnd=`: schwarz oder unbrauchbar (1280x960, nur ein Streifen des Fensters). Das Spiel selbst laeuft normal, Renderer direct3d11.
- `PrintWindow` mit Flag 2 (PW_RENDERFULLCONTENT) von aussen liefert das echte Fensterbild samt 3D. Flag 1 und BitBlt bleiben schwarz.

`form_spiel.sh` waehlt deshalb selbst: Desktop greifbar = gdigrab (Vorgabe des Skills), sonst PrintWindow; die Ausgabezeile `Verfahren:` nennt den Weg. Beide lesen den Prozess von
aussen, es gibt also keinen Readback im Spiel wie bei AUTOSHOT. Ein Bild kann schwarz sein (einmal gemessen), das Skript prueft die Helligkeit und versucht es bis zu sechsmal.
Ab etwa 14:43 war die Sitzung wieder aktiv (`qwinsta`: console, Aktiv), seitdem nimmt das Skript gdigrab: `belege/probe_spiel_cut12.png`, `belege/probe_gdigrab_cut11.png`, Blatt `belege/probe_gdigrab_blatt.png`.
Dieselbe Falle trifft jedes andere Skript, das in einer getrennten Sitzung gdigrab benutzt.

Falle im Skript, beim Bau gefunden: mit `/c/msys64/mingw64/bin` vorn im PATH loest `python` auf msys64-Python ohne PIL auf, die Helligkeitspruefung meldete dadurch jedes Bild als schwarz.
Der PATH-Eintrag ist entfernt, die exe ist statisch gelinkt (objdump: nur Windows-DLLs), und das Skript bricht ab, wenn `python` kein PIL hat.

## 6. Kameradatei

`form_buehne.py kamera_laden` liest JSON mit `pos`, `ziel` (oder `target`), `fov` oder `h`; alternativ `V` (9 Q12-Werte), `t`, `h`. Optional `name` und
`abbildung` = {ax, bx, ay, by}: Bildpunkt = a * (Port-Pixel + 0,375) + b, oder {matrix} 3 x 3 projektiv. Ohne `abbildung` gilt 2x ohne Versatz wie im Fenster mit `RE15_WINDOW_SCALE=2`.
Die Felder duerfen flach oder unter `kamera` liegen.
Offen: `kamera_bildN.json` und `form_video.py` lagen beim Abschluss noch nicht vor (Stand 14:56). Der Lader ist nur an selbstgebauten Dateien geprueft, die Tafel "video_filter" an keiner echten Funktion.
`form_video.py` wird benutzt, wenn es eine Funktion `video_filter(rgb)` hat (H x W x 3 uint8 hin und zurueck).

## 7. Grenzen

- Die Buehne ist kein Ersatz fuer den Lauf: keine Vordergrundmasken, keine Effekte, kein Schatten; `isqrt` statt BIOS-SquareRoot0, Gleitkomma-Normierung der Lichtrichtung.
- `form_spiel.sh` setzt ueber den Haken `e->y = 0`; andere Bodenhoehen zeigt es nicht.
- Clipnamen stehen nur in der Zwischenbeschreibung, das EDD kennt keine Namen.
- Die Port-Festwerte aus Abschnitt 3 (Teile 10/13/18, Clips, Kasten, Schatten) sind nicht geaendert; eine Figur mit anderem Bau braucht dort Anpassung oder muss die Nummern bedienen.
