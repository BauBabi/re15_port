# ⛔ KORREKTUR: "Der Oberkoerper faellt in RE2 nie ab" war FALSCH

Der Nutzer hat RE2 gespielt und widersprochen (2026-09-13):
> "man kann sehr wohl mit dieser [Standard-Schrotflinte] schon den Oberkoerper der
>  Zombies abschiessen. Schiesst man nach unten, kann man die Beine abschiessen."

Er hat recht. Der Fehler im Vorbefund (`oberkoerper-abwurf-KURZ.md`) war NICHT die
Mechanik, sondern die Benennung: **Part 0 IST der Oberkoerper.**

## Was wirklich passiert

`FUN_801066FC` haengt bei Anim-Frame 20 die Parts 1..7 aus der Skelettkette aus und auf
die eingefrorene Matrix `entity+0x11C` um (`sw v0,288(v1)` @0x80106D00). Laut
Knochen-Hierarchie sind das **Becken und beide BEINE**. Rumpf (0), Kopf (8) und beide
Arme (9..14) bleiben an der lebenden Entity-Matrix und laufen weiter - in Phase 2 wird
daraus mit `+0x10E = 0x2001` (@0x80106B0C) ein KRIECHER, der wegkriecht.

Am Bildschirm heisst das: **die Beine bleiben stehen, der Oberkoerper geht weg.** Genau
das, was der Nutzer als "Oberkoerper abschiessen" kennt. Die Aussage "der Oberkoerper
faellt nie ab" bezog sich auf das Abwurf-Bit 0x40 an Part 0 - das gibt es tatsaechlich
nicht, aber es waere auch die falsche Stelle: Part 0 ist die WURZEL, ein 0x40 dort wuerde
den Rumpf vom Weltursprung loesen, nicht vom Koerper. Die einzige Trennstelle zwischen
Ober- und Unterkoerper ist Part 1 - und genau dort schneidet RE2.

## Die Chance haengt an entity+0x4

Der Applier schreibt das Zustandswort VOR dem Dispatch (`addiu v0,zero,2 / sw v0,4(t0)`
@0x800418E8-EC bzw. `addiu v0,zero,3` @0x800418F8-FC):

* **Opfer ueberlebt** (+0x4 == 2): Trennung zu **100 %**.
* **Opfer stirbt** (+0x4 == 3): zu 1/4 Trennung (mit HP=10-Wiederbelebung als Kriecher
  @0x80106ACC-E8), zu 3/4 Wegschleudern ueber FUN_80109610.

Die Formulierung "3/4 Wegschleudern" im Vorbefund gilt also NUR fuer den Todesfall. Der
Port bildet das bereits richtig ab (`(re2z_rand() & 3u) != 0u && death`).

## Beine abschiessen

Zielen nach unten stempelt Zone 0 (Spalten 0/3/6) und fuehrt auf `FUN_80107438`. Der wirft
die Beinteile 3/5/6 ab, jeweils an einem eigenen 50-%-Wurf, und blendet die Fuesse 4/7 aus.
Deckt sich mit der Nutzer-Beobachtung.

## ⛔ Die Part-Benennung im Port IST vertauscht

Zwei Recherchen hatten das behauptet und dafuer rohe Byte-Griffe auf CDEMD0.EMS angegeben
(0x2A800 bzw. 0x2AE9C). **Beide Adressen liefern bei einem rohen Lesen Muell** - das EMR
traegt die Hierarchie nicht als flaches `parent[]`. Deshalb wurde die Korrektur zweimal
zurueckgestellt.

Jetzt belegt aus der GELADENEN Bank (`probe_re2z_hierarchie`, neue Diagnose):

    parent[] = [-1, 0, 1, 2, 3, 1, 5, 6, 0, 0, 9, 10, 0, 12, 13]
    Bone 0   Rumpf-Wurzel (relpos y -2067)
    Bone 1   Becken (relpos 0,0,0 - koinzident mit der Wurzel)
    2-3-4    BEIN (252 / 730 / 861)        5-6-7    BEIN
    Bone 8   KOPF (relpos -40,-783,0)
    9-10-11  ARM  (-605 / 569 / 459)      12-13-14  ARM

`re2z_leg_gore` heisst also falsch (es wirft die ARME 9/12 ab), `re2z_knockdown_gore`
ebenso (es wirft die BEINE 3/5/6 ab). Die INDIZES stammen aus dem Original und sind
richtig - umbenannt wird nichts, aber im Code steht jetzt eine Warnung.

## Was der Port daraus braucht

Nichts Grosses: die Trennung ist seit v0.7.95 drin, der Beine-Freeze seit v0.7.98. Offen
bleibt der Ausklang-Clip der abgetrennten Haelfte (FUN_8010B7D4 Zustaende 5..8) und die
Eigenanimation ueber FUN_80029E10.
