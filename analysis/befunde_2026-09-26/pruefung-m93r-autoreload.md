# Pruefung: m93r-autoreload

Prueferrolle: Skeptiker. Geprueftes Dossier: `analysis/befunde_2026-09-26/m93r-autoreload.md`.
Keine Engine-Aenderung, kein Build. Alle Adressen unten selbst gelesen (PSX.EXE,
`re15_disasm.py` bzw. Roh-Byte-Scan RAM = 0x80010000 + Dateioffset - 0x800).

## 1. Selbst nachgelesen — 12 Adressen/Bloecke, ALLE bestaetigt

| # | Adresse / Datei | Dossier sagt | Ich messe |
|---|---|---|---|
| 1 | `0x80074030` 21×u32 | Id 5 -> 0x80032E9C, 0/1/2 -> 0x80034E70, 12/14/19 -> 0x80034014, 20 -> 0 | **identisch** (0x80032E9C = 2147692188) |
| 2 | `0x80074c88`+32 B | `80074c94: 15 05 02 00` | **identisch** (`15 03 02 00 / 15 04 02 00 / 15 05 02 00 / 15 04 02 00 / 17 07 02 00 / 16 08 02 00 / 15 0c 02 00`) |
| 3 | `0x80074dcc`+48 B | id 5 @0x80074de4 = `0f 00 00 00 94 4c 07 80 03 01 00 00` | **identisch** (Portion 15, Zeiger -> 0x80074c94) |
| 4 | `0x80033300`-`0x8003339c` | Gate mit `sltiu v0,v0,0x9` @0x80033368 | **identisch**, inkl. `sh 4 -> 0x800aca5a` @0x80033378 und Leer-Klick @0x80033384 |
| 5 | `0x8004eb70`-`0x8004ebd8` | roher Slot, `sll 24` + `slt zero,v0` @0x8004ebc4/c8 | **identisch** (Slot 0 UND -1 -> 0) |
| 6 | `0x80030540`-`0x80030584` | `xor/and` -> `sw 0x800ac76c` @0x8003057c = Druck-FLANKE | **identisch** |
| 7 | jal-Zensus (eigener Scan) | ea6c 2×, eb70 2×, ebdc 2×, eae4 12× | **identisch**; zusaetzlich `FUN_8004dfec` nur 2× (0x8004ebbc, 0x8004ec34) |
| 8 | `sh -> 0x800aca5a` Zensus | „genau EIN Nachlade-Pfad" | **29 Schreiber gesamt, davon in der Standard-FSM 14 — und nur `0x80033378` schreibt den Wert 4.** Aussage haelt |
| 9 | `ROOM11D0.RDT` +0x16C8 | `50 0a 09 31 … 05 00 0f 00 9b 00 ff 00` | **byte-identisch**, id 5, Menge 15 |
| 10 | `PL00W05.PLW` EDD (selbst geparst wie `emd_common.c:55-62`) | 14 Clips, Clip 0x0D = 32 | **identisch**: `[22,16,52,1,50,30,10,22,1,23,1,23,1,32]`; W03 `…,23,1,24,1,24,1,32`; W01 `…,25,1,20,1,20,1,15` |
| 11 | `lbu 0x800aca5d` Zensus | „kein `lbu 0x800aca5d` ausser `sltiu 9`" | **45 Lesestellen; eine davon liegt IM Nachlade-Sub** (s. 2.1) |
| 12 | `0x80033e28`-`0x80033e4c` | — | `lbu 0x800aca5d` @0x80033e34 / `ori v0,zero,0x7` / `bne` @0x80033e3c |

**Keine einzige zitierte Adresse des Dossiers ist falsch.** Es wurde auch gegen die
richtige Binaerdatei geprueft (`info/Re1.5/PSX.EXE`, kein Overlay). Meine Widerlegung
betrifft die SCHLUESSE, nicht die Bytes.

## 2. Bestrittene Ausschluesse

