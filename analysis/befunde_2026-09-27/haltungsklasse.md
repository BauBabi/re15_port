# Runde 34: der Schuss-Pfad läuft jetzt über die HALTUNGSKLASSE

`bash re15_port/tools/local_build.sh all` → **`=== LOCAL-BUILD-OK (all) — Tests 357/357`**
(Protokoll: `analysis/befunde_2026-09-27/haltungsklasse.log`; Sonden-Protokolle
`haltungsklasse_r30b.log`, `haltungsklasse_r31.log`, `haltungsklasse_r33.log`).
Alle vier GUI-Haken im **ersten** Lauf grün.

---

## 0. Kurzurteil

| | |
|---|---|
| **Umgestellt** | Ja. Der Hund 0x20 läuft im Schuss-Pfad jetzt durch `re15_re2_gun_probe`, den Port-Zwilling von `FUN_800410CC`, und wird dort über die **Haltungsklasse `(word0>>26)&7`** gegen die Maskentabelle @0x800A6DB4 entschieden. |
| **Tor 5 eingegrenzt** | Ja. `@0x8004716C-A4` läuft nur noch für die Nahkampf-Waffen (RE2-Waffe 1, Aufrufstelle @0x80042F94) und nur in den Klingen-Bildern — **mit** der `+200`-Absenkung @0x80042F68. Gemessen: **Tor5-Urteile beim Schuss = 0 für jeden Typ**, beim Messer **135**. |
| **Verhalten gleich** | Ja, gemessen. Liegender Hund 0 Treffer in 77 Bildern, stehender trifft, nach dem Aufstehen wieder treffbar — identische Zahlen wie vor der Umstellung (§3). |
| **Bolzen-Aufrufstelle (B)** | Hat im Port **keinen Produzenten**: `re2z_row_from_weapon` (enemy_ai_re2_zombie.c:3829) bildet keine der 22 RE1.5-Waffen auf RE2-Id 12 ab. Nichts zu bauen, und das steht als Zeile da, nicht als Lücke. |
| **Nicht gebaut** | Der Klassen-Nuller @0x801038A0 (Spiel-tot-Zweig) — er wäre eine Dauersperre; Begründung mit der Stelle in §5. Und der Schadensrecord des Hundes @0x800A4424 ist gedumpt, aber **nicht** verdrahtet (§5). |

---

## 1. Die Maskentabelle @0x800A6DB4 — Satz, Zeile, Spalte, und was die 0 bedeutet

Eigener Dump (`re2_disasm.py read 0x800A6DB4 30 --w 1`):

```
+0 .. +8   4,2,1 | 2,1,4 | 1,2,4      (Satz 1)
+9 ..+17   4,2,0 | 2,0,0 | 1,2,0      (Satz 2)
+18..+26   0,0,0 | 0,0,0 | 0,0,0
```

Die vier Fragen, jede mit ihrer Instruktion (alle selbst disassembliert aus
`info/re2leon/PSX.EXE`):

**SATZ** — `s1 = ((Sub-Box-Flag & 8) != 0)`:
```
8004141c: lbu v0,1(s3)        ; das Flag der Sub-Box
80041424: andi v0,v0,0x8
80041428: sltu s1,zero,v0     ; s1 = 0 oder 1
80041470: sll v0,s1,3
80041474: addu v0,v0,s1       ; Versatz 9*s1
8004147c: addiu t0,t0,28084   ; Basis 0x800A6DB4
80041484: addu a0,v0,s0       ; + Zeile
```
`s1` ist 0 oder 1 — **der dritte Satz ab +18 ist damit nie adressierbar**, totes
Füllmaterial. Flag 8 ist genau das EBEN-Fenster (`FUN_80041B20` case 3 liest
Record +12/+14 = LEVEL), Satz 2 ist also **die EBEN-Spalte**.

