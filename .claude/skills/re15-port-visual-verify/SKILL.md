---
name: re15-port-visual-verify
description: Zeigt, was der NUTZER im PC-Port tatsächlich auf dem Bildschirm sieht — via ffmpeg-gdigrab-Capture des echten Fensters, im echten Flow (plain exe, accelerated Renderer). ZWINGEND verwenden, wenn der Nutzer einen VISUELLEN Fehler meldet (Textur/Farbe/Pose/fehlt/schwarz) oder wenn ein "Fix" verifiziert werden soll — weil `RE15_AUTOSHOT` und `RE15_SOFTWARE_RENDER` visuelle Bugs NACHWEISLICH MASKIEREN und dadurch kaputte Builds als "gefixt" erscheinen lassen.
---

# Port-Verifikation: das SEHEN, was der Nutzer sieht

**Warum dieser Skill existiert (2026-07-28, Waffen-Textur-Bug):** Ich habe dem Nutzer **fünf Mal hintereinander** "ist gefixt" gemeldet, während bei ihm die Waffen unverändert untexturiert waren. Ursache war NICHT der Code, sondern **meine Verifikationsmethode**. Erst als ich das echte Fenster mit `ffmpeg gdigrab` aufnahm, sah ich den Fehler sofort. Diese Regeln sind teuer bezahlt — nicht abkürzen.

## ⛔ Die zwei Verifikations-FALLEN (beide haben mich getäuscht)

1. **`RE15_AUTOSHOT` / der eingebaute Screenshot MASKIERT Textur-Bugs.**
   Der Screenshot-Pfad macht pro Frame ein Full-Frame-Readback (`SDL_RenderReadPixels`). Das **erzwingt einen GPU-Flush**, wodurch verworfene/verspätete Textur-Updates plötzlich "sticky" werden. Ein Build, der beim Nutzer kaputt ist, sieht im Autoshot korrekt aus.
   → Für Textur-/Upload-/Timing-Fragen ist Autoshot **UNZULÄSSIG**.
2. **`RE15_SOFTWARE_RENDER=1` ist ein ANDERER Renderer als der des Nutzers.**
   Der Nutzer läuft mit `SDL_RENDERER_ACCELERATED` (echte GPU). Die Software-Rasterisierung zeigte einen Platzhalter als plausible graue Form → ich hielt es für "texturiert".
   → Software-Render nur für Geometrie-/Pose-Fragen, NIE für Textur/Upload/Treiber-Verhalten.

**Merksatz:** *Wenn der Nutzer sagt „ist immer noch falsch", ist ZUERST meine Messmethode verdächtig, nicht seine Beobachtung.*

## ✅ Die Methode die FUNKTIONIERT: gdigrab auf das echte Fenster

Fenstertitel: **`RE1.5 Rebuilt — PC`** (Em-Dash!). ffmpeg liegt unter `/c/ProgramData/chocolatey/bin/ffmpeg`.

```bash
cd C:/workspace/git/reAi_v2/re15_port/build/platform/pc
export PATH="/c/msys64/mingw64/bin:$PATH"
OUT=C:/workspace/git/reAi_v2/shots

# 1) Spiel starten wie der NUTZER (plain exe, KEIN SOFTWARE_RENDER, KEIN AUTOSHOT).
#    Menü-Navigation per Input-Script ist erlaubt — sie ändert das Rendering nicht.
RE15_NO_INTRO=1 RE15_INPUT_SCRIPT="W3,D1,W2,A1,W2,A1,W25" RE15_INPUT_SCRIPT_START=1 \
./re15_pc.exe >/dev/null 2>&1 &
GP=$!
sleep 17                       # bis Gameplay wirklich steht (Menüs + Raumload)

# 2) DAS ECHTE FENSTER abgreifen (kein Readback -> nichts wird maskiert)
ffmpeg -hide_banner -loglevel error -f gdigrab -framerate 1 \
       -i "title=RE1.5 Rebuilt — PC" -frames:v 1 "$OUT/_real.png"

kill $GP 2>/dev/null; wait 2>/dev/null
```

⚠️ **Pfad-Falle:** `/tmp/...` ist MSYS-intern und für das native Python/PIL **unsichtbar**. Capture-Ziele immer als Windows-Pfad (`C:/workspace/.../shots/...`).

