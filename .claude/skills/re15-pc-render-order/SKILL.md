---
name: re15-pc-render-order
description: Das Compositing-/Z-Order-Modell des PC-Software-Renderers (re15_port/platform/pc/src/render_pc.c) — welche Layer end_frame in welcher Reihenfolge übereinander legt (Framebuffer → Shadows → 3D-Tri-Queue → Letterbox → Text-Overlay → Fade), wo jeder Layer lebt, und wie man „Element X wird von Y verdeckt / falsche Zeichen-Reihenfolge"-Bugs diagnostiziert und byte-true zum PSX-OT fixt. Verwenden bei jedem GUI-/Präsentations-Bug im PC-Port (HUD/Untertitel/Inventar/Mesh/Letterbox/Effekt/Waffe verdeckt oder in falscher Tiefe), oder bevor ein neues Overlay-/HUD-Element hinzugefügt wird.
---

# RE1.5 PC-Renderer — Compositing- & Z-Order-Modell

Der PC-Port hat **keinen Tiefenpuffer**. Wie auf der PSX (OT = Ordering Table, back-to-front) ergibt sich
die Tiefe allein aus der **Zeichen-Reihenfolge** — was später gezeichnet wird, liegt oben. Fast jeder
„X ist hinter Y / X verdeckt Y / falsche Reihenfolge"-Bug im GUI ist ein Reihenfolge-Bug in **einer** Funktion:
`re15_render_end_frame()` in `re15_port/platform/pc/src/render_pc.c`. Byte-true-Ziel: die Layer-Reihenfolge
muss der PSX-OT-Tiefe der entsprechenden Primitive entsprechen (Untertitel VOR Letterbox = OT-näher; etc.).

## Die zwei Zeichen-Ebenen (das muss man zuerst verstehen)

1. **Software-Framebuffer** `s_framebuffer` (uint32 RGBA, `SCREEN_XRES×SCREEN_YRES`). Alles, was per
   **`put_pixel`** landet, geht hierhin: `re15_render_tile`, `re15_bg_blit` (BG), Sprites, 2D-Primitive.
   **Der `z`-Parameter von `re15_render_tile` wird IGNORIERT** (`(void)z;`) — es zählt NUR die Aufruf-Reihenfolge
   im Frame. In `begin_frame` wird `s_framebuffer` (und das Text-Overlay) geleert.
2. **SDL-Renderer-Ebene** (GPU): 3D-Mesh-Dreiecke, Overlays, Balken. Diese werden in `end_frame` ÜBER den
   (als Textur hochgeladenen) Framebuffer gelegt. Deshalb liegt **jedes SDL-gezeichnete Element grundsätzlich
   über allem im Framebuffer** — die häufigste Falle.

## Der end_frame-Layer-Stack (unten → oben)

Reihenfolge in `re15_render_end_frame()` (per Symbol benannt, nicht per Zeile — grep die Anker):

| # | Layer | Wie gezeichnet | Anker (grep) |
|---|-------|----------------|--------------|
| 1 | **Framebuffer** (BG + 2D-Tiles + Sprites + HUD-Tiles) | `SDL_UpdateTexture(s_texture,…)` + `SDL_RenderCopy` | `s_texture` |
| 2 | **Character-Shadows** (subtraktive Blobs) | `SDL_RenderGeometry(s_shadow_tex,…)` | `s_shadow_quad_count` |
| 3 | **3D-Meshes** (Actors/Props, textured tris) + sprite.pri-Overdraw | `SDL_RenderGeometry(s_tim_slots[…])` aus der Queue `s_textri_queue`/`s_textri_count`; PRI via `SDL_RenderCopy(s_pri_atlas_tex,…)` | `s_textri_count`, `re15_render_textured_tri` |
| 4 | **Cinematic-Letterbox-Balken** (schwarz, oben+unten) | `SDL_RenderFillRect` | `s_letterbox_h`, `re15_render_pc_set_letterbox` |
| 5 | **Untertitel-/Text-Overlay** | `SDL_RenderCopy(s_text_overlay_tex,…)` | `s_text_overlay_used`, `re15_render_pc_msg_text`, `re15_debug_text` |
| 6 | **Fade-Overlay** (Cinematic-Fade-in, schwarz mit Alpha) | `SDL_RenderFillRect` (BLEND) | `s_fade_alpha`, `re15_render_pc_set_fade` |
| — | **Present** | `SDL_RenderPresent` | |

