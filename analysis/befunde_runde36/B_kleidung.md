# Runde 36 — Spur B "kleidung" (2026-10-10)

Zweig `r36/kleidung`, Basis master 5a9dbb7b (= v0.8.23 3ec5b1f8 + Auftrag). Dossier fortlaufend.

## Auftrag (Wortlaut, AUFTRAG.md Z.32)

> Aus irgendeinen Grund hat Leons Kleidung jetzt keine Blutflecken/Risse mehr, wenn er auf Warning/Danger runter kommt.

Spur-Zuteilung (VERTRAG.md §0): "Regression, bisecten, beheben, Pin". Keine Bank-9-Bits, keine
Nachrichten-IDs, keine Ereignis-Nummern fuer Spur B.

## 1. Messung vorher

### 1.1 Wie der Port die "zerrissene/blutige Kleidung" realisiert (Bestand)
Blut-Decal-Wundsystem (analysis/blood_decals.md, 2026-08-02/03): Wund-Tabelle `s_wounds[8]`
{level, acc} in engine/src/re15_damage.c (= Tabelle @0x800b10ec), `re15_wound_add` = FUN_80037edc,
Anwendung als Rect-Kopie Damage-Bank -> Panel im Slot-0-Stash (render_pc.c
`re15_render_pc_wound_blit`, main.c "Blut-Decal-Sync" ~Z.9288). Es gibt KEINE Gesundheits-Stufen-
Textur (kein hp-gated Texturwechsel) — "Warning/Danger" ist nur die Beobachtung des Nutzers, der
Mechanismus ist der Treffer-/Griff-Akkumulator (Schwelle 120 je Panel).

Trigger im Port (Zensus `re15_wound_add`/`re15_wound_release_stamp`, identisch in 9362dd7e,
87cc8575, 3ec5b1f8):
* enemy_ai_common.c:2493 — RE1.5-Zombie-Griff [4] THROW-OFF (FUN_80102548): `release_stamp(dir)`
* enemy_ai_common.c:4273 — RE1.5-Kriecher-Griff [4] (0x80103f38): `release_stamp(sub+1)`
* enemy_ai_common.c:5925 — RE1.5-Kraehe Angriff (0x801161e8), :7487 RE1.5-Hund Release (@0x80111b64),
  :8072 RE1.5-Hund Biss (@0x8010f360), :1799 RE1.5-Hund-Finisher (@0x80111e78-90)
* affen_11c0.c:498 — Gorilla-Finisher (@0x8011c55c-74, nur beim Tod)
* enemy_ai_re2_zombie.c / enemy_ai_re2_dog.c / enemy_ai_re2_crow.c: KEIN einziger Aufruf.

Default-KI seit e11962ec (2026-08-22, "RE2-KI ist jetzt der Standard") = RE15_AI_FLAVOR_RE2
(enemy_ai_re2_zombie.c:101); die Wahl im OPTIONS-Menue wird nicht gespeichert (pc_ai_mode_set ->
re15_ai_flavor_set, kein Config-File) — jeder Spielstart laeuft RE2.

### 1.2 Messung am gebauten Stand (master 5a9dbb7b = v0.8.23 + Auftrag)
Sonde `test_r36_kleidung griff <re2|re15>` (tests/unit/test_r36_kleidung.c): ROOM1140 ueber den
echten game_step-Weg (SCD-Spawns, re15_game_step, Pad-Kanten), Greifer = erster stehender Zombie
(Slot 2 Typ 0x10), Leon daneben, HP wird >=60 gehalten (kein Devour/One-Save), Knopf-Hammern alle 2
Takte im Griff, 4 ueberlebte Griffe (Opfer-Zustand 1 -> 3):

| Flavor | Abwurf 1 | 2 | 3 | 4 | Ergebnis |
|---|---|---|---|---|---|
| RE2 (DEFAULT) | alle 8 Panels L0 | L0 | L0 | L0 | **nie ein Stempel, gen bleibt 1** |
| RE1.5 | L0 | L0 | p4=L1 p7=L1 (gen 3) | unveraendert | Stempel beim 3. Griff (Helper 0x8010a208 von hinten: (0,10)(4,50)(7,50) -> 150 >= 120) |

Nutzer-Spielstaende (C:/workspace/git/reAi_v2/re15_port/build/platform/pc/re15_card.mcr, 5 Slots,
alle v9): wounds[8][2] in ALLEN Slots 0/0 — auch Slot 4 (ROOM1070, hp=20 = Danger) und Slot 3
(Elza ROOM1071, hp=60). befund.log des Nutzers: HP-Verlauf 100 -> 80 -> 60 in ROOM1190 (Hunde,
RE2-Hund -20 je Biss @0x80104EB8), danach 65. Also: der Nutzer kam ueber RE2-Gegner auf
Warning/Danger, und die Wund-Tabelle blieb leer — exakt die Sonde.

## 2. RE-Belege (Adressen, Bytes)

(wird gefuellt)

## 3. Bisect

Dieselbe Sonde (identischer Quelltext) gegen die Engine von v0.8.21 (9362dd7e, Quellbaum per
`git archive` in den Scratchpad, Assets per Junction, eigener Build):

| Stand | RE2 (Default) | RE1.5 |
|---|---|---|
| v0.8.21 9362dd7e | 4 Abwuerfe (F98/F198/F492/F848), alle Panels L0 | Stempel beim 3. Abwurf F416: p4=L1 p7=L1 |
| master 5a9dbb7b (= v0.8.23) | 4 Abwuerfe (F98/F198/F492/F848), alle Panels L0 | Stempel beim 3. Abwurf F416: p4=L1 p7=L1 |

Bild-fuer-Bild identisch. Zwischen v0.8.21 und v0.8.23 hat sich am Wundsystem NICHTS geaendert
(git grep der Aufrufstellen an allen drei Tags identisch; render_pc.c-Diff der Runde 35 beruehrt nur
Raum-Slots 1..18/24..49, Slot 0 = Spieler ist explizit ausgenommen, re15_render_pc_tim_slot_raum).
Der verursachende Commit ist **e11962ec (2026-08-22) "feat: RE2-KI ist jetzt der Standard"**: seit
dem Default RE2 laufen alle Griffe/Bisse ueber die RE2-Gehirne, und deren Port-Fassung ruft den
Wund-Akkumulator nirgends — die Stempel haengen im Port am RE1.5-ZOMBIE-Code (Fehlplatzierung,
s. §2: das Original stempelt auf der SPIELER-Seite, gehirn-unabhaengig). Der Nutzer hat die
blutige Kleidung zuletzt unter der RE1.5-KI gesehen (bis 2026-08-22) und den Verlust jetzt bemerkt.

## 4. Umsetzung

(wird gefuellt)

## 5. Messung nachher

(wird gefuellt)

## 6. Tests

(wird gefuellt)

## 7. OFFEN

(wird gefuellt)

## 8. Fuer den Nutzer

(wird gefuellt)
