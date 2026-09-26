# Pruefung: fahrstuhl-faehrt-nicht.md — NICHT HALTBAR

Pruefer: Skeptiker-Agent, 2026-09-26. Keine Engine-Aenderung, kein Build.
Selbst nachgelesen: 10 Adressen/Datei-Offsets (Liste in §6).

Kurzurteil: Die Adressen, die ich nachgelesen habe, stimmen — bis auf EINE: der Beifund
(§6 des Dossiers, Letterbox) ist an der FALSCHEN Funktion gemessen und faellt. Die
AUSSCHLUESSE tragen nicht: die Sonde ist nachweislich alt UND sie endet vor der Haelfte
der Kette, um die es geht.

---

## 1. Das Sondenbinaer ist alt — und die Sonde ist ausserdem die falsche Sonde

`.build_diag1d3/tests/unit/probe_elevator_1080.exe` — **2026-09-14 13:37:56**.
Seither **59 Commits** unter `re15_port/engine/src/`, darunter genau auf den Gliedern,
die das Dossier fuer geprueft erklaert:

| Datei | mtime | Commits danach |
|---|---|---|
| `scd_vm.c` | 2026-09-22 09:51 | u.a. `88a988d5`, `228bf344`, `630e87d2`, `e0914942` |
| `aot_common.c` | 2026-09-20 16:34 | `228bf344` (Klick am Cursor-Raetsel = `re15_object_notch_update`) |
| `game_step_common.c` | 2026-09-22 09:51 | die vier `discard`-Commits (neue Frueh-Rueckkehr) |
| `game_state.c` | 2026-09-22 09:51 | — |
| `scd_room_setup.c` | 2026-09-22 09:51 | — |