### 2.1 „Es gibt keinen waffenspezifischen Zweig im Nachladen" (§2.6) — FALSCH
Woertlich: „**Es gibt keinen waffenspezifischen Zweig im Nachladen.** Kein
`lbu 0x800aca5d` ausser dem `sltiu 9` @0x80033368."
Mein Zensus findet im Sub-4-Rumpf (0x80033d7c-0x80033ed8) genau eine:
```
  80033e34: lbu v1,-13731(v1)   ; 0x800aca5d
  80033e38: ori v0,zero,0x7
  80033e3c: bne v1,v0,0x80033e94
```
Waffe 7 (SUPER REDHAWK) hat im Nachladen einen eigenen Zweig (Speedloader-FX bei Bild 10)
— der Port baut ihn selbst nach: `player_common.c:1009-1020`. Folgenlos fuer die Munition,
aber der Satz „ich habe im Nachladen nichts Waffenspezifisches gefunden" ist nachweislich
zu weit gefasst und stuetzt genau die Schlussfolgerung, die das Dossier tragen soll.

### 2.2 Eigene Munitionssorte @0x80074c94 — Ausschluss HAELT
Selbst gelesen: Zeiger `0x80074de8` -> `0x80074c94`, dort `15 05 02 00`, `FUN_8004eb70`
liest `lbu a0,0(v0)` @0x8004ebb8 = **0x15**. Der Port fuehrt `s_wpn_props[5] = {15,0x15}`
(`inventory_common.c:192`), der Name 0x15 = „H. GUN BULLETS" (`inventory_common.c:270`,
Icon-Kachel 37 `item_icon_common.c:37`), die Startausruestung legt 0x15 in Slot 2
(`inventory_common.c:112`). **Die Munitions-Id-Zuordnung ist korrekt und identisch mit der
im Spiel gefundenen Sorte** — die vom Auftrag vermutete Fehlzuordnung existiert nicht.
(Nebenbefund: `FUN_8004ebdc` ruft `FUN_8004dfec` selbst noch einmal @0x8004ec34, ohne die
`slt zero`-Marotte; der Port nutzt dort `re15_ammo_reserve_slot()` MIT Marotte,
`inventory_common.c:224`. Fuer das Symptom irrelevant, weil das Gate vorher sperrt.)

### 2.3 Fehlender Nachlade-Clip PL00W05 0x0D — Ausschluss HAELT, aber nur fuer die DATEI
Ich habe W05 selbst geparst: 14 Clips, Clip 13 = 32 Bilder. Der Port fuettert die Laengen
aus genau dieser Bank (`main.c:7318-7335`, `wpn_bank_ok[5]`, Rueckfall W03 -> Clip 13
ebenfalls 32). `RE15_AIM_CLIP_MAX = 16` (`player_common.c:160`), `RE15_EMD_MAX_CLIPS = 64`
— kein Abschneiden. Ein „Clip-Laenge 0"-Haenger scheidet aus.

### 2.4 Zweiter Nachlade-Pfad — Ausschluss HAELT (haerter als im Dossier)
Das Dossier belegt ihn ueber die vier Munitions-Funktionen. Ich habe zusaetzlich ALLE
29 Schreiber von `0x800aca5a` gescannt und je Schreiber den Immediate aufgeloest:
in der Standard-FSM schreibt **nur** `0x80033378` den Wert 4. Der Ausschluss haelt.

### 2.5 Falscher Dauerfeuer-Latch `player_common.c:919` — Ausschluss HAELT, ERSETZT durch einen anderen
`s_aim_auto` latcht 12/14/19, Id 5 ist nicht dabei — korrekt. Aber im selben Latch-Block
steht `s_aim_melee = (re15_player_equipped_weapon() < 3)` (`player_common.c:917`), und
`re15_player_reload_start()` bricht **stumm** ab, wenn `s_aim_melee` gesetzt ist
(`player_common.c:340`: `if (s_player_aim_phase != RE15_AIM_READY || s_aim_recoil ||
s_aim_melee) return;`). Das Original kennt an der Stelle `sh 4 -> 0x800aca5a` @0x80033378
**keine** solche Bedingung. Der Latch wird nur beim Uebergang AIM_NONE -> RAISE neu
gesetzt (`player_common.c:915-921`); wer mit dem Messer zielt, im Menue die M93R anlegt
und ohne zwischenzeitliches AIM_NONE weiterzielt, bekommt beim leeren Magazin **weder
Nachladen noch Leer-Klick** — exakt das gemeldete Symptombild, und es steht auf keiner
Ausschlussliste. NICHT GEMESSEN, aber es ist der einzige Pfad im Port, der das Nachladen
*lautlos* verschluckt.

