# Runde 37 Spur O "szene11c0" — Ada/Marvin-Szene ROOM11C0 nach Irons' Tod

Baum: `.claude/worktrees/r37_szene11c0` (Zweig `r37/szene11c0`, Basis 07bb463a).
Auftrag: AUFTRAG.md Nachtrag 11 (woertlich):

> Und die Dialogszene mit Ada/Marvin nach Irons Tod ist auch noch nicht so geil von den Animationen....
>
> Erst schauen sie komplett in die falsche Richtung. Dann drehen sie sich einmal um 180 grad um. Dann dreht sich Ada noch mal um 90 grad um. Zwischendurch steht marvin da, spricht, schaut aber nicht zu ada.... da musst du noch mal ran, das ist noch nicht so gut.

Zuteilung (VERTRAG.md §1.4): Spur O aendert Programm 4 (k_p_11c0) in irons_tod_1150.c an Ort und Stelle (Kommentar
"Runde 37 Spur O"); die uebrigen Programme (Spur H: Nachspawn 1130) und k_msg28 (Nachtrag 10) NICHT anfassen.

## Stand (fortlaufend)

- [x] Pflichtlektuere (AUFTRAG/VERTRAG; Vorrunde L_cut1150.md, K_szene1150.md K2, K_abnahme_0.md §3.2)
- [x] Messung vorher (echte exe, Kette wie integration_r35_cut1150 Lauf A, Bild fuer Bild Gier + Kopf)
- [ ] RE-Belege (Original-Vokabular ROOM11C0, RE2-Dialogszenen/Geraeusch-Reaktion, atan2-Form des Ports)
- [ ] Umsetzung
- [ ] Messung nachher
- [ ] Tests (probes/r37_szene11c0.cmake)

## Messung vorher

### Weg und Werkzeug
* Stand: Basis 07bb463a + NUR der Mess-Haken dieser Spur (engine/src/szene11c0_r37.c `re15_s11c0_mess_takt`, Aufruf
  aus `re15_irons_tod_tick`, env-gegatet `RE15_S11C0_LOG`, kein Spielverhalten). Messkopie `re15_pc_r37o_vor.exe`.