Merksätze:
- **3D-Meshes (Layer 3) liegen IMMER über dem Framebuffer (Layer 1).** Ein Vollbild-Framebuffer-Overlay
  (z.B. das Inventar, per `re15_render_tile` gezeichnet) wird von den Meshes verdeckt, solange die Tri-Queue
  nicht verworfen wird.
- **Letterbox (4) liegt jetzt UNTER dem Text-Overlay (5)** → Cinematic-Untertitel sitzt AUF dem schwarzen
  Balken (byte-true: die Letterbox-POLY_F4-Quads `FUN_80020f8c` stehen im OT hinter den Text-Primitiven).
- **Text-Overlay (5)** ist eine EIGENE Vollbild-RGBA-Textur (`s_text_overlay`, in `begin_frame` transparent
  geleert, `s_text_overlay_used` markiert „diesen Frame beschrieben"). Untertitel gehen dorthin, NICHT in den
  Framebuffer — damit sie über den 3D-Actor-Füßen liegen.

## Diagnose-Prozedur „X wird von Y verdeckt"

1. **Layer von X und Y bestimmen** (Tabelle oben). Grep den Zeichen-Aufruf von X: schreibt er per `put_pixel`
   → Layer 1 (Framebuffer); per `SDL_RenderGeometry` → Layer 3; ins `s_text_overlay` → Layer 5; FillRect → 4/6.
2. **Regel:** X muss NACH Y in `end_frame` gezeichnet werden, um oben zu liegen. Steht X vor Y → **umsortieren**.
3. **Sonderfall Vollbild-Framebuffer-Overlay hinter Meshes:** die Tri-Queue vor dem Present verwerfen
   (`re15_render_pc_clear_textris()`, setzt `s_textri_count=0`) — so präsentiert der Framebuffer allein.
4. **Byte-true prüfen:** entspricht die neue Reihenfolge der PSX-OT-Tiefe der Primitive? (Original/`shots/`
   vergleichen; bei Cinematic-Elementen: Text > Letterbox > 3D > BG.)
5. Bau + `ctest` (Render-Reihenfolge bricht keine Unit-Tests) — **die GUI selbst ist im Agenten-Env NICHT
   verifizierbar** (SDL braucht Display); der visuelle Gegentest bleibt beim Nutzer.

## Zwei gelöste Fälle (2026-07-02, Vorlage)

- **Inventar hinter 3D-Meshes** (Commit fff4b8d5): das Inventar-Overlay (Layer 1, `re15_render_tile`) wurde
  von den Actor-Meshes (Layer 3) verdeckt. Fix: `re15_render_pc_clear_textris()` (neu in render_pc.c) im
  Main-Loop **vor** `re15_render_end_frame()` aufrufen, wenn `re15_menu_is_open()` → Meshes verworfen,
  Inventar präsentiert allein.
- **Untertitel hinter Letterbox-Balken** (Commit 86dfd39c, #1B): die Balken (Layer 4) wurden NACH dem
  Text-Overlay (Layer 5) gezeichnet → verdeckten den Text. Fix: in `end_frame` den Letterbox-Block VOR den
  Text-Overlay-Block ziehen → Untertitel auf dem Balken (byte-true zum OT).

## Gotchas

- **`re15_pc.exe` sperrt die eigene Datei, solange sie läuft** → ein Build nach einem Exe-Lauf scheitert mit
  `cannot open output file … re15_pc.exe: Permission denied` (KEIN Code-Fehler). Vor dem Build:
  `taskkill //F //IM re15_pc.exe 2>/dev/null; true`.
- **Ninja rebaut manchmal nicht nach `.inc`/Header-Änderungen** → betroffene `.c` `touch`en, um den Rebuild zu erzwingen.
- Neuer HUD-/Overlay-Layer? Entscheide bewusst, in welche Ebene (Framebuffer für 2D-Tiles unter den Meshes;
  `s_text_overlay` oder ein eigener FillRect/Copy nach den Meshes für „immer oben") — und ob Cinematic-Balken
  ihn clippen sollen (dann VOR die Balken; sonst danach).