## 3. Munitions-Id — Ergebnis
Verlangt: **0x15** (`0x80074c94`, `lbu a0,0(v0)` @0x8004ebb8). Im Spiel vorhanden: **0x15**
(Briefing-Slot 2, `inventory_common.c:112`). **Kein Fehler.** Diese Spur ist tot.

## 4. Slot->Waffen-Zuordnung `inventory_common.c:153-160` — real, aber sie erklaert das Symptom NICHT

Der Code existiert wie behauptet:
```c
static int inv_resolve_slot(void) {
    int s = s_equipped_slot;
    if (s == RE15_INV_SLOT_NONE || s < 0 || s >= RE15_INV_MAX_SLOTS) return -1;
    ...
}
```
mit `RE15_INV_MAX_SLOTS 11` (`re15_inventory.h:19`). Die Divergenz zum ungeprueften
`lbu v1,0(at)` @0x8004eb90 ist also **real**. Ihre Deutung als Ursache faellt an zwei Stellen:

1. **Sie verhindert das FEUERN, nicht das Nachladen.** Mit `resolve == -1` liefert
   `re15_ammo_mag_nonzero()` in JEDEM Bild 0 (`inventory_common.c:167-170`), also nimmt
   `game_step_common.c:1597-1601` **immer** den Leer-Zweig — die M93R koennte nie einen
   Schuss abgeben. Der Nutzer meldet eine Waffe, die geschossen hat, bis sie leer war.
   Das Dossier behauptet dennoch (Zeile 271): „Das ist eine belegte Divergenz mit exakt
   dem gemeldeten Symptombild." Das ist sie nicht.
2. **Der Zustand ueberlebt das Anlegen der M93R nicht.** Der Equip-Commit
   `menu_common.c:1344-1347` ruft `re15_player_set_equipped_weapon(wid)`, und der schreibt
   den Slot neu: `re15_inv_set_equipped_slot(find_item(id))` (`re15_damage.c:1032-1036`) —
   Ergebnis immer 0..10 oder 0x80. Ein Slot >= 11 kann also nur entstehen, wenn NACH dem
   Anlegen der M93R eine breite Waffe (0x0e..0x13) aufgesammelt wird (`+2`,
   `inventory_common.c:51`, byte-true @0x8004dc94). Das ist eine sehr viel engere
   Vorbedingung, als das Dossier nahelegt.

Nebenbefund am selben Pfad (vom Dossier nicht gesehen): `menu_common.c:1345`
`g_inv.slots[eq & 0x0f].id` liest bei `eq >= 11` **ausserhalb** des Arrays
(`re15_inventory.h:36`, 11 Elemente) — bei `eq == 11` faellt es auf
`g_inv.last_pickup_type` (`re15_inventory.h:38`).

## 5. Ueberzogene Saetze (woertlich)

* Titel: „**der Original-Mechanismus ist vollstaendig belegt, der Port spiegelt ihn
  Bedingung fuer Bedingung**" — widerlegt durch 2.1 (Zweig @0x80033e34 uebersehen) und 2.5
  (Zusatz-Bedingung `s_aim_melee` @`player_common.c:340`, im Original nicht vorhanden).
* §3, Zeile 225-226: „**Ergebnis der Gegenueberstellung: fuer ITEM-Id 5 ist im Port keine
  einzige Bedingung falsch.**" — Die Gegenueberstellungs-Tabelle §3 fuehrt
  `re15_player_reload_start()` als Spiegel von `sh 4 -> aca5a` @0x80033378, verschweigt
  aber dessen drei Abbruchbedingungen. Eine davon hat im Original kein Gegenstueck.
* §1, Zeile 20-21: „**Das heisst: der Nachlade-Kern des Ports funktioniert — gemessen**" —
  gemessen wurde eine Unit-Sonde mit synthetischer Ausruestung fuer Id 3; das Dossier
  schreibt zwei Zeilen spaeter selbst „Fuer Id 5 gibt es keine gebaute Sonde".
