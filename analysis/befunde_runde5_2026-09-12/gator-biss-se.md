# BEFUND — gator-biss-se: "Der Beiss-Sound vom Aligator passt auch noch nicht"

Nutzer 2026-09-12, nach dem Runde-4-Tausch SE4→SE3 (Bank 17). Analyse Runde 5.
Vorgänger: analysis/befunde_runde4_2026-09-12/gator-se-korrektur.md.

## KURZFAZIT

**RE1.5 HAT einen passenden Biss-Impakt — und er liegt schon geladen im Boss-Raum.**
ROOM2090s eigene Raum-Bank snd1 (RDT +0x14/18/1C) trägt auf **SE 2** einen 0,22-s-Impakt
mit Sofort-Attack und ZCR ~294 (dumpf/tieffrequent, 22050 Hz, vol 127) — und genau diese
SE-Id ist in RE1.5 byte-belegt der **"Angriff-trifft-den-Spieler"-Laut des Raum-Raubtiers**
(Adult-Spider-Connect @0x8011254c-58 und @0x801129c4-d4, STAGE2.BIN). Der heutige Bank-17-SE 3
ist dagegen ein heller 22-kHz-Wasser-Klatsch; seine Wiedergabe im Port ist **byte-true**
(Pitch 0x800 nachgerechnet) — falsch ist das SAMPLE, nicht der Player. Nutzer-Mandat
(Präzedenz Zombie-Mapper `pc_re2z_se_re15`, main.c:693): RE1.5-Sounds bevorzugen →
Gator-SE-Hook um einen Mini-Mapper ergänzen: **id 3 → `re15_audio_room_se(2)`**.

## 1. ROOM2090-Raumbänke — Dump + WAV-Render (Aufgabe 1)

