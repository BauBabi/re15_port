# Runde 34 (Granaten) — BAUPLAN (Synthese der RE-Phase)

Stand: 2026-09-29, Basis master e577e98a. STATUS: ABGESCHLOSSEN (Abschnitte 0..5, KONSTANTEN FUER DEN BAU, PORT-ABGLEICH, OFFEN).
Eingaben (alle vollstaendig gelesen): `re_wurf_flug_explosion.md` + Gegenpruefung, `re_schaden_resolver.md` + Gegenpruefung,
`re_gegner_re2_familie.md` + Gegenpruefung, `re_gegner_bosse_sonstige.md` + Gegenpruefung, `re_saeure_brand.md` + Gegenpruefung,
`re_absturz_original.md` (Fassung 21:22 mit §2.2b) + Gegenpruefung (Urteil "bestaetigt", Praezisierungen A1-A6), `port_inventar_baseline.md`,
`AUFTRAG.md`. Wo eine Gegenpruefung ein Dossier widerlegt, gilt die Gegenpruefung (markiert "GP"). Keine neue Zahl ohne Adresse;
eigene Nachpruefungen dieser Synthese sind mit "(Synthese)" markiert.

## Kurzfassung

* **Der Absturz des Originals ist erklaert** (`re_absturz_original.md` §2.7, dynamisch gemessen): Wurf, Flug, Abprall, Zuender,
  Explosion, Ton-Aufrufe, Licht und Schaden (1000) laufen im Auslieferungsstand. Das Spiel HAENGT erst im Bild nach der Explosion,
  weil die Reaktionszeile 9 der 2D-Gegnerfamilien (Zombies, Zombie-Maedchen, Gitterhaende, G-Birkin) NULL ist
  (STAGE1 `80106c00: jalr v0` mit v0 = 0). Kaputt ist also nur die Gegnerreaktion — dort ist RE2 Retail das Ziel.
* **Zustellung fuer alle drei Granaten = RE1.5**: Wurf (Spawn 0x040D1000, Routinen 30/29), Zuender (Routine 31) und
  Flaechenresolver FUN_80012d60 (Radius 500, Punkt 500 ueber der Liegestelle, alle Gegner UND der Spieler). Fuer 0x0A/0x0B ist
  das eine Port-Zuordnung (einziger Wurfmechanismus beider Spiele; Baenke, Stubs und Records bytegleich), Art 3/4 aus den
  RE1.5-Tabellen.
* **Aufschlag-Inhalt 0x0A/0x0B = RE2** (Op 49 Saeure, Op 48 Brand mit Bodenflammen) ueber eine neue RE2-FX-Maschine; die
  Aufschlagtoene liegen bytegleich in RE1.5 selbst (`SOUND/ARMS10.VB` Saeure, `ARMS11.VB` Brand).
* **Gegnerreaktion**: RE1.5, wo fertig (0x27, 0x29, 0x2b, 0x23, 0x26, Immune); RE2, wo RE1.5 NULL hat oder das NPC-System
  unfertig ist. Fuer RE2-KI-Typen: RE2-Stempel (Zeile, Spalte aus dem Explosionspunkt, 15-Bild-Sperre, Richtung aus dem Punkt).
* **Drei Spuren** mit disjunkten Dateien: A = Engine-Granate (ESP-Routinen, Spielschritt), B = Schaden und Gegnerreaktion,
  C = Plattform (Tick-Reihenfolge, Zeichnen, Licht, Ton, Harness) plus RE2-FX-Maschine und RE2-Assets. Kopplung nur ueber
  festgelegte Vertraege (Abschnitt 3.0).
* **Beschaffung wie im Original**: das eingebaute Item-Debug des Statusschirms (SELECT im Item-Raster, R1 = Id + 1,
  Menge 255, @0x8004a138-0x8004a35c) ist der einzige Original-Weg zu 0x0A/0x0B und laeuft im Auslieferungsstand
  (Absturz-Dossier §2.2b, dynamisch n1). Im Port ist es DEFERRED (`menu_common.c:1258-1260`) → Spur A10.
* **Vor dem Bau offen**: 4 Punkte (RE2-Bezugsrahmen des Saeure-Spritzers, RE2-Boden-/Wasserabfrage der Flammen, RE2-Part-
  Farbwort im Zeichner, Bodenfeuer-Schaden an RE1.5-KI-Typen) — keiner blockiert den Start der drei Spuren, jeder blockiert
  genau benannte Teilpakete (Abschnitt 5).

---

## 0. Entscheidungen vorab (verbindlich fuer den Bau)

