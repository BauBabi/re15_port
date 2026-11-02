# Spur Q "restpunkte" - Abnahme 0

Baum `.claude/worktrees/r41_restpunkte`, Zweig r41/restpunkte, Kopf 5fd6cf311, Merge-Basis 91cd9ec82. master ist seitdem weiter (Spur R u.a.), `git diff master` zeigt deren Stand als Loeschung; gelesen wurde `git diff 91cd9ec82..HEAD`. `git merge-tree --write-tree HEAD master`: nur ein Tree-Hash, kein Konflikt.
Bau: `local_build.sh configure` + `build`, `LOCAL-BUILD-OK`, ninja: no work to do, exe 11:36:38, traegt die Log-Texte des Kopfs (`gpu-grenze`, `AUSNAHME 0x`, `SDL_QUIT (Fenster`).
Scratch: `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/Q0abn` (kurz `Q0abn`): Werkzeuge `tools/`, Laeufe `runs/`, Bilder `bilder/`.

**Ergebnis: NICHT BESTANDEN.** Punkt 1 ist nur teilweise erfuellt, alle uebrigen Punkte und alle Gates halten. Der eine Mangel ist im Spiel nicht sichtbar, die Figur kommt dort nie in diese Kamera: er blockiert das Spiel nicht, aber die Begruendung im Dossier ist falsch und der Punkt ist als geschlossen vermerkt.

## Urteile
| Punkt | Urteil | Kern |
|---|---|---|
| 1 Restloch 6010/6011 C5 Teil 8 schliessen (Abnahme M 5 Mangel 1) | **teilweise** | Spaltentiefe behoben (je Stufe ein Fenster, exakt nachgemessen), an der Stelle des Mangels aber 63 von 96 sichtbar. Die echte Wange liegt rund 580 Einheiten naeher an der Kamera als die Modell-Flaeche. "Kein Loch" stimmt nicht. Im Spiel nicht sichtbar. |
| 2 Teile-Bericht, 8 Stellen, 4040 C12 | **erfuellt** | Eigenes Skript: 0 Abweichungen gegen pl_a3, 70 Sektionen, 104 Szenen, 4040 C12 an der exe ohne HINTER-Stelle. |
| 3 Elza-Riegel, Ursache der stillen Enden | **erfuellt** | Riegel gruen, Fehlzugriff selbst gelesen (srcy = -15), ohne Schutz 4 von 6 SIGSEGV, mit Schutz 0 von 6. |
| 4 Toter Alligator bleibt tot | **erfuellt** | Leon und Elza: Raum verlassen, Spielstand, Water Key zu Fuss, je gemessen. RE2-Stellen selbst gelesen. |
| 5 Spinnen-Filter unter RE15_GATOR_ORIGINAL | **erfuellt** | Eigene Laeufe Leon und Elza: 2 Spinnen, RE1.5-Alligator, keine Szene; ohne Schalter weiter verworfen. |

## Punkt 1 - Restloch ROOM6010/6011 Cut 5 Teil 8
Eigener Laeufer (Rezept run5.sh der Abnahme M 5, je Lauf eigene Kopie `re15_pc_q0abn_*`). Alter Stand ueber `RE15_CD_ROOT=<Q0abn>/alt` (nur die 4 Dateien aus 91cd9ec82), dieselbe exe.
Eichung an der Stelle des Mangels (-19912,-19720): alt 82 von 96 sichtbar (= Abnahme M 5), neu 63 von 96 (= Dossier), Figur 138 Punkte, sichtbar nur X 164..172 / Y 229..239.

Binaerdateien: `ROOM6010.MSK` = `ROOM6011.MSK`, `_PRI05.TIM` Leon = Elza; nur Sektion 5 anders, R0..R112 gleich, R113..R127 Tiefen 126..112 unveraendert. `stufen 6010 5 113 127 426`: alle Punkte da, Stufe == Rechteck-Tiefe, je Stufe ein Rechteck.

