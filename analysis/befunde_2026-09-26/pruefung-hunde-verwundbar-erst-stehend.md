# Prüfung: hunde-verwundbar-erst-stehend.md

Status: FERTIG. Urteil: **NICHT HALTBAR als Handlungsgrundlage** — die RE-Kernaussage stimmt,
der Umsetzungsplan und die Symptom-Zuordnung tragen nicht.
Prüfer: Skeptiker-Agent, 2026-09-26. Keine Engine-Änderung, kein Build.

## 1. Selbst nachgelesene Adressen (~30, alle mit re2_disasm.py)

### 1.1 RE2 `info/re2leon/PSX.EXE` (t_addr 0x80010000, RAM = 0x80010000 + off − 0x800)
```
8004712c: andi v0,v0,0x1
80047130: beq v0,zero,0x8004740c
80047138: lbu v0,467(s0)          <- UNMASKIERT
8004713c: nop
80047140: bne v0,zero,0x8004740c
80047148: lh v0,342(s0)
80047158: lhu v0,270(s0) / 80047160: andi v0,v0,0xc000
```
✅ **§2.1 des Dossiers ist BESTÄTIGT.** Zwischen `lbu` @0x80047138 und `bne` @0x80047140 steht
nur ein `nop` — kein `andi`. Das Gate prüft das ganze Byte. Bit 0x80 sperrt zeitunabhängig.

Applier @0x80047314-4C ebenfalls bestätigt:
`lbu a0,467(s1)` @0x8004731c / `andi a0,a0,0x80` @0x8004732c / `sb a0,467(s1)` @0x80047334 /
`lw v0,4(a1)` @0x80047338 / `srl v0,v0,9` @0x80047340 / `andi v0,v0,0x7f` @0x80047344 /
`or a0,a0,v0` @0x80047348 / `sb a0,467(s1)` @0x8004734c. Auch `sb s5,5(s1)` @0x80047324. ✅ §2.3.

Anim-Kette: `jal 0x8002959c` @0x80101654, `bne v0,zero` @0x8010165c, `addiu v0,zero,1`
@0x80101660, `addu v0,zero,zero` @0x80101664 ✅; `lbu v0,332(a0)` @0x800295a8 /
`lbu v0,333(a0)` @0x800295d0 ✅ §2.8.

### 1.2 Richtige Datei? JA — `info/re2leon/COMMON/BIN/EMD0G_MOD0.BIN` @0x80100000, header-los
Gegenprobe (das ist der Test, den das Dossier selbst nicht dokumentiert):
`lui at,0x8010` @0x80100064 + `lw v0,21560(at)` @0x8010006c ⇒ Tabelle @0x80105438, und die
enthält lauter gültige Overlay-Zeiger `0x801000f4 / 0x801004dc / 0x801032a8 / 0x801040dc /
0x801049e4 / 0 / 0 / 0x801049ec` — exakt §2.6. Bei falscher Ladeadresse wäre das Müll. ✅

Root-Dekrement @0x80100028 `lbu v1,467(s0)` / @0x80100030 `andi v0,v1,0x7f` /
@0x80100034 `beq` / @0x80100038 `addiu v0,v1,-1` / @0x8010003c `sb v0,467(s0)` ✅ §2.2.

Setzer/Freigeber, alle sechs selbst gelesen:
| Adresse | Instruktion | Dossier |
|---|---|---|
| 0x80102ce4/ec | `ori v0,v0,0x80` / `sb v0,467(s0)` | ✅ SET Sprung |
| 0x80102e18/20/28/3c/40 | `jal 0x80101574` / `beq v0,zero,0x80102e44` / `lbu v1,467` / `andi v1,v1,0x7f` / `sb` | ✅ CLR Landung 14 |
| 0x80102f80/88/94/a0/a4 | dito, Ziel 0x80102fa8 | ✅ CLR Landung 15 |
| 0x80103708/10/18 | `lbu v0,467` / `andi v0,v0,0x7f` / `sb v0,467` | ✅ CLR Hurt-P2 |
| 0x80103794/a4/a8 | `lbu v1,467` / `andi v1,v1,0x7f` / `sb v1,467` (+ `sw a0,332` @0x801037a0, a0=0x00030f16) | ✅ CLR Hurt-P3 |
| 0x80103e28/34/3c | `lbu v0,467(a0)` / `ori v0,v0,0x80` / `sb v0,467(a0)` | ✅ SET Hurt-Zeilen |

**Kein einziges Disasm-Zitat des Dossiers ist falsch.** Das ist die gute Nachricht.

## 2. Falsche / unvollständige Aussagen