| Nr | Entscheidung | Einordnung | Begruendung / Beleg |
|---|---|---|---|
| E1 | Alle drei Granaten nutzen den RE1.5-Wurf: Spawn 0x040D1000 im FSM-Bild 0x13/0x16/0x18 (HOCH/MITTE/TIEF), Routine 30 (Wurf-Init), 29 (Flug/Abprall), 31 (Zuender/Explosion). Das Spawn-Gate `== 9` wird fuer 10/11 erweitert. | 0x09: RE1.5 byte-true. 0x0A/0x0B: Port-Zuordnung | RE1.5 hat den Wurf nur fuer Id 9 (`ori v0,zero,0x9` / `bne v1,v0,0x800337ac` @0x80033688-8c). RE2 hat keine Handgranate (Musterscan 0 Treffer, Wurf-GP §O); die RE2-GL-Runde ist ein Werfergeschoss (Bank 2 Skr. 4; HE = 5 Teilgeschosse @0x80044bf8/c78/cfc/d80/e04) und fuer eine Handgranate unbrauchbar. PL00W09/0A/0B bytegleich (md5 50cf41fd…), Entlade-Handler 0x80033B38/58/78 bytegleich, Waffen-Parametersatz @0x800740b8/bd/c2 je `18 30 0a 01 00` (Saeure-GP §1). |
| E2 | Explosionszeitpunkt fuer alle drei = RE1.5-Zuender (Routine 31: Zuender 42, Explosion bei 7 = Liegen + 36 Bilder). RE2-Flug-Kontakte (Hitcode 0x3000x) und Op 47 werden NICHT benutzt. | 0x09 RE1.5; 0x0A/0x0B Port-Zuordnung | Nur der Zuender gehoert zu einer geworfenen Granate (Saeure-Dossier §3.1). Routine 29 hat keinen Resolver (nur `jal 0x80045024` @0x80018358/@0x80018424, Bosse-GP §11a). |
| E3 | Schadenszustellung = RE1.5-Resolver FUN_80012d60(500, P, Art) genau einmal im Zuender-7-Bild. Art = 2 (0x09), 3 (0x0A), 4 (0x0B). | Art 2 RE1.5 byte-true; Art 3/4 = Port-Zuordnung nur des Arguments a2 | Aufruf @0x80018598-bc; Tabellen @0x8006f418 `0a 00 14 00 e8 03 e8 03 e8 03 …` und @0x8006f430 `03 03 09 0a 0b …` (Art 3/4 = 1000, Reaktion 10/11, ohne Aufrufer — Resolver-GP §1, Saeure-GP §5). |
| E4 | Schadenszahl: **1000 flach** (RE1.5 Art 2/3/4) fuer alle Typen AUSSER denen unter der Port-Option "RE2-Schadens-/HP-Modell" (`re15_re2_model_owns(type) \|\| re15_re15_import_owns(type)`, `re15_damage.c:666-697`). Diese bekommen die BESTEHENDE Modell-Auswahl der Waffen-Id `react_table[Art]` = 9/10/11 → RE2-Zeile 9/11/10, **Klammer 0** (`s_re2_wpn_dmg_*[9..11]`, z. B. Zombie 200 @0x800A41CC/F4/E0). | RE1.5 byte-true bzw. bestehende Nutzer-Option | Nutzer-Auftrag 2026-08-20 im Code-Kopf `re15_damage.c:675-697`: "HP UND SCHADEN GEHEN NUR ZUSAMMEN" — RE2-HP (50..128) mit RE1.5-Schaden 1000 waere genau die verbotene Haelften-Mischung. Die Klammer-0-Auswahl fuer w9/w10/w11 existiert im Modell bereits (`re15_damage.c:581-640`); eine abweichende Klammer-1-Auswahl (Analogon RE2-Op 47/49, Bosse-Dossier §4) waere eine ZWEITE, neue Zuordnung fuer dieselben Waffen-Ids und wird deshalb verworfen. Folge: Spalte wird mit Klammer 0 gestempelt (E6). |
| E5 | Traeger-Semantik bleibt RE1.5: der Resolver trifft ALLE Gegner im Radius und den Spieler (kein Typfilter ausser E7-NPC). RE2 liefert nur Reaktion/Stempel je getroffenem RE2-KI-Typ. | RE1.5 byte-true | Resolver-GP M10; RE2-Op 47 traefe nur den ERSTEN Gegner (Hitcode-Bit 0x10000 = 0, @0x80047208-10, Bosse-GP §11b) — nicht uebernommen. |
| E6 | RE2-Stempel fuer RE2-KI-Typen beim Explosionstreffer: +0x5 = RE2-Zeile (`re2z_row_from_weapon[react_table[Art]]` = 9/11/10) fuer ALLE RE2-Besitztypen (auch Hund/Kraehe/Spinne), +0x6 = 0, Spalte +0x1D2 = Zone aus dem Explosionspunkt P + 3·0, Sperre +0x1D3 = (alt & 0x80) \| 15, Richtungsbits +0x1D0 aus P, KEIN Zonen-Reserve-Abzug. | RE2-Angleichung (RE1.5-Reaktion NULL) | Applier FUN_800470C0: Zone @0x80047294-330, Zeile `sb s5,5(s1)` @0x80047324, Sperre @0x8004732c-4c, Richtung @0x80047358-3d8; GL-Applier schreibt keine Reserve (RE2-GP §4). Die RE1.5-Waffen-Id 10 (Saeure) waere fuer Hund/Spinne sonst RE2-Zeile 10 = BRAND (Vertauschung, RE2-GP §9.2) — deshalb Zeile statt Waffen-Id. |
| E7 | Gegnerreaktion ohne RE2-KI: RE1.5, wo fertig; wo RE1.5 NULL hat (Birkin 0x30/0x36 RE1.5-KI, Writher 0x1a RE1.5-KI, RE1.5-KI-Zombies), bleibt der bestehende Port-Todeshandler der fertigen Zeilen (waffenunabhaengig). NPCs 0x40..0x4D mit HP < 0 sind KEIN Kandidat. Ivy/FX-Emitter bleiben immun. | RE2-Angleichung | RE2-G1-Tod ist waffenunabhaengig (em30 @0x80105b40 / 0x80105e50, Bosse-Dossier §4.1); RE1.5-Birkin nutzt fuer alle fertigen Zeilen denselben Handler (STAGE3 @0x8011ef44 / @0x8011f1e4). RE2-NPCs: HP −1 @0x8005d7b4-bc + Gate 3 `lh v0,342(s0)` / `bltz` @0x80047148-50; RE1.5-NPC-INIT schreibt dieselbe Kennung HP −1 (0x45 @0x8011d320/24). |
| E8 | 0x0A/0x0B im Zuender-7-Bild: Resolver Art 3/4 + Flags 0x61 (unsichtbar) wie 0x09, dann RE2-Aufschlag (0x0A → Op 49 Saeure, 0x0B → Op 48 Brand) an der Granaten-Weltlage. Die HE-Inhalte der Routine 31 (Kinder 0x03195000/0x030B5400/0x030B5800, SE 0x04080001, Licht-Latch) laufen NUR fuer 0x09. Die Zuender-Zeitstruktur (Platz frei bei Zuender 0) bleibt. | RE2-Angleichung (Inhalt) + Port-Zuordnung (Uebergabe) | RE1.5 hat keinen Saeure-/Brand-Aufschlag (Routine 31 liest weder +0x70/+0x71/+0x72, Saeure-GP §6); RE2 Op 48/49 sind die einzigen Vorlagen (Saeure-GP §7-§10). Explizite Tabelle statt "Id − 9" (RE2-GP §9.2, Saeure-GP §15). |
| E9 | Aufschlagtoene 0x0A/0x0B aus RE1.5-Eigendaten: `SOUND/ARMS10` Satz 10 (Saeure) / `ARMS11` Satz 10 (Brand) → Prog 0 Ton 3 → VAG 3, fest (nicht ueber die ARMS-Bank der ausgeruesteten Waffe). | RE2-Aufruf, RE1.5-Daten; Bank-Wahl = Port-Zuordnung | ARMS10.VB md5 39cec979 == RE2 ARMS0B.VB, ARMS11.VB 46833b5e == RE2 ARMS0A.VB; EDH Satz 10 @0x28 `00 00 33 20` (Saeure-GP §12). Die Granatenbaenke ARMS09 = ARMS0A = ARMS0B haben keinen Aufschlagton (nur VAG 2). |
| E10 | ESP-Tick im Port hinter `re15_game_step` (nach Spieler und Gegnern), fuer ALLE Effekte. | RE1.5 byte-true | `jal 0x8001a50c` @0x8001ce04 < `jal 0x80031c44` @0x8001ce0c < `jal 0x80019e20` @0x8001ce2c (Resolver-GP K22). Heute `main.c:5413` vor `main.c:7358` → jede im Spielschritt gespawnte Partikel tickt 1 Bild zu spaet. |
| E11 | Licht-Latch 0x800b5358 wird portiert (Setzer Routine 31 und Routine 9, Leser = Ein-Bild-Licht 2 der aktiven Kamera vor dem Spieler). | RE1.5 byte-true | Leser @0x8001ce5c-0x8001d084, Loeschung @0x8001d1b4 (Wurf-GP §K, Resolver-GP M2). Kein Laerm-Latch: kein Overlay liest ihn. |
| E12 | Sammel-Bodenklemme `re15_esp.c:916-922` greift NICHT fuer Plaetze unter Routine 29/30/31 (Boden = Welt-y > 0 in Routine 29). Fuer die uebrigen Effekte bleibt sie (eigenes Thema). | RE1.5 byte-true fuer die Granate | `lh t1,42(t0)` / `blez t1` @0x80018330-38; der Tick hat keine Klemme (Wurf-Dossier §2.4). |
| E13 | Treppen-Unverwundbarkeit des Spielers wird nachgebaut (+0x93 Bit 0 in den Treppen-Unterzustaenden). | RE1.5 byte-true | Resolver-GP M8/V1: Treppe hoch setzt @0x80038a58 / loescht @0x80038c24, runter @0x80038d00 / @0x80038ec0. Ohne das stirbt der Port-Spieler auf der Treppe ohne Todes-Clip (Resolver-GP P11). |
| E14 | Liegen-SE-Lage = Granatenlage (Original: Stapelrest sp+16) — ohne Folge, weil der Port den Lage-Zweig FUN_80045a64 fuer KEINEN SE fuehrt. | Port-Wahl, gekennzeichnet | Wurf-GP §C (sp+16..27 im Liegen-Pfad unbeschrieben). Lage-Zweig allgemein offen (Pan FUN_80045d6c nicht RE'd). |
| E15 | Hand-Netz: die Granate bleibt nach dem Loslassen in der Hand sichtbar (zwei Granaten bis Clip-Ende), auch bei Menge 0. | RE1.5 byte-true | FUN_8004eae4 entfernt nichts (`sb v0,0(at)` @0x8004eb60), FUN_80036b68 nur bei Raum-Init/Waffenwechsel (Wurf-Dossier §8). Port macht es schon so. |
| E16 | Schadenszahl der Boss-Module: G5 (ROOM5090/5091, RE2-Modul em36, RE2-HP 600) bekommt RE2-Records (Zeile 9/11/10, Klammer 0: 80/70/70) — gleiches "HP und Schaden zusammen"-Prinzip wie E4. Gator-Boss ROOM2090 (Nutzer-Design, HP 3000, Schaden heute ueber RE1.5-Zweig) bleibt bei 1000. | RE2-Angleichung (G5) / Bestand (Gator) | RE1.5 hat keinen G5 (STAGE5 registriert 0x36 nicht, ROOM3080-0x36 = Null-Zeile, Bosse-GP §3/§12); em36 HP `addiu v0,zero,600` @0x801003fc; Records @0x800A5F7C/90/A4 `50 40 01 05` / `46 18 51 00` / `46 18 a1 00`. Gator-Boss nimmt Waffen heute ueber `re15_enemy_take_damage` (enemy_ai_boss_gator.c:737ff) — keine Aenderung. |

---

## 1. Soll-Verhalten je Granate (Zeitlinie, nur bestaetigte Konstanten)

Takt: 1 Spielbild = 2 VBlanks = 30 Hz (`lbu a0,0(s0)` = 2 / `jal 0x80061fc0` @0x8002147c/80; Wurf-GP §L: 75/78 saubere
Spielstaende = 2). Reihenfolge je Spielbild (E10): Gegner (@0x8001ce04) → Spieler inkl. Waffen-FSM (@0x8001ce0c) → ESP-Tick
(@0x8001ce2c) → Item-Modal (@0x8001ce34) → Licht-Latch-Leser (@0x8001ce60) → Zeichnen → Latch loeschen (@0x8001d1b4).
ESP-Tick intern: Schleife 1 = Routine A fuer alle 96 Plaetze (@0x80019e64-c4), dann je Platz: Kind-Init bei Flags&8
(`xori v0,v1,0x9` + Routine A einmal, @0x80019ef4-f30) → Weltlage (@0x8001a118-2a4) → Routine B (@0x8001a2b4-d4) →
Physik falls Flags&0x20 == 0 (Flags NACH Routine B neu gelesen @0x8001a2e8; xlat += vel, DANACH vel += acc, @0x8001a2fc-388)
→ Anim (@0x8001a38c-47c, Flags&0x40 friert den Satz).

### 1.1 Wurf — gemeinsam fuer 0x09 / 0x0A / 0x0B

| Bild | Ereignis | Beleg |
|---|---|---|
| A (Abzug) | FIRE-Init: Munition −1 (Entlade-Handler 0x80033B38/58/78 = nur `jal 0x8004eae4`, `sb v0,0(at)` @0x8004eb60). Clip = 7 + 2·(acaec>>15) + ((acaec>>11)&4) = 9 HOCH / 7 MITTE / 11 TIEF. Kein Schaden (Hitscan-Tester 0x800128A0 fuer 9..11 tot). | @0x800334e0, @0x800334a8-c8, Resolver-GP K23/M9 |
| A..S | Clip laeuft (1 Clipbild je Spielbild). Drehen: Pad links/rechts → Gier um 24 je Bild (Satz @0x800740b8 Byte1 0x30, `srl 1`). R1 los UND Clipbild > 10 (Byte2 0x0a) → 0x800aca5a := 3, KEIN Spawn (Munition schon weg). | @0x8003355c-0x80033600, @0x80033604-50 (Wurf-GP N7/N8) |
| S = A+19 / A+22 / A+24 | Spawn 0x040D1000 (Effekt 4, sub 0x0D, Skala 0x1000) im Waffen-FSM, nur wenn Clipbild == 0x13/0x16/0x18 UND Zielbit 0x8000/0x4000/0x2000. Anker = Teil-11-Weltmatrix des VORBILDS ([0x800acbdc]+0x7a4) × Versatz {0,300,800} / {0,0,500} / {0,0,300}; Gier = Spieler +0x6a → Platz +0x2e. Kein Nachfuehren (Flags 3 ohne Bit 2). Pool voll → keine Granate. | @0x80033688-0x800337a8, @0x800197c8-cc, Wurf-GP F.2 |

Zeitlinie ab Spawnbild S (= Bild 0). Welt-y(k) = h + Summe vy; Routine 29 prueft Welt-y > 0 VOR der Integration von Bild k.
**h (Spawnhoehe) ist eine Pose-Groesse, keine Code-Konstante**; die Werte unten sind PORT-MESSUNGEN (Renderer-Knochen 11,
ROOM1140, Wurf-Dossier §7) und dienen als Abnahme-Anker fuer den Port, nicht als Original-Wert. Der Simulator
(`re_wurf_gegen_werkzeug/wurf_sim_gegen.py`) liefert die Zeitlinie fuer jedes h; Wurf-GP §C.2 bestaetigt die Tabelle exakt.

| Bild | Ereignis | Beleg |
|---|---|---|
| 0 | Schleife 1, Routine 30: Anim-Satz := 23, Flags := 3, Routine B := 29, A := 0, Zuender +0x1e := 42, v/acc je Zielhoehe (HOCH (380,−110,21) acc_x −2; MITTE (280,−50,24) acc_x −1; TIEF (80,0,1) acc_x −1 und Zaehler 5), Zaehler +0x26 = ((a + ((a>>7)&0xff)) & 3) + 7 mit a = u16 0x800acaec (gesund 7, Gift-Bit 0x2 → 9). Hauptlauf: Weltlage = Spawnpunkt, Routine 29 (y ≤ 0 → nichts), Physik, Satz 23 wird gezeichnet. | @0x8001843c-44, RNG @0x8001af30-4c |
| 1.. | Flug: 12-Bilder-Taumeln (Saetze 23..34, Satz 35 = Schleife auf 23), Sprite CLUT 0x7B11 (oliv, VRAM (272,492)), deckend. vz wird nie gedaempft (Seitendrift). Keine Wand-, Gegner- oder Spieler-Kollision. | CORE00.ESP @0x17E8..0x1848, @0x8001987c-88 |
| Kontakt (Welt-y > 0), Zaehler > 0 | Abprall: vx −= trunc(vx/3), vy := −trunc(vy/3), xlat_y −= Welt-y, Zaehler −1, SE 0x010A0001 \| (n<<8) an der Eindringstelle (Byte1 wirkungslos → immer ARMS-Satz 0x0A). | @0x8001834c-0x80018428 |
| Kontakt, Zaehler == 0 → L (Liegen) | SE 0x010A0001, Flags := 0x63 (Physik- und Bild-Stopp), A := 31, B := 0, KEINE y-Korrektur (steckt 3..20 im Boden). | @0x80018350-84 |
| L+1 .. L+35 | Routine 31 zaehlt den Zuender 42 → 8 (je Bild −1 @0x80018668-84). | @0x8001854c-0x800186dc |
| **X = L+36** | Zuender == 7: EXPLOSION (Inhalt je Granate 1.2 bis 1.4). | @0x8001856c-70 |

| Fall (Port-h) | 1. Kontakt | Kontakte (= SEs) | L | X | Zuender 2 | frei | Weg lokal x / z bis L | X ab Abzug |
|---|---|---|---|---|---|---|---|---|
| MITTE gesund (h −2474) | 29 | 8 (29,47,55,60,64,67,70 + Liegen 73) | 73 | **109** | 114 | 116 | 11929 / 1752 | A+131 |
| HOCH gesund (h −3171) | 40 | 8 | 88 | **124** | 129 | 131 | 18760 / 1848 | A+143 |
| TIEF gesund (h −772) | 13 | 6 | 40 | **76** | 81 | 83 | 1543 / 40 | A+100 |
| HOCH vergiftet | 40 | 10 | 94 | 130 | 135 | 137 | 18721 / 1974 | A+149 |
| MITTE vergiftet | 29 | 10 | 79 | 115 | 120 | 122 | 11938 / 1896 | A+137 |

SE-Folge MITTE gesund (Synthese aus @0x800183d0-28 / @0x80018350-58): Zaehler 7 → je Abprall −1, dann SE mit n = neuer
Zaehler: 0x010A0601, 0x010A0501, 0x010A0401, 0x010A0301, 0x010A0201, 0x010A0101, 0x010A0001 (7 Abpraller), Liegen 0x010A0001.
Wurfweiten bis ~18800: HOCH/MITTE fliegen durch Waende (keine Wandkollision, Original). Boden = Ebene Welt-y 0; in Raeumen mit
anderem Boden faellt die Granate bis y 0 (Original, Wurf-Dossier §2.4). Weltlage +0x28/+0x2a/+0x2c ist s16 (`sh`/`lh`).
Raumwechsel loescht alle 96 Plaetze (FUN_80019354 `sb zero` @0x80019378, gerufen @0x8003996c): eine liegende Granate
verschwindet ohne Explosion und ohne Schaden.

### 1.2 Explosion 0x09 Hand Grenade (RE1.5 byte-true)

| Bild | Ereignis | Beleg |
|---|---|---|
| X (ESP-Schleife 1) | Latch 0x800b5358 := 1; Flags := 0x61 (Granate UNSICHTBAR); P = (x, Welt-y − 500, z) als s32; **FUN_80012d60(500, &P, 2)** (1.5); Kind 0x03195000 (Effekt 3 sub 0x19 Skala 0x5000, Anker Einheitsmatrix 0x80072d4c + Versatz P + Gier +0x2e, Flags 0x0a); SE 0x04080001 an P; Zuender → 6. | @0x80018574-0x800185f0, @0x80019a88 |
| X (Hauptlauf) | Kind-Init (Flags 0x0a → ^9 = 3) + Routine 10 einmal: Flags := 0x13 (ABE), TPAGE \|= 0 (ABR 0 halb/halb), Anim := 10, Zeilenvorschub → steht still; wird IM SELBEN BILD gezeichnet (CLUT 0x78D1, weiss-gelb → rot, 6..16 Zellen je Satz). | @0x80019ef4-30, Routine 10 @0x800176b0-0x80017704, CORE00.ESP @0x3F4 |
| X (nach dem ESP-Tick) | Licht 2 der aktiven Kamera fuer DIESES Bild: Typ 0, Farbe max(alt, 0xD2/0x8C/0x50), Lage = Spieler + RotY(Spieler-Gier)·(1200,·,0), y = Spieler-y − 800, Helligkeit 0x1770; nach dem Zeichnen zurueckgestellt, Latch := 0. Das Licht sitzt VOR LEON, nicht an der Granate. | @0x8001ce5c-0x8001d084, @0x8001d1ac, @0x8001d1b4 |
| X+1 | Getroffene Gegner reagieren (die Gegner-Schleife laeuft vor dem ESP-Tick und liest +0x4/+0x5/+0x6 aus Bild X); getroffener Spieler: Modus 3 → Clip 7 (PL00-Basisbank) + SE 0x04030001 → spaeter Modus 7. | @0x8001ce04, Tod @0x800366bc-0x80036814 |
| X+5 (Zuender 2) | Feuerball #2 (0x03195000) + Rauch #1 (0x030B5400: Effekt 3 sub 0x0B, CLUT 0x7851, TPAGE \|= 0x40 → ABR 2 abdunkelnd, nach Vorschub vel_y −135 / acc_y +5 → steigt), beide an P. | @0x80018600-64, CORE00.ESP @0x4CC / @0x4F4 |
| X+7 (Zuender 0) | `sb zero,108(a1)` (Platz frei) VOR dem Spawn von Rauch #2 (0x030B5800) — der Rauch kann den Granatenplatz belegen. | @0x80018688-cc (Wurf-GP §D) |
| Sichtbar | Feuerball #1 X..X+12, #2 X+5..X+17 (13 Bilder, Satz 23 = Ende); Rauch #1 X+5..X+19, #2 X+7..X+21 (15 Bilder). Kein Bildschirmwackeln, kein Rumble. | CORE00.ESP Saetze @0x60..0xC8 |

### 1.3 0x0A Acid Grenade (Wurf wie 1.1; Aufschlag = RE2 Op 49; Uebergabe = Port-Zuordnung E8)

| Bild | Ereignis | Beleg |
|---|---|---|
| X (ESP) | Flags := 0x61, P wie 1.2, **FUN_80012d60(500, &P, 3)** (1000 bzw. E4-Modellwert, Reaktion 10). KEIN HE-Kind, KEIN SE 0x04080001, KEIN Licht-Latch. Aufschlag-Uebergabe an die RE2-FX-Maschine mit Q = Granaten-Weltlage (+0x28/+0x2a/+0x2c), Gier = +0x2e, Lebensdauer ≠ 255 (kein Wasser). | E3/E8 |
| X (RE2-FX, Op 49 Phase 0) | Phase := 1, Status := 0x8403, Op A := 0, Op B := 49; SE 0x01130001 → ARMS10 Satz 10 (VAG 3, E9); [RE2-Hitcodes 0x1002000B bei y / y+1800 werden NICHT benutzt, E3]; Spritzer: vel.y := 240, vel.x := 0, acc.x := −23, acc.y := 0 (Bezugsrahmen Bit 0x400 → O-VB1); Kinder 0x030F2000, 0x040C2000, 0x041D1800 (a1 = +0x22, a2 = Einheitsmatrix 0x8009DB44, a3 = Platz+0x34). | RE2 @0x80021678-0x800217d0, Saeure-GP §9 |
| X+1 .. X+4 | Phase 1: Kind 0x031F2000; Phase 2: 0x03142000; Phase 3: 0x040D2800; Phase 4: 0x030F2000, dann Op A / Op B / Status := 0 (Platz frei). Kein Nachwirken, kein Bodeneffekt. | RE2 @0x800217f0-0x80021958 |
| X+1 | Getroffene Gegner reagieren (Zeile 11 Saeure fuer RE2-KI-Typen, E6). | — |
| X+7 | Granatenplatz frei (Zuender 0, ohne Kind). | @0x800186b0 |

Kind-Skripte (RE2 CORE00.ESP; Synthese aus `re_saeure_brand_werkzeug/re2_core00_scripts.txt`): alle nur Op 1 → Op 0 plus
Physik/Anim — Bank 3 Skr. 7 @0x05B4 (Op 1 Anim 11 → Op 0 acc (0,−18,0)), Bank 3 Skr. 4 @0x050C (Op 1 Anim 38 → Op 0),
Bank 4 Skr. 4 @0x2078 (Op 1 Anim 3 → Op 0 acc (0,−8,0)), Bank 4 Skr. 5 @0x20B0 (Op 1 Anim 32, Aspekt 6656 → Op 0).
CLUT = Kopf-CLUT + (sub>>3)·0x40 (Bank 3/4 Kopf 0x7811 → Zeilen 481..483), Status 0xB003 (sichtbar, Anim, Physik).

### 1.4 0x0B Incendiary Grenade (Wurf wie 1.1; Aufschlag = RE2 Op 48 + Bodenfeuer)

| Bild | Ereignis | Beleg |
|---|---|---|
| X (ESP) | wie 1.3, aber **FUN_80012d60(500, &P, 4)** (Reaktion 11) und Aufschlag Brand. | E3/E8 |
| X (RE2-FX, Op 48 Phase 0) | Translation +0x60/64/68 := Q; Status := 0x8000; Op A := 0; Phase := 1; Op B := 48; SE 0x01120001 → ARMS11 Satz 10 (VAG 3, E9); [Hitcodes 0x0002000A ×2 NICHT benutzt]; Kinder 0x040C2800 und 0x041D2700 (a1 = 0, a2 = Platz+0x4C, a3 = 0); IMMER drei Bodenflammen 0x0505xxxx: Skala 7168 + (rng%8)·768, Gier + rng%40 / + rng%80 + 400 / + rng%80 − 400, je Flamme vel.x += rng%25, acc.y += rng%8, +0x4A := 1. | RE2 @0x80020f3c-0x800214e0, Saeure-GP §10 (Bodenflammen IMMER) |
| X+1 | Op-48-Platz frei (Phase 1 @0x800215a4). Gegner reagieren (Zeile 10 Brand). | — |
| X+7 | Granatenplatz frei. | @0x800186b0 |

Bodenflamme (Bank 5 Skr. 5 @0x07F8, Step `00 1b 2e 32 00 10 00 10 00 05 00 00 60 00 …`):
* Op 27 (Start): Status 0xB003, TPage \|= 0x20, Anim rng%3, +0x14 := FUN_8004fba0(P,2,8192,0) (Boden, O-VB2), Op A := 58,
  Op B := 28, +0x1B := 2, Zaehler +0x42 := 8 + rng%3 (@0x8001fa9c-bb8).
* Luft: Op 58 waechst ×1010/1000 · ×1007/1000 bis Zaehler 0, dann +0x1B := 1, Zaehler 2 + rng%2, schrumpft ×880/1000 ·
  ×800/1000, dann tot (@0x80022254-3e4). Op 28 faellt (acc.y 5 + rng%8), Wasser → tot, Boden bei y−900 → Op 46, Wand → Op 46
  oder Op 50 (@0x8001fbd0-d34).
* Op 46 (Landung): Op A 19, Op B 29, vel.x 180, acc.x −10 − rng%11, Zaehler 38 + rng%8, +0x1B NICHT gesetzt (@0x80020b60-c24).
* Op 19 (brennt): Zustand 2 ×1009/×1002 bis Zaehler 0 → Zustand 1, Zaehler 90 + rng%11, ×990/×980 → 0 tot (@0x8001f2c0-51c).
  Dauer bei Landung in Zustand 2: (38..45) + (90..100) Bilder; Landung im Luft-Schrumpfen: nur 38..45 (Saeure-GP §11).
* Schaden (Op 40): nur wenn +0x4A != 0 UND step[0x16] ≥ 16 UND X-Aspekt > 0x1000 (step[0x16]++ nur in diesem Zweig):
  Box {−600,0,300,150} (@0x80010910), Pruefpunkt y − 100, Hitcode 0x2002000A (Zeile 10, Klammer 2, erster ungesperrter
  Gegner, 15-Bild-Sperre); Treffer → Op 50 (Gleiten aus: step[2] := 64, acc.x/vel.x := 0, Step-Status &= ~2), Op A 19 laeuft
  weiter (@0x80020758-7cc, @0x80021970-b0). Der Spieler wird vom RE2-Applier nie getroffen (Liste = Gegner, RE2-GP E1/§9).
* Op 29 (gleitet): vel.x ≤ 0 → Stopp; solange vel.x ≥ 61 und step[2] % 15 == 0: Folgeflamme 0x0504xxxx (Skala ×0.8,
  +0x4A := 1) → Bank 5 Skr. 4 (Op 30: +0x1B := 2, Zaehler 2 + rng%8 → Op A 19 / Op B 25 Wassertest) (@0x8001fd5c-eb4,
  @0x8001fecc-f84, @0x8001fa08).
* RNG = RE2 FUN_80015FE8 (0..255, `andi v0,v0,0xff` @0x80016014).

### 1.5 Treffer-Regeln der Explosion (Resolver FUN_80012d60, fuer alle drei)

* Kandidaten: alle aktiven Gegner (Wort 0 Bit 0), dann IMMER der Spieler; Anwendung rueckwaerts (@0x80012d68-0x8001302c).
* Abstand (FUN_8002b5d0): R = r_Ziel + 500; |dx|,|dz| ≤ R, dann sqrt(dx²+dz²) < R (streng); Hoehenband |P.y − (Y + O.y)| < 500 + h
  (streng) (@0x8002b6fc-7ac). r bei Sektor-Kaesten (hb[6] ≠ hb[10]) winkelabhaengig aus ratan2(P − Mitte) − roher Gier +0x6a
  (@0x8002b61c-6f8). Mitte = Lage + Versatz O; O = RotY(+0x6a)·(Box.x, Box.z), O.y = Box.y, jedes Bild von FUN_8002b498 gesetzt
  (@0x8002b4bc-51c).
* Gate A (@0x80012f38-4c) schliesst bei der Granate niemanden aus (Platz+0x74 = Spieler-Handmatrix). Gate B: (+0x93 & 3) == 3 →
  ueberspringen, NICHT gezaehlt, Bit 0x80 bleibt (@0x80012f54-60). Dann +0x93 &= 1, Punkt hinten → \|= 0x80 (FUN_8001a7a8),
  Bit 0 schon gesetzt → \|= 2 und KEIN Schaden; sonst +0x07 := 0, +0x06 := 1, +0x05 := DAT_8006f430[Art], HP −= DAT_8006f418[Art],
  +0x93 \|= 1, +0x04 := 2, bei HP < 0 (signiert; HP == 0 lebt) := 3 (@0x80012f7c-80013020). Art ≥ 2: kein SE 10, kein Gift-Wurf.
* Geparkte Gegner (+9 & 0x20) sind Kandidaten — der Resolver prueft nur Wort 0 Bit 0 und den Kasten; sie reagieren erst,
  wenn ihre Wurzel wieder tickt (Zombie-Wurzel `8010045c` +9 & 0x20; Gorilla vc2: getroffen, ohne Tick keine Reaktion;
  Absturz-GP A4). Der Port-Resolver verhaelt sich schon so (kein Park-Test in `re15_resolve_attack`).
* Spieler (r 450, h 1530, O (0,−1530,0)): getroffen bei waagrecht < 950 UND −3560 < P.y − Spieler-y < +500; Bit 0 frei
  vorausgesetzt (Treffer-Taumel, Griff, Tod, Treppe/Klettern/Springen halten es, E13). HP 100 − 1000 → Modus 3 / Tod
  (@0x80012e18-efc, HP-Start @0x80031710).

### 1.6 Gegnerreaktion je Typ nach dem Bau

| Typ | KI im Port | Schaden (E4/E16) | Reaktion (Soll) | Einordnung |
|---|---|---|---|---|
| Zombie 0x10/0x11/0x12/0x13/0x18 | RE2 (Default) | RE2 Zeile 9/11/10 Kl. 0 = 200/200/200 | Stehend, Granate auf gleichem Boden → Zone 0 (Beine, P.y > Y − 750). Tod: HE DEATH[9][0] = 0x80107438 (Knockdown-Tod, Clip 1, Rate 15); Saeure/Brand DEATH[11/10][0] = 0x80108530 (Sturz-Tod, Clip 1 ab 0 / Clip 2 ab 10, Rate 15) + Aetzung/Verkohlung ueber die DEATH-MAIN-Leiter. Brad 0x11 (HP 250) lebt: HE HURT[9][0] = 0x80107438; Saeure/Brand HURT[11/10][0] = 0x80105BC0 Taumeln + Aetzung (+ Bein wegaetzen) bzw. Verkohlung + DoT 1 HP je 8 Bilder im Gang (EXEC[1]/EXEC[2]), Tod 0x0B03/0x0A03. | RE2-Angleichung |
| Zombie 0x16 | RE2 | 80/200/80 | wie oben (HP-abhaengig HURT/DEATH) | RE2-Angleichung |
| Zombie-Familie | RE1.5-KI (Flavor re15) | Import AN (Default): wie RE2-Modell; Import AUS: 1000 | Port-Standardtod `re15_enemy_ai_live_death` (Clip 0x0b/0x0d) — Original-Zeile 9/10/11 NULL. Ein RE1.5-Typ 0x18 mit HP 1058 (HP-Zeile Index 3, `read 0x8011f334 16`) ueberlebt 1000 → HURT (Original ebenfalls NULL → Haenger, Absturz-GP A1) → Port-HURT | E7 |
| Hund 0x20 | RE2 | 300 → Tod | Zeile 9 Kl. 0: zerplatzt (0x80104694: stumm, Teile-Wurf 0x80104440, 1× FX 7, Blut je Bild); Zeile 10: verkohlt (17 Parts 0x00202020, 6× FX 7); Zeile 11: geaetzt (17 Parts 0x00003F2F, FX 9/10) | RE2-Angleichung |
| Hund 0x20 | RE1.5-KI | 1000 | RE1.5 1D-DEATH 0x80121070[9] = 0x80110eb0 (fertig) | RE1.5 byte-true |
| Kraehe 0x21 | RE2 | 60 → Tod | GIB 0x80102CA0 (alle drei Zeilen) | RE2-Angleichung |
| Spinne 0x25 | RE2 | 60/90/130 (HP 99..132) | HE/Saeure meist HURT (Zeile 9: Knock + Blut; Zeile 11: Bein ab, Gates +0x221 ≥ 3 und Bein vorhanden), Brand meist Tod (Parts 0x00202F2F); Tod Zeile 9 = Zerplatzen + 6..9 Babys, Zeile 11 = Parts 0x00101F3F + Part 19 fliegt (Gate +0x6 == 0 && +0x224 == 0) | RE2-Angleichung |
| Baby 0x26 (RE2-Herkunft) | RE2 | HP 1 → Tod | RE2-Tod | RE2-Angleichung |
| Feuer 0x26 (ROOM1090) | RE1.5 | 1000 (HP ungelesen) | Flammen-Minderung 0x80116a04 wie jeder Treffer (Port deckungsgleich) | RE1.5 byte-true |
| Made/Gorilla 0x27 | RE1.5 | 1000 → Tod | 0x8011bb9c: Clip 10 (von hinten 11), Rueckstoss (rng&31)+80 → 0x50 − 4·(+0x9c), von hinten netto ~0, SE 0, Blut, Leiche | RE1.5 byte-true (im Original dynamisch gemessen) |
| Kakerlake 0x29 | RE1.5 | 1000 → Tod | 0x801154b4: Clip 10/11, SE 7, Rueckstoss wie 0x27, Blut, Leiche | RE1.5 byte-true (Port NEU) |
| Tyrant 0x2b | RE1.5 | 1000 → Tod | 0x80114cb0: Ph.0 +0x93 \|= 2, Clip 8/9; Bild 24 SE 7; Ph.2 Clip 0xa bzw. 0xb NUR bei +0x93 & 0x80 | RE1.5 byte-true (Port-Bit-Fix) |
| Alligator 0x23 (nicht 2090) | RE1.5 | 1000 → Tod | 0x8010ea30: Clip 13, Blut an (+0x188)+2644, Bild 20 → Ph.2, Leiche; Kasten-Versatz (1000,−720,0) gedreht | RE1.5 byte-true (Port NEU) |
| Gator-Boss (2090) | Port-Modul | 1000 (Bestand) | Modul-Absorb (Nutzer-Design) — unveraendert | Bestand |
| Birkin 0x30 / 0x36 (ROOM3080) | RE1.5-KI | 1000 → Tod | Port-Todeshandler 0x8011a5d8 (Mutations-Schutz HP 50) — Original NULL | E7 |
| G5 0x36 (5090/5091) | RE2-Modul | RE2 80/70/70 | Flinch-Zuschlag 14 je Treffer-ZEILE, Schwelle 15 → STAGGER, Zerfall −1 je 16 Bilder mit Fenster 15, Sperre 15 | E16 |
| Writher 0x1a | RE1.5-KI / RE2-Zellenarm | 1000 | RE1.5-KI: Port-Tod Clip 3 → Leiche (Original NULL, E7); RE2: Rueckzug, unsterblich (zeilenunabhaengig) | E7 / RE2 |
| Ivy 0x2d / FX 0x24 | RE1.5 | — | immun (+0x93 = 3 → Gate B; +0x93 = 1 → \|= 2 ohne Schaden, dann Gate B) | RE1.5 byte-true |
| NPC 0x40..0x4D | RE1.5 NPC | — | KEIN Kandidat (HP −1) | E7 |
| Liegender Fresser 0x16 ROOM1140 | RE1.5-KI | — | +0x93 = 1 in Ruhe → nur \|= 2, KEIN Schaden (16/16 Saves) | RE1.5 byte-true |
| Liegender Fresser 0x16 ROOM1140 | RE2-KI | wie Zombie | Port-RE2-Bruecke macht die Spawn-Pose treffbar (`enemy_ai_re2_zombie.c:8809-8815`) → stirbt; bewusste RE2-KI-Semantik des Ports, bleibt | Bestand |

---

## 2. Einordnung je Teil

"laeuft" = im Auslieferungsstand dynamisch ohne Absturz gemessen (`re_absturz_original.md` §2.4-2.10, MZD-Disc = Auslieferung
bis auf CAPCOM.STR). Regel (Memory reai-v2-beta-zu-retail): RE1.5 fertig → RE1.5 massgeblich; RE1.5 nachweislich unfertig →
RE2 Retail; weder noch → Port-Zuordnung mit Begruendung.

| Teil | Einordnung | Begruendung | Beleg |
|---|---|---|---|
| Wurf 0x09 (Spawn, R30, R29, Abprall, Liegen) | **RE1.5 byte-true** | vollstaendig im Code, keine Stub-Zweige; laeuft (va_a_01..04: B = 29, Zaehler 5 → 4 → 3, Liegen A = 31, Flags 0x63) | Wurf-Dossier §1, Absturz §2.4 |
| Zuender + Explosion 0x09 (R31, Kinder, SE, Licht) | **RE1.5 byte-true** | laeuft (va_b_01: 4 Kinder aktiv, Platz frei; Leon im Standbild orange angestrahlt) | Absturz §2.4, §3 |
| Flaechenschaden (Resolver, 1000, Reaktion 9) | **RE1.5 byte-true** | laeuft: Zombie 81 → −919, 105 → −895, Gorilla 180 → −820, Spieler 100 → −900 (je genau −1000) | Absturz §4 |
| Spieler-Eigenschaden / Tod | **RE1.5 byte-true** | laeuft (vs: Modus 3, Clip 7, Modus 7) | Absturz §2.8 |
| Treppen-/Kletter-i-Frames | **RE1.5 byte-true** | Setzer/Loescher in den Modus-1-Unterzustaenden 9..13 | Resolver-GP M8 |
| Reaktion 2D-Familien (Zombie, 0x13, 0x1a, 0x30/0x36) | **unfertig → RE2** (RE2-KI) bzw. Port-Standardtod (RE1.5-KI, E7) | Zeile 9 (und 10/11) NULL → `jalr` nach 0 → Haenger (gemessen: EPC 0, RA 0x80106c08) | Absturz §1.4/§2.7, Resolver-GP K24, Bosse-GP §8/§9 |
| Reaktion 1D-Familien RE1.5 (0x27, 0x29, 0x2b, 0x23, 0x26; Hund/Kraehe/Spinne im RE1.5-Flavor) | **RE1.5 byte-true** | eigene Eintraege fuer 9/10/11; Gorilla-Granatentod dynamisch gemessen (vc3) | Absturz §1.5/§2.9, Bosse-GP §4-§7 |
| NPC-Treffer | **unfertig → RE2** (kein Kandidat) | RE1.5: laufender Clip einmal, dann Stillstand in Zustand 3 (0x80050f00 kehrt bei +6 == 2 sofort zurueck), kein Weg ins Skript | Resolver-GP M11, Bosse-GP §10 |
| Ivy / FX-Emitter | **RE1.5 byte-true** (immun) | +0x93 = 3 / 1 | Bosse-GP §9 |
| Wurf + Zuender fuer 0x0A/0x0B | **Port-Zuordnung** | RE1.5 unfertig (Gate `== 9`, Stubs), RE2 hat keinen Handgranatenwurf; der RE1.5-Wurf ist der einzige Mechanismus beider Spiele; Bank, Handler, Parametersatz bytegleich | E1/E2 |
| Schaden 0x0A/0x0B | **RE1.5-Daten, Port-Zuordnung nur des Arguments** (Art 3/4) | Resolver behandelt Art 3/4 vollstaendig, es fehlt nur der Aufrufer | E3 |
| Aufschlag-Effekt 0x0A/0x0B + Bodenfeuer | **RE2-Angleichung** | RE1.5 hat keine Row-Menge und keine Routine dafuer (Routine 31 nur HE-Konstanten) | Saeure-GP §6, §7-§11 |
| Aufschlagton 0x0A/0x0B | **RE2-Aufruf mit RE1.5-Daten** | ARMS10/ARMS11 bytegleich RE2 ARMS0B/0A | E9 |
| Uebergabe Granatenplatz → RE2-FX-Platz (Lage Q, Gier, Lebensdauer ≠ 255, Matrix) | **Port-Zuordnung** | weder RE1.5 (kein Aufschlag) noch RE2 (Runde traegt Waffenknochen-Matrix) haben ein Pendant | E8, O-VB1 |
| RE2-Stempel (Zeile, Zone aus P, Sperre, Richtung) | **RE2-Angleichung**; Klammer 0 = bestehende Modell-Auswahl | Applier FUN_800470C0; Klammer siehe E4 | E4/E6 |
| Schadenszahl RE2-KI-Typen | **bestehende Port-Option** (RE2-Schadens-/HP-Modell) | Nutzer-Auftrag 2026-08-20 | E4 |
| G5 Schaden/Flinch/Zerfall/Sperre | **RE2-Angleichung** | RE1.5 hat keinen G5 | E16 |
| Bodenfeuer-Schaden an RE2-KI-Typen | **RE2-Angleichung** | Op 40 → Applier | 1.4 |
| Bodenfeuer-Schaden an RE1.5-KI-Typen | **OFFEN** (keine Quelle) | O-VB4 | — |
| Tick-Reihenfolge, Licht-Latch, CLUT-Farben, Kind-Flags 0x0a, Weltlage mit Drehung | **RE1.5 byte-true** | E10/E11, Wurf-GP §F/§G/§H.2 | — |
| Liegen-SE-Lage | **Port-Wahl** | Original = Stapelrest | E14 |
| Farbe der Granate in der Hand / im Flug fuer 0x0A/0x0B | **RE1.5** (oliv, gleich wie 0x09) | Baenke bytegleich; RE2 hat keine Wurfgranate | Saeure-GP §16.10 |
| Beschaffung 0x09/0x0A/0x0B: Item-Debug des Statusschirms (SELECT im Item-Raster; R1/L1 ±1, R2/L2 ±10; Menge 255; Kreis beendet) | **RE1.5 byte-true** (Port heute DEFERRED, `menu_common.c:1258-1260`) | laeuft im Original ohne RAM-Eingriff (n1: SELECT + 9× R1 → Platz `09 ff 00 00`, der Schliess-Commit ruestet aus; Acid 10×, Incendiary 11×); einziger Original-Weg zu 0x0A/0x0B | Absturz §2.2b, Absturz-GP §4.1 → Spur A10 |

---

## 3. Arbeitspakete in drei Spuren

### 3.0 Vertraege zwischen den Spuren (vor dem Start festgelegt, keine Spur weicht ab)

| Vertrag | Anbieter → Nutzer | Inhalt |
|---|---|---|
| V1 `include/re15_esp.h` | A → C | (a) `re15_esp_fx_t` erhaelt `int16_t wpos[3]` = Platz +0x28/+0x2a/+0x2c (s16, je Tick nach @0x8001a118-2a4 gerechnet); der Zeichner nimmt wpos statt `x + xlat_x`. (b) `extern uint8_t g_re15_licht_latch;` = 0x800b5358 (gesetzt von Routine 9 und 31, gelesen und geloescht vom Plattform-Zeichner). (c) `extern void (*re15_esp_se_hook)(uint32_t code, const int32_t pos[3]);` = FUN_80045024-Analogon (Byte3 Bank, Byte2 Satz, Byte0 Lage-Flag, Byte1 wirkungslos). (d) `extern void (*re15_esp_aufschlag_hook)(int re2_art, const int32_t q[3], int16_t gier);` mit re2_art 2 = Saeure (Op 49), 1 = Brand (Op 48). (e) Einstieg `re15_esp_fx_tick(bank)` bleibt. |
| V2 `include/re15_damage.h` | B → A, C | (a) `re15_resolve_attack(&box, art, -1)` Signatur unveraendert (A ruft mit box = {P.x, P.y, P.z, 500}); B aendert nur das Innere. (b) NEU `int re15_re2_gl_apply(const int32_t p[3], int16_t gier, const int16_t box[4], uint32_t hitcode);` = FUN_800470C0-Zwilling (C ruft aus Op 40 ueber einen Funktionszeiger, siehe V3). |
| V3 `include/re2_fx.h` (NEU) | C → C | `re2fx_register_core()`, `re2fx_aufschlag(re2_art, q, gier)`, `re2fx_tick()`, Slot-Zugriff fuer den Zeichner, `extern int (*re2fx_applier)(…)` (Signatur wie V2b, Vorgabe NULL; `main.c` bindet ihn an `re15_re2_gl_apply`, sobald Spur B gemergt ist). Nur Spur C inkludiert den Kopf. |
| V4 `re15_actor_t.re2z_part_tint[16]` (besteht, `re15_actor.h:519`) | B → C | B schreibt die Part-Farbwoerter auch fuer Hund und Spinne (und den Leichen-Ausblender); C setzt sie im Zeichner um (nach O-VB3). |
| V5 Harness | C → A, B | `RE15_STATE_LOG` mit `hp=` je Gegner und Spieler; `RE15_EQUIP` laedt die ARMS-Bank (`re15_audio_prime_weapon`); `RE15_FX_LOG` mit wpos, Routine A/B, Zuender +0x1e, Zaehler +0x26, Flags. Als ERSTES Teilpaket von C gemergt (C0). |

Datei-Eigentum (disjunkt; nur der Eigentuemer schreibt):
* **A**: `engine/src/re15_esp.c`, `include/re15_esp.h`, `engine/src/game_step_common.c`, `engine/src/player_common.c`,
  `engine/src/menu_common.c` (Item-Debug A10), `engine/src/re15_inv_screen.c` (nur falls die Item-Bildanzeige des Debugs dort
  liegt), `tests/unit/probe_r34_wurf.c`, `tests/unit/probes/r34_wurf.cmake`.
* **B**: `engine/src/re15_damage.c`, `include/re15_damage.h`, `engine/src/enemy_ai_re2_zombie.c`, `engine/src/enemy_ai_re2_dog.c`,
  `engine/src/enemy_ai_re2_spider.c`, `engine/src/enemy_ai_common.c`, `engine/src/enemy_ai_boss_g5.c`, `engine/src/stair_common.c`,
  `include/re15_actor.h`, `include/re15_ai_flavor.h`, `RE15_FUN_CATALOG.md`, `tests/unit/probe_r34_schaden.c`,
  `tests/unit/probe_r34_reaktion.c`, `tests/unit/probes/r34_schaden.cmake`, bestehende Tests, deren Erwartung sich durch B aendert.
* **C**: `platform/pc/main.c`, `platform/pc/src/audio_pc.c`, `platform/pc/src/render_pc.c`, `engine/src/re2_fx.c` (neu),
  `include/re2_fx.h` (neu), `shared_assets/RE2/CORE00.ESP` + `shared_assets/RE2/TEX.TIM` (neu, Kopien aus `info/re2leon/COMMON/DATA`),
  `tools/` (neue Schnitt-Werkzeuge), `tests/unit/probe_r34_re2fx.c`, `tests/unit/probes/r34_re2fx.cmake`,
  `tests/integration/test_r34_granaten.cmake`, `tests/unit/probes/r34_granaten_exe.cmake`.
* **Integration (eine Hand, zuletzt)**: `tools/local_build.sh` (RE15_MIN_TESTS), Bindung `re2fx_applier` in `main.c` falls C vor B
  fertig war, Nachzug verschobener Pins mit belegtem Grund. `tests/test_support.c` bleibt unberuehrt (Hooks werden in den Sonden
  selbst auf Spione gesetzt).

Gemeinsame Regeln fuer alle Spuren: eigener Worktree auf aktuellem master (`git worktree list`, `merge --ff-only master`), eigenes
Bauverzeichnis `re15_port/build_r34_<thema>` (nie zwei Builds in `build/`), aus Git-Bash nur ueber `tools/local_build.sh` bzw.
PATH mit msys64 zuerst, Unit-Sonden nach Muster `probes/r30_granate.cmake` (Rueckgabe 0 = gruen, sonst Nummer der Pruefung),
`test_support.c` erzwingt RE1.5-Flavor (RE2-Sonden rufen `re15_ai_flavor_set(RE15_AI_FLAVOR_RE2)` selbst), jede Konstante mit
`@0x…` im Kommentar UND in der Commit-Message (Commit-Message per `git commit -F datei`), Visual nur per gdigrab / `RE15_FRAMEDUMP`
(nie AUTOSHOT/SOFTWARE_RENDER).

### 3.1 Spur A — Engine: Wurf, Flug, Zuender, Explosion (RE1.5-ESP)

Reihenfolge:
1. **A1 Slot-Felder** (`re15_esp.h`): `wpos[3]` (V1a), Granaten-Art (2/3/4), Latch (V1b), Hooks (V1c/d). Euler (+0x20..+0x25) und
   Winkelgeschwindigkeit (+0x18..+0x1d) liegen schon in der Zeilenkopie `row[]` (+0x00..+0x27).
2. **A2 Tick in zwei Durchgaengen** (`re15_esp.c` `re15_esp_fx_tick`, heute A und B je Platz verschraenkt): Durchgang 1 = Routine A
   fuer alle aktiven Row-VM-Plaetze (@0x80019e64-c4); Durchgang 2 je Platz = Kind-Init bei Flags&8 (`Flags ^= 9`, Routine A einmal,
   @0x80019ef4-f30) → Follow (Flags&4) → Weltlage bei Flags&0x80 == 0: wpos = Anker + RotMatrix(euler + (0, Gier +0x2e, 0))·xlat
   (s16-Addition, @0x8001a118-2a4; lokal +x = Blickrichtung) → Routine B → Physik falls Flags&0x20 == 0 (Flags neu lesen; euler +=
   Winkelgeschw., xlat += vel, danach vel += acc) → Anim. Plaetze mit Flags&0x80 behalten die heutige Anker+xlat-Lage (Zweig nicht RE'd,
   OFFEN). Legacy-Plaetze ohne Row-VM unveraendert.
3. **A3 Routine 30** (`case 30`, @0x8001843c-544) inkl. Zaehler-Formel mit a = Zielbit (HOCH 0x8000 / MITTE 0x4000 / TIEF 0x2000
   aus `re15_player_aim_elevation()`) \| Spielerstatus-Unterbits (Gift 0x2 aus `status_flags`, `re15_actor.h:92`); im Kommentar
   festhalten, welche Unterbits der Port fuehrt (N12: Formel, nicht die 7/9-Tabelle).
4. **A4 Routine 29** (`esp_fx_dispatch_b` `case 29`, @0x80018320-434): Kontakt = wpos[1] > 0; Abprall/Liegen wie 1.1; SE ueber
   `re15_esp_se_hook` mit der Lage VOR der Korrektur; Liegen-SE mit Granatenlage (E14).
5. **A5 Routine 31** (`case 31`, @0x8001854c-6dc) mit Kind-Spawner **FUN_800199d4-Zwilling** (Flags 0x0a, Anker Einheitsmatrix,
   Versatz P, Gier des Elternplatzes, erster freier Platz ab 0 @0x80019a88). Art 2: Latch, Flags 0x61, P, `re15_resolve_attack(&{P,500},
   2, -1)`, Kind 0x03195000, SE 0x04080001; Zuender 2: 0x03195000 + 0x030B5400; Zuender 0: Platz frei, dann 0x030B5800. Art 3/4 (E8):
   Flags 0x61, P, Resolver Art 3/4, `re15_esp_aufschlag_hook(2 bzw. 1, wpos, Gier)`; keine HE-Kinder, kein SE, kein Latch; Platz frei
   bei Zuender 0.
6. **A6 Bodenklemme aus** fuer Granatenplaetze (E12, `re15_esp.c:916-922`).
7. **A7 Routine 9** setzt den Latch (@0x80017694 `sb v0,21336(at)`), zusaetzlich zum bestehenden Knall-Haken.
8. **A8 Spielschritt** (`game_step_common.c`): `ENT[9].resolve := 0` (`:1775`; @0x80033b40 nur Munition); Spawn-Gate 9/10/11
   (`:1951`) mit Art-Kennung 2/3/4; `param := pl->rot_y` statt 0 (`:1960-1961`; @0x800336cc → @0x800197e4). `floor_y` fuer die
   Granate bedeutungslos. Drehen im Wurf (`player_common.c:980-990`, Rate 24 im Rueckstoss) und Recoil-Break 10 (`:940-975`)
   sind im Port vorhanden — nur per Sonde bestaetigen, bei Abweichung in `player_common.c` korrigieren.
9. **A9 Sonde** `probe_r34_wurf` (+ `probes/r34_wurf.cmake`, Bau `build_r34_wurf`).
10. **A10 Item-Debug des Statusschirms** (`menu_common.c` `item_mode`, heute DEFERRED `:1258-1260`; Original FUN_8004a0cc-Kopf
   @0x8004a138-0x8004a35c, Absturz-Dossier §2.2b): SELECT (Flanke Pad 1 @0x800ac762, `andi v0,a1,0x100` @0x8004a140) → Debug-Zustand
   1 (0x800b2668), SE 0x04090000 (@0x8004a154/58); Zustand 1 laedt Datei 0xb nach 0x801a0000 (`jal 0x80013b60` @0x8004a194-9c);
   Zustand 2 zeigt das Item-Bild (`jal 0x800492b8` @0x8004a1d4) und schreibt in den CURSOR-Platz 25bd: Id := 0x800b2669
   (@0x8004a1f0), Menge := 255 (@0x8004a1f8/204), +3 := 0 (@0x8004a220), Zustand := 3 (@0x8004a224-2c); Zustand 3: R1 +1
   (@0x8004a238, @0x8004a258/60), L1 −1, R2 +10, L2 −10 (= +246), je Zustand := 2; Kreis beendet (`sb zero,9832(at)` @0x8004a308);
   Id-Grenze ≤ 0x47 (`sltiu v0,v0,0x48` / `sb` 0x47 @0x8004a350-5c). Nur wenn der Cursor auf einem Item-Platz steht (Nutzer-
   Hinweis; im Code = ITEM-Modus des Statusschirms, 25c1 = 3). Datei 0xb und das Bild-Hochladen (ITEMALL-Pfad) aus dem
   Disasm @0x8004a160-0x8004a1d4 uebernehmen, keine Zahl ohne Adresse.

Abnahme Spur A (Unit-Sonde, echte Engine, RE1.5-Flavor; Granate direkt mit Spawnhoehe h und Gier gespawnt, Hooks auf Spione):
* MITTE, a = 0x4000, h = −2474, Gier 0: Bild 0 vel (280,−50,24), acc_x −1, Zuender 42, Zaehler 7, Anim 23, B 29; Kontakte in Bild
  29/47/55/60/64/67/70; Liegen 73 (Flags 0x63, A 31, B 0); SE-Spion exakt 0x010A0601 … 0x010A0001 + 0x010A0001 (8 Aufrufe);
  Explosion Bild 109: genau EIN Resolver-Aufruf, P.y = wpos[1] − 500, Kind 0x03195000 im selben Bild sichtbar (Flags 0x13), SE
  0x04080001 an P, Latch = 1, Flags 0x61; Bild 114 zwei Kinder (0x03195000, 0x030B5400); Bild 116 Platz frei + Kind 0x030B5800;
  lokale Wegstrecke bis L = (11929, 1752) ± 0 (Simulator `wurf_sim_gegen.py`).
* HOCH (h −3171) und TIEF (h −772) und MITTE/HOCH vergiftet (a | 0x2): Werte der Tabelle 1.1 exakt.
* Gier 1024: Endlage = RotY(1024)·(11929, ·, 1752) um den Spawnpunkt (Drehung aktiv).
* Ziel-Dummy Typ 0x27 (HP 180) 300 neben der Liegestelle: HP 180 → −820, +0x4 = 3, +0x5 = 9, +0x6 = 1 im Explosionsbild; Spieler
  900 waagrecht entfernt auf gleichem Boden: HP 100 → −900; 1000 entfernt: unveraendert.
* 0x0A / 0x0B: gleicher Flug; im Explosionsbild Resolver-Art 3 / 4 (+0x5 = 10 / 11 am Dummy), Aufschlag-Spion 1× mit (2 / 1, wpos,
  Gier); kein HE-Kind, kein SE 0x04080001, Latch 0; Platz frei im Zuender-0-Bild.
* Abzug mit Gegner in 1299 vor Leon: KEIN Schaden im Abzugsbild (Bruecke weg).
* Pool voll (96 belegt) → kein Spawn, kein Absturz; Raumwechsel-Reset → Platz frei ohne Resolver-Aufruf.
* R1 los nach Clipbild 10 → kein Spawn, Munition trotzdem −1; Drehen ±24 je Bild aendert die Spawn-Gier.
* Item-Debug (Unit, Statusschirm im ITEM-Modus, Cursor auf Platz 0): SELECT → Platz 0 = `00 ff 00 00`; 9× R1 → `09 ff 00 00`;
  10× / 11× R1 → Id 0x0A / 0x0B; L1/R2/L2 wie A10; Id nie > 0x47; Kreis beendet; Schliessen ruestet die Granate aus
  (Vergleich mit n1_sel / n1_r9 / n1_zu des Absturz-Dossiers).
* Alle bestehenden ESP-Tests gruen (Blut-, Huelsen-, Muendungs-Pins); jede Abweichung ist mit dem Zwei-Durchgang-Tick belegt zu
  erklaeren, sonst Fehler.

### 3.2 Spur B — Schaden und Gegnerreaktion

Reihenfolge:
1. **B1 Resolver-Tore** (`re15_resolve_attack`, `re15_damage.c:3294-3355`): Gate B `(hit_react & 3) == 3 → continue` VOR `&= 1`
   (nicht zaehlen, Bit 0x80 unberuehrt; @0x80012f54-60; Kommentar "inert/OMITTED" `:3334-3339` ersetzen); NPC-Ausschluss Typ 0x40..0x4D
   mit HP < 0 (E7); Explosionspunkt P an den RE2-Stempel durchreichen.
2. **B2 Hitbox-Versatz FUN_8002b498** (`re15_hitbox_test` `:3268-3286`, Tabelle ab `:3468`): Mitte = Lage + (RotY(rot_y)·(Box.x, Box.z),
   Box.y) mit der ROHEN Gier (+0x6a-Konvention je Typ pruefen, Resolver-GP P7). Kaesten: Hund {0,−720,0,900,720,450} (Sektor),
   Alligator {1000,−720,0,2200,720,800}, Feuer 0x26 {0,0,0,600,720,600} (Versatz y 0, heute −720), NPC 0x45 {0,−1440,0,500,1440,500},
   0x4b {0,−1440,0,300,1440,300}, kastenlose Typen {0,0,0,1,1,1}.
3. **B3 Explosionstreffer fuer RE2-Modell-Typen** (`re15_enemy_take_damage` Art ≥ 2): Schaden = `re15_enemy_dmg_row(e)[react_table[Art]]`
   (E4) statt 1000; RE2-Stempel (E6): +0x5 = RE2-Zeile fuer ALLE RE2-Besitztypen, +0x6 = 0, +0x1D2 = Zone(P) + 0, +0x1D3 =
   (alt & 0x80) \| 15, +0x1D0 &= 0xFF00 dann \|= 1 und Richtungsbits aus P, kein Reserve-Abzug (`re2z_stamp_hit` `:7004`,
   `:6623-6632`), Gore-Peilquelle = P statt Spieler (`re15_damage.c:2986`); `re2z_row_from_atktype[2..4]` = 9/11/10
   (`enemy_ai_re2_zombie.c:3847`, Beleg DAT_8006F430 + `re2z_row_from_weapon`).
4. **B4 `re15_re2_gl_apply`** (V2b): FUN_800470C0 vollstaendig fuer RE2-KI-Typen (Leerliste, 4 Gates, Band, Box FUN_80041EF8 auf den
   Trefferkasten-Mittelpunkt +0x84/+0x8C, Radius-Erweiterung bleibt im Puffer, Bit 0x10000, Record-Worte je Typ Zeile 9/10/11 mit
   allen drei Klammern, Wort 2/3, +0x1FC, Zone, Zeile, Sperre, Richtung). Frueh mergen (C braucht es fuer Op 40).
5. **B5 Zombie** (`enemy_ai_re2_zombie.c`): DoT (Zaehler +0x236 im Root, INIT 0; Bloecke EXEC[1] @0x80101DC0-EC0 und EXEC[2]
   @0x8010249C-54C; Tod 0x0A03/0x0B03, +0x1D2 = 4, +0x1D3 \|= 0x80, +0x21A \|= 0x2000); Kommentare `:1493-1494` / `:1603-1606`
   korrigieren; Leichen-Ausblender @0x8010A810-868; Liegend-HURT-Leiter bleibt TOT (RE2-GP W2, `:7052-7063` richtig).
6. **B6 Hund** (`enemy_ai_re2_dog.c`): DEATH Zeile 10/11 (`:2152-2156`), Zeile-9-Teile-Wurf 0x80104440 + Blut je Bild + Budget 1
   (`:2159-2161`), HURT 11 Part-Farbe, FX-8-Anzahl ceil(n/2) (`:2053-2066`), Immunitaet +0x1D3 \|= 0x80 nach HURT 10.
7. **B7 Spinne** (`enemy_ai_re2_spider.c`): DEATH 10/11 Part-Farben (FUN_8010609C, 20 Parts) + Part 19 Flug, Kommentar `:2285`
   (Zeile 11 = Saeure).
8. **B8 RE1.5-Typen** (`enemy_ai_common.c`): 0x29 Explosions-Tod 0x801154b4 (+ Spurgrenzen HURT/DEATH je +0x5, `:11367/:11381`);
   0x23 Todesablauf 0x8010ea30 (`:13173`); 0x2b Ph.0 `hit_react |= 2` und Ph.2 Clip nach `hit_react & 0x80` (`:13590`); 0x27
   Todes-Spurgrenzen (`:9419/:9439`); RE1.5-Fresser 0x16 haelt +0x93 = 1 in Ruhe (per Sonde pruefen, sonst korrigieren).
9. **B9 G5** (`enemy_ai_boss_g5.c`): Zuschlag nach Treffer-Zeile (Byte @0x801056B3 + Zeile; 9/10/11 = 14) statt Waffe (`:1534-1535`);
   Zerfall −1 je 16 Bilder mit Fenster 15 beim ersten Akku-Treffer (`:1500`); Sperre +0x1D3 = 15 mit Abbau; Granaten-Schaden
   RE2-Record (E16) statt 1000 (`:1506`).
10. **B10 Treppe** (`stair_common.c`): `pl->hit_react |= 1` / `&= ~1` an den Original-Stellen (hoch @0x80038a58 / @0x80038c24,
   runter @0x80038d00 / @0x80038ec0; Phasentabelle 0x80010c0c).
11. **B11 Katalog** `RE15_FUN_CATALOG.md`: Gate B = +0x93 Bits 0/1; FUN_8001a7a8 Rueckgabe 1 = Punkt HINTEN.
12. **B12 Sonden** `probe_r34_schaden` + `probe_r34_reaktion` (+ `probes/r34_schaden.cmake`, Bau `build_r34_schaden`).

Abnahme Spur B (Unit-Sonden; RE2-Teile mit `re15_ai_flavor_set(RE15_AI_FLAVOR_RE2)`):
* Gate B: Gegner mit hit_react = 0x83 → HP unveraendert, hit_react bleibt 0x83, Rueckgabe zaehlt ihn nicht.
* NPC 0x45 (HP −1) 300 neben P → kein Zustandswechsel; Ivy (0x2d) und FX (0x24) unveraendert immun.
* Alligator mit rot_y 0 und 1024: Trefferkasten-Mitte = Lage + RotY·(1000,0) → Treffer genau dann, wenn Original-Rechnung trifft
  (Grenzfaelle ±1 Einheit an R und am Hoehenband); Hund Sektor 900/450 in Blick- bzw. Querrichtung; Feuer 0x26 Band ohne −720.
* RE2-Zombie 0x10 (HP 80) auf gleichem Boden, P 300 entfernt: HP 80 − 200 < 0 → state 3, sub_state_1 = 9, re2z_hits1d2 = 0,
  re2z_self1d3 = 15, Richtungsbits aus P, Zonen-Reserve unveraendert; Saeure: Zeile 11; Brand: Zeile 10. Brad 0x11 (HP 250):
  HURT, Saeure/Brand setzen +0x21A 0x1800/0x800, danach im Gang HP −1 genau alle 8 Bilder (+0x236 & 7 == 0), Tod mit 0x0B03/0x0A03.
* Hund RE2: Zeile 9 → Teile-Flags 0x4A an Parts {2,3,4,7,8,9,10}, +0x21F = 18; Zeile 10 → 17 Tints 0x00202020, 6× FX 7; Zeile 11 →
  17 Tints 0x00003F2F; HURT-FX-8-Zahl bei n = 1/2/3/4 → 1/1/2/2; nach HURT 10 kein weiterer Applier-Treffer bis `&= 0x7F`.
* Spinne RE2: Zeile 10/11 Tod → 20 Tints 0x00202F2F / 0x00101F3F.
* RE1.5: 0x29 Tod → Clip 10 (von hinten 11), SE 7, zwei pos_advance (0x800 und 0 bei 0x80); 0x2b Ph.2 Clip 0xb nur bei +0x93&0x80;
  0x23 Clip 13, Leiche nach Bild 20; 0x27 unveraendert.
* G5: Granate Zeile 9 → HP 600 − 80, Akku +14, Fenster 15; zwei Treffer innerhalb des Zerfalls → STAGGER (≥ 15).
* Treppe aktiv → Explosion in 900 → Spieler-HP unveraendert; nach der Treppe → Tod mit cmd 3 (Clip 7).
* `re15_re2_gl_apply`: Zombie Zeile 10 Kl. 2 → HP −5, Sperre 15, nur der erste Gegner, zweiter Aufruf mit erweiterter Box.
* Bestehende Suite gruen; jede geaenderte Erwartung (Zeile 17 → 9, 1000 → Modellwert) mit Adresse im Test-Kommentar.

### 3.3 Spur C — Plattform, RE2-FX-Maschine, Assets

Reihenfolge:
1. **C0 (zuerst, klein, frueh mergen)**: Harness V5 (`main.c:7404-7450` State-Log mit `hp=`, `main.c:4244-4252` RE15_EQUIP mit
   `re15_audio_prime_weapon`, FX-Log `main.c:225-232` erweitert), Assets `shared_assets/RE2/CORE00.ESP` (8572 B) und
   `shared_assets/RE2/TEX.TIM` (132320 B) als Kopien (md5 im Commit), `include/re2_fx.h` mit den Vertrags-Signaturen.
2. **C1 Tick-Reihenfolge** (E10): `re15_esp_fx_tick` von `main.c:5413` hinter `re15_game_step` (`main.c:7358`) im selben
   30-Hz-Durchgang; direkt danach `re2fx_tick()` (RE2: Gegner-Schleife 0x800267c0-0x80026930 vor der FX-Pumpe 0x80026980).
3. **C2 ESP-Zeichnen** (`pc_draw_effects` `main.c:236-452`): Lage aus `wpos`; CLUT-abhaengige 4-bpp-Blaetter aus RE1.5
   `DATA/TEX.TIM` (Zeilen 481/483/491/492 usw.; Muster `tools/tex_tim_effect_slice.py`, heute nur Effekt 8) statt der eingebackenen
   16-bpp-Blaetter `effect3_smoke.tim` / `effect4_shell.tim`; Sichtbarkeit Flags&1 && Flags&2 (@0x800532fc-0c); ABE = Flags-Bit 4;
   ABR aus TPAGE Bits 5-6; Zellen je Satz = Byte1; Zeichenreihenfolge Platz 95 → 0 (@0x800532f0), OT-Einsortierung nach Skill
   re15-pc-render-order.
4. **C3 Licht-Latch-Leser** (E11): nach dem ESP-Tick, vor dem Figuren-Zeichnen: Kopie der aktiven Kamera-Lichtzeile, Licht 2 wie 1.2
   veraendert, fuer dieses Bild benutzen, danach zurueck, Latch := 0 (`re15_light_cut_t`, `re15_light.h:55-62`; Einhaengen bei
   `re15_light_apply_cut` `main.c:2945`).
5. **C4 Ton** (`audio_pc.c`): `re15_esp_se_hook` binden (Bank 1 → `re15_audio_weapon_se(Satz)` der geladenen ARMS-Bank, Bank 4 →
   `re15_audio_core_se(Satz)`, Byte1 ignorieren, Lage-Flag ohne Wirkung wie alle Port-SEs); RE2-FX-SEs 0x01130001 → ARMS10 Satz 10,
   0x01120001 → ARMS11 Satz 10 (E9; ARMS10/11 als zusaetzliche Baenke laden, `load_weapon_se_vab_pc` `:856-898` als Muster).
6. **C5 RE2-FX-Maschine** (`re2_fx.c`, Plan `analysis/konstruktion_2026-08-23/re2-fx-system.md` §3): Bank-Registrierung
   (FUN_8001bca0, Skripttabelle = Kopf + (2·ca + cb + 2)·4 @0x8001bd08-1c), Spawner (FUN_8001cbe8 / FUN_8001bf10: a0 = Bank<<24 \|
   Sub<<16 \| Skala, Sub&7 = Skript, Sub>>3 = CLUT-Zeile, +0x4000 aufgeschoben), Pumpe (FUN_8001d300 Update-Pass Op A; Draw-Pass:
   0x4000-Befoerderung + Op A einmal, dann FUN_8001d68c: FUN_8001d894 Weltlage (Normal: M.t + M.rot·offset + RotY(a1)·lokal;
   0x400: M.rot·(basis·lokal + offset) + M.t), Op B, Physik Bit 2, Anim Bit 1), Ops 0/1/2/19/25/27/28/29/30/40/46/48/49/50/58 mit den
   Adressen aus 1.3/1.4, Aufschlag = Platz mit Op B 48/49 dessen Phase 0 im Aufschlagbild laeuft, Op 40 ueber `re2fx_applier`,
   RNG = RE2-Strom (`re15_re2_rand`), Pause-Gate RE15_PAUSE_ACTION. Op 49 Phase 1..4-Lage erst nach O-VB1, Ops 27/28/29/25-Boden
   erst nach O-VB2.
7. **C6 RE2-FX-Zeichnen** (`main.c`): RE2 TEX.TIM → VRAM (768,256) (Synthese, s. KONSTANTEN), tpage 0x1E = Bildspalten 128..191 hw,
   0x1F = 192..255 hw; CLUT-Block (256,480) 32×19; Billboard FUN_80077924 / FUN_80077ed0 (rueckwaerts, Groesse = size·PARAM·zoom /
   (sz<<4), nprim Zellen, CLUT/TPage aus dem Platz, Halbtransparenz Status 0x1000, Mischmodus aus TPage-OR 0x20/0x40/0x60).
8. **C7 Part-Farben** (V4): `re2z_part_tint` als RGBC je Part im Figuren-Zeichner — erst nach O-VB3.
9. **C8 Harness** `RE15_FORCE_AUFSCHLAG="<re2_art>@<bild>"` (ruft `re2fx_aufschlag` 1500 vor Leon; Muster RE15_FORCE_SPLAT
   `main.c:5394-5400`) fuer die eigene Sichtabnahme vor dem Merge von A.
10. **C9 Tests**: `probe_r34_re2fx` (Unit), `test_r34_granaten.cmake` (exe, aktiv nach Integration).

Abnahme Spur C:
* Unit `probe_r34_re2fx`: RE2 CORE00.ESP registriert (Ids `03 05 00 01 02 06 07 04`, Bank 5 @0x05F0 Skr. 5 Step `00 1b 2e 32 …`);
  Saeure-Aufschlag: Kinder-Codes und Bildfolge exakt 1.3 (X: 3 Kinder; X+1..X+4 je 1; X+4 Platz frei); Brand-Aufschlag: 2 Kinder + 3
  Flammen (Skala-Formel, Gier-Streuung, +0x4A = 1); Flamme mit flachem Bodenstub: Landung → Op 19; `re2fx_applier`-Spion erst ab
  step[0x16] ≥ 16 und Aspekt > 0x1000; Lebensdauer (38..45) + (90..100) bzw. 38..45; Folgeflamme alle 15 Bilder solange vel.x ≥ 61.
* Exe (gdigrab + `RE15_FRAMEDUMP`, RE15_WINDOW_SCALE 3): Handgranate MITTE in ROOM1140 — olives Sprite (CLUT 0x7B11) taumelt im
  12-Bilder-Zyklus, prallt 7× ab, liegt; Explosion: weiss-gelber Feuerball (halbtransparent) + dunkler, steigender Rauch (abdunkelnd);
  Leon genau 1 Bild orange angestrahlt (Pixeldifferenz Bild X gegen X−1/X+1). `RE15_FORCE_AUFSCHLAG=2@…` / `=1@…`: Saeure-Spritzer
  bzw. Feuerball + drei gleitende Bodenflammen, die ~130..145 Bilder brennen.
* Waffen-Log mit `RE15_NOAUDIO=1`: je Wurf 8 Zeilen `SE arms_rec=0x0A` (MITTE gesund), 1 Zeile CORE-Satz 8; Saeure/Brand je 1
  Aufschlag-SE (ARMS10/ARMS11 Satz 10).
* Tick-Reihenfolge: im Spawnbild steht im FX-Log schon vel (280,−50,24) (Routine 30 im Spawnbild).
* Volle Suite gruen; durch E10 um genau 1 Bild verschobene Pins werden mit @0x8001ce0c < @0x8001ce2c begruendet nachgezogen.

---

## 4. Abhaengigkeiten und Integrationsreihenfolge

```
C0 (Harness, Assets, re2_fx.h)  ──►  master
        │
        ├──► Spur B (B1..B12; B4 re15_re2_gl_apply FRUEH)  ──►  master   (2.)
        ├──► Spur A (A1..A9)                                ──►  master   (3.)
        └──► Spur C (C1..C9)                                ──►  master   (4., bindet Hooks + Applier)
                                                             ──►  Integration (5.)
O-VB1 ─► C5 (Op-49-Lage Phase 1..4)     O-VB2 ─► C5 (Flammen: Ops 27/28/29/25)
O-VB3 ─► C7 (Part-Farben sichtbar)      O-VB4 ─► B4/C5 (Nachbrenner an RE1.5-KI-Typen)
```

* **Keine Datei-Ueberschneidung** (3.0). Kopplung nur ueber V1..V5; Hooks und Funktionszeiger haben die Vorgabe NULL, jede Spur
  baut und testet fuer sich.
* **Reihenfolge der Merges**: (1) C0 zuerst — damit A und B nach ihrem Merge sofort exe-Messungen mit HP im State-Log fahren
  koennen. (2) B — Resolver-Tore, Stempel und `re15_re2_gl_apply` liegen danach fest; A's Sonden-Ziel (Typ 0x27) ist von B
  unabhaengig. (3) A — die Granate fliegt und explodiert (ohne C noch mit alter Zeichnung, alter Tick-Lage und ohne Ton). (4) C —
  verlegt den Tick (globale Zeitverschiebung um 1 Bild fuer alle Spieler-Effekte, deshalb zuletzt, damit verschobene Pins eindeutig
  E10 zuzuordnen sind), bindet Hooks/Applier, zeichnet, spielt Ton, bringt die RE2-FX-Maschine.
* Vor jedem Merge: Worktree auf master rebasen (`merge --ff-only`), volle Suite ueber `tools/local_build.sh` im EIGENEN
  Bauverzeichnis; GUI-/exe-Tests nicht unter paralleler Last bewerten (Memory reai-v2-gui-tests-flattern-bei-parallelen-agenten).
* **Integration (5.)**: `tools/local_build.sh` mit `RE15_MIN_TESTS` = 428 + Zahl der neuen Tests (Kopf-Historie ergaenzen);
  exe-Test `test_r34_granaten` (ROOM1140 per `RE15_DEBUG_JUMP=1140@250`, `RE15_GIVE=9:5,10:5,11:5`, `RE15_EQUIP`, `RE15_INPUT_SCRIPT`
  wie `port_inventar_werkzeug/lauf_baseline.sh`; je Granate MITTE nah an Zombie 2): Gegner-HP/-Zustand aendert sich im Bild X+1
  (nicht im Abzugsbild), Kinder/SE/Latch wie 1.2-1.4, kein Haenger; dasselbe mit `RE15_AI_FLAVOR=re15`; Sichtabnahme per gdigrab;
  danach Hauptbaum neu bauen (Memory reai-v2-hauptbaum-exe-neu-bauen), paketieren nach Memory reai-v2-immer-paketieren
  (Release nur ueber docker_win_build.sh / build_linux_deck.sh, keine Pipe, die Fehler schluckt), Worktrees entfernen.

---

## 5. Offene RE-Punkte VOR dem Bau (mit Weg)

Keiner blockiert den Start; jeder blockiert nur das genannte Teilpaket. Die drei Spuren beginnen sofort.

| Nr | Offen | blockiert | Weg (naechster Schritt) |
|---|---|---|---|
| O-VB1 | **Bezugsrahmen des Saeure-Spritzers.** Op 49 setzt Status 0x8403 (Bit 0x400) und vel.y 240 / acc.x −23; die Weltlage folgt dann FUN_8001d894 im 0x400-Modus `welt = M.rot·(basis·lokal + offset) + M.t` mit M = Matrix der RE2-Runde (Waffenknochen beim Schuss). "basis" ist nicht belegt, und die RE1.5-Granate hat keine Waffen-Matrix. | C5 (Lage der Saeure-Kinder Phase 1..4); Phase 0 und alles andere baubar | (1) RE2 FUN_8001d894 0x400-Zweig disassemblieren (`re2_disasm.py dis 0x8001d894 120`): was ist "basis", welche Matrixspalten; (2) Achsenlage des RE2-GL-Waffenknochens (`*(+0x198)+0x7AC`) im Schussbild statisch aus Claires PL01 + PL01W09 (bytegleich W0A/W0B) mit dem Port-Skelettcode rechnen; (3) daraus M.rot = RotY(Wurf-Gier)·B als Port-Zuordnung festlegen und mit Adressen dokumentieren. |
| O-VB2 | **Boden-/Wand-/Wasserabfrage der Flammen.** Op 27 `+0x14 := FUN_8004fba0(P,2,8192,0)` (schreibt 0x800DCBC8 @0x8004fc34/58, 0x8004fd18, 0x8005000c/8c), Op 28 Boden bei y−900 / Wand 0x800DCBC8, Op 25/28 Wasser `jal 0x800527b4`. Rueckgabe-Semantik unbelegt; die Port-Raeume sind RE1.5-Raeume (Granate selbst kennt nur Welt-y 0). | C5 Ops 27/28/29/25 (Landung und Gleiten der Flammen) | RE2 FUN_8004fba0 und FUN_800527b4 disassemblieren (Rueckgabe = Bodenhoehe? Wand-Flag?); im Port die RE1.5-Bodenhoehen-/Kollisionsabfrage suchen (SCA-Zellen, `floor_y`-Quellen) und die Abbildung mit Adressen festhalten; RE1.5 hat kein Wasser-Gegenstueck in den Stage-1-Raeumen → belegen, dass FUN_800527b4 dort "kein Wasser" liefert, oder Wasserraeume auflisten. |
| O-VB3 | **RE2-Part-Farbwort +0x70 im Zeichner.** Der Port schreibt `re2z_part_tint[]` (Verkohlung 0x404040.., Aetzung 0x304040.., Russ, Knockdown 0x10104F), rendert es aber nirgends (Synthese: `grep part_tint` ausserhalb `enemy_ai_re2_zombie.c` leer). Wie RE2 den Wert anwendet (RGBC je Part fuer NCCT?), ist nicht belegt. | C7 (Sichtbarkeit von Verkohlung/Aetzung/Russ sowie Hund-/Spinnen-Farben) — Schaden und Reaktion nicht | RE2-Figuren-Zeichner (Part-Schleife) disassemblieren: Suche nach `lw …,112(part)` gefolgt von `mtc2/ctc2` auf das RGBC-Register; Gegenprobe RE1.5 FUN_80039b2c (face_rgb 0x808080, `re15_light.h` RE15_FACE_RGB_CODE). |
| O-VB4 | **Bodenfeuer-Schaden an Typen ohne RE2-KI.** Der RE2-Applier stempelt +0x5 = RE2-Zeile 10 (Brand); eine RE1.5-KI liest +0x5 als RE1.5-Waffen-Id (10 = Saeure). Fuer RE1.5-KI-Typen gibt es keine Original-Zahl (RE1.5 Art 5 "Flaechenfeuer" 50 / Reaktion 14 ohne Aufrufer; RE2-Records nur fuer RE2-Kreaturen). | B4 / C5 Umfang des Nachbrenners | Keine RE-Quelle vorhanden — Bau-Vorgabe bis zur Nutzer-Bestaetigung: Nachbrenner trifft NUR RE2-KI-Typen (`re15_re2_owns_type` + Zellenarm), RE1.5-KI-Typen und Boss-Module werden nicht getroffen; als Port-Zuordnung kennzeichnen. |
| O-VB5 | ~~Item-Debug im Inventar~~ → **GESCHLOSSEN** durch den Nachtrag im Absturz-Dossier §2.2b (statisch @0x8004a138-0x8004a35c und dynamisch n1, von der Gegenpruefung §4.1 bestaetigt); die fruehere Aussage "kein Item-Weg" (§2.10 alt) galt nur fuer das SELECT-Debug-Menue FUN_8001443c. | — | Bau als A10 |

Hinweis: `re_absturz_original.gegenpruefung.md` ist abgeschlossen (Urteil "bestaetigt"): Ursache kausal belegt (Ein-Wort-Gegenprobe:
Zelle 0x8011ffd0 := `jr ra` → kein Haenger; := 0x80106c18 (Pistolen-Tod) → Zombie stirbt normal mit denselben 1000). Praezisierungen
ohne Einfluss auf den Plan: A1 Typ 0x18 mit HP 1058 haengt auch ueber HURT; A2 G-Birkin haengt nur phasenabhaengig (+0x1dc,
+0x1dd Bit 3); A3 Efeu ohne Granaten-Absturz (dauerhaft gesperrt); A4 geparkte Gegner werden getroffen (1.5); A5 spieleigener
DEBUG.BIN-Haken @0x80013b7c; A6 Wortlaut RA-Kette.

---

## KONSTANTEN FUER DEN BAU

Nur bestaetigte Werte (Dossier + Gegenpruefung). Widerlegt und deshalb NICHT enthalten: Spieler-Versatz 0 (richtig (0,−1530,0)),
RE2-Box {−600,0,300,150} fuer Brand/Saeure-Aufschlag, "Bodenflammen nur bei Wand/Boden", Element-Leiter im Liegend-HURT des
Zombies, Hund-FX-8-Anzahl "n" bzw. "4−K", "alle VSync-Schreiber setzen 2" (als Begruendung). RE1.5 = `info/Re1.5/PSX.EXE` bzw.
`info/Re1.5/PSX/BIN/STAGEn.BIN` (@0x80100000); RE2 = `info/re2leon/PSX.EXE` bzw. genanntes Overlay; Dateien relativ zum Repo.

### K1 Wurf und Flug (RE1.5 PSX.EXE) — Spur A

| Name | Wert | Adresse | Instruktion(en)/Bytes | Verwendung |
|---|---|---|---|---|
| SPAWN_GATE | nur Id 9 (Port: 9/10/11, E1) | @0x80033684-8c | `lbu v1,-13731(v1)` / `ori v0,zero,0x9` / `bne v1,v0,0x800337ac` | Spawn-Gate |
| SPAWN_CODE | 0x040D1000 | @0x800336bc-c0 (+@0x8003371c-20, @0x80033778-7c) | `lui a0,0x40d` / `ori a0,a0,0x1000`; `jal 0x80019700` @0x800336ec/@0x80033748/@0x800337a4 | Effekt 4 sub 0x0D Skala 0x1000 |
| SPAWN_BILD | 0x13 / 0x16 / 0x18 | @0x80033690 / @0x800336a4+@0x800336fc / @0x80033758 | `ori v0,zero,0x13` usw. gegen `lbu v1,-13591(v1)` (0x800acae9) | HOCH / MITTE / TIEF |
| ZIEL_BIT | 0x8000 / 0x4000 / 0x2000 | @0x800336b4 / @0x80033714 / @0x80033770 | `andi v0,v0,0x8000` usw. auf `lhu 0x800acaec` | Zielhoehe |
| WURF_CLIP | 9 / 7 / 11 | @0x800334a8-c8 | `srl v0,a0,15; sll v0,v0,1; addiu v0,v0,7; srl a0,a0,11; andi a0,a0,0x4; addu`; `sb v0,-13592(at)` | Clip der Bank W09/W0A/W0B |
| VERSATZ | {0,0x12c,0x320} / {0,0,0x1f4} / {0,0,0x12c} | @0x800336d8-e8 / @0x80033738-44 / @0x80033794-a0 | `ori v0,zero,0x12c; sw v0,20(sp); ori v0,zero,0x320; sw zero,16(sp); sw v0,24(sp)` usw. | Teil-11-lokal |
| ANKER | [0x800acbdc] + 0x7a4 (Teil 11, Weltmatrix des Vorbilds) | @0x800336d0-f0; Kopie @0x80019820-5c; Komposition @0x8001d09c | `lw a2,-13348(a2)` / `addiu a2,a2,1956` | Spawnpunkt = R·Versatz + T |
| GIER | Spieler +0x6a → Platz +0x2e | @0x800336cc, @0x800197e4 | `lh a1,-13634(a1)`; `sh s2,46(t0)` | Wurfrichtung (Port `pl->rot_y`) |
| MUNITION | −1 im FIRE-Init | @0x800334e0 → 0x80033B38/58/78; @0x80033b40/60/80 | `jal 0x8004eae4`; darin `sb v0,0(at)` @0x8004eb60 | Entlade-Stub |
| DREHEN_IM_WURF | 24 je Bild | Satz @0x800740b8 (`18 30 0a 01 00`), @0x8003355c-0x80033600 | Byte1 0x30 `srl v0,v0,1` | Port player_common.c:980-990 (Rate 24) |
| WURF_ABBRUCH | R1 los und Clipbild > 10 → kein Spawn | Byte2 0x0a @0x800740ba; @0x80033604-50 | `sh` 3 → 0x800aca5a | Port Recoil-Break 10 |
| R30_ANIM / FLAGS / B / A / ZUENDER | 0x17 / 3 / 29 / 0 / 42 | @0x80018448-50 / @0x8001845c-60 / @0x8001846c-70 / @0x80018478 / @0x80018474+7c | `ori v0,zero,0x17; sb v0,110(v1)`; `ori v0,zero,0x3; sb v0,108(v1)`; `ori v0,zero,0x1d; sh v0,2(v1)`; `sh zero,0(v1)`; `ori v0,zero,0x2a; sh v0,30(v1)` | Wurf-Init |
| WURF_HOCH | v (380,−110,21), acc_x −2 | @0x80018494-b0, @0x800184dc | `ori 0x17c`; `addiu -110`; `ori 0x15`; `addiu v0,zero,-2` → `sh v0,8(v1)` im Delay-Slot des `jal 0x8001af20` | Anfangsgeschw. lokal |
| WURF_MITTE | v (280,−50,24), acc_x −1 | @0x800184bc-dc | `ori 0x118`; `addiu -50`; `ori 0x18`; `addiu v0,zero,-1` | " |
| WURF_TIEF | v (80,0,1), acc_x −1, Zaehler 5 | @0x80018518-38 | `ori 0x50`; `ori 0x1`; `addiu -1`; `sh zero,18(v1)`; `ori 0x5`; `sh v0,38(v1)` | " |
| ZAEHLER | ((a + ((a>>7)&0xff)) & 0xff) % 4 + 7, a = u16 0x800acaec | @0x80018484, @0x800184d8-0x8001850c; RNG @0x8001af30-4c | `lhu a0,-13588(a0)`; `jal 0x8001af20`; `srl v1,a0,7 / andi 0xff / addu / andi 0xff`; `addiu v0,v0,7; sh v0,38(a0)` | 7 gesund, 9 mit Gift-Bit 0x2 (Formel verwenden) |
| GRAVITATION | acc_y 10 | CORE00.ESP @0x1AC2 (Zeile @0x1AB8) | `0a 00` | Zeile 0 der Granate |
| BODEN | Welt-y > 0 | @0x80018330-38 | `lh t1,42(t0)` / `blez t1,0x8001842c` | einzige Kollision |
| ABPRALL_X | vx −= trunc(vx/3); vz ungedaempft | @0x8001834c (Delay `lui v1,0x5555`) .. @0x800183cc | `ori v1,v1,0x5556; mult; mfhi; subu a2,a2,a1; subu a3,a3,a2; sh a3,16(t0)` | Abprall |
| ABPRALL_Y | vy := −trunc(vy/3); xlat_y −= Welt-y | @0x800183c4-e0, @0x800183e4-f8 | `lw v0,56(t0); subu v0,v0,t1; sw v0,56(t0)`; `…; subu v0,zero,v0; sh v0,18(t0)` | Abprall |
| ZAEHLER_ABZUG | −1 je Abprall | @0x800183d0-d4 | `addiu v1,v1,-1` / `sh v1,38(t0)` | vor dem SE |
| SE_ABPRALL | 0x010A0001 \| (n<<8), n = neuer Zaehler (wirkungslos) | @0x80018410-28, Punkt @0x800183fc-14 | `lui v1,0x10a; ori v1,v1,0x1; sll a0,a0,8; or a0,a0,v1; jal 0x80045024` | ARMS-Satz 0x0A |
| LIEGEN | SE 0x010A0001; Flags 0x63; A 31; B 0 | @0x80018350-84 | `lui a0,0x10a / ori a0,a0,0x1 / jal 0x80045024`; `ori v0,zero,0x63; sb v0,108(v1)`; `ori v0,zero,0x1f; sh v0,0(v1)`; `sh zero,2(v1)` | keine y-Korrektur |
| WELTLAGE | Anker.R·Versatz + Anker.T + RotMatrix(euler + (0,Gier,0))·xlat, s16 | @0x8001a118-2a4 | `jal 0x80068098` @0x8001a1a0; `jal 0x800661c0`; `sh … 40/42/44(a0)` | wpos (V1a) |
| ROTY | m[0][0] = cos, m[0][2] = sin, m[2][0] = −sin → lokal +x = Blickrichtung | @0x8006820c, @0x8006816c, @0x800682a4 | `sh t6,0(a1)`, `sh t6,4(a1)`, `sh t6,12(a1)` | Drehung der Flugbahn |
| TICK_PHYSIK | xlat += vel, danach vel += acc; nur Flags&0x20 == 0 (nach Routine B neu gelesen) | @0x8001a2e8-388 | `lbu v0,108(a2)` | Reihenfolge im Tick |
| POOL | 96 × 0x84 @0x800a73b8; erster freier Platz ab 0; voll → 0xff | @0x8001978c, @0x800197c8-cc | `sltiu v0,t3,0x60` | Spawner |
| RAUMWECHSEL | alle Plaetze frei | @0x80019378 (gerufen @0x8003996c) | `sb zero` auf Pool+0x6c | liegende Granate verschwindet |
| TAKT | 2 VBlanks im Spielbild | @0x8002147c/80 | `lbu a0,0(s0)` (= 2) / `jal 0x80061fc0` | 30 Hz |
| REIHENFOLGE | Gegner < Spieler < ESP | @0x8001ce04 / @0x8001ce0c / @0x8001ce2c | `jal 0x8001a50c` / `jal 0x80031c44` / `jal 0x80019e20` | E10 |

### K2 Explosion, Kinder, Licht (RE1.5 PSX.EXE) — Spur A (Engine) / C (Licht-Leser, Zeichnen)

| Name | Wert | Adresse | Instruktion(en)/Bytes | Verwendung |
|---|---|---|---|---|
| EXPLOSION_BEI | Zuender == 7 (= Liegen + 36) | @0x80018560-70 | `lhu v1,30(a1)`; `ori v0,zero,0x7`; `bne v1,v0,0x800185f4` | genau ein Resolver-Aufruf |
| LICHT_LATCH_SETZEN | 0x800b5358 := 1 | @0x8001857c (R31), @0x80017694 (R9), @0x800180f0 (R26, nicht portiert) | `sb v0,21336(at)` | E11 |
| FLAGS_EXPLOSION | 0x61 (unsichtbar) | @0x80018580-84 | `ori v0,zero,0x61; sb v0,108(a1)` | Granate weg |
| EXPLOSIONSPUNKT | (x, Welt-y − 500, z) s32 | @0x80018594-bc | `lh v0,40/42/44(v1)`; `addiu v0,v0,-500` @0x800185a8; `sw v0,24(sp)` im Delay-Slot | P |
| RESOLVER_AUFRUF | FUN_80012d60(500, &P, 2) | @0x80018598/a4/b4/b8 | `ori a0,zero,0x1f4; addiu a1,sp,16; ori a2,zero,0x2; jal 0x80012d60` | Port `re15_resolve_attack(&{P,500}, 2/3/4, -1)` |
| KIND_FEUERBALL | 0x03195000 | @0x800185c0-c4, @0x8001860c-14 | `lui a0,0x319; ori a0,a0,0x5000` | Zuender 7 und 2 |
| KIND_RAUCH | 0x030B5400 / 0x030B5800 | @0x80018648-4c / @0x80018698+a8 | `lui a0,0x30b; ori a0,a0,0x5400` / `0x5800` | Zuender 2 / 0 |
| KIND_ANKER | Einheitsmatrix 0x80072d4c, Versatz P, Gier +0x2e | @0x800185d0-e0 | `lui a2,0x8007; addiu a2,a2,11596`; `lh a1,46(v0)`; `addiu a3,sp,16` | Kinder an P |
| KIND_FLAGS | 0x0a (Init + Routine A im Hauptlauf desselben Bilds) | @0x80019a88; @0x80019ef4-30 | `ori v0,zero,0xa`; `xori v0,v1,0x9` | FUN_800199d4-Zwilling |
| SE_EXPLOSION | 0x04080001 an P | @0x800185e4-ec | `lui a0,0x408; ori a0,a0,0x1; jal 0x80045024` | CORE-Satz 8 |
| ZUENDER_2 / 0 | Kinder / Platz frei VOR Kind | @0x80018604-08; @0x80018688-cc | `ori v0,zero,0x2; bne`; `sb zero,108(a1)` @0x800186b0 vor `jal 0x800199d4` @0x800186c8 | |
| ZUENDER_ABZUG | −1 je Bild | @0x8001867c-84 | `addiu v0,v0,-1`; `sh v0,30(v1)` (Delay-Slot) | |
| LICHT_LESER | Licht 2: Typ 0, Farbe max(·, 0xD2/0x8C/0x50), Lage Spieler + RotY(Gier)·(0x4b0,·,0), y Spieler − 800, Helligkeit 0x1770, 1 Bild | @0x8001ce60, @0x8001cef8, @0x8001cf20-ac, @0x8001cebc-cc, @0x8001d010-1c, @0x8001d080-84; zurueck @0x8001d1ac; Latch := 0 @0x8001d1b4 | `lbu v0,21336(v0)`; `sb zero,3(v0)`; `sltiu v0,v0,0xd2` …; `ori v0,zero,0x4b0`; `addiu v1,v1,-800`; `ori v1,zero,0x1770; sh v1,38(v0)`; `sb zero,0(s0)` | C3; Lichtsatz-Format `re15_light.h:55-62` |
| ZEICHNEN | sichtbar nur Flags&1 && Flags&2; ABE = Flags-Bit4; Teile = Satz-Byte1; Reihenfolge Platz 95 → 0 | @0x800532fc-0c, @0x80053500-04, @0x80053354, @0x800532f0 | `srl v0,v0,3; andi v0,v0,0x2`; `lbu v0,1(s3)`; `addiu s0,s0,-132` | C2 |

### K3 Daten-Bytes (RE1.5-Dateien) — Spur A/C

| Name | Wert | Datei @ Offset | Bytes | Verwendung |
|---|---|---|---|---|
| EFF4_KOPF | ca 36, cb 28, CLUT 0x7AD1, TPAGE 0x001F | `DATA/CORE00.ESP` @0x1728 | `24 00 1c 00 d1 7a 1f 00` | Granaten-Effekt |
| EFF4_SUB5 | Strom @0x1AB0 (1 Strom, 2 Zeilen) | CORE00.ESP @0x18CA / @0x1AB0 | `7c 00` / `01 00 00 00 02 00 00 00` | sub 0x0D |
| GRANATE_ZEILE0 | A 30, w/h 0x1000, acc (0,10,0) | CORE00.ESP @0x1AB8 | `1e 00 00 00 00 10 00 10 00 00 0a 00` + Nullen | |
| FLUG_ANIM | Saetze 23..34 je 1 Bild, 35 → 23 | CORE00.ESP @0x17E8..0x1848 | `10 01 01 10` … `17 01 ff 10` | Taumeln |
| EFF3_KOPF | ca 29, cb 163, CLUT 0x7811, TPAGE 0x001E | CORE00.ESP @0x0008 | `1d 00 a3 00 11 78 1e 00` | Kinder |
| FEUERBALL_ZEILE | A 10, Flags 0x13, Anim 10, steht | CORE00.ESP @0x3F4 (sub 0x19 → [1] @0x386 → 0x3EC) | `0a 00 … 13 00 … 0a 00` | 13 Bilder |
| RAUCH_ZEILEN | A 10, acc (0,5,0), vel (0,−130,0), TPAGE\|0x40, Anim 8; Zeile 1 vel (0,−135,0) | CORE00.ESP @0x4CC / @0x4F4 (sub 0x0B → [3] @0x38A → 0x4C4) | `… 13 00 00 00 7e ff 00 00 40 00 … 08 00` / `… 79 ff …` | 15 Bilder, ABR 2 |
| CLUT_GRANATE | 0x7B11 → (272,492) | `DATA/TEX.TIM` @0x334; Rechnung @0x8001987c-88 | `0000 b631 b1ef adce …`; `lhu v0,4(t5); addu v0,v0,s0; sh v0,50(t0)` | oliv |
| CLUT_FEUERBALL | 0x78D1 → (272,483) | TEX.TIM @0x0F4 | `0000 ffff ebde d7de …` | |
| CLUT_RAUCH | 0x7851 → (272,481) | TEX.TIM @0x074 | `0000 9ce7 98c6 94a5 …` | |
| SE_ABPRALL_SATZ | ARMS Satz 0x0A → Prog 0 Ton 1 → VAG 2 | `SOUND/ARMS09.EDH` @0x28 (= ARMS0A = ARMS0B) | `00 00 13 10` | Bank 1 der ausgeruesteten Waffe |
| SE_EXPLOSION_SATZ | CORE Satz 8 → Ton 9 → VAG 6, Direkt-Zweig | `SOUND/CORE00.EDH` @0x20 | `00 00 93 00` | Bank 4 |
| SE_SAEURE_SATZ | ARMS10 Satz 10 → Prog 0 Ton 3 → VAG 3 (10992 B) | `SOUND/ARMS10.EDH` @0x28; VB md5 39cec979 == RE2 ARMS0B.VB | `00 00 33 20` | E9 |
| SE_BRAND_SATZ | ARMS11 Satz 10 → VAG 3 (11664 B) | `SOUND/ARMS11.EDH` @0x28; VB md5 46833b5e == RE2 ARMS0A.VB | `00 00 33 20` | E9 |
| WURF_CLIP_LAENGE | 35 / 40 / 40 (MITTE/HOCH/TIEF) | `PLD/PL00W09.PLW` @0x24 / @0x2C / @0x34 (W0A/W0B bytegleich) | `23 00 08 03` / `28 00 98 03` / `28 00 3c 04` | Clip 7/9/11 |

### K4 Resolver, Spieler, Hitboxen (RE1.5) — Spur B

| Name | Wert | Adresse | Instruktion(en)/Bytes | Verwendung |
|---|---|---|---|---|
| SCHADEN_ART | [2] 1000, [3] 1000, [4] 1000 | @0x8006f41c / @0x8006f41e / @0x8006f420 | `e8 03 e8 03 e8 03`; Leser `lhu a0,0(s3)` @0x80012ff4 | E4 (ausser Modell-Typen) |
| REAKTION_ART | [2] 9, [3] 10, [4] 11 | @0x8006f432 / @0x8006f433 / @0x8006f434 | `09 0a 0b`; `lbu v0,0(at)` @0x80012fe8 / `sb v0,5(s1)` @0x80012ff0 | +0x5 (RE1.5-Waffen-Id) |
| ABSTAND | R = r_Ziel + 500; \|dx\|,\|dz\| ≤ R; sqrt < R | @0x8002b6fc-770 | `addu s0,v0,s1`; `sltu v0,a2,v0`; `jal 0x80065f60`; `blez s0` | `re15_hitbox_overlap` (bytegleich) |
| HOEHENBAND | \|P.y − (Y + O.y)\| < 500 + h (streng) | @0x8002b778-7ac | `addu a2,s1,v0`; `slt`; `slt`; `ori fp,zero,0x1` | " |
| SEKTOR | hb[6] ≠ hb[10] → r nach Winkel (ratan2 − rohe +0x6a) | @0x8002b61c-6f8 | `beq v1,v0,0x8002b6fc`; `lhu v1,106(s3)` @0x8002b650; `jal 0x800683e8` / `jal 0x80068348` | Hund, Alligator |
| VERSATZ_7C | O = (RotY(+0x6a)·(Box.x, Box.z), Box.y), jedes Bild | @0x8002b4bc-51c | `lw s0,120(s1); lw s2,124(s1); lhu v0,106(s1)`; `jal 0x80068098`; `jal 0x800661c0`; `sh v0,0(s2)`; `lhu v0,2(s0); sh v0,2(s2)`; `sh v0,4(s2)` | B2 |
| GATE_A | Platz+0x74 == Gegner+0x188+0x40 → ueberspringen | @0x80012f38-4c | `lw v1,21188(v1); lw v0,392(s1); lw v1,116(v1); addiu v0,v0,64; beq v0,v1` | Granate: niemand (attacker −1) |
| GATE_B | (+0x93 & 3) == 3 → ueberspringen, nicht zaehlen | @0x80012f54-60 | `lw v0,144(s1); lui v1,0x300; and v0,v0,v1; beq v0,v1,0x8001302c` | B1 |
| RIEGEL | +0x93 &= 1; Punkt hinten → \|= 0x80; Bit 0 gesetzt → \|= 2 ohne Schaden | @0x80012f7c-cc | `andi v0,v0,0x1; sb v0,147(s1)`; `jal 0x8001a7a8`; `ori v0,v0,0x80`; `ori v0,v1,0x2` | |
| TREFFER_SCHREIBT | +0x07 := 0, +0x06 := 1, HP −= , +0x93 \|= 1, +0x04 := 2 (3 bei HP < 0) | @0x80012fd0-80013020 | `sb zero,7(s1)`; `sb v0,6(s1)`; `sh v1,154(s1)`; `ori v0,v0,0x1`; `sb v0(=2),4(s1)` im Delay; `sb v0(=3),4(s1)` | |
| SEITE | ((w − Gier + 0x400) & 0xfff) < 0x800 = Punkt HINTEN | @0x8001a7e0-ec | `subu; addiu v0,v0,1024; andi v0,v0,0xfff; slti v0,v0,2048` | Port-Etikett "front" vertauscht |
| ART_TOR | Art < 2: SE 10 + Gift-Wurf; Art ≥ 2: nichts | @0x80012e58 / @0x80012f68 | `sltiu a0,a0,0x2` / `sltiu v0,s0,0x2` | |
| SPIELER_ZWEIG | +0x04 := 2, +0x05 := 2 + Seite, +0x06 := 0, +0x93 \|= 1; HP < 0 → 3/0/0 | @0x80012e18-efc | `sb v0,4(s1)`; `addiu v0,v0,2; sb v0,5(s1)`; `sb zero,6(s1)`; `ori v0,zero,0x3; sb v0,4(s1)` | |
| SPIELER_HP | Start 100 | @0x80031710-18 | `ori v0,zero,0x64; sh v0,-13586(at)` | 1000 = Tod |
| SPIELER_TOD | Clip 7, SE 0x04030001, dann Modus 7 | @0x80036778-80, @0x80036744/64/a8, @0x80036814 | `ori v1,zero,0x7; sb v1,-13592(at)`; `lui a0,0x403 / ori a0,a0,0x1 / jal 0x80045024` | Port cmd 3 (vorhanden) |
| SPIELER_HITBOX | r 450, h 1530, O (0,−1530,0) | @0x80073e94 (Zeiger @0x80073ea0), O @0x800b2354 zur Laufzeit | `00 00 06 fa 00 00 c2 01 fa 05 c2 01`; RAM `00 00 06 fa 00 00` | NICHT zurueckbauen (re15_damage.c:3460) |
| TREPPE_IFRAME | hoch: setzen @0x80038a58 / loeschen @0x80038c24; runter: @0x80038d00 / @0x80038ec0 | Phasentabelle 0x80010c0c | `sb v0,-13593(at)` (+0x93) | B10 |
| BOX_ZOMBIE | {0,−1440,0,400,1440,400} | STAGE1 @0x8011f778 | `00 00 60 fa 00 00 90 01 a0 05 90 01` | Port schon so |
| BOX_HUND | {0,−720,0,900,720,450} | STAGE1 @0x80120f64 (INIT `sw v0,120(v1)` @0x8010da70) | `00 00 30 fd 00 00 84 03 d0 02 c2 01` | B2 |
| BOX_ALLIGATOR | {1000,−720,0,2200,720,800} | STAGE2 @0x80118b98 | `e8 03 30 fd 00 00 98 08 d0 02 20 03` | B2 |
| BOX_FEUER_26 | {0,0,0,600,720,600} (Versatz y 0) | STAGE1 *(0x80121264) = 0x80121258 | RAM bestaetigt (Bosse-GP §3) | B2 |
| BOX_NPC_45 / 4B | {0,−1440,0,500,1440,500} / {0,−1440,0,300,1440,300} | STAGE1 @0x80121728 / @0x801218c8 | `8011d2f8 lw v0,5940(v0)` / `8011e3b0 lw v0,6356(v0)` | B2 |
| BOX_VORGABE | {0,0,0,1,1,1} | @0x80072be0 (in der Typ-Tabelle 0x80072bac!) | `lui v0,0x8007 / addiu v0,v0,11232 / sw v0,120(s0)` @0x800422c8-d0 | kastenlose Typen |
| NPC_HP | −1 | STAGE1 0x45 @0x8011d320/24 | `addiu v0,zero,-1 / sh v0,154(v1)` | E7-Kennung |
| RE2_NPC_GATE | HP < 0 → kein Kandidat | RE2 @0x80047148-50, NPC-Init @0x8005d7b4-bc | `lh v0,342(s0) / bltz`; `addiu v0,zero,-1 / sh v0,342(s0)` | E7 |

### K5 RE1.5-Gegnerreaktionen (STAGE-Overlays) — Spur B

| Name | Wert | Adresse | Instruktion(en)/Bytes | Verwendung |
|---|---|---|---|---|
| MADE_DEATH_SPUREN | 9..11, 15..18 → 0x8011bb9c; 7/8/13 → 0x8011b998; 0..6/12/14/19/20/21 → 0x8011b7b8 | STAGE1 `table 0x80121500` | `8011b780 lbu v0,5(v0)` / `8011b790 addiu at,at,5376` / `8011b7a0 jalr` | Port `:9419/:9439` |
| MADE_EXPL_TOD | Clip 10 (0x80: 11), +0x93 \|= 2, Crossfade 7, +0x8c = (rng&31)+80, SE 0 | STAGE1 @0x8011bbf0-0x8011bd24 | `ori v0,zero,0xa` @0x8011bc10 / `ori v0,zero,0xb` @0x8011bc34; `andi 0x1f; addiu 80`; `jal 0x800453d0` + `addu a0,zero,zero` | fertig |
| KAKERLAKE_SPUREN | 9..11, 15..18 → HURT 0x80114cb8 / DEATH 0x801154b4 (STAGE3; ST4 0x80110358/0x80110b54; ST5 0x801104d8/0x80110cd4) | STAGE3 `table 0x8011ed84` / `table 0x8011eddc` | `80114824 addiu at,at,-4732`; `80115048 addiu at,at,-4644` | B8 |
| KAKERLAKE_TOD | Clip 10 / 11 (0x80), SE 7, Blut-Vektor i32 {200,−800,0} | STAGE3 @0x80115528 / @0x8011554c, @0x8011561c, @0x8011ec84 | `ori v0,zero,0xa`; `ori v0,zero,0xb`; `jal 0x800453d0` + `ori a0,zero,0x7`; `c8 00 00 00 e0 fc ff ff 00 00 00 00` | B8 |
| RUECKSTOSS | pos_advance(0x800), bei +0x93&0x80 zusaetzlich pos_advance(0) (netto ~0) | STAGE3 @0x801156c8 / @0x801156f0 (0x27: @0x8011bd80 / @0x8011bda8) | `ori a0,zero,0x800; jal 0x800245d8`; `jal 0x800245d8` + `addu a0,zero,zero` | FUN_800245d8 addiert nur x/z |
| TYRANT_TOD | alle +0x5 → 0x80114cb0; Ph.0 +0x93 \|= 2; Clip 8 (0x80: 9); Ph.2 Clip 0xa, 0xb nur bei +0x93 & 0x80 | STAGE4 `table 0x8011a218`, @0x80114d04, @0x80114d24/4c, @0x80114f2c-40 | `ori v0,v0,0x2`; `ori v0,zero,0x8`; `andi v0,v0,0x80` @0x80114f34 / `ori v0,zero,0xb` im Delay | B8 (Port-Bit falsch `:13590`) |
| TYRANT_SCHLUCK | Byte[9..11] = 0 (nie geschluckt) | STAGE4 @0x8011a1b1..b3 | `00 00 00` | |
| ALLIGATOR_TOD | Clip 13, Blut an (+0x188)+2644, Bild 20 → Ph.2, Leiche | STAGE2 @0x8010eaa4, @0x8010eb34, @0x8010eba8, @0x8010ec14-18 | `ori v0,zero,0xd`; `addiu a2,a2,2644`; `ori v0,zero,0x14`; `ori v0,zero,0x7 / sw v0,4(v1)` | B8 |
| BIRKIN_ZEILE9 | NULL (Spalte 1 @+4) | STAGE3 @0x8011f068 / @0x8011f308; STAGE5 @0x8012002c / @0x801202cc | 12 × `00` | E7 (Port-Tod bleibt) |
| ZOMBIE_ZEILE9_RE15 | NULL (Haenger `80106c00: jalr v0`) | STAGE1 @0x8011ffd0 (DEATH [9][1]), @0x8011fcb0.. (HURT) | `00 00 00 00`; `09 f8 40 00` = `jalr ra,v0` @0x80106c00 | Grund fuer E7 |
| IVY / FX_RIEGEL | +0x93 = 3 / 1 | STAGE4 @0x8011693c-44 / STAGE2 @0x8010ef28/58 | `ori v0,zero,0x3; sb v0,147(a1)` / `ori v1,zero,0x1; sb v1,147(v0)` | immun |

### K6 RE2-Stempel, RE2-Applier, RE2-Records (RE2 PSX.EXE) — Spur B

| Name | Wert | Adresse | Instruktion(en)/Bytes | Verwendung |
|---|---|---|---|---|
| APPLIER_LEERLISTE | Gegnerzahl 0x800CFBF3 == 0 → 0 | @0x800470c4, @0x8004710c | `lbu v0,-1037(v0)`; `beq v0,zero,0x8004762c` | B4 |
| APPLIER_LISTE | 0x800CFE1C .. *(0x800CE334) | @0x800470fc, @0x80047118, @0x80047410 | `addiu s7,s7,-15896` / `addiu s2,s7,15412` / `lw v0,8524(s7)` | B4 (Port: aktive RE2-KI-Gegner) |
| APPLIER_GATES | aktiv; +0x1D3 == 0 (ganzes Byte); HP ≥ 0; +0x10E & 0xC000 == 0 | @0x8004712c-64 | `andi v0,v0,0x1 / beq`; `lbu v0,467(s0) / bne`; `lh v0,342(s0) / bltz`; `lhu v0,270(s0) / andi v0,v0,0xc000 / bne` | B3 (Port-Bruecke `enemy_ai_re2_zombie.c:8809-8815` spiegelt sie in hit_react Bit 0), B4 |
| RICHTUNG_LOESCHEN | +0x1D0 &= 0xFF00 je Kandidat vor dem Band | @0x8004716c-84 | `lhu v1,464(s0); andi v1,v1,0xff00; sh v1,464(s0)` | B3/B4 |
| APPLIER_BAND | (Y + h98 + 100 + d9e − P.y) <u 2·(d9e + 100) | @0x8004716c-a4 | `lh a0,152(s0)`; `addiu v0,v0,100`; `lhu v1,158(s0)`; `sll v1,v1,1`; `sltu` | B4 |
| APPLIER_BOX | FUN_80041EF8: Ecke = P + R(Gier)·(p0, 0, −p1 − 4·p3), Kanten R·(p2,0,0) und R·(0,0,2·p3), Viertel-Raster; Pruefpunkt = Trefferkasten-Mitte +0x84/+0x8C | @0x80041f28-0x80042118; `addiu a2,s0,132` @0x800471b4 | `sw v0,84(sp)` …; `jal 0x8008e1f4`; `sra 2` | B4 (Weltmass x ∈ [p0, p0 + 4·p2 + r], z ∈ [−4·p3 − r, 4·p3 + r]) |
| APPLIER_RADIUS | p2 += r/4, p3 += r/4 (r = +0x1EE); nur im Nicht-Treffer-Zweig zurueck → bleibt im Puffer des Aufrufers | @0x800471bc-ec, Ruecknahme @0x800473dc-408 | `lhu v1,494(s0); sll 16; sra 18; sh v0,6(s3)`; `sh v0,4(s3)` | B4 |
| APPLIER_MODUS | Hitcode-Bit 0x10000 = alle, sonst erster | @0x80047208-10 | `lui v0,0x1; and v0,s5,v0; beq v0,zero,0x80047434` | Nachbrenner: erster |
| APPLIER_SCHADEN | (w0 >> 10·K) & 0x3FF, Record = *(0x800A6A88 + Typ·4) + (Zeile − 1)·20, Zeilenindex = Hitcode & 0xFFFF | @0x80047214, @0x8004722c-68 | `andi v1,s5,0xffff`; `lw a1,27272(at)`; `addiu v0,v0,-20`; `srlv`; `andi v1,v1,0x3ff`; `sh v0,342(s1)` | B4 (Op 40, K2) |
| APPLIER_ZUSTAND | +0x4-WORT := 2 bzw. 3 bei HP < 0 (nullt +0x5/+0x6/+0x7); +0x1FC := altes Wort ausser 0xC02 | @0x80047264-90 | `lw v1,4(s1); addiu v0,zero,3074; beq; sw v1,508(s1)`; `addiu v0,zero,2; bgez; sw v0,4(s1)`; `addiu v0,zero,3; sw v0,4(s1)` | B3/B4 |
| STEMPEL_ZONE | Zone 1; Bit 0x20000 && Y + h/2 < P.y → 0 (Beine); Kopf-Regel P.y < Y + 3h/2 nur bei Wort0 & 0x10000000; +0x1D2 = Zone + 3·K | @0x80047294-330 | `addiu v0,zero,1; sb v0,466(s1)`; `lui v0,0x2; and; beq`; `sra a0,v0,17`; `slt`; `sb zero,466(s1)`; `lui v1,0x1000`; `sll v1,s6,1 / addu / addu / sb v0,466(s1)` | B3 (P = RE1.5-Explosionspunkt, K = 0, E4/E6) |
| STEMPEL_ZEILE | +0x5 := Hitcode & 0xFF (RE2-Zeile) | @0x80047324 | `sb s5,5(s1)` | B3: Zeile = `re2z_row_from_weapon[react_table[Art]]` = 9/11/10 fuer alle RE2-Besitztypen |
| STEMPEL_SPERRE | +0x1D3 := (alt & 0x80) \| ((w1 >> 9) & 0x7F) = 15 (alle GL-Zeilen, w1 = 0x078F1E0A / 0x078F1FB4 / 0x078F1F68) | @0x8004731c-4c | `lbu a0,467(s1); andi a0,a0,0x80; sb a0,467(s1); lw v0,4(a1); srl v0,v0,9; andi v0,v0,0x7f; or; sb a0,467(s1)` | B3/B4 |
| STEMPEL_RICHTUNG | a = Peilung(P → Gegner) − Gier(+0x76): (a+0x400)&0xFFF < 0x800 → \|0x20; (a+0x600)&0xFFF < 0x400 → \|0x40; (a−0x200)&0xFFF < 0x400 → \|0x80; vorher +0x1D0 \|= 1 | @0x800471f8-204, @0x80047358-3d8 | `ori v0,v0,0x1; sh v0,464(s1)`; `jal 0x800154ac`; `lh v1,118(s1)`; `slti v0,v0,2048` / `slti v0,v0,1024` | B3 (Peilquelle P statt Spieler) |
| RESERVE | GL-Applier zieht KEINE Zonen-Reserve ab (nur Hitscan-Applier @0x80041954-88) | Store-Liste FUN_800470C0 (RE2-GP §4) | — | B3: Abzug `enemy_ai_re2_zombie.c:7004` fuer Explosion aus |
| SPERRE_ABBAU | −1 je Bild (untere 7 Bit) im Gegner-Root | Zombie EMZ0 @0x80100484-98, Hund @0x80100028-3C, Kraehe @0x80100160-74, Spinne @0x801000EC-100 | `lbu v1,467; andi v0,v1,0x7f; beq; addiu v0,v1,-1; sb v0,467` | Port vorhanden (Pruefen) |
| REC_ZOMBIE | Z9 200/50/10, Z10 200/50/5, Z11 200/50/10 | @0x800A41CC / @0x800A41E0 / @0x800A41F4 | `c8 c8 a0 00` / `c8 c8 50 00` / `c8 c8 a0 00` (w1 `0a 1e 8f 07`) | E4 (K0 = 200); Op 40 K2 = 5 |
| REC_ZOMBIE16 | Z9 80/50/10, Z10 80/50/5, Z11 200/50/10 | @0x800A4348 / @0x800A435C / @0x800A4370 | `50 c8 a0 00` / `50 c8 50 00` / `c8 c8 a0 00` | E4 |
| REC_HUND | Z9 300/50/10, Z10 300/50/5, Z11 300/50/10 | @0x800A44C4 / @0x800A44D8 / @0x800A44EC | `2c c9 a0 00` / `2c c9 50 00` / `2c c9 a0 00` | E4 |
| REC_KRAEHE | 60/60/15 (alle drei) | @0x800A4640 / 54 / 68 | `3c f0 f0 00` | E4 |
| REC_SPINNE | Z9 60/60/20, Z10 130/130/5, Z11 90/60/10 | @0x800A4C30 / @0x800A4C44 / @0x800A4C58 | `3c f0 40 01` / `82 08 52 00` / `5a f0 a0 00` | E4 (0x25/0x26) |
| REC_ARM | Z9 60/60/20, Z10 60/60/10, Z11 60/60/10 | @0x800A5220 / 34 / 48 | `3c f0 40 01` / `3c f0 a0 00` / `3c f0 a0 00` | Zellenarm (Nachbrenner) |
| REC_G5 | Z9 80/80/80, Z10 70/70/5, Z11 70/70/10 | @0x800A5F7C / @0x800A5F90 / @0x800A5FA4 | `50 40 01 05` / `46 18 51 00` / `46 18 a1 00` | E16 |

### K7 RE2-Gegnerreaktionen (RE2-Overlays) — Spur B

| Name | Wert | Adresse | Instruktion(en)/Bytes | Verwendung |
|---|---|---|---|---|
| Z_HURT_TAB | Basis 0x8010C940 + Zeile·36 + Spalte·4; Z9 {7438,5BC0,5BC0,5438×6}, Z10/11 {5BC0×3,5438×6} | EMZ0.BIN @0x801053E0-410; Zeilen @0x8010CA84 / @0x8010CAA8 / @0x8010CACC | `lui a2,0x8011 / addiu a2,a2,-14016`; `lbu v1,466(a0)`; `jalr v0` | Port-Tabelle vorhanden |
| Z_DEATH_TAB | Basis 0x8010CC24; Z9 {7438,8BEC,8BEC,8530×6}, Z10/11 8530×9 | EMZ0.BIN @0x801084E0-518; Zeilen @0x8010CD68 / 8C / B0 | `lui a2,0x8011 / addiu a2,a2,-13276` | Spalte 0 (E6): HE 0x80107438, Saeure/Brand 0x80108530 |
| Z_KNOCKDOWN_TOD | Clip 1, Rate 15 | EMZ0 @0x801074C8 | Wort 0xF0001 | HE-Tod bei Zone 0 |
| Z_STURZ_TOD | Clip {1,2}[Seite], Rate 15; Seite = rand&1, bei +0x21A & 0x2000 := 0 | EMZ0 @0x8010855c-60, @0x8010860c, @0x801085a4-c0 | `sb 1,16(sp) / sb 2,17(sp)`; `lui v1,0xf`; `andi v1,v1,0x2000` | Saeure/Brand-Tod |
| Z_TAUMELN | Clip 4 (Ruecken 3), Rate 3 | EMZ0 @0x80105C38-74 | — | Brad HURT Zeile 10/11 |
| Z_ELEMENT | Z10 → FUN_80106128 Verkohlung, Z11 → FUN_80106310 Aetzung (+0x21A \|= 0x1800), Z9 → FUN_8010640C Russ | lebend: STAGGER P0 @0x80105DC4-F18 (0x800 @0x80105df8), Kriecher @0x80107960-9B0, Aufsteh @0x80107BE0-CE0, DEATH-MAIN @0x801086E4-7C0, DEATH-Liegend @0x80108444-4B8; TOT: HURT-Liegend @0x80105188-208 (`sw 0x00060501` @0x80105184) | `addiu v0,zero,10/11/9` + `jal` | Port vorhanden (`:4668-4738`), Liegend-Leiter NICHT bauen |
| Z_DOT_TAKT | HP −1 wenn (+0x236 & 7) == 0 UND +0x21A & 0x800 UND (+0x10E & 0x80 ODER +0x21A & 0x1000) | EMZ0 EXEC[1] @0x80101DC0-E4C, EXEC[2] (0x80102260) @0x8010249C-54C | `lhu v0,566; andi v0,v0,0x7`; `lhu v0,538; andi v0,v0,0x800`; `addiu v0,v0,-1; sh v0,342(s1)` | B5 |
| Z_DOT_ZAEHLER | +0x236 += 1 je Root-Aufruf; INIT 0 | EMZ0 @0x801004F8-508, @0x801008AC | `lhu v0,566(s0); addiu v0,v0,1; sh v0,566(s0)`; `sh zero,566(s2)` | B5 |
| Z_DOT_TOD | Wort 0x0A03 (Brand) / 0x0B03 (Saeure); +0x1D2 := 4; +0x1D3 \|= 0x80; +0x21A \|= 0x2000 | EMZ0 @0x80101E54-90 | `addiu v1,zero,2563`; `addiu v0,zero,2819`; `addiu v0,zero,4; sb v0,466`; `ori v0,v0,0x80`; `ori v1,v1,0x2000` | B5 |
| Z_DOT_ZUCKEN | Brand: Gier += 64 − ((r1 >> (r2&3)) & 1)·128; Saeure: Gier += 64 − (rand >> 1) je Bild | EMZ0 @0x80101E10-50, @0x80101EA8-C0 | `srav` (Rohwort 0x00508007); `sra v0,v0,1` | B5 |
| Z_LEICHE_AUSBLENDEN | bei +0x10E & 0x80: jedes 4. Bild 15 Parts −0x010101, Stopp bei Part-0-Farbbyte 16 | EMZ0 @0x8010A810-868 | `lbu v1,112(a2); addiu v0,zero,16; beq`; `addiu a0,zero,15`; `0xFFFEFEFF` | B5 |
| H_DEATH_ROUTER | Z9 → 0x80104610 (P0 0x80104694); Z10/11 → 0x80104118 (+0x6 == 0 → 0x80105618[+0x5]) | EMD0G_MOD0 @0x801040E4-F8, @0x80104120-58 | `lbu v0,5(a0); lw v0,21964(at)` | B6 |
| H_TOD_HE | K0: +0x231 := 1, Kern, Teile-Wurf 0x80104440 (Parts {2,3,4,7,8,9,10}, Flags \|= 0x4A, +0x9C 800, +0x9A −150, +0x9E 10, +0xA4 −100, +0xA0 0, Farbe 0x00101040, Part-+0x98 := Gier, +0x1C0 \|= 1), Budget 1, FX 7 an rand&0xF (Flags&0x4A == 0), +0x21F := 18; je Bild FX 0 Part 3, FX 0 Part 2, FX 1+(rand&1) Part 2 | EMD0G_MOD0 @0x80104694-758, @0x80104440-4D8, Tabelle @0x80105680, @0x80104708, @0x8010464c/5c/78 | `sltiu v0,v0,0x3`; `ori v0,v0,0x4a`; `lui a2,0x10 / ori a2,a2,0x1040`; `addiu v0,zero,18` | B6 |
| H_TOD_BRAND | 17 Parts +0x70 := 0x00202020; 6× FX 7; +0x21F := 6; Gate +0x1D2 < 3 oder Zeile 16 | EMD0G_MOD0 @0x80104774-800, Gate @0x8010478C-A8 | `lui a0,0x20 / ori a0,a0,0x2020 / sw a0,112(v0) / sltiu v0,s0,0x11`; `sltiu v0,s0,0x6` | B6 |
| H_TOD_SAEURE | 17 Parts 0x00003F2F; +0x21F := 2; FX 9 an rand&7, FX 10 an (rand&7)\|8; Gate +0x1D2 < 3 | EMD0G_MOD0 @0x8010481C-89C | `addiu a1,zero,16175`; `sb v0(=2),543`; `ori a1,v0,0x8` | B6 |
| H_HURT | Z9 0x80103CE4, Z10 0x80103D9C (+0x1D3 \|= 0x80 → Immunitaet), Z11 0x80103E60 (1 Part rand&0xF := 0x3F2F, +0x21F := 1, FX 9); FX-8-Anzahl = ceil(n/2) | EMD0G_MOD0 Tabelle @0x80105538, @0x80103E28-3C, @0x80103E80-EC, Schleife @0x80103d38-58 + Spawner-Abzug @0x8010518c-98 | `ori v0,v0,0x80 / sb v0,467(a0)`; `addiu v1,zero,16175 / sw v1,112(v0)` | B6 |
| H_FX_TAB | FX7 {0x85,3,0,0x1000}, FX8 {0x85,4,0,0x1000}, FX9 {0x84,0x0F,0,0x1400}, FX10 {0x84,0x0F,0,0x0C00} | EMD0G_MOD0 @0x801056D6 / DC / E2 / E8 | `85 03 00 00 00 10` / `85 04 00 00 00 10` / `84 0f 00 00 00 14` / `84 0f 00 00 00 0c` | B6 |
| S_TOD_FARBE | Z10: 20 Parts 0x00202F2F; Z11: 20 Parts 0x00101F3F + Part 19 fliegt (Flags \|= 0x10, +0x9C/9D/9E 100/100/90), Gate +0x6 == 0 && +0x224 == 0 | EMS25.BIN @0x80104B44-4C, @0x80104BC0-F4, @0x80104ba4-bc; FUN_8010609C @0x8010609C-C4 | `lui a1,0x20 / ori a1,a1,0x2f2f`; `lui a1,0x10 / ori a1,a1,0x1f3f`; `sltiu v0,a2,0x14` | B7 |
| S_HURT11_GATES | +0x221 ≥ 3 und Bein k (rand&7) noch vorhanden | EMS25.BIN @0x801039dc-a00 | `lbu 545 / sltiu 3 / bne`; `lbu 544 / srlv / andi 1 / bne` | Port hat es |
| G5_FLINCH | Byte @0x801056B3 + Zeile; Z9/10/11 = 14; Schwelle 15; ≥ 11 → +0x225 := 7 | em36 @0x801029bc, @0x801056BC-BE, @0x80102a58, @0x801029c4-d0 | `lbu v0,22195(at)`; `0e 0e 0e`; `sltiu v0,v0,0xf`; `sltiu v0,v0,0xb / addiu v0,zero,7 / sb v0,549(s3)` | B9 |
| G5_ZERFALL | Fenster 15 beim ersten Akku-Treffer; Akku −1 je 16 Bilder | em36 @0x80102a48-4c; Main @0x80100118-15c | `addiu v0,zero,15; sb v0,545(s3)`; `addiu v0,v1,255`; `addiu v0,zero,15` | B9 (Port: 15 ohne Fenster) |
| G5_SPERRE | +0x1D3 −1 je Bild | em36 @0x801000ec-100 | wie SPERRE_ABBAU | B9 |
| G5_HP | 600 | em36 @0x801003fc | `addiu v0,zero,600; sh v0,342(s0)` | Port hat 600 |

### K8 RE2-FX-Maschine, Aufschlaege, Bodenfeuer, Assets — Spur C

| Name | Wert | Adresse | Instruktion(en)/Bytes | Verwendung |
|---|---|---|---|---|
| RE2_CORE00 | 8572 B; Ids `03 05 00 01 02 06 07 04`; Bank 3 @0x0008, Bank 4 @0x1BCC, Bank 5 @0x05F0 | `info/re2leon/COMMON/DATA/CORE00.ESP` → `shared_assets/RE2/CORE00.ESP` | Skripttabelle = Kopf + (2·ca + cb + 2)·4 (@0x8001bd08-1c) | C0/C5 |
| RE2_TEX_TIM_VRAM | Bild 256×256 hw → VRAM (768,256); CLUT (256,480) 32×19 → y = 480 + 0 | RE2 @0x8002b8cc-d4 (0x800cfbf0 := 28); Lader FUN_80076a40 @0x80076a64-a8, CLUT @0x80076afc-0c | `addiu v0,zero,28; sh v0,-1040(at)`; `lbu v1,-1040(v1); sll v0,v1,6; sltiu v1,v1,0x10; addiu v0,v0,-1024` (x = 28·64 − 1024 = 768); `sltiu; xori v0,v0,0x1; sll v0,v0,8` (y = 256); `addiu v0,v0,480` | C6 (Synthese: tpage 0x1E = Spalten 128..191 hw, 0x1F = 192..255 hw) |
| RE2_FX_POOL | 0x800D8CF0, Schritt 0x7C, 96 Plaetze | Spawner FUN_8001cbe8 (`addiu t2,zero,96`) | — | C5 |
| RE2_SPAWN_DEKODER | a0 = Bank<<24 \| Sub<<16 \| Skala; Sub&7 = Skript, Sub>>3 = CLUT-Zeile; +0x1C Bank, +0x1E Sub, +0x1B := 0 | FUN_8001BF10 @0x8001bf1c-c0 | `srl v0,a0,16; andi t5,v0,0xff`; `sll v0,t5,16; addu; sw v0,28(t0)`; `sb zero,27(t0)` | C5 |
| RE2_PUMPE | Update-Pass Op A (Status 0x8000); Draw-Pass: 0x4000-Befoerderung + Op A einmal, dann FUN_8001d68c (FUN_8001d894 Weltlage, Op B, Physik Bit 2, Anim Bit 1) | FUN_8001d300 (einziger Aufrufer @0x80026980), FUN_8001d68c, FUN_8001d894 | re2-fx-system.md §1d | C5 |
| RE2_OPTAB | [0] 0x8001dc28, [1] 0x8001dc30, [2] 0x8001dd2c, [19] 0x8001F2C0, [25] 0x8001FA08, [27] 0x8001FA9C, [28] 0x8001FBD0, [29] 0x8001FD5C, [30] 0x8001FECC, [40] 0x80020758, [46] 0x80020B60, [48] 0x80020F3C, [49] 0x800215C8, [50] 0x80021970, [58] 0x80022254 | Tabelle @0x8009D868 | `lw v0,-10136(at)` + `jalr` (@0x8001d6b8/0x8001d6f0) | C5 |
| OP49_PHASE0 | Phase 1, Status 0x8403, Op A 0, Op B 49, SE 0x01130001, Spritzer vel.y 240 / vel.x 0 / acc.x −23 / acc.y 0 | RE2 @0x80021678-7c, @0x80021690-b0, @0x80021738-7c | `lui a0,0x113 / ori a0,a0,0x1 / jal 0x8005ba28`; `ori v0,zero,0x8403; sh v0,24(v1)`; `addiu v0,zero,-23; sb v0,8(a0)` | C5 (Hitcodes 0x1002000B NICHT) |
| OP49_KINDER | Bild 0: 0x030F2000, 0x040C2000, 0x041D1800 (a1 +0x22, a2 0x8009DB44, a3 Platz+0x34); Ph.1 0x031F2000; Ph.2 0x03142000; Ph.3 0x040D2800; Ph.4 0x030F2000, dann frei | RE2 @0x80021780-84, @0x800217a8-ac, @0x800217c8/d0; @0x800217f0/0x80021804; @0x80021888-90; @0x800218c8-cc; @0x80021920-24; frei @0x80021950-58; Sprungtabelle @0x80010950 | `lui a0,0x30f; ori a0,a0,0x2000` usw.; `jal 0x8001cbe8` | C5 |
| OP48_PHASE0 | Translation := Aufschlag; Status 0x8000; Phase 1; Op B 48; SE 0x01120001; Kinder 0x040C2800 / 0x041D2700 (a1 0, a2 Platz+0x4C, a3 0) | RE2 @0x80020fe8-0x8002102c, @0x80021094-d8 | `ori v0,zero,0x8000; sh v0,24(a3)`; `sw v1,96(a3)` …; `lui a0,0x112`…`ori a0,a0,0x1`; `lui a0,0x40c; ori a0,a0,0x2800` | C5 (Hitcodes 0x0002000A NICHT) |
| OP48_FLAMMEN | IMMER 3 × 0x0505xxxx; Skala 7168 + (rng%8)·768; Gier + rng%40 / + rng%80 + 400 / + rng%80 − 400; vel.x += rng%25; acc.y += rng%8; +0x4A := 1 | RE2 @0x800210e0-0x80021410; +0x4A @0x8002121c/0x8002135c/0x800214b4; `jal 0x8001cbe8` @0x8002116c/0x800212c0/0x80021418 | `addiu s0,s0,7168`; `lui v0,0x505`; `0x66666667` >> 4 / >> 5; `addiu a1,a1,400` / `-400`; `0x51eb851f >> 3`; `sh s1,-29382(at)` | C5 |
| FLAMME_SKRIPT | Bank 5 Skr. 5: Op B 27, step[2] 46, step[3] 50, acc (0,5,0), vel (96,0,0) | RE2 CORE00.ESP @0x07F8 / Step @0x0800 | `00 1b 2e 32 00 10 00 10 00 05 00 00 60 00 …` | C5 |
| OP27 | Status 0xB003, TPage \|= 0x20, Anim rng%3, +0x14 := Boden, Op A 58, Op B 28, +0x1B := 2, Zaehler 8 + rng%3 | RE2 @0x8001fabc-bb8 | `ori v1,zero,0xb003; sh v1,24(a0)`; `jal 0x8004fba0` @0x8001fb4c; `addiu v0,v0,8; sh v0,66(v1)` | C5 (Boden → O-VB2) |
| OP58 | Zustand 2: ×1010/1000, ×1007/1000; dann Zustand 1, Zaehler 2 + rng%2, ×880/1000, ×800/1000; Zustand 0 tot | RE2 @0x80022350-84, @0x800223b8-e4, @0x800222c0-328, @0x800222a0-a8 | `0x10624dd3 >> 6` | C5 |
| OP28 | Wasser → tot; Boden bei y − 900 → Op[step[2]] (46); Wand → Op 46 oder Op[step[3]] (50) | RE2 @0x8001fc20-24, @0x8001fc6c-cc, @0x8001fcf8-30 | `addiu v0,v0,-900` | C5 (O-VB2) |
| OP46 | Op A 19, Op B 29, vel.x 180, acc.x −10 − rng%11, Zaehler 38 + rng%8, +0x12 &= 0xFFFE, +0x1B unveraendert | RE2 @0x80020b70-c24 | `addiu v0,zero,19` / `29` / `180`; `addiu v1,zero,-10`; `0x2e8ba2e9 >> 1`; `addiu v0,v0,38` | C5 |
| OP19 | Schaden-Tor +0x4A ≠ 0 && step[0x16] ≥ 16 && X-Aspekt > 0x1000 → Op 40; step[0x16]++ nur dort; Zustand 2 ×1009/×1002; Zustand 1 Zaehler 90 + rng%11 ×990/×980; Zustand 3 → 2 mit 30 + rng%8 | RE2 @0x8001f2d0-328, @0x8001f40c-444, @0x8001f4c0, @0x8001f3a4-3e4, @0x8001f4e8-51c | `sltiu v0,v0,0x10`; `sltiu v0,v0,0x1001`; `jal 0x80020758`; `addiu v0,v0,90` | C5 |
| OP40 | Box {−600,0,300,150}, Pruefpunkt y − 100, Hitcode 0x2002000A (Zeile 10, Kl. 2, erster); Treffer → Op 50 | RE2 @0x80020768-cc; Box @0x80010910 | `a8 fd 00 00 2c 01 96 00`; `addiu v0,v0,-100`; `lui a3,0x2002; ori a3,a3,0xa`; `jal 0x80021970` | C5 → `re2fx_applier` (V2b/V3) |
| OP50 | step[2] := 64, acc.x := 0, step[3] := 0, vel.x := 0, Step-Status +0x12 &= 0xFFFD | RE2 @0x80021978-b0 | `addiu v0,zero,64; sb v0,2(v1)`; `andi v0,v0,0xfffd` | C5 |
| OP29 | vel.x ≤ 0 → Stopp; Folgeflamme 0x0504xxxx (Skala ×0.8) wenn vel.x ≥ 61 und step[2] % 15 == 0; +0x4A := 1; step[2]++ | RE2 @0x8001fd6c-84, @0x8001fd78, @0x8001fd94-b8, @0x8001fdf0, @0x8001fe20, @0x8001fe30-3c | `slti v0,v0,61`; `0x88888889`; `lui v0,0x504` | C5 |
| OP30 | Op 2 (Anim-Start + rand); Sub == 4 → +0x1B := 2, Zaehler 2 + rng%8 | RE2 @0x8001fecc-f30 | `jal 0x8001dd2c` | C5 |
| OP25 | Wassertest der Folgeflamme: unter Wasser → Op A 0 / Status 0 | RE2 @0x8001fa20, @0x8001fa54-58 | `jal 0x800527b4` | C5 (O-VB2) |
| KIND_SKRIPTE | Bank 3 Skr. 7 @0x05B4 (Op1 Anim 11 → Op0 acc −18), Skr. 4 @0x050C (Op1 Anim 38 → Op0); Bank 4 Skr. 4 @0x2078 (Op1 Anim 3 → Op0 acc −8), Skr. 5 @0x20B0 (Op1 Anim 32, Aspekt 6656 → Op0); Status 0xB003 | RE2 CORE00.ESP (`re2_core00_scripts.txt`) | z. B. Skr. 7 S00 `01000b00001000100000000100000000000003b020000000` | C5 |
| RE2_RNG | 0..255 | RE2 FUN_80015FE8 @0x80016014 | `andi v0,v0,0xff` | C5 (Strom `re15_re2_rand`) |

### K9 Item-Debug des Statusschirms (RE1.5 PSX.EXE) — Spur A10

| Name | Wert | Adresse | Instruktion(en)/Bytes | Verwendung |
|---|---|---|---|---|
| DBG_START | SELECT-Flanke Pad 1 → 0x800b2668 := 1, SE 0x04090000 | @0x8004a138-58 | `lhu a1,-14494(a1)`; `andi v0,a1,0x100`; `beq v0,zero,0x8004a160`; `sh v0,9832(at)`; `lui a0,0x409` / `jal 0x80045024` | Einstieg |
| DBG_LADEN | Datei 0xb → 0x801a0000 | @0x8004a194-9c | `jal 0x80013b60` (a0 = 0xb, a1 = 0x801a0000, a2 = 0) | Zustand 1 |
| DBG_SCHREIBEN | Cursor-Platz 25bd: Id := 0x800b2669, Menge := 255, +3 := 0; Zustand := 3 | @0x8004a1d4, @0x8004a1f0, @0x8004a1f8/204, @0x8004a220, @0x8004a224-2c | `jal 0x800492b8`; `sb v1,0(at)`; `ori v1,zero,0xff / sb v1,1(at)`; `sb zero,1(at)` | Zustand 2 |
| DBG_TASTEN | R1 +1, L1 −1, R2 +10, L2 +246 (−10), Kreis Ende | @0x8004a238, @0x8004a258/60, @0x8004a308 | `andi 0x8` / `andi 0x4` / `andi 0x2` / `andi 0x1` / `andi 0x20`; `sb zero,9832(at)` | Zustand 3 |
| DBG_GRENZE | Id ≤ 0x47 | @0x8004a350-5c | `sltiu v0,v0,0x48`; `sb v0(=0x47)` | Kappung |

---

## PORT-ABGLEICH (konsolidiert; Datei:Zeile Stand a358fd5d..e577e98a, Zeilen laut Dossiers + Gegenpruefungen)

| Nr | Port heute (Datei:Zeile) | Soll (Beleg) | Luecke | Spur |
|---|---|---|---|---|
| P1 | `engine/src/game_step_common.c:1775` `ENT[9] = {1,1,1,0}` → `re15_player_weapon_fire(9)` `:1808-1809`: Sofort-Hitscan im Abzugsbild (gemessen: Zombie in d = 1299 stirbt 22 Bilder bevor die Granate die Hand verlaesst) | Entlade 0x80033B38 nur Munition @0x80033b40 | `resolve := 0` | A8 |
| P2 | `game_step_common.c:1951` Spawn-Gate `== 9`; `:1779-1780` ENT[10]/[11] nur Munition | E1/E8 | Gate 9/10/11 + Art-Kennung; ENT[10]/[11] bleiben | A8 |
| P3 | `game_step_common.c:1960-1961` Spawn mit `param 0`, `floor_y = pl->y` | Gier @0x800336cc → +0x2e @0x800197e4 | `param := pl->rot_y` | A8 |
| P4 | `engine/src/re15_esp.c:523-712` Routine A kennt 0/3/4/5/8/9/10/11/15/16/17/18/38; `:724-736` Routine B nur 12 | Routinen 30/31 (A), 29 (B) | fehlen | A3-A5 |
| P5 | `re15_esp.c:851-944` Tick: A und B je Platz verschraenkt, keine Weltlage, keine Euler-Integration, Kind-Init nur bei hoeherem Index im selben Bild | zwei Durchgaenge @0x80019e64-0x8001a47c | Tick umbauen | A2 |
| P6 | `re15_esp.c:916-922` Sammel-Bodenklemme fuer jeden phys-Platz ("50% restitution") | Boden nur in Routine B (12/29) | fuer Granatenplaetze aus | A6 |
| P7 | Weltlage ungedreht `f->x + f->xlat_x` (`re15_esp.c:728/916`, `platform/pc/main.c:296-298`) | RotMatrix(euler + (0,Gier,0))·xlat @0x8001a118-2a4 | wpos | A2 / C2 |
| P8 | Kinder ueber `re15_esp_fx_spawn_rows` (Flags 3) an der Ankerlage (`re15_esp.c:582-583, 641-642, 784`) | FUN_800199d4: Flags 0x0a, Anker Einheitsmatrix + Versatz P, Gier des Eltern | Kind-Spawner | A5 |
| P9 | Routine 9 (`re15_esp.c:646-651`) setzt keinen Latch; Latch ohne Leser (`game_step_common.c:1867` "consumer un-RE'd") | Setzer @0x80017694/@0x8001857c, Leser @0x8001ce60 | Latch + Leser | A7 / C3 |
| P10 | Zaehler aus xorshift `re15_engine_rand8` (`re15_damage.c:73-79`) | Formel aus 0x800acaec @0x80018484 | Formel | A3 |
| P11 | `platform/pc/main.c:5413` ESP-Tick vor `re15_game_step` `main.c:7358` | @0x8001ce0c < @0x8001ce2c | Tick hinter den Spielschritt | C1 |
| P12 | `main.c:445-448` uebergibt clut 0; Blaetter `extracted_fx/effect4_shell.tim` (CLUT 491 eingebacken) / `effect3_smoke.tim` (`main.c:3715-3719`) | CLUT 0x7B11 / 0x78D1 / 0x7851 aus TEX.TIM @0x334 / @0xF4 / @0x74 | Granate orange statt oliv, Feuerball grau | C2 |
| P13 | Waffen-SE ohne Haken fuer ARMS 0x0A und CORE 8; `RE15_EQUIP` laedt ARMS09 nicht (`main.c:4244-4252`) → Satz 0x0A ausserhalb von ARMS01 (10 Saetze) | @0x80018410-28, @0x800185e4-ec | Haken + Harness | C0 / C4 |
| P14 | `re15_damage.c:3334-3339` Gate B "OMITTED" (Kommentar falsch: es ist +0x93) | @0x80012f54-60 | einfuegen | B1 |
| P15 | NPC 0x40..0x4D werden vom Resolver getroffen; `enemy_ai_common.c:10550` haelt dann Idle | RE2: kein Kandidat (HP −1) | ausschliessen | B1 |
| P16 | Hitbox-Versatz fest (0,−h,0) und ungedreht (`re15_damage.c:3552-3561`, `:3276`); Hund 500/600 (`:3508`); NPC 450/1530 (`:3488-3490`) | FUN_8002b498 @0x8002b4bc-51c; Kaesten K4 | Versatz drehen, Kaesten | B2 |
| P17 | `re15_enemy_take_damage` Art 2..4 = 1000 flach auch fuer RE2-Modell-Typen (`re15_damage.c:2973`) | E4 | Modellwert | B3 |
| P18 | RE2-Stempel: Zeile 17 fuer Art 2..4 (`enemy_ai_re2_zombie.c:3847`), Zone aus der Spieler-Zielhoehe (`re15_damage.c:2932-2940`), Peilung Spieler (`:2986`, `enemy_ai_re2_zombie.c:6611-6621`), Reserve-Abzug (`:7004`, `:6623-6632`), +0x5-Zeile nur fuer die Zombie-Familie | E6, K6 | umbauen | B3 |
| P19 | Zombie-DoT fehlt, als "edge-fall"/"Jitter" fehlgedeutet (`enemy_ai_re2_zombie.c:1493-1494`, `:1603-1606`), kein Feld +0x236 (`:8062-8066`); Leichen-Ausblender OFFEN (`:7712`, `:7863`) | K7 Z_DOT_*, Z_LEICHE_AUSBLENDEN | bauen | B5 |
| P20 | Hund: DEATH 10/11 auf den Kern gefallen (`enemy_ai_re2_dog.c:2152-2156`), Teile-Wurf Render-OPEN (`:2159-2161`), HURT 11 ohne Part-Farbe (`:2070-2073`), FX-8-Anzahl n statt ceil(n/2) (`:2053-2066`), keine Immunitaet nach HURT 10 | K7 H_* | bauen | B6 |
| P21 | Spinne: Tod-Farben OPEN (`enemy_ai_re2_spider.c:2280`, `:2289`), Kommentar `:2285` ("verkohlt" = Saeure) | K7 S_TOD_FARBE | bauen | B7 |
| P22 | 0x29 Tod immer Clip 0xe + SE 7 (`enemy_ai_common.c:11381`), HURT ohne +0x5-Spuren (`:11367`) | K5 KAKERLAKE_* | Explosions-Spur | B8 |
| P23 | 0x23 Tod → sofort Zustand 7 (`enemy_ai_common.c:13173`) | K5 ALLIGATOR_TOD | Todesablauf | B8 |
| P24 | 0x2b Ph.2 `(hit_react & 2) ? 0x0b : 0x0a` (`enemy_ai_common.c:13590`), Ph.0 ohne `\|= 2` | K5 TYRANT_TOD | Bit 0x80, `\|= 2` | B8 |
| P25 | 0x27 Todes-Spurgrenzen (`enemy_ai_common.c:9419/9439`) schicken 12/13/14/19/20/21 in die Explosions-Spur | K5 MADE_DEATH_SPUREN | Grenzen | B8 |
| P26 | G5: Zuschlag nach ausgeruesteter Waffe (`enemy_ai_boss_g5.c:1534-1535`: 9 → 20, 10/11 → 5), Zerfall je 15 Bilder ohne Fenster (`:1500`), Treffer-Bit sofort geloescht (`:1525`), 1000 = Sofort-Tod (`:1506`) | K7 G5_*, E16 | bauen | B9 |
| P27 | Treppe setzt `hit_react` nie (`game_step_common.c:1383-1391`, `stair_common.c`); Folge: Tod ohne Clip 7 (`:1296-1300`) | K4 TREPPE_IFRAME | i-Frames | B10 |
| P28 | kein RE2-FX-System (`grep re2fx` leer; Plan re2-fx-system.md §3 unumgesetzt); `shared_assets/RE2/` ohne CORE00.ESP / TEX.TIM | K8 | Maschine + Assets | C0 / C5 / C6 |
| P29 | `re2z_part_tint[]` wird geschrieben, aber nirgends gezeichnet (Synthese: kein Leser ausserhalb `enemy_ai_re2_zombie.c`) | O-VB3 | Zeichner | C7 |
| P30 | Harness: State-Log ohne HP (`main.c:7404-7450`) | V5 | HP-Feld | C0 |
| P31 | `engine/src/menu_common.c:1258-1260` SELECT-Item-Debug "DEFERRED" | Absturz §2.2b, K9 | einziger Original-Weg zu 0x0A/0x0B fehlt (Port nur `RE15_GIVE`) | A10 |
| — | BLEIBT: Spieler-Versatz −1530 (`re15_damage.c:3460`), `re15_hitbox_overlap`/Sektor (`:3243-3286`, `re15_math.c:276-286`), Tabellen (`re15_damage.c:41-58`), Spielerzweig (`:198-246`), Spieler-Tod cmd 3, Hand-Netz (`main.c:8990-9010`), Birkin-Port-Tod (`enemy_ai_common.c:11846`), Liegend-Leiter tot (`enemy_ai_re2_zombie.c:7052-7063`), Pool-Reset bei Raumwechsel (`room_pc.c:130`, `scd_room_setup.c:199`), Drehrate 24 im Rueckstoss (`player_common.c:980-990`), Recoil-Break 10 (`:940-975`) | bytegleich bzw. belegt | keine | — |

---

## OFFEN (nicht vor dem Bau zu klaeren; mit versuchten Wegen)

| Nr | Offen | Versucht | Naechster Weg |
|---|---|---|---|
| O1 | Spawnhoehe h im Original (Pose-Ergebnis, Port nutzt Renderer-Knochen 11 des Vorbilds = Original-Zeitbezug) | statisch nicht ableitbar; Zensus 79 Savestates ohne Granate (Wurf §7); Absturz-Lauf va_a_01 zeigt TIEF Welt-y −704 bei Zaehler 5 / Zuender 42, aber ohne Bildstempel | Absturz-Werkzeug `re_absturz_werkzeug/lauf.py` mit `p1/p1_nach_start.sav`, Zwischenstaende je Bild um den Spawn (Frame-Advance), Platz 0x800a73b8+k·0x84 mitschreiben; Vergleich mit Port-h |
| O2 | Lage des Liegen-SEs (sp+16 Stapelrest) | volle Disasm R29, Aufrufkette | PCSX-Redux-Haltepunkt @0x80018358 (Skill re15-pcsx-watchpoint); ohne Folge, solange der Lage-Zweig fehlt (E14) |
| O3 | Randfall Zielbits 0xA000 (Pad oben + unten im selben Bild → Spawns in Bild 19 UND 24, Clip 13) | Wurf-GP N12 (@0x80033100-04, @0x80033150-54, @0x800334a8-bc) | Port fuehrt die Zielhoehe als einen Wert (`s_aim_elev`); fuer Bytetreue als Bitwort wie 0x800acaec fuehren (eigene Runde) |
| O4 | ESP-Weltlage im Flags-0x80-Zweig (@0x8001a0e8 / @0x8001a100) | nicht Teil der Granate | `re15_disasm.py dis 0x8001a014 80`; bis dahin Anker+xlat fuer diese Plaetze |
| O5 | Lage-Zweig der SEs: Daempfung belegt (h = sqrt(dx²+dz²), d = sqrt((cam.y − \|cam.y − P.y\|)² + h²), Pegel min(d·0x10624dd3 >> 36, 127) @0x80045b18-5c), Panorama FUN_80045d6c nicht RE'd | Wurf-GP §I | `dis 0x80045d6c`, dann fuer ALLE positionalen SEs bauen (nicht granatenspezifisch) |
| O6 | ARMS-Stimmenpfad (Byte3 & 0x1f = 16 → Warteschlange 0x800b2420, Prio-Tor FUN_80045a18 @0x8004523c-64) | Wurf-GP N13; Port hat das Prio-Tor (`audio_pc.c:703`) | Warteschlange im Port gegen @0x8004523c-64 pruefen |
| O7 | Flug der Hund-Teile (Flags 0x4A, +0x9A/+0x9C/+0x9E/+0xA4) und des Spinnen-Parts 19 (Flags 0x10): Part-Integrator in RE2 nicht RE'd; Port "Render-OPEN" | Dossiers nennen nur die Setzer | Xrefs auf Part+0x9C/+0xA4 in EMD0G_MOD0/EMS25 bzw. RE2-EXE; mit dem Zombie-Ragdoll-Code (`re15_re2z_ragdoll_part_anchor`) vergleichen |
| O8 | FUN_80065de0 (ratan2) Quadranten fuer Sektor-Kaesten | Struktur wie PsyQ ratan2, rsin/rcos belegt (Resolver-GP M7) | `dis 0x80065de0 80` gegen PsyQ; B2-Sonde mit allen vier Quadranten |
| O9 | Aussehen der RE2-Kinder (Anim/UV Bank 3/4/5 aus RE2-TEX.TIM) | CLUT-Zeilen 480..484 bytegleich RE1.5, Pixelseiten nicht (Saeure OFFEN 2) | Sprite-Katalog aus RE2 CORE00.ESP + TEX.TIM (Seiten ab (768,256)) rendern, TPage \| 0x20 additiv; Abnahme-Bild in C6 |
| O10 | NPC-Skript-Wiedereinfang nach Zustand 3 | statisch bis 0x80050f00 | durch E7 ohne Belang |
| O11 | "toxic gas" (RE1.5-Text 0x0A @0x800C53CC) gegen RE2-Saeure-Runde | RE2 hat keine Gas-Runde | Port-Zuordnung Op 49 bleibt (E8) |
| O12 | Absturz-Restpunkte: literaler Haltepunkt auf `80106c00` (durch die kausale Ein-Wort-Gegenprobe der GP ersetzt), Schreiber des Halbworts 0x8 (bestimmt nur die Form Standbild), voll natuerlicher Treffer ohne Lage-Patch, dynamische Belege fuer 0x13/0x1a/G-Birkin | Absturz §2.5-§2.11 (vb, vb4, vb6), GP §2.2 (K0/K1/K2) | fuer den Port ohne Belang (RE2-Ziel); bei Bedarf re15-pcsx-watchpoint auf 0x80000008..b |
| O13 | Nebenbefunde ohne Granatenbezug: VSync-Haken FUN_8007d904 (Waffe 12 → 19, 13 → 8, @0x8007d928-60); DEBUG.BIN laedt bei 0x800c0000 (Werkzeug nimmt 0x80100000 an); NPC-Band im Schusswaffen-Test (@0x800120d0-ec) | Saeure-GP §2, Wurf-GP §N, Bosse-GP §12 | Waffen-Dossier / Werkzeug-Pflege |