Eigene Stellen (nicht die des Bau-Agenten), Leon, Bild 120, Figur auf Teil 8 / sichtbar neu / sichtbar alt:
| Stelle | auf Teil | neu | alt |
|---|---|---|---|
| (-19812,-19620) | 141 | 50 | 84 |
| (-19612,-19420) | 237 | 0 | 14 |
| (-19412,-19320) | 308 | 0 | 0 |
| (-19212,-19420) | 253 | 0 | 0 |
| (-19012,-19320) | 219 | 0 | 0 |
| (-19812,-19920) | 103 | 47 | 73 |
| (-19512,-19820) | 171 | 0 | 0 |
| (-19912,-19520) | 98 | 54 | 78 |
| (-19912,-19320) | 77 | 32 | 47 |
| (-20012,-19820) | 61 | 58 | 59 |
| (-20212,-19920) | 16 | 16 | 16 |
Elza gleiche Liste: (-19812,-19620) 62/23/41, (-19712,-19820) 74/14/21, (-19612,-19720) 114/14/20, (-19512,-19820) 121/2/2, ab x -19412 0.
Stellen des Bau-Agenten gegengemessen, stimmen exakt: (-19712,-19820) 155/9 (alt 41), (-19612,-19720) 195/0 (alt 3), (-20112,-19620) 29/27. Seine Verschiebe-Probe stimmt ebenfalls (k = 0,1,2,4,8: 63, 59, 48, 23, 0).

Der Fix behebt also genau das, was er behebt: die Spaltentiefe. Ab x -19612 ist Leon verdeckt. Bis x -19812 bleibt aber ein grosser Teil sichtbar, und das ist kein Rest "vor der Flaeche":

**Die Begruendung "kein Loch, Figur VOR der Flaeche" gilt nur gegen das Modell, nicht gegen die Wange.** Gemessen:
- Kamera 5 steht bei (-23912,-9927,-23336). Der Koerper der Modell-Zeile ist Zelle @0x0092C: Westflaeche x -19919, y -1800..-4111. Das ist eine Kollisionszelle (Band 1/4), keine Wange: `Q0abn/bilder/linien.png`, der Prismen-Deckel laeuft waagerecht quer durch die Stufen, die Wange steil parallel zur Treppe.
- Treppenlauf aus den Stair-Records (`RE15_STAIR_DBG=1`: `[STAIRREG] slot=9/10 type=13 ... corner=-24300 extent=6895 centre=(-18487,-20853)`, Band 1 -> 4): die Stufenlinie bei x -19919 laeuft MITTEN durch die Stufen, die Stufenenden reichen bis etwa x -20500, direkt links daneben liegt die olivfarbene Wange (`Q0abn/bilder/stufen.png`, Linien gruen x -19919, gelb -20284, magenta -20600).
- Wange als Linie parallel zur Treppe, Bildkanten aus Teil 8 und Hintergrund: an x -19919 laege sie 1550..2900 unter der Stufenlinie, an x -20500 nur 250..1400. Die Unterkante der Ostwange liegt in den Kameras 6 und 11 rund 1125..1225 unter der Stufenlinie, in Kamera 13 nur 400 (Kamera 11 zeigt die Ostwange von innen, aus ihr ist die Westwange nicht zu bestimmen). Die Unterkante reicht also hoechstens rund 1250 unter die Stufen, damit gilt x <= ~ -20500: die Wange liegt mindestens rund 580 naeher an Kamera 5 als die Modell-Flaeche (Seitenansichten `Q0abn/bilder/tri2.png`). Je Punkt von Teil 8 sind das gerechnet 10..12 OT-Stufen fuer x -20284, 16..19 fuer x -20500 (`tools/plan.py`).
- Damit stimmt auch die Geometrie an der Stelle: die Schnittlinie an der exe (X 171/172) ist die Ebene x -19919, sie laeuft mitten durch den Kopf (`Q0abn/bilder/rest_p01.png`), nicht an der Wangenkante.
- Gegenprobe an der exe, Masken-Kopie mit R113..R127 um k Stufen vorgezogen:
| Stelle | Figur auf Teil | sichtbar k=0 | k=12 | k=16 | k=20 | uebermalt ausserhalb k=12 |
|---|---|---|---|---|---|---|
| (-19912,-19720) | 96 | 63 | 0 | 0 | 0 | 0 |
| (-19812,-19620) | 141 | 50 | 0 | 0 | 0 | 0 |
| (-20012,-19820) | 61 | 58 | 0 | 0 | 0 | 0 |
| (-20112,-19920) | 32 | 32 | 0 | 0 | 0 | 0 |
| (-20212,-19920) | 16 | 16 | 0 | 0 | 0 | 0 |
| (-20312,-19820) | 1 | 1 | 0 | 0 | 0 | 0 |
| (-20312,-20020) | 0 | 0 | 0 | 0 | 0 | 0 |
| (-20412,-19920) | 0 | 0 | 0 | 0 | 0 | 0 |
Elza k=12: (-19812,-19620) 62: 23 -> 0, (-19612,-19720) 114: 14 -> 0, (-19712,-19820) 74: 14 -> 0, (-20012,-19820) 20: 20 -> 0, ueberall 0 uebermalt.
Keine Stelle hat eine Figur vor der Wange: westlich von x -20312 erreicht die Figur Teil 8 nicht mehr. "Sie zu verdecken waere der Fehler, den Regel 6 verbietet" (Dossier Punkt 1), "kein Loch" und "mit einer Tiefe je Stufe nicht weiter zu trennen" (OFFEN 3) stimmen also nicht. Die Gegenprobe des Dossiers misst gegen dasselbe Modell, das sie belegen soll.

