# AUDIT — Sichtbare Hoehe beim Schrot-Bauch-HURT des RE2-Zombies (port-y-audit)

Nutzer 2026-09-12: „Beim Schuss mit Schrotflinte Richtung Bauch der Zombies taumeln sie
manchmal merkwuerdig, gehen dann mit Animation unterhalb des Bodens taumeln weiter und
stehen dann wieder." — NICHT-toedlich, RE2-Flavor, W8 (M870) -> RE2-Zeile 7, Zone RUMPF/TIEF.

Nur statische Analyse (Read/Grep). Quellen: Port `enemy_ai_re2_zombie.c`,
`enemy_ai_common.c`, `re2_ems.c`, `skeleton_common.c`, `anim_select_common.c`,
`re15_damage.c`, `platform/pc/main.c`; alle @0x-Adressen sind die bereits im Port
zitierten EMZ0.BIN-/PSX.EXE-Belege (verifizierte Kommentar-Zitate, hier gegengelesen).
Vorbefunde: torso-schrot.md, lobby-aufstehen.md (Runde 5), abtrennen.md.

---

## 1. Der Pfad: Treffer -> Zeile 7 -> Handler (Zustandswort-Folge)

**Stempel** (re15_damage.c): RE1.5-Hitscan -> `re15_re2_stamp_hit`:
Spalte `+0x1D2 = (liegt || elev<0) ? 0 : 1` (re15_damage.c:1869; Original-Rechnung
@0x80047294-0x80047334, Zombie ohne Kopf-Bit: HOCH/EBEN->RUMPF, TIEF->BEINE),
`+0x6 = 0`, Zeile `+0x5 = re2z_row_from_weapon[8] = 7` (enemy_ai_re2_zombie.c:3703ff
Mapping-Block, :3648-3653), Zustandswort 2 (HURT, `sw 2,4` @0x80047288).

**HURT-Wurzel** `re2z_hurt` (:6472, FUN_80104F40): stamp_hit -> grab_abort -> Kriecher-
Zweig (`f10e&1` -> `re2z_crawl_hurt` :6431, Clip 6 Start rand&7) -> Flinch-Gate
(:6503-6516, `0x501` nur bei `+0x6==0 && res223<=thr && (flag222==1 || Zeile==1)`) ->
Liege-Zweig (`+0x21A&2` -> `0x60501` :6521) -> Zerleger -> Dispatch `tbl[7][col]`
(Tabelle :3703, Zeile 7 = `7438 66FC - | 7438 5BC0 - | 7438 5438 -`, Bracket fest 0):

| Bauch-Treffer | Spalte | Handler | Zustandswort-Folge |
|---|---|---|---|
| EBEN/HOCH (RUMPF) | 1 | `re2z_hit_ragdoll(death=0)` :5765 (0x801066FC) | (2,7,0) -> (2,7,1) [ab Frame 20 BOUNCE auf `e->y`] -> (2,7,2) -> P2: **0x0001** + `f10e=0x2001` (KRIECHER, Clip 5) :5880-5885 |
| HOCH zusaetzlich | 1 | Kopf-Explosion `death_magnum` :5779-5790 (Gate @0x8010679C-A8, auch im HURT) | -> DEATH-Kette |
| TIEF (BEINE) | 0 | `re2z_hit_knockdown(death=0)` :6162 (0x80107438) | (2,7,0) [Clip 1, **Startframe rand&7** :6168 @0x801074CC-EC] -> (2,7,1) -> (2,7,2) -> `f10e=0x2001`, **0x0001/0x0201** (rand) :6205-6208 |
| Folge-Treffer, flag222==1, res<=0 | — | Flinch **0x501** :6507 -> EXEC[5] `re2z_exec_knockdown` :1984 | (1,5,0..8) -> P8 **0x101** = er STEHT wieder :2166-2200 |
| Treffer auf Liegenden | — | **0x60501** :6521 -> EXEC[5] P6 | -> P7/P8 -> 0x101 |

