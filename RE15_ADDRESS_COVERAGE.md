# RE1.5-Port — Adress-Coverage-Übersicht (zitierte Original-Adressen)

**Stand: 2026-10-30** — generiert von `analysis/coverage/generate_address_coverage.py`
aus `analysis/coverage/cited_addresses.json` (Zensus des Port-Quellcodes via
`extract_cited_addresses.py`), `original_exe_inventory.json` und `original_overlay_inventory.json`.
Rohdaten: `analysis/coverage/coverage_result.json`.

> Arbeitsregel aus `lesson.txt` (wörtlich): *„Verlange für jede Zahl/jedes Verhalten, das ich in den Port schreibe, die @0x…-Adresse.“*
>
> Dieses Dokument ist die quantitative Gegenprobe zu dieser Regel: Es zählt, welche
> Original-Adressen der Port tatsächlich als Beleg zitiert — und welche Original-Funktionen
> noch ohne jedes Adress-Zitat sind.

Verwandte Dokumente: [`RE15_FUN_COVERAGE.md`](RE15_FUN_COVERAGE.md) (qualitative Sicht:
was die Funktionen tun und wie gut sie verstanden sind) und
[`RE15_FUN_CATALOG.md`](RE15_FUN_CATALOG.md) (kuratierter Katalog code-verifizierter Adressen).

## Methodik & Semantik — was die Zahlen bedeuten (und was NICHT)

**ZITIERT** heißt: Der Port-Quellcode (`re15_port/{engine,platform,include,tools,tests}`,
nur `*.c`/`*.h`, ohne `build/`) referenziert mindestens eine Adresse im Funktionsintervall
als `0x80xxxxxx`-, `FUN_`-, `DAT_`- oder `LAB_`-Zitat. Das ist ein **Signal für RE-Abdeckung**
— die Stelle wurde disassembliert/gelesen und als Beleg in den Code geschrieben — aber
**KEIN Beweis vollständiger Byte-Treue** der Funktion: Ein einzelnes Zitat kann auch nur eine
Konstante, einen Teilaspekt oder einen Kommentar-Verweis abdecken.

**NICHT ZITIERT** heißt umgekehrt **nicht zwingend unportiert**: Das Verhalten kann portiert
sein, ohne dass eine Adresse im Code steht (z.B. über RE2-Referenzen, Format-Doku oder alte
Commits) — dann fehlt allerdings der Adress-Beleg im Code, und genau das soll diese Liste
sichtbar machen. Viele nicht-zitierte Funktionen sind außerdem PsyQ-/Lib-Code, toter Code
oder schlicht noch nicht erreichte Subsysteme.

Zuordnungsregeln (implementiert in `generate_address_coverage.py`):

1. **EXE-Funktions-Coverage:** Die 386 `exe_functions`-Starts werden sortiert;
   eine Funktion mit Start `S` gilt als zitiert, wenn mindestens eine zitierte Adresse `A`
   `S <= A < min(nächster_Start, S + 0x4000)` erfüllt. Der `0x4000`-Deckel schützt gegen
   Riesen-Lücken zwischen den Decompile-Starts (ohne ihn würde eine Funktion Zitate aus weit
   entferntem, nie decompiliertem Code „erben“).
2. **Overlay-Coverage je Stage:** analog über die Stage-Funktionslisten, nur für zitierte
   Adressen `>= 0x80100000`. ⚠️ **Mehrdeutigkeit:** Alle Stages teilen sich den Adressraum
   ab `0x80100000` — eine zitierte Overlay-Adresse kann in **mehreren** Stages in einem
   Funktionsintervall liegen und wird dann für **jede** dieser Stages gezählt
   (9992 von 10062 Overlay-Zitaten treffen mehr als eine Stage).
   Aus dem Zitat allein ist NICHT entscheidbar, welche Stage gemeint war; die Stage-Spalten
   sind daher obere Schranken je Stage, keine disjunkte Aufteilung.
3. **DAT_-Zitate (Daten-Globals):** Adressen, die mindestens einmal als `DAT_…` zitiert werden,
   zählen als Daten-Globals: separat gezählt, **nicht** auf Funktionen gemappt — auch wenn
   dieselbe Adresse zusätzlich als rohes `0x…`-Literal zitiert wird (172 solcher Mischfälle).
4. **Subsystem-Sicht:** Katalog-Adressen aus `RE15_FUN_CATALOG.md` (via `original_exe_inventory.json`)
   werden mit derselben Intervallregel auf EXE-Funktionen gemappt (exakter Start gewinnt, sonst
   niedrigste Katalog-Adresse im Intervall); der Kontext-Text wird auf sein Subsystem-Label gekürzt.

Nicht Teil des Nenners: die 872 benannten (PsyQ-/SDK-)Symbole aus
`named_functions` — sie sind Bibliothekscode, keine Spiel-Logik-Decompiles.

## Gesamtzahlen

| Metrik | Wert |
|---|---|
| Zitierte Original-Adressen im Port (unique) | **20364** |
| … davon DAT_-Daten-Globals (separat, nicht gemappt) | 326 (EXE-Raum 317, Overlay-Raum 9) |
| … davon Funktions-Zitate im EXE-Raum (< 0x80100000) | 9976 |
| … davon Funktions-Zitate im Overlay-Raum (>= 0x80100000) | 10062 |
| EXE-Funktionen (Decompile-Inventar) | 386 |
| EXE-Funktionen ZITIERT | **332** (86.0%) |
| EXE-Funktionen NICHT zitiert | 54 |
| EXE-Raum-Zitate auf Funktionen gemappt / ungemappt | 8564 / 1412 (ungemappt = Datenraum oder > Start+0x4000) |
| Overlay-Zitate, die mehrere Stages treffen (Mehrdeutigkeit) | 9992 |
| Overlay-Zitate ohne Stage-Funktions-Treffer | 66 |

