# RE1.5-Port — Adress-Coverage-Übersicht (zitierte Original-Adressen)

**Stand: 2026-08-08** — generiert von `analysis/coverage/generate_address_coverage.py`
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
   (2697 von 2720 Overlay-Zitaten treffen mehr als eine Stage).
   Aus dem Zitat allein ist NICHT entscheidbar, welche Stage gemeint war; die Stage-Spalten
   sind daher obere Schranken je Stage, keine disjunkte Aufteilung.
3. **DAT_-Zitate (Daten-Globals):** Adressen, die mindestens einmal als `DAT_…` zitiert werden,
   zählen als Daten-Globals: separat gezählt, **nicht** auf Funktionen gemappt — auch wenn
   dieselbe Adresse zusätzlich als rohes `0x…`-Literal zitiert wird (69 solcher Mischfälle).
4. **Subsystem-Sicht:** Katalog-Adressen aus `RE15_FUN_CATALOG.md` (via `original_exe_inventory.json`)
   werden mit derselben Intervallregel auf EXE-Funktionen gemappt (exakter Start gewinnt, sonst
   niedrigste Katalog-Adresse im Intervall); der Kontext-Text wird auf sein Subsystem-Label gekürzt.

Nicht Teil des Nenners: die 872 benannten (PsyQ-/SDK-)Symbole aus
`named_functions` — sie sind Bibliothekscode, keine Spiel-Logik-Decompiles.

## Gesamtzahlen

| Metrik | Wert |
|---|---|
| Zitierte Original-Adressen im Port (unique) | **5651** |
| … davon DAT_-Daten-Globals (separat, nicht gemappt) | 200 (EXE-Raum 192, Overlay-Raum 8) |
| … davon Funktions-Zitate im EXE-Raum (< 0x80100000) | 2731 |
| … davon Funktions-Zitate im Overlay-Raum (>= 0x80100000) | 2720 |
| EXE-Funktionen (Decompile-Inventar) | 386 |
| EXE-Funktionen ZITIERT | **260** (67.4%) |
| EXE-Funktionen NICHT zitiert | 126 |
| EXE-Raum-Zitate auf Funktionen gemappt / ungemappt | 2243 / 488 (ungemappt = Datenraum oder > Start+0x4000) |
| Overlay-Zitate, die mehrere Stages treffen (Mehrdeutigkeit) | 2697 |
| Overlay-Zitate ohne Stage-Funktions-Treffer | 23 |

### Overlay-Coverage je Stage

⚠️ Spalten sind wegen des geteilten Adressraums **nicht disjunkt** (siehe Methodik Punkt 2).

| Stage | Funktionen | zitiert | nicht zitiert | zitiert % | gemappte Zitate |
|---|---:|---:|---:|---:|---:|
| STAGE1 | 650 | 433 | 217 | 66.6% | 2677 |
| STAGE2 | 456 | 331 | 125 | 72.6% | 2477 |
| STAGE3 | 526 | 364 | 162 | 69.2% | 2660 |
| STAGE4 | 459 | 343 | 116 | 74.7% | 2564 |
| STAGE5 | 535 | 359 | 176 | 67.1% | 2697 |
| STAGE6 | 73 | 50 | 23 | 68.5% | 416 |

## Subsystem-Sicht (EXE-Funktionen × Katalog-Kontext)

Kontext aus `RE15_FUN_CATALOG.md`; Funktionen ohne Katalog-Treffer stehen unter
„(ohne Katalog-Kontext)“ — das ist der größte Block und zugleich die ehrliche Aussage,
dass der Katalog nur einen Teil des EXE-Inventars abdeckt.

| Subsystem (Katalog-Kontext) | Funktionen | zitiert | nicht zitiert |
|---|---:|---:|---:|
| (ohne Katalog-Kontext) | 280 | 156 | 124 |
| Player / animation | 16 | 16 | 0 |
| Combat / player damage | 11 | 11 | 0 |
| Prop / fixture rendering + perf | 11 | 10 | 1 |
| Sprite / BG / MDEC / TIM | 11 | 11 | 0 |
| Enemy AI | 8 | 8 | 0 |
| RVD / AOT | 8 | 8 | 0 |
| Render / fade / letterbox | 6 | 5 | 1 |
| Stairs / floor-band traversal | 6 | 6 | 0 |
| Camera | 5 | 5 | 0 |
| Lighting / NCCT | 5 | 5 | 0 |
| RE2 RENDERING PIPELINE | 5 | 5 | 0 |
| SCD VM | 5 | 5 | 0 |
| ⚠️ HIGH-VALUE CORRECTIONS | 4 | 4 | 0 |
| Save / item-box | 3 | 3 | 0 |
| Audio | 2 | 2 | 0 |

## Anhang A — nicht-zitierte EXE-FUN-Adressen

126 von 386 EXE-Funktionen ohne Adress-Zitat im Port-Code
(Semantik siehe oben: nicht zwingend unportiert, aber ohne Beleg im Code):

