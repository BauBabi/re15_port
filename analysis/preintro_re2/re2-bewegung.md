# RE2-Opening: Bild-BEWEGUNG + Ein-/Ausblend-Rampen (selbst disassembliert)

Anlass: Nutzer-Beobachtung 2026-08-30 — „Es gibt bei den einen Bild auch eine Bewegung
von oben nach unten." Bestätigt: der Element-Renderer `0x801c1a0c` (OPENING.BIN, Basis
0x801bfa18, `datei_offset = addr - 0x801bfa18`) hat drei Bewegungs-/Blend-Mechanismen.
Disassembliert mit einem eigenen MIPS-I-Overlay-Dekoder (scratchpad/opdis.py; verifiziert
gegen den bekannten Skip-Check `sltiu v0,v0,401` @0x801bfb1c).

## 1. Der Element-Zustand ist ein BITFELD (byte0 des 12-B-Records)

Die im Sequenzer gesetzten „Codes" 7/11/23/27/39/43/71/75 zerfallen in Basis + Modifier:

| Bit | Wert | Bedeutung | Beleg |
|---|---|---|---|
| 0 | 1 | Element aktiv/sichtbar | `andi v0,a0,0x1` @0x801c1b1c |
| 1 | 2 | (immer mitgesetzt) | — |
| 2 | 4 | **EINBLENDEN** (Helligkeits-Rampe aufwärts) | `andi v0,a0,0x4` @0x801c1b8c |
| 3 | 8 | **AUSBLENDEN** (Rampe abwärts) | `andi v0,v0,0x8` @0x801c1bdc |
| 4 | 16 | **VERTIKALES WANDERN** (Position Y) | `andi v0,v0,0x10` @0x801c1c28 |
| 5 | 32 | **ZOOM AUF** (Quad wächst, zentriert) | `andi v0,v0,0x20` @0x801c1d78 |
| 6 | 64 | **ZOOM ZU** (Quad schrumpft, zentriert) | `andi v0,v0,0x40` @0x801c1dbc |

⇒ 7 = ein­blenden, 11 = ausblenden, 23/27 = dito **mit vertikalem Wandern**,
39/43 = dito **mit Zoom auf** (Bild wächst), 71/75 = dito **mit Zoom zu** (Bild schrumpft).

## 2. Die Helligkeits-Rampe (das „weiche" Ein-/Ausblenden statt Hartschnitt)

Der Record trägt bei `+6(s4)` (= Record-Byte 8..11) ein **gepacktes RGB-Wort**, das direkt
die Prim-Modulation speist:

- **Einblenden:** `+= 0x00020202` pro Frame (**+2 je Kanal**), Klemme gegen `0x0060605f`
  → auf `0x00606060` gesetzt und Bit 2 gelöscht (`andi 0xf9`).
  Instruktionen: @0x801c1b94-0x801c1bd0 (Typ 0), @0x801c1ce4-0x801c1d20 (Typ 2).
  ⇒ **0 → 0x60 (96) in 48 Frames = 1,6 s**.
- **Ausblenden Typ 0:** `+= 0xfffdfdfe` (**−2 je Kanal**), bei `<= 0` auf 0 und Bit 3
  gelöscht (`andi 0xf7`). @0x801c1be4-0x801c1c0c ⇒ **48 Frames**.
- **Ausblenden Typ 2:** `+= 0xfffefeff` (**−1 je Kanal**) @0x801c1d34-0x801c1d5c
  ⇒ **96 Frames = 3,2 s** (die langsamere Variante).