RDT-Adresstabelle ROOM2090.RDT (Datei-Offsets): snd0 edt=0xB538 vh=0xB5B8 vb=0xD058;
snd1 edt=0xC3D8 vh=0xC438 vb=0x15D68. snd1: EDT 96 B (24 Records), VH "pBAV" ps=1 ts=11
vs=10, VB 53104 B. Alle SEs als WAV: **analysis/befunde_runde5_2026-09-12/gator_room_se/**
(Pitch je Tone via note2pitch2(min,shift,center,shift) — identisch zur Port-Wiedergabe).

snd1 (die Bank, aus der `re15_audio_room_se` = FUN_800453d0 spielt; beim Raumladen
resident, room_common.c:412 / audio_pc.c load_room_se_vab_pc):

| SE | Rec | Stimme/Prio | VAG (B) | Rate | Dauer | Profil | RE1.5-Bedeutung (Beleg) |
|---|---|---|---|---|---|---|---|
| 0 | 00001117 | v7 p1 | 2 (5024) | 22050 | 0,40s | hell (zcr 2221), schneller Abfall | Platsch/Splash |
| 1 | 00002114 | v4 p1 | 1 (48) | 689 | 0,08s | Stille (Dummy-VAG) | — |
| **2** | **00003214** | **v4 p2** | **3 (2752)** | **22050** | **0,22s** | **Attack 0,00s, decay 0,18s, zcr 294 — kurzer DUMPFER Impakt** | **Angriff-CONNECT des Raum-Raubtiers: Spider-STRIKE @0x8011254c-58, Spider-LEAP @0x801129c4-d4 (Port: enemy_ai_common.c:10599/10632); auch Flinch @0x80113a0c** |
| 3 | 00004215 | v5 p2 | 4 (11200) | 22050 | 0,89s | hell-rauschig (zcr 3192), 63% laut | großes Wasser-Toben |
| 4 | 00005216 | v6 p2 | 5 (5968) | 22050 | 0,47s | Doppel-Burst `#+@#=-` | Gift-SPUCK-Attacke (clip 0xc) @0x80112778 (Port :10616) |
| 5 | 00006116 | v6 p1 | 6 (7056) | 22050 | 0,56s | hell, kurz | Spider-Hurt/Death @0x80111f4c u.a. |
| 6 | 00007115 | v5 p1 | 7 (7712) | 22050 | 0,61s | unregelmäßig (zcr 494) | (FUN_801127f0-Familie) |
| 7 | 00008116 | v6 p1 | 8 (4640) | 11025 | 0,73s | mittel | (FUN_8011461c) |
| 8 | 00009217 | v7 p2 | 9 (5520) | 22050 | 0,44s | 86% getragen | Spider-Schritte @0x80111a44-a8, alle 13 F @0x80111d04-44 |
| 9 | 0000a317 | v7 p3 | 10 (3184) | 11025 | 0,50s | Anschwellen+Stopp | Angriffs-WINDUP @0x80111e90-a0 |
| 10 | 00004215 | v5 p2 | 4 | 22050 | 0,89s | = SE 3 (Record byte-identisch) | Spieler-getroffen-SE der Engine: FUN_80012d60 ruft FUN_800453d0(10) — aber NUR bei attack_type<2 (RE_15_Quellcode_V2/FUN_80012d60.c:38-40/66-68) |

**Dieselbe snd1-Bank liegt byte-identisch in allen vier Wasser-Räumen** 2080/2090/2091/20A0
(md5 edt=383c852c2d, vh=0f5b769b07, vb=98a5c10234) — es ist die STAGE2-Wasserareal-Kampfbank.
snd0 (32 Records, Schritte/Ambiente, ebenfalls gedumpt: gator_room_se/room2090_snd0_*.wav)
enthält keinen Biss-Kandidaten (Schritt-/Tür-/Ambiente-Sätze).

## 2. Wer wohnt in RE1.5 wirklich in ROOM2090 — und was sagt der 0x23-Code? (Aufgabe 2)

- Sce_em_set-Scan (op 0x44, 20 B, Typ auf +2 — Handler 0x800420A0, `addiu v1,v1,20`
  @0x8004262c): ROOM2090.RDT spawnt **2× Typ 0x25 (Adult Spider)** @Datei 0x0AB0/0x0AC4;
  ROOM20A0 ebenso (0x0AB0-Analoge @0x18CE ff.). **Kein 0x23-Spawn** — den Alligator setzt
  erst der Port ein (main.c:6045, Nutzer-Design).
- Die RE1.5-0x23-KI (Familie 0x8010c448..0x8010ee38, STAGE2_full, 23 Funktionen) ruft
  **keine einzige SE-Funktion** (grep über alle 23 Dateien: 0 Treffer auf
  func_0x800453d0/func_0x80045024). Ihre Laute kämen nur aus EDD-Frame-Flags — und das
  RE1.5-EM023-Modell ist eine belegte DATENLÜCKE (kein Record im RE1.5-CDEMD0.EMS,
  main.c:932-941). **Es existiert also nirgends ein RE1.5-Alligator-Sample**; die
  snd1-Bank des Raums ist die einzige RE1.5-Originalquelle, und ihre Angriff-Connect-Id
  ist SE 2 (§1). Global-Bänke (CORE/ARMS) mussten daher nicht durchsucht werden —
  die Raum-Bank hat einen Kandidaten, und das Mandat bevorzugt genau sie.

## 3. Port-Player-Prüfung: Pitch/Volume von Bank-17-SE 3 (Aufgabe 3)

`re15_audio_re2_enemy_se` (audio_pc.c:1105 ff.) spielt EDT-byte-true: Map-Eintrag SE3 =
0x02430000 → prog 0, tone 4, Kanal 2, Prio 3; Tone-Bytes (ENEMSE.VBS Bank-17-VH, selbst
geparst): center=75, shift=0, min=63, vol=90 → note2pitch2 = **0x800 = 22050 Hz** (SE4:
center=85, min=64 → 0x4C1 = 13102 Hz — Runde-4-Wert bestätigt). step_q16 = pitch<<4 gegen
44100-Ausgabe (audio_pc.c) == exakt die Render-Rate der Runde-4-WAVs. **Der Port spielt
SE 3 also korrekt — "falscher Pitch macht ein Zip" ist WIDERLEGT.** SE 3 IST ein heller
Klatsch (RE2-Kontext Wasser-Einschlag, Clip 5 f80); als Zubeiß-Laut bleibt er ein Fremdkörper.

## 4. FAZIT — Rangliste + Patch-Plan (Aufgabe 4)

**Rang 1 (empfohlen, RE1.5-Mandat): ROOM2090 snd1 SE 2 als Zubeiß-Impakt.**
Anhören: `analysis/befunde_runde5_2026-09-12/gator_room_se/room2090_snd1_se02_vag3.wav`
(0,22 s, dumpf, 95% FS). Beleg: die authored Connect-Id des Raum-Raubtiers (§1).
Patch (Muster = Zombie-Mapper pc_re2z_se_re15, main.c:693, "NUTZER-MANDAT 2026-08-23"):

1. `re15_port/platform/pc/main.c` — neben pc_re2z_se_re15 (nach :722) einfügen:
   ```c
   /* Gator-SE-Mapper (Nutzer 2026-09-12 "Beiss-Sound passt nicht"): das ZUBEISSEN
    * (id 3, 6 Rufstellen enemy_ai_boss_gator.c) wechselt auf die RE1.5-Raum-Bank:
    * ROOM2090 snd1 SE 2 = der Angriff-Connect-Impakt des Raums (Spider-STRIKE
    * @0x8011254c-58 / LEAP @0x801129c4-d4). Rest bleibt ENEMSE Bank 17. */
   static void pc_gator_se_re15(int se_id, int flag2000)
   {
       if (se_id == 3) { re15_audio_room_se(2); return; }
       re15_audio_re2_enemy_se(se_id, flag2000);
   }
   ```
2. `main.c:946` — `re15_gator_audio_hook(pc_gator_se_re15, re15_audio_re2_enemy_bank);`
3. `enemy_ai_boss_gator.c` — KEINE Codeänderung nötig (die 6 gb_se(3)-Stellen :360, :1313,
   :1377, :1711, :1895, :1944 bleiben); nur den Audio-Kopfkommentar :238-252 ergänzen.
   Chirurgisch sicher: die EM23-Frame-Flags liefern nur Ids 2/4 (Clips 0/6/7; Clip 5 mit
   dem SE-3-Flag wird nie gesetzt — kein `motion = 5` in der Datei), id 3 kommt also
   ausschließlich von den 6 Zubeiß-Stellen.

**Rang 2 (falls SE 2 allein zu trocken): SE 2 + Raum-SE 9 als Windup.** Am Schnapp-START
zusätzlich `re15_audio_room_se(9)` (Windup-Beleg @0x80111e90-a0;
room2090_snd1_se09_vag10.wav). Andere Stimme (v7 vs. v4) — kollisionsfrei.

**Rang 3 (Beim-FRESSEN-Variante): Raum-SE 3/10** (room2090_snd1_se03_vag4.wav, 0,89 s
Wasser-Toben) — die Engine-eigene "Spieler-wird-getroffen"-Wahl dieses Raums (SE 10,
FUN_80012d60.c:40) — nur an den Schnapp-AM-LEON-Stellen (:1895/:1944), nicht am Luft-Schnapp.

**Rang 4 (Status quo): Bank-17-SE 3 behalten** — Wiedergabe nachweislich byte-true (§3),
aber klanglich der falsche Kontext (heller Wasser-Klatsch statt Biss).

## Reproduktion

`analysis/befunde_runde5_2026-09-12/tools/room_banks.py <room…>` — dumpt snd0/snd1
(EDT-Records mit FUN_800453d0-Feldsemantik, VH-Tones, note2pitch2-Pitch) und rendert
jeden SE als WAV nach gator_room_se/. Eingaben: re15_port/shared_assets/PSX/STAGE2/*.RDT,
info/re2leon-Daten nur für §3 (ENEMSE.VBS + PSX.EXE-TOC @0x800A7B1C).