**ZEILE** — `s0 ∈ {0,3,6}`, aus der Höhe des Kandidaten IM FENSTER
(`dy = Gegner+0x3C − Spieler+0x3C` @0x800413C0/DC, `third = (lo−hi)/3` aus
`FUN_80041B20`):
```
8004142c: sll v0,a0,1         ; 2*third
80041430: addu v0,a1,v0       ; hi + 2*third
80041434: slt v0,s5,v0        ; dy < hi+2*third ?
80041438: bne v0,zero,0x80041444
8004143c: addiu s0,zero,6     ; DELAY -> Vorgabe 6
80041440: addiu s0,zero,3     ; sonst 3
80041444: addu v0,a1,a0       ; hi + third
80041448: slt v0,s5,v0
8004144c: bne v0,zero,0x80041458
80041454: addu s0,zero,zero   ; dy >= hi+third -> 0
```
PSX-Y wächst nach unten: großes `dy` = der Gegner steht **tiefer** als der Spieler.
Zeile 0 = am tiefsten im Fenster, Zeile 6 = am höchsten.

**SPALTE** — die drei Prioritätsstufen eines Eintrags, gelesen in der Reihenfolge
`pb[2] → pb[1] → pb[0]`; jeder Treffer überschreibt `s4`, also **gewinnt `pb[0]`**:
```
80041488: lbu v1,2(a0)  / 80041490: and v0,v1,s6 / 80041494: beq -> überspringen
8004149c: srl s7,v1,1   / 800414a0: sll v0,s7,3  / 800414a4: addiu s4,v0,1
800414a8: lbu v1,1(a0)  …  (dasselbe)
800414c8: lbu v1,0(a0)  …  (dasselbe, LETZTER Schreiber gewinnt)
```

**WERT** — er ist **beides**: die Bitmaske gegen die Haltungsklasse (`and v0,v1,s6`)
und der Zonen-Index (`srl s7,v1,1`: 1→0, 2→1, 4→2). Bit 0 (=1) Beine, Bit 1 (=2)
Rumpf, Bit 2 (=4) Kopf.

**WERT 0** = **leere Stufe**. `and 0,s6` ist immer 0, der Zweig springt weiter; die
Stufe kann nie treffen. In Satz 2 (EBEN) heißt das:
* Zeile 0 `4,2,0` → Bit 2 oder Bit 1 nötig
* Zeile 3 `2,0,0` → **allein Bit 1**
* Zeile 6 `1,2,0` → Bit 0 oder Bit 1

Genau darauf beruht der Nutzer-Befund: `FUN_80104088(0)` löscht Bit 1.

---

## 2. Warum der Hund eine EIGENE Fensterzeile braucht

Der Record hängt am **Entity-Typ**, nicht an der Waffe — das war im Port bisher
nicht abgebildet, weil nur die Zombie-Familie durch den Applier lief:

```
80041380: lbu v1,8(s2)         ; ENTITY-TYP des Kandidaten
80041388: sll v1,v1,2
80041390: lui at,0x800a
80041398: lw  v1,27272(at)     ; PTR_DAT_800A6A88 + Typ*4
800413a0: sw  v1,80(sp)
800413a4: srl v1,t0,16         ; RE2-Waffen-Id (zweites Argument >> 16)
800413a8-b8: + 20*Id
800413c0: lw  a0,60(s2)        ; dy-Zaehler = Gegner +0x3C
```

Eigener Dump der Zeigertabelle (48 Worte ab 0x800A6A88):

| Typ | Record |
|---|---|
| 0x10..0x14, 0x18..0x1F, 0x2C | 0x800A412C (Zombie, `s_re2z_fen`) |
| 0x15..0x17 | 0x800A42A8 |
| **0x20 HUND** | **0x800A4424** (neu: `s_re2d_fen`) |
| 0x21 Krähe | 0x800A45A0 |
| 0x25/0x26 | 0x800A4B90 |

Gegenprobe, dass der Dump stimmt: die Zombie-Zeile ergibt Zeichen für Zeichen die
schon im Port stehende `s_re2z_fen` **und** `s_re2z_rec_w0` (Id 1 = Wort 3, Id 2 =
0x00E03C10 = 16/15/14 …).