Prim: Typ 0/2 = Code **0x2E** (`lui a0,0x2e00` @0x801c1c14 / @0x801c1d64) = POLY_FT4
**semi-transparent**, Größenfeld 160; Typ 1 = Code **0x2C** (opak, @0x801c1c9c) mit
FESTER Helligkeit `0x00808080` (`lui/ori` @0x801c1c5c-64) — Typ 1 blendet also nicht,
sondern steht (das ist die „harte" Schicht, z. B. Text/Logo).

## 3. Die BEWEGUNG (Nutzer-Beobachtung) — zwei globale Schritt-Takte

Zwei Zähler laufen im Renderer-Kopf und erzeugen je einen Schritt-Impuls:

| Zähler | Reload | Ausgabe-Zelle | Wert | Beleg |
|---|---|---|---|---|
| `0x801c2ee0` | **10** | `0x801c2edc` | 0, **alle 11 Frames** −1 | @0x801c1a0c-0x801c1a78 |
| `0x801c2ee2` | **3** | `0x801c2ede` | 0, **alle 4 Frames** −1 | @0x801c1a78-0x801c1abc |

Der Zähler läuft `N → 0` und feuert **bei** 0 (dann Reload N) — die Periode ist also
**N+1** Frames, nicht N.

**(a) Vertikales Wandern (Bit 4, Typ 0):** @0x801c1c34-0x801c1c48
```
lhu v0,4(s4)          ; Element-Y (Bildschirmposition)
lhu v1,0x801c2edc     ; Schritt-Impuls (0 oder -1)
addu v0,v0,v1
sh   v0,4(s4)
```
⇒ **1 Pixel je 11 Frames** — ein sehr langsames, kontinuierliches Wandern der Bildposition
über die ganze Anzeigedauer. Das ist die vom Nutzer gesehene Bewegung.

⚠️ **Das gepannte Bild ist ABSICHTLICH ÜBERGROSS:** RE2 setzt dafür eine Vierergruppe aus
4 Quads à 160×160 = **320×320** bei Y = 10/170 ein und fährt sie im 240-px-Fenster nach oben
durch — **80 px Überhang**. Ohne diesen Überhang liefe schwarzer Rand ins Bild.

**(b) Zoom (Bits 5/6, Typ 2):** @0x801c1d84-0x801c1df4
```
lh   v0,0x801c2ede    ; Schritt-Impuls (0 oder -1), Takt 3 Frames
lbu  v1,8(s5) / 9(s5) ; Quad-BREITE / -HÖHE
sll  v0,v0,1          ; Schrittweite = 2
subu v1,v1,v0   (Bit 5: Größe -= (-2) ⇒ +2 = ZOOM AUF)
addu v1,v1,v0   (Bit 6: Größe += (-2) ⇒ -2 = ZOOM ZU)
```
Da der Impuls **−1** ist, ergibt `sll v0,v0,1` = **−2**: Bit 5 rechnet `w -= (−2)` = **+2**
(Quad wächst = hineinfahren), Bit 6 rechnet `w += (−2)` = **−2** (Quad schrumpft =
herausfahren) — je **alle 4 Frames**, auf **Breite UND Höhe** gleichzeitig.
Die zwei tatsächlich gefahrenen Zooms: Element 10 fährt 200 → ~50 px zu (Zustand 71),
Element 19 fährt 20 → ~198 px auf (Zustand 39) — eine symmetrische Klammer um die Sequenz.
Anschließend wird das Quad **zentriert**: @0x801c1df8-0x801c1e24
```
a2 = (320 - breite) / 2      ; x-Zentrum
fp = (240 - hoehe)  / 2      ; y-Zentrum
```
⇒ ein sauberer, um die Bildmitte zentrierter **Ken-Burns-Zoom mit ~0,67 px/Frame**.

## 4. Was das für die Übernahme auf unsere ROOM1240-Montage bedeutet

Unsere 9 Standbilder wechseln heute per **1-Frame-Hartschnitt** (byte-true, aber „karg").
Die vier RE2-Bausteine sind direkt übertragbar, weil unsere BG-Blit-Kette bereits einen
320×240-RGBA-Puffer je Bild hält (`s_bg_cache` in `platform/pc/src/bg_pc.c`):

1. **Ein-/Ausblenden** über eine Helligkeits-Rampe 0 → 0x60 → 0 mit **+2/−2 je Frame**
   (48 Frames je Richtung; die langsame Variante −1/Frame = 96 Frames für ruhige Bilder).
   Der Blend ist **ADDITIV**, nicht Alpha: TPAGE trägt +160 = 0xA0 ⇒ ABR=1 (B+F)
   (@0x801c1ec4/@0x801c1ec8) — die Bilder „glühen aus dem Schwarz auf".
   Da alle drei Kanäle gleich rampen (R=G=B), ist es eine reine Helligkeitsrampe.
2. **Vertikales Wandern** mit **1 px je 11 Frames**, mit 80 px Überhang als Sockel.
3. **Zentrierter Zoom** mit **2 px je 4 Frames** auf Breite und Höhe.
4. Optional der **TV-Rausch-Overlay** (OPEN07.TIM, 128×256 Weißrauschen, per Mode-3-Handler)
   als Übergangs-/Störschicht.

Alles sind PRÄSENTATIONS-Parameter mit RE2-Beleg; unsere Bild-Reihenfolge, Standzeiten,
Texte und der Skip bleiben byte-true RE1.5 (Inventar: `re15-montage-inventar.md`).

## 5. ⛔ Die Bewegung laeuft UEBER den Bildwechsel weiter (Korrektur 2026-08-30)

Nutzer-Report: „3. Bild ist korrekt das es raus zoomed, aber es muss bis in das naechste
Standbild zoomen. Bei dir wird das Logo ploetzlich wieder gross." — bestaetigt am Code.

**Der Zustand ist ein BITFELD, und beim Uebergang kippt nur das BLEND-Bit:**

| Uebergang | Bits vorher | Bits nachher | was bleibt |
|---|---|---|---|
| 71 -> 75 | 0x47 = aktiv+rampe+**einblenden**+Zoom-zu | 0x4B = aktiv+rampe+**ausblenden**+Zoom-zu | **Zoom-Bit 0x40** |
| 39 -> 43 | 0x27 = …+**einblenden**+Zoom-auf | 0x2B = …+**ausblenden**+Zoom-auf | **Zoom-Bit 0x20** |
| 23 -> 27 | 0x17 = …+**einblenden**+Wandern | 0x1B = …+**ausblenden**+Wandern | **Wander-Bit 0x10** |

**Und die Timeline laesst das naechste Bild dazwischen erscheinen** (§3.5 in
`re2-sequenzer.md`):

```
Phase 2, t=905 : {10} -> 71     Logo erscheint und zoomt ZU
Phase 3, t=94  : {11..14} -> 7  DAS NAECHSTE STANDBILD ist da — das Logo zoomt weiter
Phase 3, t=204 : {10} -> 75     erst JETZT blendet das Logo aus … und zoomt dabei WEITER
```

Zwischen dem Erscheinen des naechsten Standbildes und dem Ausblenden des Logos liegen
**110 Frames**, in denen beide gleichzeitig sichtbar sind und das Logo durchgehend zoomt.
Dasselbe Muster bei {19}: Phase 4 t=580 -> 39, Phase 5 t=96 naechstes Bild, t=261 -> 43.

**Konsequenz fuer den Port:** Es reicht NICHT, „dem aktuellen Bild" eine Bewegung zu
geben. Jede Bild-EBENE traegt ihre eigene Bewegung und fuehrt sie ueber den Bildwechsel
hinaus fort; erst wenn sie ausgeblendet ist, verschwindet sie. Umgesetzt als
Zwei-Ebenen-Modell in `re15_montage_fx.c` (`fx_layer_t cur, prev` — beide werden
getickt, beide zeichnen mit ihrem eigenen Pan/Zoom).

## 6. Die Zuordnung auf unsere neun Bilder

| Cut | Motiv | Bewegung | RE2-Vorbild |
|---|---|---|---|
| 1 | Zombie-Kopf | **Standbild** | {0,1}/{6..9}, Zustand 7 |
| 2 | T-Virus-Mikroskopbild | **wandert** von oben nach unten | {2..5}, 23 -> 27 |
| 3 | S.T.A.R.S.-Abzeichen | **zoomt heraus**, laeuft weiter | {10}, 71 -> 75 |
| 4 | S.T.A.R.S. vor dem Villentor | Standbild | {11..14}, 7 -> 11 |
| 5 | Helikopter ueber der Explosion | Standbild | dito |
| 6 | Gruppe vor dem Helikopter | Standbild | dito |
| 7 | Umbrella-Laborinneres | Standbild | {20..23}, 7 -> 11 |
| 8 | Umbrella-Logo | **zoomt hinein**, laeuft weiter | {19}, 39 -> 43 |