**Natuerlicher Weg:** ohne RE15_FORCE_CUT steht an diesen Plaetzen nie Kamera 5. RVD ROOM6010 (40 Saetze, je Cut der erste das Regionsviereck, alle Stockwerk 0xFF): Zustandssuche ueber die 18 181 erreichbaren Band-0-Plaetze: Start in 0/1/2/3/12 bleibt dort, 6/7/8/9/13 bleibt dort, 4/10 bleibt dort; Cut 5 nur aus Cut 11. Tueren nach 6010: Cut 7 (5080), Cut 0 und 9 (6000), keine in 5/11. Der Abstieg schaltet 5 -> 11 -> 7 (Streifen 11->7 x -23000..-22000 liegt quer ueber dem unteren Lauf, der in x verlaeuft: `corner=-23250 extent=2345`).
An der exe zu Fuss: Tuer aus ROOM5080/5081 (`RE15_FIRE_AOT=0`, Door_aot_set @0x6FA Ziel Cut 7 (-26350,0,-25900)), Wegplan in den Gang unter der Treppe: Leon und Elza Cut 7 (F30..F210), dann **Cut 13** bei (-19988,-19711) und (-19766,-19726) bis F750 (`[pri] cut=13 ... masks=118`), kein Bild in Cut 5. ROOM6010 hat keine Sce_em_set-Saetze, auch keine andere Figur steht im Spiel dort. Die Spielfigur sieht dieses Restloch also nicht; der Mangel der Abnahme M 5 war mit RE15_FORCE_CUT gemessen.

**Urteil: teilweise.** Am Wortlaut "Restloch schliessen" gemessen bleibt die Stelle des Mangels mit 63 von 96 offen, der Grund ist die falsche Ebene, nicht ein Gleichstand. Nicht blockierend: im Spiel unsichtbar.