Die Hunde-Zeile für die Pistolen-Familie (RE2-Id 2/3/4/13/19):
`UP [-4000,-2000] · LEVEL [-3000,+2000] · DOWN [-500,+3000]`.
Daraus `third = (−3000−2000)/3 = −1666`, also **Zeile 0 ab dy ≥ 334, Zeile 3 ab
dy ≥ −1332, sonst Zeile 6** — ein Hund auf der Spielerebene (dy = 0) landet in
**Zeile 3**, und Zeile 3 von Satz 2 ist `2,0,0`.

Ebenfalls typabhängig: **+0x1EE**, der Tiefenzuschlag der Nah-Sub-Box
(`lhu v0,494(s2)` / `sra 18` / `sh v1,8(s3)` @0x8010133C-50).
Zombie 500 (@0x8010096C), **Hund 600** (`addiu v1,zero,600` @0x80100290 /
`sh v1,494(s0)` @0x801002C4, EMD0G_MOD0.BIN).

**+0x9A** (Breitenzuschlag) des Hundes: Vollscan `sh rt,154(rs)` über
EMD0G_MOD0.BIN ergibt **zwei** Treffer, @0x801002E0 und @0x801044BC — und **beide
gehen auf den TEILEPOOL**, nicht auf das Entity (`lw v0,408(s0)` @0x801002D0 bzw.
`lw v1,408(a0)` @0x80104490 unmittelbar davor). Das Entity-Feld bleibt damit 0, es
gibt keinen Breitenzuschlag. Das ist kein „nicht gefunden", sondern ein Befund.

---

## 3. Die Haltungsklasse des Hundes — 11 Stellen, 3 gebaut, und der Beweis dafür

Vollscan aller `sw rt,0(rs)` in EMD0G_MOD0.BIN (21 Treffer) und aller
`lui reg,0x400|0x800|0xc00|0xe7ff|0xf3ff` (15 Treffer), jede Stelle einzeln
disassembliert. Auf das **eigene** Entity-word0 schreiben elf:

| Adresse | Wirkung auf die Klasse | gebaut? |
|---|---|---|
| @0x80100490 | Spawn-Dispatch: `\|= 3` (`lui v1,0xc00` @0x80100488) für +0x10E 0..6/9 und alles Unbekannte; `\|= 2` (`lui v1,0x800` @0x80100458) für 7/8; `\|= 1` (`lui v1,0x400` @0x801003F4) für 0x2003 | **ja** (`re2d_init`, Zweig 0xC00 — der Port führt Raw 0) |
| @0x801040D8 | `FUN_80104088`: a1==0 → `& 0xE7FFFFFF \| 0x04000000` = **1**; a1!=0 → `\| 0x0C000000` = **\|3** | **ja** (`re2d_hitbox`) |
| @0x801005FC / @0x801008A8 / @0x801013C0 / @0x80101600 / @0x80101774 | `& 0xEFFFFFFF \| 0x04000000` | nein — **berührt Bit 1 nicht** |
| @0x80101348 | `& 0xFBFFFFFF \| 0x10000000` | nein — **berührt Bit 1 nicht** |
| @0x80100C28 | `& 0xEFFFFFFF \| 0x0C000000` | nein — **setzt** Bit 1 |
| @0x80101A3C | `\| 0x08000000` | nein — **setzt** Bit 1 |
| @0x801038A0 | `& 0xE3FFFFFF` = Klasse **0** | nein, s. §5 |

**Warum drei genügen — als Beweis, nicht als Hoffnung:** die EBEN-Zeile 3 der
Maskentabelle (`2,0,0`) fragt **allein Bit 1** ab. Bit 1 wird von **keiner** der
acht ungebauten Stellen gelöscht — fünf davon maskieren mit `0xEFFFFFFF` (löscht nur
Bit 2), eine mit `0xFBFFFFFF` (löscht nur Bit 0), zwei setzen Bit 1 zusätzlich.
Gelöscht wird Bit 1 nur von `and 0xE7FFFFFF` in `FUN_80104088(0)` und vom
`0xE3FFFFFF`-Wurf. Die Treffer-/Nichttreffer-Entscheidung beim ebenen Zielen ist
damit vollständig; die übrigen Stellen verschieben nur Bit 0/Bit 2, also die ZONE
(die der Port für den Hund nicht als Schaden liest) und die EBEN-Zeilen 0/6, die
`dy ≥ +334` bzw. `dy ≤ −1332` verlangen — Höhenunterschiede, die ein Hund auf der
Spielerebene nicht erreicht.