## Den Nutzer-Flow EXAKT reproduzieren (nicht Raum-Sprung!)

Ein Raum-Sprung überspringt Boot-/Load-Reihenfolge und versteckt genau die Bugs, die dabei entstehen (Upload-Reihenfolge, Latches). `RE15_START_ROOM` gibt es deshalb seit 2026-08-01 **nicht mehr** — es bootete an `re15_room_apply_pending` vorbei. Muss es doch ein Sprung sein, dann nur über das Debug-Menü (`RE15_DEBUG_JUMP=<hexraum>@<frame>`), das durch dieselbe Install-Sequenz läuft wie eine Tür. Der echte Weg bleibt aber:

```bash
# Title -> LOAD GAME -> Slot 1, mit ECHTEM Pad-Input:
RE15_INPUT_SCRIPT="W3,D1,W2,A1,W2,A1,W25"   # W=warten, D=Down, A=SQUARE(=Confirm)
```
Debug-Abkürzungen (`RE15_CONTINUE_TEST`, `RE15_CARD_AUTO`) gehen auch, sind aber ein *anderer* Pfad — für die Schluss-Verifikation echten Input nehmen.

**Den Save des Nutzers auslesen** (welcher Raum/Waffe/Position?) — `re15_card.mcr` im build-dir:
```python
import struct
b=open("re15_port/build/platform/pc/re15_card.mcr","rb").read()
i=b.find(struct.pack('<I',0x35314552))              # RE15_SAVE_MAGIC "RE15"
room,cnt   = struct.unpack_from('<HH',b,i+0x18)
rot,hp,st  = struct.unpack_from('<hhH',b,i+0x1c)
char,eqsl,weapon,cam = struct.unpack_from('<BBBB',b,i+0x22)
# -> room=0x1150 char=0 weapon=1(Messer) cam=2 ...
```

## Das gesuchte Objekt ZUERST auf dem Bildschirm LOKALISIEREN

**Der zweitteuerste Fehler:** Ich habe stundenlang die falschen Pixel gemessen (eine Körper-Geometrie am Gürtel statt der Waffe in der Hand) und daraus falsche Schlüsse gezogen.

Zwei zuverlässige Lokalisierungs-Techniken:

**(a) Prim-Tint** — die Prims genau dieses Meshes einfärben:
```c
/* render_pc.c: globales Flag */          int s_wpn_draw_mark = 0;
/* main.c: um den fraglichen Draw */      s_wpn_draw_mark = 1; ... s_wpn_draw_mark = 0;
/* im _lit-Enqueue: */
if (s_wpn_draw_mark && getenv("RE15_TINT")) { r0=g0=b0=…=255,0,255; }   /* magenta */
```
→ Screenshot per gdigrab, Magenta-Pixel suchen → exakte Bildschirm-Bbox des Meshes.

**(b) Kill-Switch-A/B** — den Draw per env abschalten und Bilder vergleichen:
```c
if (!getenv("RE15_NOWPN") && vis && …) { …draw… }
```
→ Verschwindet die verdächtige Form NICHT, gehört sie zu einem ANDEREN Mesh.
⚠️ Zwei Läufe unterscheiden sich durch Idle-Animation → ein reiner Bild-Diff ist verrauscht; (a) ist präziser.

## Wo sampelt ein Mesh WIRKLICH? (statt UVs aus der Datei zu raten)

Datei-Parses per Regex/Heuristik lieferten mir falsche UV-Bänder. **Wahrheit ist der Render-Pfad** — im Enqueue (`re15_render_textured_tri_lit`, render_pc.c) loggen:
```c
if (s_wpn_draw_mark && getenv("RE15_UVDBG")) {   /* min/max über alle Prims akkumulieren */
    … fprintf(stderr,"[WPNUV] u[%d,%d] v[%d,%d] -> texY[%d,%d] voff=%d slot=%d\n",
              umin,umax,vmin,vmax, vmin+v_offset, vmax+v_offset, v_offset, s_active_slot);
}
```
→ ergab: Waffen-Mesh sampelt **X[200,255] Y[364,507]** von slot 0 (Hand-Band + Waffen-Band).
⚠️ NICHT nach `clut`-Wert filtern — Body-Prims teilen denselben clut. Nach dem Draw-Mark filtern.