## Punkt 2 - Teile-Bericht, Freistellungs-Liste, 4040 C12
Eigenes Skript `tools/teile_q0.py` (nicht das des Bau-Agenten) gegen `m41nb5/pl_a3/platzieren_r41.json`:
`Sektionen 81 mit 267 103112 ohne 277 144225 Abweichungen 0`; Summenzeile der Datei gleich; Liste 70 Sektionen gegen die JSON: keine Abweichung (der eine Treffer ROOM30A0 C0 ist die Sonderzeile "roter Ordner" hinter der richtigen Zeile 1573); Summe 144 225, 277 Teile.
Die 8 Stellen stehen in der Liste (Zeilen im Diff von `M_masken36.md`), Restbilder 70, keines fehlt; `NB5_rest_ROOM3040_C10.png` angesehen: brauchbar.
Szenen in den Dateien: 11+11+13+11+12+12+12+11+11 = 104 (Dossier nennt 104, Berichtigung 108 -> 106 -> 104 stimmt); 38 M-Szenen, alle mit A0.
4040 C12: eigene Suche ueber alle Plaetze (`PLAETZE 1060`), Figur-Proben gegen R105..R125 (723 Punkte, X 75..105, Y 57..106): HINTER-Proben nur an (-4000,4678) und (-3900,4578), die uebrigen 127 Plaetze mit Ueberlappung sind DAVOR. An der exe: (-4000,4678) Leon Figur 377, Elza 220, (-3900,4578) Leon 374, jeweils **auf dem Teil 0**, 372/219/370 Punkte von alten Masken verdeckt. "Keine HINTER-Stelle" stimmt, die Zeile im Dossier M ist richtig berichtigt.
Riegel: Art M zaehlt mit A0 gegen Lauf D (`test_r41_masken36_stage2.cmake` Z. 189, 282). Gruppen a1..d2 gruen.
**Urteil: erfuellt.**

## Punkt 3 - Riegel und stille Enden
- `integration_r41_ventilkette_kette_elza` Passed 250 s, `integration_r41_ventilkette_kette` Passed 235 s; der Riegel prueft `Spieler-Familie PL04`, Raeume 2031, 20b1, 2051, Weg zu Fuss.
- psx-spx selbst gelesen (Hauptbaum `info/.../psx-spx.github.io-master/docs`): `graphicsprocessingunitgpu.md` Z. 332 wortgleich, `geometrytransformationenginegte.md` Z. 418/419 wortgleich. SDL 2.28.5 `SDL_triangle.c`: `s2_x_area.x = s2->x * area;`, `TRIANGLE_GET_TEXTCOORD` ohne Klemme, wie zitiert.
- **Fehlzugriff jetzt gelesen** (die Luecke des Dossiers). Stand ohne Schutz (Hauptbaum-exe master, Kopie `Q0abn/ohneschutz`), Rezept des Riegels, gdb mit `_NO_DEBUG_HEAP=1` (Halde wie ohne Debugger): **4 von 6 Laeufen SIGSEGV**, die zwei uebrigen bis Bild 60 in 2050. Alle mit letzter Log-Zeile `[walk] F750 cut=10`.
  `SDL_BlitTriangle_Slow+1394: mov (%rax),%eax`, rax 0x61e22d8, rdx 0x61e5ec0 (Textur), rcx -15360 = -15 x Pitch 1024, Rest 0x18 = srcx 6: gelesen wird srcy = -15, 15 Zeilen VOR der Textur. Stapel `SDL_SW_BlitTriangle <- SW_RunCommandQueue <- FlushRenderCommands <- SDL_RenderPresent <- re15_render_end_frame <- main`. Die Endung `22d8` steht auch in allen vier AUSNAHME-Zeilen des Bau-Agenten.
- **Wirkung des Schutzes, gleiche Bedingungen:** ohne Schutz 4 von 6 SIGSEGV, mit Schutz 0 von 6 (alle `EXIT_AT: Bild 60 in Raum 2050`, je 16 Zeilen `[gpu-grenze]`). Fenstergroesse 4 im CPU-Rasterer: 0 von 3.
- Spieler-Renderer: `direct3d11`, 0 Zeilen `[gpu-grenze]`, Helligkeitsreihe F750..F772 mit und ohne Spur Q identisch: das Spielerbild aendert sich nicht.
- CPU-Rasterer-Gruppe (68 Tests, alle Skripte mit RE15_SOFTWARE_RENDER, die der Bau-Agent nicht gefahren hat): siehe "Gezielte Gruppen".
**Urteil: erfuellt.**

