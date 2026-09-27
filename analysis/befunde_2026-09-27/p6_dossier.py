import io

p = "analysis/befunde_2026-09-27/elza-zweig.md"
s = io.open(p, encoding="utf-8", newline=None).read()
alt = """## 3. MESSUNGEN

(folgt — Bau laeuft)
"""
neu = """## 3. MESSUNGEN

Werkzeug: `analysis/befunde_2026-09-27/elza_run.sh` (ein Vollstart, Auswahlschirm per
`RE15_PSELECT_AUTO`, mit `RE15_PSELECT_AUTO_SWITCH` = rechts = Elza) und
`elza_cmp.py` (Pixelvergleich der PPM-Reihen). Beide Staende wurden im SELBEN
Arbeitsbaum gebaut: erst der Stand VOR der Aenderung (Commit 617fef68 in re15_port/),
Lauf, dann der Stand danach (04e8694d), Lauf.

### 3.1 DER WICHTIGSTE RIEGEL: Leon vorher == Leon nachher

| Messgroesse | Leon VORHER | Leon NACHHER |
|---|---|---|
| Auswahl | `pselect done ch=0` | `pselect done ch=0` |
| Startraum | `STAGE1/ROOM1240.RDT (163744 bytes)` | `STAGE1/ROOM1240.RDT (163744 bytes)` |
| Spieler-Spawn | `(-26214,0,-3861) yaw=0` | `(-26214,0,-3861) yaw=0` |
| Koerper-TIM | `384x256 bpp=8 clut=1` | `384x256 bpp=8 clut=1` |
| Koerper-Mesh | `17 meshes` | `17 meshes` |
| Skelett | `PL00: 15 bones, 24 clips, 712 keyframes` | `PL00: 15 bones, 24 clips, 712 keyframes` |
| W01-Spur | `PL00W01: 15 bones, 14 clips, 222 kf` | `PL00W01: 15 bones, 14 clips, 222 kf` |
| W03-Spur | `PL00W03: 15 bones, 14 clips, 248 kf` | `PL00W03: 15 bones, 14 clips, 248 kf` |
| Waffenmeshes | `21/21 PL00W**` | `21/21 PL00W**` |
| Waffenbaenke | `21/21 aus PL00W**.PLW` | `21/21 aus PL00W**.PLW` |
| Raumkette im Lauf | room1240 -> room1170 | room1240 -> room1170 |
| Containerschnitte | — | **keine** (`[pl-part]` kommt im Log nicht vor) |

Die letzte Zeile ist der Grund, warum das bitgenau sein MUSS: fuer Leon liegen alle
Teildateien vor, der neue Helfer nimmt sie und kommt nie zum Schnitt — er liest
buchstaeblich dieselben Dateien wie vorher.

**Bildvergleich, 7 Bilder (Nr. 200/400/600/800/1000/1200/1400):**

```
000200  abweichende Pixel 0 von 691200
000400  abweichende Pixel 0 von 691200
000600  abweichende Pixel 0 von 691200
000800  abweichende Pixel 0 von 691200
001000  abweichende Pixel 0 von 691200
001200  abweichende Pixel 0 von 691200
001400  abweichende Pixel 0 von 691200
SUMME abweichende Pixel: 0
```

### 3.2 Die Wahl hat jetzt Folgen

| Messgroesse | Leon (ch=0) | Elza (ch=1) |
|---|---|---|
| Charakter-Byte | `character=0, Elza-Bit=0` | `character=4, Elza-Bit=1` |
| Startraum | `STAGE1/ROOM1240.RDT (163744 B)` | **`STAGE1/ROOM1241.RDT (163748 B)`** |
| Folgeraum (Tuer der Montage) | **room1170** | **room1031** |
| Koerper-Mesh | 17 meshes | **21 meshes** |
| Skelett | `PL00: 15 bones, 24 clips, 712 kf` | **`PL04: 15 bones, 24 clips, 705 kf`** |
| W01-Spur | `PL00W01: 14 clips, 222 kf` | **`PL04W01: 14 clips, 217 kf`** |
| W03-Spur | `PL00W03: 14 clips, 248 kf` | **`PL04W03: 14 clips, 243 kf`** |
| Waffenmeshes / -baenke | 21/21 PL00W\\*\\* | **21/21 PL04W\\*\\*** |
| Containerschnitte | keine | `PL04W01.EDD (+8, 1336 B)`, `PL04W01.EMR (+1344, 17368 B)`, `PL04.EDD (+8, 3156 B)` |

Die drei Schnitte sind genau die drei Teildateien, die im entpackten Baum fehlen —
gemessen, nicht angenommen.

**Bildvergleich Leon gegen Elza, dieselben 7 Bildnummern:**

```
000200  abweichende Pixel 677349 von 691200
000400  abweichende Pixel 687087 von 691200
000600  abweichende Pixel 691147 von 691200
000800  abweichende Pixel 691147 von 691200
001000  abweichende Pixel 690979 von 691200
001200  abweichende Pixel 549591 von 691200
001400  abweichende Pixel 554733 von 691200
SUMME abweichende Pixel: 4542033
```

**Vorher stand hier 0 von 691200 in allen fuenf verglichenen Bildern**
(elza-portzustand.md). Das ist die eigentliche Zahl dieser Runde.

### 3.3 Der Tuer-Uebergang bleibt ungerade

Der Riegel verlangt ausdruecklich, dass der Versatz nicht nur beim Start greift.
Gemessen im Elza-Vollstart:

```
[boot] room RDT: STAGE1/ROOM1241.RDT (163748 bytes)
[room] PC loaded room1031.rdt (219120 bytes)
```

Der Uebergang laeuft ueber den Door_aot_set der Montage, also ueber
`aot_common.c:572-575`, der die Variante aus `g_current_room_id & 0x000F`
mitschleppt. Leon im selben Aufbau: room1240 -> room1170, beide gerade.

### 3.4 Sichtpruefung

`analysis/befunde_2026-09-27/elza_zweig_bilder/`

| Bild | was darauf zu sehen ist |
|---|---|
| `nachher_leon_f1400.png` | Leon auf dem Helipad (ROOM1170), blaue R.P.D.-Uniform, PL00 |
| `elza_lobby_f4000.png` | **Elza in der Lobby (ROOM1031)** vor dem Drehkreuz, PL04 — anderes Modell (21 statt 17 Meshes), andere Haarfarbe, andere Silhouette |
| `nachher_elza_f1400.png` | Elzas Vorspann-Montage (ROOM1241) laeuft zu diesem Zeitpunkt noch (Umbrella-Blende) — ihre Montage ist laenger als Leons |

### 3.5 Testsuite

(folgt)
"""
assert alt in s
s = s.replace(alt, neu)
io.open(p, "w", encoding="utf-8", newline="\n").write(s)
print("ok")