**Region-Probe** (sampelt das Mesh eine bestimmte Textur-Region?): die Region im Composite mit Magenta übermalen (`RE15_MAGBAND="y0,y1"`), gdigrab, und die **korrekten** Objekt-Pixel prüfen. Voll-Textur-Magenta als Kontrolle = "wird diese Textur überhaupt benutzt?".

## Byte-true Anker aus dem Savestate holen (statt VRAM-Layout zu raten)

Die VRAM-Position eines Sprites findet man, indem man die **Datei-Bytes im Savestate-Blob sucht**:
```python
import re15_ss, struct
r=re15_ss.Ram(r"C:/Users/…/HASH-957757946319438E_1.sav")
row0 = plw[poff:poff+56]                       # erste Sprite-Zeile aus der PLW dir[3]
i = r.blob.find(bytes(row0), r.vram_base, r.vram_base+1024*512*2)
off=(i-r.vram_base)//2 ;  print(off%1024, off//1024)    # -> (548,480) halfwords
```
→ Waffen-Sprite liegt bei VRAM-Halfword **(548,480)** = Texel (1096,480) = Page-8-Offset **u=72** = exakt `u_min` des Meshes ⇒ die Port-Platzierung (200,480) ist byte-true bestätigt.
Ebenso die **CLUT** verifizieren: Sprite mit VRAM-CLUT-Kandidaten dekodieren und vergleichen — (0,482) ergab dasselbe Bild wie die eigene dir[3]-Palette.
⚠️ `re15_ss.Ram(path)` nimmt einen **Pfad**, keine Bytes. Der VRAM-**Region**-Decode kann verzerrt aussehen ([[reai-v2-savestate-vram-base]]) — die **Byte-Suche** ist trotzdem zuverlässig.

## Code-Muster das den Bug verursachte (danach immer prüfen)

Ein einmaliger Init/Composite, der bei Fehlschlag trotzdem "erledigt" latcht, **wiederholt sich nie**:
```c
/* FALSCH */ if (key != s_key) { s_key = key; try_apply(); }
/* RICHTIG */ if (key != s_key || gen != s_gen) {
                 if (try_apply())            /* nur bei ECHTEM Erfolg latchen */
                     { s_key = key; s_gen = gen; }   /* sonst nächster Frame erneut */
             }
```
Plus **Generation-Counter**: wird die Zielressource (z.B. TIM-Slot 0) neu hochgeladen, muss die Ableitung neu gebaut werden (`re15_render_pc_slot0_generation()`).

Ebenso: **`SDL_UpdateTexture` auf eine bereits gesampelte `STATIC`-Textur wird von echten GL-Treibern verworfen.** Statt Runtime-Sub-Rect-Updates die Textur **neu erzeugen** (`SDL_DestroyTexture` + `SDL_CreateTexture` + Full-Upload) — derselbe Pfad wie der normale Asset-Upload, der beim Nutzer nachweislich rendert.

## Checkliste vor jedem "ist gefixt"

1. [ ] **Clean rebuild** (`cmake --build … --clean-first`) + Zeitstempel der `.exe` nennen.
2. [ ] Fix im Source verifiziert (`grep`), Symbol im Binary vorhanden.
3. [ ] Lauf **ohne** `RE15_SOFTWARE_RENDER`, **ohne** `RE15_AUTOSHOT`.
4. [ ] Flow = der des Nutzers (echter Load/Weg, kein Raum-Sprung; wenn unvermeidbar, dann nur Debug-Menü).
5. [ ] **gdigrab**-Capture des echten Fensters angesehen.
6. [ ] Objekt vorher **lokalisiert** (Tint/Kill-Switch) — die richtigen Pixel gemessen.
7. [ ] Gegen das ORIGINAL verglichen (DuckStation-Savestate-Framebuffer, `re15_ss.py <sav> <png>`).
8. [ ] `ctest` grün.

Bleibt es kaputt: **eine Logzeile einbauen, die der Nutzer zurückmeldet** (hat der Composite/Init angewandt: ja/nein) — statt eine weitere Runde zu raten.

Verwandt: [[reai-v2-weapon-render]] (der konkrete Bug + alle @0x-Anker), `re15-savestate-ghidra` (Savestate lesen), `re15-parity-verify`, [[reai-v2-playthrough-not-jumpin]] (Durchlauf statt Raum-Sprung).