## Punkt 4 - Der tote Alligator bleibt tot
Selbst gelesen: ROOM40A0.RDT SCD init @0x2500: @0x0050 `06 00 64 00`, @0x0054 `21 04 33 00`, @0x00A0 `44 00 00 23 ...`; main @0x25C0: sub02 @0x00A4 `46 05`, @0x00AE `22 04 33 01`; @0x0828 `46 00`, @0x0832 `46 01`, @0x0CC8 `46 04`, @0x0CDE `21 0b 1f 00`; @0x07D0 `21 05 1e 00`, @0x0D56/@0x0DAC `21 05 1e 01`. EM23_OVL_0000.BIN: `beq v1,v0,0x80102654` @0x801021dc mit `addiu a0,s2,17240` @0x801021e0 im Delay-Slot, `addiu a1,zero,30` @0x80102658, `ori v0,v0,0x4000` @0x8010265c, `jal 0x8007730c` @0x80102660. RE2 PSX.EXE 0x8007730c = Bit setzen (`srl a2,a1,5` .. `or v1,v1,v0`), Sce_em_set `lbu a1,9(v0)` @0x800571c0. Bank-Tabelle PTR_DAT_800a78c8: Index 5 = 0x800d4874 @0x800a78dc, also wirklich Bank 5. Die Zeitpunkt-Wahl (Tod statt Kampfbeginn) ist als PORT-WAHL gekennzeichnet und begruendet.

An der exe (Riegel selbst gefahren, Logs gelesen):
| Szenario | Lauf 1 | Lauf 2 Laden | Lauf 3 zu Fuss | Lauf 4 |
|---|---|---|---|---|
| Leon | R1-Tod, Tuer, `[save] saved (room 2080)`, Leiter, 1 x Alligator gesetzt | `CONTINUE: resumed in room 2080`, 0 t=23, Ersatzlage (-170,-1702,-23629) | `[inv-r39] F212 R2090 ... 34:1` | 0 Alligator, 0 Schluessel |
| Elza | dasselbe, PL04 | `resumed in room 2081`, 0 t=23 | `[inv-r39] F212 R2091 ... 34:1` | 0 / 0 |
Vorher (Dossier Lauf v4L, Abnahme P H3): Alligator wieder da, hp 25300, nicht mehr toetbar.
Eigene Zusatzprobe: Tuer im Fenster zwischen R1-Start (F322) und Bit 111 (F358), `RE15_FIRE_AOT=0@330`: der Alligator kommt wieder (hp 25300), er war noch nicht tot, aber auch der Kanister bleibt (`Kanister im Becken ... (Bit 116)`), also toetbar, kein Softlock.
**Urteil: erfuellt.**

## Punkt 5 - Spinnen-Filter unter RE15_GATOR_ORIGINAL
ROOM2090.RDT selbst gelesen: @0x00AB0 `44 00 25 00 00 00 00 73 9c ff 00 00 98 df`, @0x00AC4 `44 01 25 00 00 00 00 74 04 f7 00 00 a8 a2`.
Eigene Laeufe: mit Schalter Leon und Elza: 0 x verworfen, 0 Zeilen `[gatorszene]`, 2 x `Sce_em_set type=0x25`, `EM23 loaded: 15 meshes, 15 bones, 43 clips`, letztes Bild `[1 t=25]`, `[2 t=25]`, `[35 t=23]`. Ohne Schalter: 2 x verworfen, nur `[35 t=23]`. `re15_gator_original` wird einmal je Prozess gelesen.
**Urteil: erfuellt.**