* §3.1(iii), Zeile 271: „**mit exakt dem gemeldeten Symptombild**" — siehe 4.1.
* §2.7: „**Fuer Id 5 existiert also GENAU EIN Nachlade-Pfad**" — als Aussage ueber den
  Sub-Wechsel korrekt (von mir unabhaengig bestaetigt), als Aussage ueber den Nachlade-
  VORGANG durch 2.1 relativiert.

## 6. Risiko des Umsetzungsplans

Schritt 2 will die Schranke in `inventory_common.c:153-160` aufgeben. Beide dort genannten
Formen sind gefaehrlich:
* **Schranke entfernen** -> `re15_ammo_consume()` (`inventory_common.c:180-185`) schreibt
  `g_inv.slots[11].qty--` bzw. `[12]`. Das Array hat 11 Elemente (`re15_inventory.h:36`);
  dahinter liegen `last_pickup_type` / `last_pickup_amount` / `last_pickup_display_frames`
  (`re15_inventory.h:38-40`). Ein Magazin-Abzug wuerde die HUD-Aufsammel-Anzeige
  ueberschreiben — stiller Speicherfehler statt Fix.
* **Array verbreitern** -> `RE15_INV_MAX_SLOTS` (`re15_inventory.h:19`) steht im
  Save-Layout `re15_savedata.h:114` UND `re15_savedata.h:152`. Jede Aenderung bricht
  bestehende Spielstaende.
Und: Schritt 2 wuerde eine Divergenz schliessen, die laut 4.1 das gemeldete Symptom gar
nicht erzeugen kann.

## 7. Messplan, der in EINEM Lauf entscheidet

Der Haken existiert schon: `RE15_WAFFEN_LOG`, `game_step_common.c:1575-1588`. Er ist rein
lesend zu erweitern um sechs Felder (alle ueber vorhandene Getter, keine Logik-Aenderung):
`slot=` `re15_inv_equipped_slot()` · `res=` `re15_ammo_reserve_slot()` ·
`amm=` `re15_inv_find_item(0x15)` · `wq=` Magazin-Menge des aufgeloesten Slots ·
`edge=` `(c->pad_pressed & RE15_PAD_BIT_SQUARE)` · `melee=` `s_aim_melee`
(neuer Getter neben `re15_player_aim_dbg`), plus EIN voller 11-Slot-Dump (id/qty/flags)
bei jeder Aenderung von `slot`/`amm`.

Lauf: Briefing -> ROOM11D0, M93R aufsammeln, im Menue anlegen, R1, 5 Bursts bis `wq=0`,
Abzug **loslassen**, neu druecken. Entscheidungstabelle der einen Log-Zeile beim
Flanken-Bild:

| Messung | Ursache |
|---|---|
| `slot >= 11` (und `wq` schon vor dem 1. Schuss 0) | (iii) — aber dann hat die Waffe nie gefeuert |
| `slot < 11`, `amm == 0` | die 0x15-Schachtel fehlt wirklich — Nutzerirrtum |
| `slot < 11`, `amm == 0` bei vorhandener Schachtel in Slot 0 | (i) Slot-0-Marotte, byte-true @0x8004ebc8 |
| `edge == 0` in allen Bildern nach `wq=0` | (ii) Abzug gehalten, byte-true @0x80033344 |
| `edge == 1`, `res > 0`, `w == 5`, aber `ph` wird nie RELOAD | **2.5** — `s_aim_melee`/`s_aim_recoil` verschlucken `player_common.c:340` |
| `ph == RELOAD`, aber `frame` erreicht nie `fc-1` | Clip-Laenge — statisch bereits ausgeschlossen (32, s. 2.3) |

Ohne diese eine Zeile ist jede Aenderung geraten.

## 8. Urteil

**Nicht haltbar** — nicht wegen falscher Adressen (es ist keine falsch), sondern weil der
Mechanismus das Symptom nicht erklaert: die einzige vom Dossier zum Fix vorgeschlagene
Divergenz (Slot >= 11) wuerde die M93R am Feuern hindern, nicht am Nachladen, und wird vom
Equip-Commit selbst wieder aufgehoben. Zugleich uebersieht das Dossier den einzigen Pfad im
Port, der das Nachladen lautlos verschluckt (`player_common.c:340`), und behauptet zwei
Absolutheiten, die ich widerlegen konnte (waffenspezifischer Zweig @0x80033e34;
„Bedingung fuer Bedingung").