```
80010140  800116f4  8001306c  800130c4  80013114  8001311c  80013404  800134f0
80013a88  80013df0  80013f80  800160c4  8001613c  800161e0  800164c8  80016518
800166c4  800170e0  800171b4  80019ca8  8001a504  8001e5b0  8001f1e0  8001f220
80020610  80020674  80020794  80020894  800209b8  800209ec  8002134c  80021bbc
80021eb4  80022150  8002288c  8002441c  80025360  800253fc  80025940  80025a98
80026c34  80026ca8  80026dd4  80026f90  80027108  80027160  800271a8  80027368
80027488  800275f0  800276b0  80027724  80027780  80027828  800278bc  8002877c
8002939c  80029560  80029690  800297c8  800298b0  800299a4  80029a28  80029b48
80029e84  80029f40  8002a050  8002a16c  8002a1b8  8002a8d0  8002abec  8002ac38
8002ad34  8002ad64  8002adb0  8002adf4  8002ae38  8002b7e8  8002bc04  8002d100
8002d2c0  8002d3d8  8002dacc  8002f3f0  8002f600  8002f884  80037250  800396a8
80039a30  80039c2c  8003b558  8003bc2c  8003da78  8003e9d4  8003ea3c  8003edbc
80043850  80043a34  80043b80  80044fec  80045a18  8004694c  80046f68  800487b0
8004c830  8004d4c4  8004d96c  8004df90  8004edfc  8004ee60  8004f090  80052fd0
800537e4  80053a8c  8005a99c  8005abf8  8005ad40  8005f87c  8005f89c  8005fb00
80060d8c  800614d0  80064768  80066d60  8006e170  8006e468
```

## Anhang B — nicht-zitierte STAGE1-Overlay-Funktionen

217 von 650 STAGE1-Overlay-Funktionen ohne Adress-Zitat:

```
8010093c  801010f8  801012e8  801016f0  80101d48  801021e4  80102bbc  801031fc
801035e8  80103978  80103b94  80103d48  80103e6c  80103f60  80104008  801040e8
80104250  80104340  80104548  8010466c  80104760  80104780  80104b40  80105848
801060a0  80106238  8010626c  80106290  801063a4  80106418  80106624  801066dc
80106a24  80106ea0  80107200  80107244  801074b0  80107634  801076e0  80107c9c
801082bc  80108504  80108574  8010899c  801089e4  80108abc  80108af4  80108b9c
80108c70  80108d04  80108d54  80108d74  80109100  80109164  801091e4  801092dc
801092f4  80109470  801099cc  80109a08  80109a44  80109d74  80109d78  80109ef8
8010a0c8  8010a8b8  8010aed4  8010af84  8010b26c  8010b2e8  8010b364  8010b780
8010b814  8010b87c  8010b888  8010b8f8  8010b974  8010b9ec  8010b9f0  8010ba6c
8010bae8  8010bb3c  8010be38  8010bf14  8010bf68  8010c088  8010c138  8010c334
8010c938  8010cb14  8010cb34  8010cfc8  8010cfe0  8010d5c0  8010d8c4  8010e2f0
8010e690  8010e6c4  8010ea24  8010efa4  8010f7f0  8010fc48  8010fdbc  801101e0
801108d0  8011099c  80110b98  80110db0  80111648  80112230  80112684  80112938
80113384  8011340c  8011347c  801137fc  80113900  801152cc  8011539c  8011552c
80115efc  80116750  8011696c  8011723c  8011926c  80119284  80119634  8011970c
8011971c  80119770  80119998  801199f8  80119d0c  8011a160  8011a818  8011afb8
8011b750  8011b988  8011bb8c  8011c084  8011c150  8011c1cc  8011c248  8011c290
8011c2d8  8011c370  8011c414  8011c818  8011c8a8  8011c940  8011c950  8011c9cc
8011ca48  8011ca90  8011cab8  8011cad8  8011cb70  8011cc60  8011ccac  8011ccb4
8011cd30  8011cdac  8011cdf4  8011ce3c  8011cf9c  8011d018  8011d04c  8011d060
8011d0a8  8011d1f4  8011d248  8011d2c4  8011d340  8011d388  8011d3d0  8011d460
8011d468  8011d4b4  8011d530  8011d598  8011d5ac  8011d5f4  8011d63c  8011d740
8011d80c  8011d888  8011d904  8011d94c  8011d994  8011da2c  8011dac4  8011db70
8011dbd0  8011dc68  8011dd18  8011dd98  8011de44  8011dec0  8011df3c  8011df40
8011df84  8011dfcc  8011e00c  8011e064  8011e088  8011e104  8011e14c  8011e194
8011e22c  8011e370  8011e518  8011e634  8011e644  8011e6c0  8011e73c  8011e784
8011e7cc
```

## Anhang C — STAGE2–6 (nur Zählwerte)

Die vollständigen Lückenlisten für STAGE2–6 stehen in
`analysis/coverage/coverage_result.json` (`overlays.stageN.uncited_starts`);
die Funktionsinventare in `analysis/coverage/original_overlay_inventory.json`.

| Stage | nicht zitiert / gesamt |
|---|---:|
| STAGE2 | 125 / 456 |
| STAGE3 | 162 / 526 |
| STAGE4 | 116 / 459 |
| STAGE5 | 176 / 535 |
| STAGE6 | 23 / 73 |