## Gates
- **RE-Gate haelt.** Selbst disassembliert, Zitat traegt: @0x80025640 `4b58002d` AVSZ3; @0x80025654 `sra v0,v1,6`, @0x80025658 `beq v0,zero`, @0x8002565c `sra v1,v1,4`; ZSF3 @0x80066c70 `addiu t0,zero,341` + `48c8e800` (CTC2 Register 29); @0x80039650 `lh a0,2(s3)` vor `addu a0,v0,a0` / `jal 0x8006b538`; @0x8003928c `lbu v0,7(a0)`; @0x80053534 `4a180001` RTPS; @0x80053578 `sra v0,a0,2`, @0x8005357c `beq`; @0x8005369c `sb v0,7(t2)` nach `addiu v0,s5,44`; dazu die RE2-Stellen aus Punkt 4. Konstanten des Diffs: 1023/511 und -400h/+3FFh mit psx-spx belegt, Bit (9,111) vorhanden, Schranken in `r41_masken36.cmake` als Messwert. Kein "deferred/tunable/interim/for now/faithful/plausibel/TODO" in den neuen Zeilen, kein neuer Schalter. Die falsche Aussage aus Punkt 1 ist eine Mess-Schlussfolgerung, keine erfundene Konstante.
- **Vertrag haelt.** Pfade: `shared_assets` nur 4 Dateien unter `MASKS/` (Masken-Restloch ist der Spur Q zugeteilt), keine `release/`, `platform/android/`, `pri/`, `synchro/`, keine Test-CMakeLists (Riegel in `probes/r41_restpunkte.cmake`). Keine neuen Bits, Bit 126 frei, keine Nachrichten-Ids. Gemeinsame Dateien nur markierte Haken (main.c 2, render_pc.c 4 + include), neue Dateien `re15_gpu_grenze_r41.h`, `endgrund_r41_pc.c`. Neue Laeufer mit eigenem Praefix, `cmake_minimum_required`, `m` fuer Linux.

## Gezielte Gruppen
| Gruppe | Regex | Ergebnis |
|---|---|---|
| Spur | `r41_restpunkte\|integration_r41_ventilkette_kette` | 12/12, 897 s |
| Masken | `^(integration_r41_masken36\|unit_r41_masken36\|unit_r37_masken36\|unit_pri_\|integration_pri_masken)` | 30/31, 2560 s; rot `integration_r41_masken36_fern`, Hinweis H3 |
| Alligator, Wasser | `^integration_r41_(gatorszene\|alligator2\|kanister2080\|re2gator\|wasser20b0)\|^integration_r38_wasser\|^unit_r41_(gatorszene\|alligator2\|kanister2080\|re2gator)` | 92/92, 1414 s |
| CPU-Rasterer (68 Tests mit RE15_SOFTWARE_RENDER) | Liste `Q0abn/sw_tests.txt` | 65/68, 7926 s; die 3 roten einzeln 3/3 gruen (Hinweis H4); darin die Ketten-Riegel und alle Weg-Riegel |
| Quer | `^(unit_\|prop_\|probe_)` | 1054/1054, 892 s |

## Maengel
1. **Punkt 1: Restloch nicht geschlossen, Begruendung falsch.** ROOM6010/6011 C5 Teil 8, Stelle des Mangels (-19912,-19720): 63 von 96 Punkten sichtbar, (-19812,-19620) 50 von 141, (-20012,-19820) 58 von 61; Elza entsprechend. Ursache: die Rechteck-Tiefen kommen aus der Kollisionszelle @0x0092C (Westflaeche x -19919), die echte Wange liegt bei x ~ -20500. Falsch sind Dossier Q Punkt 1 ("kein Loch", "der Fehler, den Regel 6 verbietet"), OFFEN 2 und 3, der Nutzertext "Figuren hinter der Wange sind jetzt verdeckt" und `M_masken36.md` OFFEN 18 "[Geschlossen durch Spur Q ...]". Nachpruefbar: `Q0abn/tools/kshift.py` + `kl.sh`, k = 12 ergibt an 8 Stellen 0 sichtbar, 0 uebermalt; westlich x -20312 keine Figur mehr auf dem Teil. Weg, eins von beiden: (a) Teil 8 mit der Ebene der Wange (R113..R127 um rund 12 Stufen vor, Beleg Stufenenden + Stair-Record, Modell-Tabelle und Riegel `unit_r41_restpunkte_teil8` nachziehen, Figur auf der Treppe mitmessen); (b) Teil 8 so lassen, aber die Aussagen berichtigen: "nur mit RE15_FORCE_CUT sichtbar, im Spiel nie in Cut 5", OFFEN 18 wieder offen bzw. als unerreichbar begruendet.