Die Quelle `re15_port/tests/unit/probe_elevator_1080.c` ist unveraendert (2026-08-08); das
Binaer ist also nicht wegen der Sonde veraltet, sondern wegen der gelinkten Engine.
=> Satz 5 des Dossiers, „**das ist heute gemessen, nicht argumentiert**", ist unzutreffend:
gemessen wurde heute nur der LAUF eines zwoelf Tage alten Standes. Das Dossier weiss das
selbst und widerspricht sich (§6: „Ob die heutige Engine die Sonde noch besteht, ist NICHT
gemessen"). Der Titel „**Port-Kette GEMESSEN korrekt**" traegt das nicht.

**Schwerer als das Alter: die Sonde endet, wo der Verdacht anfaengt.**
`probe_elevator_1080.c:174` liest `g_room_change.pending` / `.room_id` und bricht ab.
`re15_room_apply_pending` (`room_common.c:196`) wird **nie** aufgerufen — kein `load_rdt`,
kein Teardown, kein Re-Alias, kein Spawn, kein Cut. Die Sonde beweist:
*die Etagenwahl wird richtig in die Warteschlange gelegt.* Sie beweist NICHT, dass der
richtige Raum geladen wird. Genau das ist der Nutzer-Befund.

**Drittens: der Frame der Sonde ist nicht der Frame des Spiels.**
`probe_elevator_1080.c:73-85` ist ein handgebauter Tick aus acht Aufrufen. Das Spiel faehrt
`scd_vm_tick()` → `re15_game_step()` → `re15_room_apply_pending()`. Strukturell unsichtbar
fuer die Sonde:
* der Zweig `game_step_common.c:1251` — `if (rdt_ok && (g_re15_pauseflags & RE15_PAUSE_PLAYER))`.
  Den erzeugt das Panel SELBST (`ROOM1080` sub01 @0x0682 `Set(2,0,1)`+`Set(2,2,1)`). Dort
  laeuft **kein** `re15_player_tick`, **keine** Kollision, und `g_aot_action_pressed = 0`
  (`game_step_common.c:1273`). Die Sonde ruft stattdessen `re15_actor_step_all_walkers()`
  ungegatet und laesst das Pause-Wort (Zone 2 = 0x800aca40, `game_state.c:124-160`) ohne
  Wirkung — ein Defekt dort ist fuer sie unsichtbar.
* die Pad-Maske `g_scd_pad_held &= 0xf000` bei `RE15_PAUSE_PAD` (`game_step_common.c:1063`).
  Virtuell 0x0040 (SQUARE) und 0x0001/0x0004 (UP/DOWN) liegen NICHT in 0xf000. Die Sonde
  schreibt `g_scd_pad_held` direkt und umgeht die Maske.
* der Confirm ueber den ECHTEN Pfad: `g_aot_action_pressed` setzt die Sonde nur einmal
  (Zeile 126, Panel-Oeffnen); `hold(RE15_PAD_BIT_SQUARE,1)` setzt es NICHT. Im Spiel kommt
  es aus `game_step_common.c:1012`. Jede AOT-ACTION-Wechselwirkung im Bestaetigungs-Bild
  ist damit ungetestet.

=> „Das ist der ECHTE Engine-Pfad … nur ohne `re15_game_step`" (Zeile 43-45) ist ein
Widerspruch in sich: `re15_game_step` IST der Pfad, und der weggelassene Teil enthaelt
genau den Freeze, den das Raetsel selbst einschaltet.

## 2. Die Ausschlussliste — wo jeweils die Luecke sitzt

| Ausschluss | Luecke |
|---|---|
| Tuer-Payload (§2.1/§3) | Belegt und korrekt (`aot_common.c:598`, `((stage+1)<<12)\|(room<<4)\|(cur&0xF)`). Entscheidet aber nur, WAS in `g_room_change` steht — nicht, was `room_common.c:196` daraus macht. |
| `Aot_on` (§2.5) | `scd_vm.c:4144-4153` feuert sofort. Ungeprueft: ob die Warteschlange im SELBEN Frame noch ueberschrieben wird. Reihenfolge ist SCD-Tick → `re15_aot_scan` (in `game_step`) → `apply_pending`; der Scan laeuft NACH dem `Aot_on`. |
| SCE-3-Panel-Ausloeser (§2.2) | Tabelle `@0x8007469c[3] → 0x800430f0` sauber. Aber er kann im Spiel mehrfach feuern; die Sonde druckt ihn genau einmal. |
| Objekt-Stempel +0x0B | `re15_object_notch_update` wurde am 2026-09-20 (`228bf344`) geaendert — NACH dem Sondenbau. Der Ausschluss stammt aus einem Binaer ohne diese Fassung. |
| `Work_set`/`Member_cmp` | Nur Code gelesen, im heutigen Stand nicht gemessen. |
| Pad-Tabelle @0x80073dbc | Tabelle stimmt. Ungeprueft, ob das Wort zur Panel-Zeit ungemaskt ankommt (s.o. 0xf000). |
| Bank-5-Raumclear | `@0x8003ed74 sw zero,4136(at)` → 0x800b1028. Selbst nachgelesen, stimmt. |
| Bank-3-Etagenflags | `@0x80074664[3] = 0x800b0ff8`, selbst nachgelesen; auch `FUN_800396fc` fasst 0x800b0ff8 nicht an. Ausschluss haelt. ABER: ein Debug-JUMP nach ROOM1080 laesst alle drei Bits 0 — das ist NICHT die Lage des Nutzers (er kam von 1F, Bit 54 = 1). Messplan §5.1 misst damit einen anderen Zustand als den gemeldeten. |
| Panel-Kamera / Region-Cull (§2.6/2.6b) | Gerechnet, nicht gesehen. „Die These … ist damit **widerlegt**" ist zu stark fuer eine Rechnung ohne Bild: welche Zelle der Nutzer als „2F" liest, haengt am gerenderten Bild (Hintergrund-Beschriftung, Cursor-Projektion, `sprite.pri`), nicht am RVD-Quad. Das Dossier raeumt das drei Absaetze spaeter selbst ein; der Satz „widerlegt" bleibt trotzdem stehen. |

## 3. Der Weg zwischen „Knopf gedrueckt" und „Raum geladen"

Gelesen: `scd_vm.c` (`op_aot_on` 4144-4153, `op_sce_key_ck` 4715-4776), `aot_common.c:540-600`,
`room_common.c:186-360`, `platform/pc/main.c:6788-6845`.

1. `scd_vm.c:4151` `re15_aot_fire_slot(slot)` → `aot_common.c:598`
   `re15_room_request_change(dest_id, …)` — einzige Schreibstelle je Fahrt.
2. `room_common.c:186-194` — `g_room_change` ist EIN globaler Datensatz ohne Warteschlange.
   **Letzter Schreiber gewinnt.** Weitere Schreiber im selben Frame moeglich:
   `aot_common.c:598` (jede weitere Tuer), `re15_damage.c:340` (Tod/Continue, **ohne**
   `pending`-Gate) und `platform/pc/main.c:6800-6806` (F1/F2-Browser, mit Gate).
3. `room_common.c:198` `g_room_change.pending = 0;` steht **vor** dem Laden;
   `room_common.c:212` `if (c->load_rdt(...) != 0) return 0;` — schlaegt das Laden fehl,
   ist die Anforderung weg, ohne Meldung.
4. `platform/pc/main.c:6841` `re15_room_apply_pending(&rc)`.

=> Die Stelle, an der die Etagenwahl verloren gehen KANN und die die Sonde nicht erreicht:
**`re15_port/engine/src/room_common.c:196-212`**, gespeist aus
**`re15_port/engine/src/aot_common.c:598`**.

## 4. Der Beifund des Dossiers ist FALSCH — falsche Funktion gemessen

Dossier §6: „sub07/08/09 setzen `Set(1,27,1)` (Letterbox) @0x0730 und **loeschen es nie**.
Im Original wird Bank 1 beim Raumaufbau nur um die Bits 0x800 und 0x400 beschnitten
(@0x8003ed98/eda0) — Bit 27 (Maske 0x10) ist NICHT darunter."

Erste Haelfte stimmt (selbst nachgelesen): `ROOM1080.RDT @0x0730 = 22 01 1b 01` =
`Set(1,27,1)`, dreimal (0x0730/0x07C2/0x0854), kein `22 01 1b 00` in ROOM1080.
Bit 27 = Maske 0x10 stimmt ebenfalls (MSB-first, `@0x8003fdd4 lui t0,0x8000` + `srlv`).

Zweite Haelfte ist an der **falschen Funktion** gemessen. `FUN_8003ecec` ist nicht die
Raum-Ladekette. Die ist `FUN_800396fc` — selbst disassembliert:

```
8003970c: lw   v0,-13764(v0)     ; v0 = [0x800aca3c]  = Bank 1
80039710: lui  v1,0xffff         ; v1 = 0xFFFF0000
80039728: and  v0,v0,v1
80039730: sw   v0,-13764(at)     ; [0x800aca3c] = v0
```

0xFFFF0000 loescht alle Bits mit Wert < 0x10000 — in MSB-first-Zaehlung idx 16..31,
**einschliesslich idx 27 (Wert 0x10)**. Das Letterbox-Bit faellt im Original bei JEDEM
Raumladen. Der Port tut dasselbe: `scd_room_setup.c:241`
`for (int fi = 16; fi < 32; fi++) re15_game_flag_set(1, (uint8_t)fi, 0);` — mit demselben
Zitat @0x80039710-30. Gegenprobe im Skript: `ROOM1040.RDT` hat sogar ein eigenes
`22 01 1b 00` @0x1A2C; `ROOM10C0`/`ROOM1120` haben weder Set noch Clear — konsistent damit,
dass die Engine raeumt. **Es gibt hier keinen zweiten Fehler.**
Auch der Verweis „Wer das Letterbox-Latch im Port loescht, steht in `room_common.c:300-302`"
geht fehl: dort steht ein Kommentar ueber `flag(2,7)`/`player_mode`; geloescht wird in
`scd_room_setup.c:241`.

## 5. Messplan, der die Ursache in EINEM Lauf entscheidet

Vorgeschaltet (billigster Schritt): **`probe_elevator_1080` gegen HEAD neu bauen** — das
allein entscheidet die Staleness-Frage.

Dann EIN echter gdigrab-Lauf (Skill `re15-port-visual-verify`, nicht AUTOSHOT/SOFTWARE_RENDER),
**als Durchlauf ab ROOM1040**, nicht als Debug-JUMP (sonst ist `flag(3,54)` = 0 und der
gemessene Zustand ist nicht der gemeldete). Gleichzeitig an: `RE15_EVT_TRACE=1`
(`scd_vm.c:1110`), `RE15_FLAG_TRACE=1`, plus die eine fehlende Zeile (siehe unten).

Aus dem EINEN Log ablesbar:
1. `Evt_exec sub=` 7/8/9/10 → falsche Zelle gewaehlt? (dann Bild/Projektion, nicht die Kette)
2. Cursor-`member_0b` je Bild + `flag(3,54/55/56)` beim Eintritt → Stempel oder Vorzustand?
3. `g_room_change.room_id` im Moment des `Aot_on` → Warteschlange richtig?
4. `g_current_room_id` nach `apply_pending` → **Laden** richtig? (Der Punkt, den die Sonde
   prinzipiell nicht erreicht.)
Die Videobilder beantworten parallel, welche Zelle der Nutzer als „2F" liest.

## 6. Adressen/Offsets, die ich selbst gelesen habe

1. `0x80074664` (Zonen-Tabelle, 32 B) — [1]=0x800aca3c, [2]=0x800aca40, [3]=0x800b0ff8, [5]=0x800b1028
2. `0x8003ed50`–`0x8003edb8` (`FUN_8003ecec`-Ende, 32 Instr.)
3. `0x8003ed74` `sw zero,4136(at)` → 0x800b1028 (Bank 5 Wort 0)
4. `0x8003ed98`/`eda0`/`eda8` (`and` ~0x800, ~0x400 auf 0x800aca3c)
5. `0x80039700`–`0x8003973c` (`FUN_800396fc`: `lui v1,0xffff` / `and` / `sw` 0x800aca3c)
6. `FUN_800396fc` + 60 Instr. — kein Store auf 0x800b0ff8 (Bank 3 ueberlebt)
7. `ROOM1080.RDT` @0x06EE–0x0780 (sub07 vollstaendig, roh)
8. `ROOM1080.RDT` @0x0730 `22 01 1b 01`; @0x077A `08 00`, @0x077C `47 00`, @0x077E `01 00`
9. `ROOM1080.RDT` @0x06F6 `21 03 36 00`, @0x081A `21 03 38 00`
10. Zensus `22 01 1b 01` / `22 01 1b 00` in ROOM1040/1080/10C0/1120