Hinweis: Kommentar :6157 („Spalte 0 im Port unerreichbar, stempelt immer Zone 1") ist
seit dem Zonen-Fix vom 2026-09-12 VERALTET — re15_damage.c:1869 stempelt TIEF -> 0.

## 2. Alle `e->y`-Schreiber und Root-Y-Quellen im Reaktionspfad

Vollscan `->y =|+=` ueber enemy_ai_re2_zombie.c: **genau DREI Schreiber, alle im
Ragdoll-Handler** — sonst schreibt im ganzen RE2-Zombie-Modul niemand `e->y`:
1. `:5940` `e->y += t15a` (Fall, +5/Frame, +55 ab Frame 35; @0x80106E84-A4)
2. `:5949` `e->y = gy232 - 300 - rooty` (Bounce-Klemme; @0x80106EF0-F10)
3. `:5874` `e->y = gy232` (P2-Gegenbuchung; @0x80106AC8) — die Gegenbuchung EXISTIERT.

* **`re15_re2z_move_root`** (enemy_ai_common.c:2536 -> `re15_clip_root_motion_delta`
  :689-717): wendet **NUR X/Z** an (:714-716; Original-Paar 0x80015E7C/0x800152C8 ebenso,
  Apply nur +0x38/+0x40 @0x80015314-34; die y-fuehrenden RE2-Resolver FUN_80015b94/cb8
  @0x80015c54-64/@0x80015d6c-7c haben fuer die ausgelieferten Clips sy==0, Kommentar
  :638-643). **Kandidat 3(a) ENTLASTET**: kein Fall-Clip-Y wird je auf `e->y` integriert,
  eine fehlende Gegenbuchung dafuer kann es nicht geben.
* **`re2z_thrust`/`re2z_hit_move`** (:1134, :3898): nur X/Z bzw. Steer+move_root.
* **root_y_fix** (`re2_build_rootfix`, re2_ems.c:299-322): Tabelle mit **einem int16 pro
  KEYFRAME des ganzen Pools** (`n = hyb->keyframe_count`), gebaut fuer ALLE drei Skelette
  (skel/skel_loco/skel_own, re2_ems.c:363-365) — deckt damit JEDEN Clip inkl. Fall-/
  Knockdown-Clips 1/2 und JEDEN Startframe ab. `re15_compute_actor_kf` klemmt den kf-Index
  in den Pool (anim_select_common.c:171-174), `re15_skel_root_y_fix` prueft die Grenze
  (skeleton_common.c:183-185). **Kandidaten 3(b)/3(c) ENTLASTET.** Der Renderer addiert den
  Fix auf BEIDE Tween-Seiten und VOR dem FRAC-Blend (skeleton_common.c:256/:264) —
  gerendertes Wurzel-Y = `kf_py + K(kf)`; Hybrid laeuft im RE2-Flavor immer
  (pc/main.c:586-618, Fallback nur bei Ladefehler).
* **Boden-Klemme**: es gibt im Port KEINEN generellen y>=Boden-Klemmer fuer Gegner. Eigene
  Klemmen: nur der Hund (`dog_floor_y`, enemy_ai_common.c:6990-7299) und die Ragdoll-
  Bounce-Klemme selbst. Spieler: climb_common.c:533 (`p->y = s_floor_y`). EXEC[5]/EXEC[9]
  schreiben `e->y` NIE — deren „Fallen" liegt komplett im Clip-Wurzel-py (gerendert).

## 3. KERNBEFUND — die Bounce-Klemme misst OHNE den Hybrid-Rootfix

**`re2z_root_py` (:5742-5759) liest das rohe Keyframe-py (`frames[fi]&0xfff` ->
`re15_emd_get_keyframe_position`) und laesst `re15_skel_root_y_fix` weg** — der Renderer
zeichnet aber `kf_py + K` (skeleton_common.c:256). Das Original liest an dieser Stelle die
GERENDERTE Part-0-Translation (`lw 48(s3)` @0x80106E64/EAC), im Port ist das die Fassung
MIT Fix. Folge in der Bounce-Physik (:5938-5951) mit `rooty_raw = rooty_gerendert - K`:
* Schwelle :5945 feuert erst, wenn die gerenderte Wurzel **K zu tief** liegt
  (`gy-200-raw = (gy-200-gerendert)+K`),
* Klemme :5949 setzt die gerenderte Wurzel auf **`gy - 300 + K`** statt `gy - 300`.

K ist gemessen **+108..+185** fuer EM010 (re2_ems.c:230-236: Clip 0 RE2 -21..+47, HYB roh
-206..-129) — der geworfene Koerper sinkt waehrend der Frames 20..Clip-Ende also rund eine
halbe Koerpertiefe **unter den Boden** („mit Animation unterhalb des Bodens taumeln
weiter"), bis P2 :5874 `e->y = gy232` zurueckschnappt (wieder auf Bodenhoehe; danach
Kriecher, Clip 5 — byte-true). Der Pfad ist exakt der EBEN-Bauch-Treffer (Spalte 1 ->
66FC), und NUR dieser Pfad schreibt `e->y` — die Lokalisierung ist eindeutig.

Nebenbefunde an derselben Funktion (klein, im selben Patch erledigen):
* `re2z_root_py` ueberspringt 0x8000-Marker-Frames nicht (`&0xfff` eines Markers = Gewicht,
  kein kf-Index; Muster-Vorlage: 0x8000-Skip in enemy_ai_common.c:633-635). EM010 fuehrt
  keine Marker (anim_select_common.c Schluss-Kommentar) — latent, nicht akut.
* `gy232`-Produzent :5853 ist `(int16_t)e->y` statt des Original-Boden-Y +0x1C2
  (EXE-Produzent @0x8003EE04-18, im Port ohne Zwilling — OPEN, adressbelegt). Heute
  ungefaehrlich, weil der 1d3-Trefferfilter (:8046-8049, @0x80047138-40) waehrend der
  Reaktion (P0 `self1d3|=0x80` :5860) Re-Hits blockt und `e->y` am Treffer-Tick auf dem
  Boden steht; dokumentieren, nicht raten.

## 4. FAZIT — Patch-Plan + Messzeile

1. **Fix (Kern)** enemy_ai_re2_zombie.c:5756-5758: `re2z_root_py` gibt
   `py + re15_skel_root_y_fix(sk, kf)` zurueck (kf vorher in eine Lokale; Beleg: Original
   liest die GERENDERTE Wurzel `lw 48(s3)` @0x80106E64/EAC = Port-Pose skeleton_common.c:256).
   Dazu den 0x8000-Skip wie enemy_ai_common.c:633-635. Bank bleibt `b->skel/b->anim`
   (Zustand 2/66FC faehrt Paar 2, `re15_re2z_poses_loco_bank` :6730ff liefert dort 0).
2. **Doku** :6157 (Knockdown-Kopf): veralteten Satz „Spalte 0 unerreichbar" auf den Stand
   re15_damage.c:1869 (TIEF->0) bringen.
3. **Messzeile** (env-gegatet, Kanal re2_ki.log via `re15_re2_trace_out()`,
   enemy_ai_re2_zombie.c:5938 direkt nach `int rooty = re2z_root_py(e);`):
   ```c
   if (getenv("RE15_RE2_TRACE")) { FILE *tf = re15_re2_trace_out(); if (tf) fprintf(tf,
       "[z-ragdoll] slot=%d f=%d s2=%d clip=%d y=%d gy=%d rooty=%d wurzelwelt=%d dir=%d v=%d\n",
       (int)(e - g_actors), frame, e->sub_state_2, (int)e->motion, (int)e->y,
       (int)e->re2z_gy232, rooty, (int)e->y + rooty, (int)(int8_t)e->re2z_dir16a,
       (int)e->re2z_t15a); }
   ```
   Erwartung vor dem Fix: `wurzelwelt` klemmt bei `gy - 300 + K` (~gy-115 statt gy-300);
   nach dem Fix: `gy - 300`. Pin-Idee: Direktaufruf-Test 66FC death=0, Frames 20..Ende,
   Invariante `e->y + rooty_gerendert <= gy - 200`.