Nebenbefund derselben Bits: `word0 & 0x18000000` (@0x8010403C-44) ist im Original der
**Sprung-Kick-Test** des Hundes; der Port bildet ihn seit langem auf `re2d_air219 == 0`
ab. Dass beide Leser dasselbe Bitpaar meinen („aufrecht"), stützt die Lesart —
verdrahtet wurde daran nichts.

---

## 4. Was gebaut wurde, und was es misst

### 4.1 Der Schuss-Pfad

`re15_re2_gun_probe` (re15_damage.c) nimmt jetzt auch den Hund: Fensterzeile
`s_re2d_fen`, `RE2D_RAD1EE = 600`, Klasse aus `re2z_parts`. Angeschlossen wird er im
Band-Zweig der Kandidatenschleife (`e->type == 0x20 && re15_ai_re2_for_type(...)`),
**nur für die Hitscan-Ids**: RE2-Id 1 (Messer — und über
`re2z_row_from_weapon` auch w0/w21) bleibt draußen. Das ist gemessen und nicht
vorsichtshalber: mit Id 1 im Applier verfehlten w0 und w21 den Hund vollständig
(„Schuss kam nicht an", unit_re2_hp_model, erster Versuch dieser Runde), weil die
Messer-LEVEL-Box nur bis ~2450 reicht.

Die Port-Ersatzmaske „`parts==0 → 3`" (Fixture-Fall) gilt für den Hund **nicht** mehr —
sonst wäre der liegende Hund per Hintertür wieder Klasse 3. Für ihn greift stattdessen
das Original-Gate `beq s6,zero,0x80041774` @0x800413D8.

### 4.2 Tor 5 auf seine zwei echten Aufrufstellen

```c
if ((weapon_id == 1 || weapon_id == 2) && e->re2_hit_box_set) {
    if (re15_player_slash_window() && kf >= 6 && kf <= 10) {
        ... int32_t ty = mz[1] + 200;   /* addiu v0,v0,200 @0x80042F68 */
```
Bildfenster byte-true: `lbu v0,333(s1)` / `addiu v0,v0,-7` / `sltiu v0,v0,0x5` /
`beq v0,zero,0x80042FBC` @0x80042F48-58 — die Bilder 7…11. Port-Abbildung benannt:
RE2s Messer-Pattern @0x800A6608/6434/656C (`ff/6 00/1 01/1 02/1 03/1 04/1 00/255`) hat
sechs Bilder Ausholen und dann **fünf** Record-Bilder; der Port bildet diese fünf auf
`anim_frame 6..10` ab (`re15_re2_knife_step`). Die fünf Torbilder sind dieselben fünf.
Die Absenkung wird nach dem Tor nicht zurückgenommen, weil der Port die Höhe je Aufruf
neu holt statt sie wie RE2 auf dem Stack zu halten (@0x80042FAC-B8).

### 4.3 Die Mechanismus-Zähler

Neu in `re15_damage.c`/`.h`: `re15_dmg_mech_reset()` / `re15_dmg_mech_counts()` liefern
`gate5` (Urteile @0x8004716C-A4), `cls` (Lesungen @0x800413C4-D8) und `cls_rej`
(davon Klasse 0). Ohne sie wäre jeder Riegel dieser Runde **vor und nach** der
Umstellung grün gewesen — das Verhalten ändert sich ja nicht.

---

## 5. Was NICHT gebaut wurde — mit Zahl bzw. Stelle

* **Der Klassen-Nuller @0x80103874-A0** (`lui a0,0xe3ff` / `ori 0xffff` / `and` / `sw`,
  im HP-Reroll-/Spiel-tot-Zweig). Er setzt die Klasse auf 0 und **keine Stelle des
  Hundebaums setzt sie zurück** — gebaut wäre er eine Dauersperre (Runde-13/14-Falle).
  Dieselbe Aussage trägt im Port bereits `+0x1D3 |= 0x80` @0x80103894-A4, das der
  Trefferfilter liest (`re2d_hurt_p4_reroll`, Wache `test_re2_dog_playdead_gate`).
* **Der Schadensrecord des Hundes** @0x800A4424 ist gedumpt (`s_re2d_rec_w0`, Pistole
  18/16/15), aber nicht verdrahtet: der Schadens-Ersatz hängt ausdrücklich an
  `dmg_row == s_re2_wpn_dmg_zombie(16)`. Diese Runde stellt den TREFFER-Mechanismus um,
  nicht das Schadensmodell; wer die Zeile scharf schaltet, verschiebt jede Hunde-HP-Messung.
* **Die Aufrufstelle (B), Waffe 12 (Bolzen)** @0x800467C0: `re2z_row_from_weapon`
  (1,1,1,3,2,4,4,5,7,9,11,10,15,8,16,9,11,10,17,18,13,1) bildet **keine** der 22
  RE1.5-Waffen auf RE2-Id 12 ab. Es gibt nichts zu bauen.
* **Das Tor für Krähe 0x21, Spinne 0x25, Baby 0x26** bleibt aus — und ist jetzt
  gegenstandslos: für Schüsse läuft das Tor für **keinen** Typ mehr (gemessen Tor5 = 0).

---

## 6. Die Messungen

### 6.1 Trefferzensus, 900 Bilder Dauerbeschuss je Typ (`probe_r33_aufrufstelle`)

```
ZOMBIE 0x10 stehend        82 / 900   Tor5 0 / Klasse 82
ZOMBIE 0x10 Kriecherbox    82 / 900   Tor5 0 / Klasse 82   (Box erzwungen -350/350)
HUND   0x20                11 / 900   Tor5 0 / Klasse 82
KRAEHE 0x21                40 / 900   Tor5 0 / Klasse  0
SPINNE 0x25                82 / 900   Tor5 0 / Klasse  0
BABY   0x26                41 / 900   Tor5 0 / Klasse  0
```
Der Hund liegt bei **11**, exakt wie in Runde 33 vor der Umstellung. Kein Typ ist
dauerhaft untreffbar.

### 6.2 Die beiden Kontrollen

```
TEIL 3a  Box kuenstlich 0/0 (der alte Hebel):
  ZOMBIE 28 / 300   Tor5 0      HUND 4 / 300   Tor5 0
TEIL 3b  Haltungsklasse kuenstlich 4 (nur Bit 2):
  ZOMBIE  0 / 300              HUND 0 / 300    Klassen-Lesungen 28
```
Die **Box-Kontrolle ist umgedreht**: sie fällt nicht mehr auf 0 (28 bzw. 4 sind genau die
Hochrechnung von 82/900 bzw. 11/900 auf 300 Bilder), und das Tor fällt 0 Urteile — genau das
beweist, dass @0x8004716C-A4 nicht mehr im Schuss-Pfad liegt.
Die **Klassen-Kontrolle** fällt auf 0. Klasse 4 statt 0, weil für die Zombie-Familie die
Port-Ersatzmaske greift (gemessen 70 Treffer trotz Klasse 0); Klasse 4 fällt an der
EBEN-Zeile `2,0,0` durch, ohne die Ersatzmaske zu treffen.

### 6.3 Der Nutzer-Befund als Bilderzahl (Teil 4)

```
stehend getroffen: 1 | Kette bis er wieder steht: 78 Bilder
waehrend er LIEGT: 0 Treffer in 77 Bildern (Klasse != 1 in 0 Bildern)
nach dem AUFSTEHEN: 1 Treffer in 60 Bildern (Klasse MIT Bit 1 in 10 Bildern)
MECHANISMUS: Tor5-Urteile 0 | Klassen-Lesungen 13
```
Die Zeile „Klasse MIT Bit 1 in 10 von 60 Bildern" ist kein Mangel: in diesem Abschnitt
wird weitergeschossen, der Hund geht erneut nieder und trägt dann richtigerweise wieder
Klasse 1. Die strenge Zuordnung Box ↔ Klasse prüft Teil 1 (0 Abweichungen in 400 Bildern).

### 6.4 Verhalten VOR und NACH der Umstellung (`probe_r30b_muendung`)

```
[ALT] Treffer am STEHENDEN Hund: 1 | Niederschlag bei f10 | LIEGEND 17 Treffer in 389 Bildern
[NEU] Treffer am STEHENDEN Hund: 1 | Niederschlag bei f10, wieder auf den Beinen nach 78 Bildern
      | LIEGEND 0 Treffer in 77 Bildern | nach dem Aufstehen 1 in 60 Bildern
```
Der ALT-Hebel musste mitgezogen werden: er machte früher das Tor inert
(`re2_hit_box_set = 0`) und wirkte nach der Umstellung nicht mehr — ALT und NEU lieferten
gemessen **dieselbe** Zeile. Jetzt hält ALT die Klasse jedes Bild auf 3
(`lui v1,0xc00` @0x801040CC) und reproduziert damit **17 Treffer in 389 Bildern** — exakt
die Runde-30-Zahl, die das Dossier als „Stand ALT" führt.

### 6.5 Boxen-Sonde (`probe_r31_boxen` Teil 2c, 900 Bilder)

```
                KLASSE FEST 3   WIE GEBAUT   KONTROLLE K=4   Tor5   Klasse
ZOMBIE 0x10          70             45             0          0       82
HUND   0x20          41             12             0          0       82
KRAEHE 0x21          41             41            41          0        0
SPINNE 0x25          82             82            82          0        0
BABY   0x26          41             41            41          0        0
```
Die drei letzten Zeilen laufen nicht über den Applier; ihre Klassen-Kontrolle ist **nicht
anwendbar** und wird als Zahl ausgewiesen statt als Schranke behauptet.

### 6.6 Das Messer trifft weiter (Teil 5)

```
ZOMBIE 0x10  14 Treffer in 300 Bildern | Tor5-Urteile 135
HUND   0x20   4 Treffer in 300 Bildern | Tor5-Urteile 135
```
Damit ist belegt, dass das Tor **umgezogen** und nicht still gelöscht ist: beim Schuss 0
Urteile, beim Messer 135. Der Keil `{−200,0,250,125}` @0x8001101C und die
Nahkampf-Reichweite aus Runde 28 sind unberührt.

---

## 7. Sichtprüfung

`analysis/befunde_2026-09-27/r34_sichtpruefung.sh` fährt ROOM1190 im echten Lauf
(kein AUTOSHOT, kein Softwarerenderer), lässt die Hundewelle herein und schießt.
Bilder in `analysis/befunde_2026-09-27/r34_bilder/`:

* `f548.png` — die drei Hunde springen durchs Fenster
* `f604.png` — ein Hund beißt, Blut (der Treffer-/Schadensweg lebt)
* `f612.png` — **ein Hund liegt flach zu Leons Füßen, während er zielt und feuert**
* `f620.png` — derselbe Hund wieder auf den Beinen

⛔ **Ehrlich dazu:** das Bild zeigt die Situation, es **beweist** die Sperre nicht — ob in
einem Bild Schaden fiel, sieht man ihm nicht an. Der Beweis ist §6.3 (0 Treffer in 77
Bildern bei 0 Tor-Urteilen und gelesener Klasse). Und: diese Runde ändert das Verhalten
**nicht**, es gibt also kein Vorher/Nachher zu fotografieren.

Zwei Messhaken waren dafür nötig, beide env-gegatet und ohne die Variable wirkungslos:
* `RE15_DEBUG_SUB="<sub>@<frame>"` startet einen Sub-Thread wie `Evt_exec` (0x04,
  scd_vm.c:1076) — ROOM1190 hält sein `Sce_em_set` der Hunde in **sub 13**, dasselbe sub,
  das `probe_dog_attack_live.c:213` und `probe_r33_aufrufstelle` headless hochfahren.
* `RE15_DEBUG_DOGWAKE="<frame>"` zündet die SCD-Marke **grid 0x43**, auf die der
  Skript-Spawn wartet (@0x801113E4-EC; derselbe Weg wie probe_dog_attack_live.c:266).

Gemessen und protokolliert (`debug_sub.log`, weil die GUI-exe stderr verliert — `run.log`
bleibt leer):
`[debug-sub] Frame 450: sub13 subs=16 Slot 2 rc=0` und
`[dogwake] Frame 480: 3 Hund(e) auf grid 0x43`.
**Ohne beide Haken kommt kein Hund ins Bild** — sub 13 allein weckt nichts (gemessen: kein
einziger RE2-Hunde-Tick), und ein Raum-Sprung überspringt den Ereignis-Vorlauf.

---

## 8. Offen / ehrlich benannt

1. **Der Zonen-Wert des Hundes wird jetzt gestempelt.** `+0x1D2 = Teil + 3*Klammer`
   (@0x800413CC-D4) landet über `s_re2_probe` in `re2z_hits1d2`, das der Hund in
   `re2d_knockdown` liest (`2 - hits1d2/3` @0x80103D00-28). Vorher lieferte der Port dort
   den Näherungswert 0/1. Byte-true ist der neue Weg, aber die Wirkung auf die
   Knockdown-Zweige ist **nicht eigens gemessen** — die Suite bleibt grün, mehr sagt diese
   Runde dazu nicht.
2. **Acht der elf Klassen-Stellen sind nicht gebaut** (§3). Der Beweis in §3 deckt die
   Treffer-Entscheidung beim ebenen Zielen; die Zonen-Wahl in den EBEN-Zeilen 0/6 kann
   abweichen, sobald ein Hund mehr als 334 Einheiten unter bzw. 1332 über dem Spieler steht.
3. **Der Schadensrecord des Hundes ist gedumpt, nicht verdrahtet** (§5).
4. **Der `+0x1D0`-Seiteneffekt** des fünften Tores (@0x80047178/84) bleibt unportiert wie
   bisher — der Port führt +0x1D0 nicht.
5. **Kein PATH-Prepend beim Sichtlauf.** Gemessen: mit `PATH=/c/msys64/...:$PATH` hängt die
   exe vor dem Titelbild (zweimal 140 s Zeitlimit, keine geschriebene Datei). Steht als
   Warnung im Skript.
6. **Die Sichtläufe enden mit RC=1 bzw. RC=124** — der Prozess bricht während der
   Hundewelle ab bzw. wird vom Zeitlimit beendet. Ob der Abbruch ein eigener Defekt ist
   (drei Hunde + Dauerbeschuss) oder nur der Spielertod, ist **nicht** ermittelt. Kein
   Riegel hängt daran; es ist als Spur notiert, nicht als Feststellung.

---

## 9. Berichtigte Kommentare

| Ort | Stand bis Runde 33 | jetzt |
|---|---|---|
| `re15_damage.c` (Muzzle-Block) | „NICHT GEBAUT: die Absenkung um 200" | gebaut, mit Bildfenster und gemessener Wirkung |
| `re15_actor.h` | Tabelle „Tor SCHARF/UNSCHARF je Typ" | ersatzlos gestrichen — das Tor ist nicht der Schuss-Pfad |
| `re15_damage.h` | „MUENDUNGSHOEHE" | Zielhöhe = Klingenlage, Verbraucher nur @0x80042F94 |
| `enemy_ai_re2_dog.c` | „die andere Hälfte ist das fünfte Gate" | die Haltungsklasse; „heute ohne Wirkung" → Messzahlen |
| `enemy_ai_re2_spider.c` | „DAS TOR IST FUER 0x25 SCHARF" | gegenstandslos |
| `enemy_ai_re2_crow.c` | box_set = 0 wegen der Mündungshöhe | auf das Messer eingegrenzt |
| `player_common.c`, `game_step_common.c`, `platform/pc/main.c` | „Mündungshöhe des fünften Tores" | Klingenlage, kein Schuss |