### Overlay-Coverage je Stage

⚠️ Spalten sind wegen des geteilten Adressraums **nicht disjunkt** (siehe Methodik Punkt 2).

| Stage | Funktionen | zitiert | nicht zitiert | zitiert % | gemappte Zitate |
|---|---:|---:|---:|---:|---:|
| STAGE1 | 650 | 579 | 71 | 89.1% | 9784 |
| STAGE2 | 456 | 435 | 21 | 95.4% | 9326 |
| STAGE3 | 526 | 485 | 41 | 92.2% | 9932 |
| STAGE4 | 459 | 436 | 23 | 95.0% | 9611 |
| STAGE5 | 535 | 483 | 52 | 90.3% | 9992 |
| STAGE6 | 73 | 72 | 1 | 98.6% | 4133 |

## Subsystem-Sicht (EXE-Funktionen × Katalog-Kontext)

Kontext aus `RE15_FUN_CATALOG.md`; Funktionen ohne Katalog-Treffer stehen unter
„(ohne Katalog-Kontext)“ — das ist der größte Block und zugleich die ehrliche Aussage,
dass der Katalog nur einen Teil des EXE-Inventars abdeckt.

| Subsystem (Katalog-Kontext) | Funktionen | zitiert | nicht zitiert |
|---|---:|---:|---:|
| (ohne Katalog-Kontext) | 280 | 226 | 54 |
| Player / animation | 16 | 16 | 0 |
| Combat / player damage | 11 | 11 | 0 |
| Prop / fixture rendering + perf | 11 | 11 | 0 |
| Sprite / BG / MDEC / TIM | 11 | 11 | 0 |
| Enemy AI | 8 | 8 | 0 |
| RVD / AOT | 8 | 8 | 0 |
| Render / fade / letterbox | 6 | 6 | 0 |
| Stairs / floor-band traversal | 6 | 6 | 0 |
| Camera | 5 | 5 | 0 |
| Lighting / NCCT | 5 | 5 | 0 |
| RE2 RENDERING PIPELINE | 5 | 5 | 0 |
| SCD VM | 5 | 5 | 0 |
| ⚠️ HIGH-VALUE CORRECTIONS | 4 | 4 | 0 |
| Save / item-box | 3 | 3 | 0 |
| Audio | 2 | 2 | 0 |

## Anhang A — nicht-zitierte EXE-FUN-Adressen

54 von 386 EXE-Funktionen ohne Adress-Zitat im Port-Code
(Semantik siehe oben: nicht zwingend unportiert, aber ohne Beleg im Code):

```
80010140  8001306c  80013114  80013404  800134f0  80013f80  800160c4  800170e0
8001a504  800209b8  800209ec  8002288c  80026c34  80026f90  80027108  800276b0
80027724  80027780  80027828  8002939c  80029560  800297c8  8002a16c  8002a1b8
8002abec  8002ac38  8002ad34  8002ad64  8002adb0  8002adf4  8002ae38  8002bc04
8002f3f0  8002f600  80039c2c  8003e9d4  8003ea3c  8003edbc  80043b80  80044fec
8004694c  80046f68  8004d4c4  8004d96c  8004df90  8004edfc  8004ee60  8005ad40
8005f87c  8005f89c  80060d8c  800614d0  80064768  80066d60
```

## Anhang B — nicht-zitierte STAGE1-Overlay-Funktionen

71 von 650 STAGE1-Overlay-Funktionen ohne Adress-Zitat:

```
80106a24  801092dc  80109a44  80109d74  80109d78  80109ef8  8010a8b8  8010b26c
8010be38  8010bf14  8010bf68  8010c088  8010c334  8010cb14  8010cfc8  8010d5c0
8010e2f0  8010e690  8010e6c4  8010fc48  801108d0  80110db0  80112684  80115efc
80116750  8011723c  8011970c  80119d0c  8011a818  8011b988  8011bb8c  8011c818
8011c8a8  8011c940  8011c950  8011c9cc  8011ca48  8011ca90  8011cab8  8011cdf4
8011ce3c  8011d018  8011d04c  8011d060  8011d1f4  8011d460  8011d4b4  8011d530
8011d598  8011d5ac  8011d5f4  8011d63c  8011d994  8011db70  8011dbd0  8011df3c
8011df40  8011df84  8011dfcc  8011e00c  8011e088  8011e104  8011e14c  8011e194
8011e518  8011e634  8011e644  8011e6c0  8011e73c  8011e784  8011e7cc
```

## Anhang C — STAGE2–6 (nur Zählwerte)

Die vollständigen Lückenlisten für STAGE2–6 stehen in
`analysis/coverage/coverage_result.json` (`overlays.stageN.uncited_starts`);
die Funktionsinventare in `analysis/coverage/original_overlay_inventory.json`.

| Stage | nicht zitiert / gesamt |
|---|---:|
| STAGE2 | 21 / 456 |
| STAGE3 | 41 / 526 |
| STAGE4 | 23 / 459 |
| STAGE5 | 52 / 535 |
| STAGE6 | 1 / 73 |