### 2.1 ⛔ Kräher-Bilanz: 5 Setzer, nicht 4 — und die vier genannten sind LEICHEN
Voll-Scan `EMOVL21_S0.BIN` (0x4a88 Byte, @0x80100000), alle `sb rt,467(rs)`:
`0x80100174, 0x801004a8, 0x80100fd0, 0x801039f4, 0x80103c1c, 0x80103c88, 0x80103f08`.
`li 128` unmittelbar davor: `0x801004a0, 0x801039f0, 0x80103c18, 0x80103c84, 0x80103f04`
= **FÜNF** Setzer. Das Dossier nennt vier und übersieht @0x801004A0-A8
(`lhu v0,270(s1)` @0x80100490 / `andi v0,v0,0x40` @0x80100498 / `beq` @0x8010049c /
`addiu v1,zero,128` @0x801004a0 / `sb v1,467(s1)` @0x801004a8).

Wichtiger: die vier genannten liegen ALLE im Leichen-Router (Tabelle @0x80104A70:
[0] 0x8010398C Normal, [1] 0x80103C0C GIB, [2] 0x80103C44 Wand-Splat, [3] 0x80103EB0 Launch —
port-seitig `enemy_ai_re2_crow.c:1514/1597/1618/1695` in `re2c_corpse`, state 7). Eine tote
Krähe dauerhaft aus der Kandidatenliste zu nehmen ist genau das RICHTIGE Verhalten, kein
Unverwundbarkeits-Risiko. Der Dossier-Satz *„**Kraehe: 4 Setzer, 1 Loescher.** ... **Hier zuerst
messen.**"* ist in der Zahl falsch und in der Deutung falsch gerahmt.

