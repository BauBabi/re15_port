# Spur C (Generator ROOM11F0/11F1) - ABNAHME 1 (unabhaengig)

Stand: in Arbeit (Geruest angelegt beim Start der Abnahme).
Pruefer: Abnahme-1-Agent, hat den Code NICHT geschrieben. Ziel: widerlegen, dass Spur C fertig ist.

## 0. Urteil

(offen)

## 1. Gelesener Stand (git log, Dossier, Gegenpruefung)

* Zweig `r34n/generator`, HEAD `71057ac1` (= Cherry-pick Bau-Fix 7d4d11dd) auf `f0ff11cf`
  (Abschluss Spur C). Spielcode seit `42baac77` unveraendert (`git diff --stat 42baac77..HEAD --
  re15_port/engine re15_port/platform re15_port/include` = leer) -> die exe-Messungen des Bauers
  (§9.6, Kopie `re15_r34nc.exe`) gelten fuer den heutigen Spielcode.
* Geaendert gegen die Basis cf0e68ba (Code): `panel_zeiger_common.c` (Sperre = nur noch
  `aktiv && !4:238 && maske == 0x155`, Lampenzustand je Tick), `re15_panel_zeiger.h` (Kopf,
  Konstanten), NEU `platform/pc/src/panel_lampen_pc.c` (+ `shared_assets/RE2/LAMPE2130.TIM`),
  2 Zeilen `main.c` (Haken hinter dem Zeiger-Block), Kommentar `game_step_common.c`, Gate in
  `release/make_package.sh`, Riegel `unit_r34n_c_generator` + Umbau `r31_generator` Teil D.
* Dossier `C_generator.md` §0..§9 und Gegenpruefung (haltbar mit 10 Auflagen, alle als
  ERLEDIGT abgehakt §9.1) gelesen. Vorhandene Belege des Bauers: S1..S7 + gdigrab
  (`C_belege/bau_*`). Live NICHT gefahren (§9.9 Nr. 8): ROOM11F1, der Ablauf NACH "Power supply
  OK." (Meldung wegdruecken, Flackern Cut 0x0D/0x0E, Rueckgabe der Steuerung), Wiedereintritt/
  EXIT mit brennender Lampe, Statusschirm (START) bei brennender Lampe, falscher Schalter
  RECHTS an der exe (nur Riegel F).
* Selbst nachgelesen (Bytes): `LAMPE2130.TIM` == `ROOM2130.RDT[0xE398, +4256)` (True),
  Kopfwort[20] = 0xE398; effect.esp @0x3188 `16 ff ..`; Zellen @+0x70 (`60 00 f0 f0` / `80 00
  f0 f0` = Zelle 3/4); Anim-Saetze @+0x10: Satz 3 `00 01 ff 20` (Sprung auf Satz 0), Satz 4
  `03 01 01 20`, Satz 5 `04 01 01 20`, Satz 6 `04 01 ff 20` (Sprung auf Satz 4) -> Zelle 3,4,3,4;
  @0x01294 `64 01 16 02 00 00 ba 02`, @0x017A8 `64 0d 16 10 .. b4 0b`, @0x017B8 `64 0e 16 10`,
  @0x01814 `65 0d 65 0e 22 02 07 00`. RE1.5 ROOM11F0 sub06/sub16/sub17/sub18 im Dump des
  Bauers (`build/r34n_c/room11f0_scd.txt`) gelesen: sub17 (EXIT) setzt Cursor + alle zehn Hebel
  zurueck (@0x01602 Pos_set, @0x0160E..0x0167A Dir_set) und loescht Bank 5 Bits 0..22.

## 2. Eigener Bau + Suite-Zeile

(offen)

## 3. Nutzerpunkte aus AUFTRAG.md an der echten exe

### 3.1 Freie Bewegung nach einem Schalter (keine Sperre)

(offen)

### 3.2 Endsperre bis 80 + Ruhezeit, dann OK + Lichter

(offen)

### 3.3 Lampen-Zuordnung links/rechts

(offen)

### 3.4 Lampen-Kunst / Lage / Zeichenreihenfolge

(offen)

### 3.5 Varianten 11F0/11F1, Laden/Speichern, Wiederbetreten, nach der Loesung

(offen)

## 4. Gegenproben (falsche Eingaben, Durchlaufen der 80, Nachbarverhalten)

(offen)

## 5. RE-Gate-Pruefung des Codes

(offen)

## 6. Maengelliste

| # | Titel | Schwere | Beleg |
|---|-------|---------|-------|

## 7. Belege (Dateien unter C_belege/)

(offen)