* Weg wie integration_r35_cut1150 Lauf A (Werkzeug `re15_port/tools/r36_k/kette_lauf.sh`): Karte
  `probe_r35_cut1150_karte re15_card.mcr 1130 nach10f0 ersteszene`, CONTINUE in ROOM1130, Aktionstaste an der Tuer ->
  ROOM1150 Szene -> Montage 1130 -> 1040 -> 1030 -> ROOM11C0 bis `RE15_EXIT_AT=498#11C0` (das Programm schneidet nach
  ~500 Bildern zurueck nach 1150; ein EXIT_AT dahinter wird nie erreicht und die Bildnummern-Dumps wuerden von der
  Rueckkehr nach 1150 ueberschrieben — erster Versuch mit 520#11C0 so verworfen).
* Je Bild: `RE15_FRAMEDUMP=0-498/2:f_` (RE15_WINDOW_SCALE=1, 320x240), `RE15_S11C0_LOG=s11c0.log` (Untertitel, je Figur
  Ort/Gier/Clip/Kopf-Flags +0x1b8/Kopf-Zielpunkt +0x160,+0x164/Kopf-Akku/Gier zum Partner), `RE15_NECK_TRACE`
  (skeleton_common.c: geklemmtes lokales Kopfziel tgt, Akku acc, Kopf-Keyframe kf je gezeichnetem Bild).
* Auswertung `re15_port/tools/r37_o/szene_mess.py <lauf>`: Drehabschnitte je Figur (Summe der Gier-Schritte), je
  Untertitel Koerper- und Kopfblick-Abweichung zum Partner. Kopf-Blick (Welt) = Gier + Kopf-Keyframe + Akku (die
  Kopf-FSM addiert den Akku auf den Keyframe-Euler vy des Kopfteils, skeleton_common.c `ay + a->neck_yaw`).
* Ergebnis: `O_belege/o_vor_mess.txt` (Zahlen), `O_belege/o_vor_bilder.txt` (jedes 4. Bild), Kontaktbogen
  `O_belege/o_vor_11c0.png` (F20..F470).

### Zahlen (o_vor_mess.txt)
```
Drehungen Ada:    F45..F64  Gier  475 -> 2763  -158.9 Grad | F84..F94 2763 -> 3679 +80.5 Grad | F384..F394 3679 -> 2763 -80.5 Grad
Drehungen Marvin: F35..F55  Gier  475 -> 2745  -160.5 Grad | F184..F196 -97.9 | F251..F263 +97.9 | F311..F323 -97.9 | F374..F389 +98.3 (Losrennen)
Untertitel: 10 F95..F193, 11 F197..F251, 12 F252..F323, 13 F324..F394, 14 F395..F493
msg 10 Ada  "What was this noise?":  Ada Koerper 0.1 Grad zu Marvin, Kopf auf Marvin | Marvin (Zuhoerer) Koerper 98 Grad weg,
                                     Kopf-Ziel Ada, aber Kopf an der Klemme in 93/99 Bildern -> Blick 35.4 Grad an Ada vorbei
msg 11 Marvin "Yes!....":            Marvin Koerper 0.1 Grad, Blick max 6.8 Grad (Nicken)        | Ada sieht ihn an
msg 12 Marvin "Oh, no, Leon!":       Marvin Koerper 98 Grad weg (2/72 im Kegel), Kopf-Ziel TUER 60/72, Blick 97.9 Grad an Ada vorbei
msg 13 Marvin "I have to help him":  Marvin Koerper bis 160.5 Grad weg (51/71), Kopf frei 20/71 — er dreht sich und rennt los,
                                     waehrend die Zeile noch steht (F374..F394)
msg 14 Ada "Marvin!...":             Ada Koerper zur Tuer gedreht (80.5 Grad), Kopf zur Tuer
```

### Gegen den Wortlaut des Nutzers (Punkt fuer Punkt)
| Nutzer | gemessen |
|---|---|
| "Erst schauen sie komplett in die falsche Richtung." | F6..F35: beide Gier 475 = Ruecken zur Kamera (Kamera-Blick Cut 13 f = (0,597,-0,802); Gier 475 zeigt (0,746,-0,666), Skalarprodukt 0,98). Ada 78 Grad an Marvin vorbei, Marvin 102 Grad an Ada vorbei. Herkunft: Ada = Original-Record ROOM11C0 sub00 @0x01770 `44 00 42 40 .. fb dc 00 00 f5 c7 00 00 db 01` (rot 0x01db = 475: ihre Warte-Haltung fuer Leons Ankunft im Original), Marvin = r35 hat dieselbe 475 uebernommen (OP_EM rot 475). |
| "Dann drehen sie sich einmal um 180 grad um." | Ada F45..F64 -158.9 Grad, Marvin F35..F55 -160.5 Grad (Schritt (1) "beide zum Gebaeude": die Tuer Slot 0 @0x01712 liegt HINTER der Kamera). |
| "Dann dreht sich Ada noch mal um 90 grad um." | Ada F84..F94 +80.5 Grad (Schritt (2) Ada zu Marvin). |
| "Zwischendurch steht marvin da, spricht, schaut aber nicht zu ada" | msg 12 "Oh, no, Leon!": Marvins Koerper 98 Grad weg, Kopf zur Tuer, Blick 97.9 Grad an Ada vorbei (Schritt (4)). Dazu: bei msg 10 (Ada fragt) steht Marvins Kopf an der NPC-Klemme ±0x2c8 (= 62,6 Grad, @0x8011c7a0) — der Koerper zeigt 98 Grad weg, der Kopf erreicht Ada nicht (35 Grad daneben). Marvin dreht sich in 11C0 insgesamt fuenfmal um ~98 Grad hin und her. |

Plc_neck wirkt bei Marvin (Typ 0x40) sichtbar: neck_trace slot=4 hat Bilder mit fl=84 (Modus 1), der Akku laeuft
auf das Ziel (msg 11: Blick 6,8 Grad zu Ada; msg 10: an der Klemme -707/-712); im Bild F130/F190 (o_vor_11c0.png)
ist Marvins Kopf sichtbar zu Ada gedreht, der Koerper zur Kamera. Die Kopfdrehung allein reicht nicht, wenn der
Koerper mehr als 62,6 Grad vom Partner weg zeigt (Klemme).

### Regression?
Nein. Alle vier Beobachtungen sind die gebaute Choreografie der Runde 36 Spur K (K2, Commit 2eb6c38e "11C0-Dialog
einander zugewandt", in v0.8.24 ausgeliefert; Schritte (1) beide zum Gebaeude, (2) Ada zu Marvin, (4) "Oh, no, Leon!"
zum Gebaeude) auf der Grundlage der r35-Fassung (Spur L: beide zum Gebaeude, Anfangs-Gier 475). Der Nutzer schreibt
"noch nicht so geil" (= noch nicht gut), nicht "wieder falsch". Kein Bisect noetig.

## RE-Belege

(folgt)

## Umsetzung

(folgt)

## Messung nachher

(folgt)

## Tests

(folgt)

## OFFEN

(folgt)

## Fuer den Nutzer

(folgt)