## Hinweise (keine Maengel der Spur)
- **H1 Ursache von Abnahme P H1 vermutlich gefunden.** Absturz in einer eigenen Serie (1 von 48, dazu 1 weiterer unter Last), direct3d11, nur Messlauf: `[flow] AUSNAHME 0xC0000005 ... Adresse +0x1f7510 Ziel 0000000000000000`, Stapel `D3D_RenderReadPixels+101 <- SDL_RenderReadPixels <- re15_bildhalt_r41_merken+387 <- re15_render_end_frame <- main` (Lauf `runs/s2_3_ohne`). SDL prueft dort `defaultRenderTarget` nicht auf NULL (`SDL_render_d3d.c` D3D_RenderReadPixels), das Feld ist nach einem gescheiterten Geraete-Reset NULL (Z. 1416-1418). `re15_bildhalt_r41_merken` liest nur mit RE15_FRAMEDUMP oder nach einem Ruecklese-Auftrag: Spieler ohne Abzug nicht betroffen. Nicht im Diff. Der neue Ausnahme-Haken der Spur hat das erst sichtbar gemacht. Dass Abnahme P H1 derselbe Hergang ist, ist nicht belegt (damals kein Stapel), es passt aber: dort lief ein Messlauf mit direct3d, ohne WER-Eintrag. Eine Abhilfe ist nicht geprueft.
- **H2 Nebenbefund des Bau-Agenten bestaetigt, gehoert in die Release Notes oder in eine eigene Spur.** ROOM20B0 nach der Szene, Spielerbild: grobe Bloecke fuellen F755..F768 grosse Teile des Bilds (`Q_belege/n3_westgang_F753_F768.png`). Helligkeit je Bild F750..F772 mit Spur Q und auf master identisch (41 .. 109 .. 35), also vorbestehend. Der beschleunigte Renderer zeichnet, was die GPU verwerfen wuerde; Entscheidung des Nutzers offen.
- **H3 Riegel `integration_r41_masken36_fern`, Szene d30A0_01 flackert (vorbestehend):** 2 x rot in 4 Laeufen, "3 von 1619". Je Lauf eine von zwei Bildvarianten (16 Punkte, Standbild-Animation mo 200 / mo 211 bei F120). Dieselben zwei Varianten auf der exe ohne Spur Q, ROOM30A0 nicht im Diff. Fuer die Integration: A und B muessen gleiche Variante zeigen oder die Szene braucht festen Stand.
- **H4 SDL-Assertion `WIN_AddDisplay` (exit 42)** riss drei Riegel gleichzeitig ab (integration_r35_karte_fahrstuhl, _cut10f0, _cut1150): Wechsel der Anzeige-Konfiguration der Maschine, diese Riegel setzen kein `SDL_ASSERT=always_ignore`. Einzeln nachgefahren 3/3 gruen.
- H5 Schutz im CPU-Rasterer: die Rechnung im Kopf von `re15_gpu_grenze_r41.h` gilt fuer Fenstergroesse 1, SDL skaliert die Ecken vor dem Rastern (Vorgabe `WINDOW_SCALE` 4). Gemessen 0 von 3 Abstuerzen bei Groesse 4. Schatten-Quader (`render_pc.c:1041`, `:1116`) laufen am Schutz vorbei, Textur klein.
- H6 Kleinkram im Dossier: `PLAETZE` 1060 statt 1062; Zeile `| 6010 | 5 | 132/40 | - |` in `M_masken36.md` nennt noch den Stand vor Spur Q; UEBERGABE 3b nicht nachgezogen (der Bau-Agent nennt es selbst).
- H7 Die Elza-Szenen q8h1e/q8h2e unterscheiden alt und neu nicht (vom Bau-Agenten genannt, bestaetigt durch die Elza-Messung: alt und neu liegen dort dicht beieinander).

## Nicht geprueft
PSX-Bau, volle Suite (nach Auftrag nicht im Spurbaum), Linux-Lauf, Original am Emulator: Teil 8 ist im Original nicht als Maske eingesetzt.