### 2.2 ⛔ §3.1 „Alle Setz-/Loesch-Stellen des Originals stehen im Port" — Tabelle unvollständig
Die Tabelle listet 9 Zeilen. `grep -n re2z_self1d3 enemy_ai_re2_dog.c` liefert **12**
Verhaltensstellen. Es fehlen `:753` (`&= 0x7f` @0x8010088C-94), `:809` (@0x80100C00-08) und
**`:1789` (`|= 0x80u` @0x80103894-A4)** — ein SETZER. Entsprechend widerspricht Schritt 3
(„Hund: 4 Setzer / 8 Loescher (Tabelle §3.1)") der eigenen Tabelle (3 + 6). Die Zahl 4/8 stimmt
zufällig mit dem Port überein, die zitierte Quelle belegt sie nicht.

### 2.3 ⛔ §3.2 zitiert die Zeile und unterschlägt die Begründung direkt darüber
Das Dossier schreibt: *„### 3.2 ⛔ DER DEFEKT: das Gate maskiert Bit 0x80 weg"* und zitiert
`re15_damage.c:3098`. Es verschweigt den 20-Zeilen-Kommentar `re15_damage.c:3076-3097`, der
die Maske als **gemessene** Sicherung dokumentiert:
> „⛔ NUR FREIGEBEN, NIE SETZEN - und das ist eine Messung, keine Vorsicht. ... Vier solche
> Loecher sind gemessen und erreichbar: der wartende ROOM1190-Zwingerhund ... bleibt auf
> +0x1D3 = 14; ein ROOM1140-Zombie ... bleibt auf +0x1D3 = 5; die ROOM1090-Feuer-Emitter
> (Typ 0x26 ...) ... gemessen 900 von 900 Bildern auf 15; die beiden Sitz-Import-Ausstiege"

und `:3062-3068`: *„Ersetzt man ihn fuer RE2-Typen durch das 1D3-Gate, laeuft der Resolver
endlos (vier Tests in Timeouts ...)"*. Eine Zeile als „DER DEFEKT" auszuweisen, ohne das
dagegen stehende Messprotokoll auch nur zu erwähnen, ist die Überziehung dieses Dossiers.

### 2.4 ⛔ Der Plan widerspricht sich selbst
Schritt 1 patcht `re15_damage.c:3098` in `re15_re2_pause_filter_apply`. Diese Funktion gilt
laut ihrem eigenen Typ-Filter (`re15_damage.c:3083`)
`if (e->type != 0x20u && e->type != 0x21u && e->type != 0x25u && e->type != 0x26u) return;`
für **Hund, Krähe UND Spinne**. Schritt 3 verlangt gleichzeitig *„Schritt 1/2 nur fuer Typ 0x20
scharf schalten"*. Der in Schritt 1 abgedruckte Code tut das nicht — er schaltet alle vier Typen
scharf. Der Plan ist in der vorgelegten Form nicht ausführbar, ohne genau die Falle auszulösen,
vor der er in Schritt 3 warnt.

## 3. Die teuerste Frage: hat JEDES Setzen eine erreichbare Freigabe? — NEIN

| Setzer | Datei:Zeile | Freigabe |
|---|---|---|
| Sprung ACTIVE 14/15 @0x80102CE4 | dog:1436 | ✅ @0x80102E28-40 / @0x80102F94-A4 (Clip-Ende) |
| Hurt-Zeilen 10/16 @0x80103E34 | dog:1938 | ✅ @0x80103708-18 / @0x801037A4-A8 |
| HP-Re-Roll @0x80103894 | **dog:1789** | ❌ keine — laut Port-Doku (`:1974-1982`, `:2088-2095`) unerreichbarer `default:`-Arm; bleibt eine Mine |
| Devour @0x80104F64 | dog:566 | ❌ Dossier §6.3 selbst: *„im Hunde-Overlay habe ich keinen Loescher dafuer gefunden"* — ungemessen |
| ACTIVE-Sub 16 @0x80103188 | — | ❌ im Port Stub; wird der Stub je gefüllt, sperrt er dauerhaft |
| Krähe Leichen-Router ×4 | crow:1514/1597/1618/1695 | ❌ einziger Löscher `crow:583` (@0x80100FC4-D0) liegt im LEBENDEN Flug-Sub-3-P1, nicht im Leichenpfad. Für die Leiche folgenlos, aber die Bilanz des Dossiers stimmt trotzdem nicht |
| Spinne `= 255` @0x801007C8 | **spider:2714** | ⚠️ Nullung nur `spider:2717` (@0x801007E4); die beiden Root-Dekremente `spider:2838/2964` stoppen bei 0x80 |

**Antwort auf die gestellte Frage: nein.** Mindestens `dog:566` (Devour) und `spider:2714`
haben keine belegt-erreichbare Freigabe. Die Krähe würde der Plan **nicht** unverwundbar machen
(die vier Setzer sind Leichen-Zustände), aber aus dem falschen Grund harmlos — das Dossier hat
die Krähe fälschlich als Hauptrisiko markiert und das echte Risiko (Spinne/Typ 0x26, Devour)
nicht als Blocker geführt.

## 4. Erklärt der Mechanismus das Symptom? — NEIN

Der Nutzer-Satz in §1 lautet: *„Im Original Resident Evil 2 sind die Hunde erst dann wieder
verwundbar, sobald sie wieder stehen."* Das Dossier gibt in §4.1 selbst zu:
> „Fuer Pistole/Schrot/Magnum setzt der Hund Bit 0x80 im Original **NICHT**; dort endet die
> Sperre nach den Stun-Bildern der Schadenszeile (Hund 0x800A4424: meist 15)."
und in §6.2: *„Welchen der beiden Faelle der Nutzer gesehen hat. ... kann ich das nicht
entscheiden."*

Für den Normalfall (Pistole) ändert der Plan also **nichts** — der Port macht dort bereits die
byte-true 15 Bilder. Und `enemy_ai_re2_dog.c:2229-2242` protokolliert wörtlich das Gegenteil
als Nutzer-Meldung vom 2026-09-14:
> „man kann die Zombie Hunde erst wieder treffen, sobald sie nach dem schiessen wieder komplett
> stehen. Vorher sind sie unverwundbar. Ist das im Original genauso?" - NEIN

Der Port hat diese Sperre also einmal gehabt (gemessen 120 Bilder statt 15) und sie auf
Nutzer-Beschwerde entfernt. Der Plan baut sie teilweise wieder ein, ohne zu belegen, dass die
neue Nutzer-Aussage denselben Fall meint. Das ist der Punkt, an dem das Urteil kippt.

## 5. Was fehlt: die eine Messung

Ein DuckStation-Mitschnitt von `+0x1D3` **je Bild** an einem echten RE2-Hund über einen
gewöhnlichen Pistolentreffer hinweg (Treffer → Hurt-Kette → Aufstehen), daneben derselbe
Ablauf im Port. Nur das entscheidet, ob der Nutzer den Sprung-/Feuer-Fall (dann ist der Plan
für genau diese zwei Zweige richtig) oder den Pistolen-Fall meint (dann ändert der Plan nichts
und die Ursache liegt woanders — z. B. in der Hurt-Clip-Länge des Ports).

## 6. Urteil

Die Disassembly ist sauber: das Gate @0x80047138/40 ist wirklich unmaskiert, Bit 0x80 ist
wirklich ein Pose-Riegel, und Runde 13 hat mit *„RE2 sperrt ueber ZEIT, NICHT ueber den
Zustand"* die halbe Wahrheit erzählt. **Aber** der Plan ist nicht ausführbar (Schritt 1 schaltet
vier Typen scharf, Schritt 3 will einen), die Risiko-Bilanz zeigt auf den falschen Gegner, eine
gemessene Gegen-Evidenz (`re15_damage.c:3076-3097`) wird beim Zitieren ausgelassen, und für den
vom Nutzer beschriebenen Fall sagt das Dossier selbst, dass der Mechanismus nicht greift.
**Nicht haltbar** — als RE-Befund wertvoll, als Umsetzungsauftrag nicht.
