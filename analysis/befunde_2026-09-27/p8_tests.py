import io

p = "analysis/befunde_2026-09-27/elza-zweig.md"
s = io.open(p, encoding="utf-8", newline=None).read()
alt = """### 3.5 Testsuite

(folgt)
"""
neu = """### 3.5 Testsuite

`bash re15_port/tools/local_build.sh all`:

```
100% tests passed, 0 tests failed out of 360
Total Test time (real) = 161.29 sec
=== LOCAL-BUILD-OK (all) — Tests 360/360
```

Die vier GUI-Haken liefen im selben Durchlauf durch: `integration_boot_bg_pin`
3.60 s, `integration_dark_start_pin` 2.71 s, `integration_relatch_pin` 24.65 s,
`integration_save_counter_pin` 27.78 s — alle *Passed*.

**359 -> 360 ist ein NEUER Haken, keine gesenkte Schranke.** `unit_elza_zweig`
(Test #340, `tests/unit/test_elza_zweig.c`, registriert in
`tests/unit/probes/r35_elza-zweig.cmake`) haelt die fuenf Stellen dieser Runde
fest; `RE15_MIN_TESTS` in `local_build.sh` ist mit angehoben, wie es der
Kommentar dort verlangt. Seine Ausgabe:

```
A  Leon: character=0 variante=0 start=1240
A  Elza: character=4 variante=1 start=1241
B  &4-Tor: Leon nimmt Zweig-A=1, Elza nimmt Zweig-A=0
C  0x1170 -> Leon 1170 / Elza 1171 ; 0x1030 -> Leon 1030 / Elza 1031
D  ROOM1240@0x0531=0x17 (Raum 0x17)  ROOM1241@0x0531=0x03 (Raum 0x03)
E1 Tuer aus 1031 nach Stage 0 Raum 17 -> 1171
E2 geladen: character=4 room=1031 work_vars[0x10]=4
```

Teil **B** traegt seine eigene Gegenprobe: der ALTE Port-Wert 1 muss auf Leons
Seite fallen (`(1 & 4) == 0`). Ohne sie wuerde der Haken nur bestaetigen, dass 4
das Bit hat. Teil **D** liest die beiden Bytes aus der echten ROOM124x.RDT —
faellt der Beleg weg, faellt der Haken. Teil **E2** prueft nicht nur, dass der
Charakter zurueckkommt, sondern auch `work_vars[0x10]`; ohne den waere der Load
eine Taeuschung, weil der naechste Raumwechsel Elza gegen PL00 getauscht haette.

Ein bestehender Haken wurde angepasst: `test_savedata.c` speicherte bisher
`character = 1`. Es bleibt derselbe Rundlauf-Pin, prueft ihn jetzt aber mit 4 —
einem Wert, den das Spiel wirklich annimmt.

---

## 4. WAS HINTER DEM ZWEIG NOCH FEHLT (gezaehlt, nicht geschaetzt)

Der Auftrag war DER ZWEIG, nicht Elzas ganzes Szenario. Ehrliche Restliste:

| # | offen | Zahl / Stelle |
|---|---|---|
| 1 | **Elzas Raum-Inhalte sind ungeprueft.** Die 120 ungeraden RDT existieren lueckenlos (120/120, elza-portzustand.md §5) und werden jetzt geladen — ob ihre Skripte, Gegner und Gegenstaende im Port durchlaufen, ist NICHT gemessen. Fuer Elzas Kette ist bisher genau ein Uebergang gefahren: ROOM1241 -> ROOM1031. | 119 von 120 Raeumen ungefahren |
| 2 | **Vier ROOM1170-Sonderfaelle bleiben auf der geraden Id.** `main.c:3878` (Spawn-Ueberschreibung), `main.c:5886` (Cut 7 schwarz), `main.c:7137`, und der Flag-Vorlauf `main.c:3839`. Alle vier gehoeren zu Leons Helipad-Vorspann; Elzas Kette laeuft nicht darueber, deshalb sind sie hier KEIN Defekt — aber ein Debug-Sprung nach ROOM1171 wuerde sie vermissen. | 4 Stellen |
| 3 | **Elzas Westen-Variante ist nicht ermittelt.** Leons R.P.D.-Weste ist PLD-Index 1 (Flag(3,0x75), ROOM1190/1191). Welchen Index Elza dort bekaeme, steht nirgends belegt; der Ladeweg laesst ihren Index deshalb unveraendert, statt zu raten. | 1 offener Index |
| 4 | **Die Startposition aus DEBUG.BIN.** FUN_8001d22c ueberschreibt die fest verdrahtete Startpose unbedingt aus der Tabelle @0x800c263c (26 Byte je Raum). Ob DEBUG.BIN im Auslieferungsstand zu diesem Zeitpunkt resident ist, ist nicht gemessen (elza-original.md §3). Der RAUM ist davon unberuehrt, die POSE moeglicherweise nicht. | 1 offene Messung |
| 5 | **0x800b0fbe Bit 0 (Elza-Kartentitel).** Im ORIGINAL setzt es keiner der fuenf Schreiber von 0x800aca5c — mit Werten 0/4 ist Bit 0 immer 0 und das Elza-Template @0x800107cc nie erreichbar. Der Port trifft mit `re15_mc_title.c:38` das offenbar Gemeinte; die Original-Luecke bleibt offen. | 1 Original-Luecke |
| 6 | **Das PSX-Target hat keine Charakterwahl.** `RE15_BOOT_ROOM 0x1170` wird dort noch fest benutzt (`platform/psx/main.c:375`, `asset_psx.c:677/751`). Nicht angefasst — dieser Auftrag ist der PC-Zweig. | 3 Stellen |
"""
assert alt in s
s = s.replace(alt, neu)
io.open(p, "w", encoding="utf-8", newline="\n").write(s)
print("ok")
