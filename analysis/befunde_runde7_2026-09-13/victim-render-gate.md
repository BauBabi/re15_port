# victim-render-gate — Gator-Fress-Finisher: Leon steht aufrecht statt im Maul

Runde 7, 2026-09-13. Nutzer-Marker `r7_befund_2090_F286_marke1.png` / `F432_marke2.png`:
"Beim Gator-Fress-Finisher STEHT Leon aufrecht neben dem liegenden Gator statt die
Opfer-Animation zu spielen ('hat Leon immer noch nicht im Maul')". Statische Analyse
(nur Port-Code). Referenz: Runde 6 `gator-vollausbau.md` 6.1/7b (Finisher-Umbau),
`enemy_ai_boss_gator.c` GBP_FRESSEN.

---

## 0. BEFUND-INTEGRITAET (⛔ zuerst lesen)

Die zwei gelieferten Marken zeigen den Finisher NICHT. `befund.log` (Zeilen 60214/60257):

```
F286  R2090 C0  hp=25 ... | keine Maske am Koerper
   Opfer-FSM des Spielers: state=0 typ=0x00
   15 typ=0x23 st=1 ss=0/0 clip=0 bild=280 hp=3000 dist=0 pos=(-7200,-1200,-9200)
F432  R2090 C0  hp=25 ... Opfer-FSM des Spielers: state=0 typ=0x00
```

- Leon **lebt** (hp=25), die **Opfer-FSM steht auf 0** (Mode 4 nicht aktiv), der Gator
  liegt mit **hp=3000 auf dem Lauerplatz** (-7200,-9200), clip 0.
- Der HP-Abfall im Lauf ist 65→45→25 in **-20-Schritten** (befund.log F5/F65/F185).
  Der Gator-Biss ist `GB_BITE_TYPE=5` → `dmg_table[5]=50` (enemy_ai_boss_gator.c:75).
  **-20 ist kein Gator-Biss** — das sind die Spinnen (0x25/0x26). Der Gator hat in
  diesem Lauf **nie zugebissen**, der Finisher wurde nie betreten.

Konsequenz: die Marken belegen die *Symptomklasse* (Leon steht/posiert aus eigener
Bank, keine Maul-Pose), nicht den Finisher-Frame selbst. Die folgende Diagnose ist
daher **code-mechanisch** hergeleitet und mit der Headless-Probe kontrastiert — die
Probe treibt `re15_gator_boss_tick` DIREKT und umgeht `re15_game_step` komplett
(probe_gator_fress.c:129 `re15_gator_boss_tick(slot)` in der Schleife, KEIN
`re15_game_step`). Genau die Schicht, die im echten Spiel den Victim-Render steuert
(Death-FSM, victim_tick, Render-Gate in main.c), fehlt der Probe. Deshalb "Probe grün,
Spiel falsch".

---

## 1. WO der PC-Renderer die Victim-Pose waehlt (Frage 1)

`platform/pc/main.c:6931` (Spieler-Render):

```c
if (re15_player_victim_state() != 0 && !re15_player_victim_own_bank() && pl00_ok) {
    re15_enemy_bank_t *vb = re15_enemy_find(re15_player_victim_type());
    if (vb && vb->victim_ok && vb->anim_victim.clip_count > 0) {
        static re15_emd_skeleton_t s_victim_skel;
        s_victim_skel = pl00_skel;                     /* Leons Bones + Bindpose */
        s_victim_skel.keyframe_data      = vb->skel_victim.keyframe_data;  /* Bank-2-Posen */
        ... p_skel = &s_victim_skel; p_anim = &vb->anim_victim;
        p_clip_override = (int)player_ref->motion;      /* main.c:6952 */
    }
}
```

Bedingungen fuer Mode 4 (Gator, 0x23):
- `re15_player_victim_state() != 0` → **4 ≠ 0 ✓** (enemy_ai_common.c:807 `g_player_victim=4`).
- `re15_player_victim_own_bank()` = nur Gorilla 0x27 in Wurf-Phase (enemy_ai_common.c:1371-1375)
  → fuer 0x23 **false ✓** (Override greift).
- `pl00_ok` ✓ (PL00 im Bosskampf geladen).
- `vb=re15_enemy_find(0x23)` → gesetzt durch `re15_player_victim_force(e->type,…)`
  (`g_player_victim_type=0x23`, enemy_ai_common.c:808).
- `vb->victim_ok` — **identisch zur Probe**: beide laden EM23 aus
  `shared_assets/RE2/CDEMD0.EMS` ueber `pc_enemy_load_re2` → `re2_ems_load_bank`
  → `re2_emd_parse_bank` → `eb->victim_ok = re2_parse_pair(D[5],D[2],D[6])` (re2_ems.c:115).
  Der echte Spiel-Pfad ist main.c:929 (`type==0x23u && pc_enemy_load_re2`), der
  Probe-Pfad probe_gator_fress.c:66 (`re2_ems_load_bank(…,0x23,…)`) — **dieselbe Datei,
  derselbe Parser, dasselbe victim_ok**. Das ist NICHT die Divergenz.

**FAZIT Q1:** Der Render-Gate greift genau dann, wenn `g_player_victim==4`. Ist Mode 4
am Render-Zeitpunkt gesetzt, posiert Leon aus der Opfer-Bank. Der Defekt liegt darin,
dass Mode 4 im echten Spiel entweder **zu spaet** (erst Gator-Clip-4-Frame 13,
s. Q3/§3) oder **gar nicht durchgehend** gesetzt ist — nicht am Gate selbst.

---

## 2. Welche Spieler-FSM Leon parallel ueberschreibt (Frage 2)

`re15_game_step` (game_step_common.c) — relevante Reihenfolge in EINEM Frame:

1. **Kopf** (Zeile 1013): `if (pl->hp<0 && s_prev_hp>=0 && !re15_player_is_grabbed()
   && !re15_stair_active()) re15_player_death_cmd3();`
   → `re15_player_death_cmd3()` (game_step_common.c:169-179) armt die generische
   Tod-FSM (`s_death3_on=1`) **und ruft `re15_player_victim_reset()`** (Zeile 177 →
   `g_player_victim=0`).
2. **Zweig-Kette** (else-if, Reihenfolge zwingend): PAUSE → climb → stair →
   **`re15_player_is_dead()`** (:1116, DEATH) → `re15_player_is_grabbed()` (:1129, GRAB)
   → knockdown → flinch → normal (player_tick).
3. **Ende** (nach `re15_enemy_ai_run_all`, das den Gator-Tick faehrt):
   `re15_player_victim_tick()` (:1943) → bei Mode 4 sofort `return` (enemy_ai_common.c:1454)
   → dann `re15_player_death_cmd3_tick(c)` (:1949).

Die Konkurrenten um `pl->motion / anim_frame / x / z`:

- **DEATH-Zweig (:1116):** `re15_player_is_dead()` liefert fuer den Gator **sofort true**,
  sobald hp<0 — die RE2-Victim-Ausnahme in re15_damage.c:284-290 deckt nur
  `re15_re2z_owns_type(vt) || vt==0x20` (Zombie-Familie + Hund), **0x23 ist NICHT dabei**.
  Der DEATH-Zweig laeuft also VOR dem GRAB-Zweig und ueberspringt `player_tick` — gut,
  Leon steuert nicht. ABER:
- **`re15_player_death_cmd3_tick` (:1949 → :185):** solange `re15_player_victim_state()==0`
  (die ersten ~13 Finisher-Frames, s. §3) laeuft **Ph0/Ph1** und setzt
  `pl->motion=7` (PL00-Tod-Clip, game_step_common.c:198) + advanct ihn. Das
  **ueberschreibt** das vom Gator P0 gesetzte `pl->motion=2`. Sobald Mode 4 gesetzt ist
  (Frame 13), sieht cmd3_tick `victim_state!=0` und **schaltet sich selbst ab**
  (:190-193 `reset+return`).
- **NORMAL-Zweig (player_tick):** würde Leon steuern/idlen (= aufrecht STEHEN). Er ist
  aktuell nur unerreichbar, WEIL `is_dead` true ist. Faellt `is_dead` je auf false
  (z.B. Teil-Fix, Continue-Respawn, oder hp≥0), **greift genau dieser Zweig und Leon
  steht** — das ist die Symptomklasse der Marken (hp=25 → is_dead false → NORMAL →
  player_tick → stehen).

Tick-Reihenfolge der Kopplung: der Gator schreibt `pl->motion/anim_frame/x/z` im
`run_all` (Step-Ende), `victim_tick` weicht bei Mode 4 aus, `death_cmd3_tick` folgt
danach. Wer "gewinnt", entscheidet also `re15_player_victim_state()`:
Mode 4 gesetzt → Gator gewinnt; Mode 0 → cmd3 (Tod-Clip) bzw. bei is_dead=false der
NORMAL-Zweig gewinnt.

**FAZIT Q2:** Konkurrent Nr. 1 ist die **Tod-FSM** (`re15_player_death_cmd3` +
`…_tick`): sie **nullt** den Victim-Mode beim hp<0-Uebergang (game_step_common.c:177)
und posiert Leon aus PL00, solange Mode 4 noch nicht gesetzt ist. Konkurrent Nr. 2 ist
der **NORMAL-Zweig**, der Leon aufrecht stehen laesst, sobald `is_dead` false ist.

---

## 3. hp=-1 / Game-Over-Kette + wie der ALTE Code es unterdrueckte (Frage 3)

- **Eintritt:** `gb_biss_abschluss` (enemy_ai_boss_gator.c:349-360):
  `re15_player_take_damage(pl, GB_BITE_TYPE=5, …)` → hp -= 50 (re15_damage.c:208).
  Bei hp<0 → `g->phase=GBP_FRESSEN; g->timer=0; e->motion=4`. **hp wird NICHT auf -1
  geklemmt** — der Wert bleibt der Biss-Rest (z.B. 25-50 = -25) und ist **ab diesem
  Tick negativ**.
- **Folge:** naechster Frame, game_step:1013 → `pl->hp<0 && s_prev_hp>=0` → `re15_player_death_cmd3()`
  → **normale Tod-Kette startet SOFORT**: `s_death3_on=1`, `re15_player_victim_reset()`
  (victim→0), und `re15_player_is_dead()` (0x23 nicht ausgenommen) → DEATH-Zweig +
  `re15_gameover_fsm_tick()` (game_step_common.c:1035, top-level). Die Gameover-FSM
  legt bei sub1→2 `g_death_blackbg=1` (schaltet die Raum-Kulisse ab) + Death-Cam +
  YOU-DIED (game_step_common.c:575-583). Der Finisher spielt also **unter der schwarzen
  Todes-Praesentation**, nicht sichtbar im Raum.
- **Mode 4 kommt zu spaet:** `re15_player_victim_force(e->type,1,0)` wird erst bei
  **Gator-Clip-4-anim_frame==13** gerufen (enemy_ai_boss_gator.c:1861/1867). Bis dahin
  (~13 Frames) ist Mode 0 → cmd3-Tod-Clip (Ph0/Ph1) besitzt Leon; danach uebernimmt
  Mode 4. `re15_player_victim_force` setzt zudem **`g_player_victim_zombie` NICHT**
  (enemy_ai_common.c:806-813) — kosmetisch hier ohne Folge (own_bank/grabber_type
  laufen ins 0), aber vermerkt.
- **Halb-Riegel:** `re15_gator_fressen_hold()` (enemy_ai_boss_gator.c:2040-2052) haelt
  NUR den FINALEN Fade (Gameover sub 6, `s_go_ctr<0x50 && timer<215`,
  game_step_common.c:610-611) — er verhindert **nicht** Blackbg/Death-Cam/cmd3, die
  schon in sub 0/1/2 laufen.
- **Wie der ALTE Wirbelbahn-Code es unterdrueckte:** ueber `pl->no_draw` +
  `pl->fress_skip_mask` (Halbkoerper-Trick) — der neue Finisher hebt beide in P0 auf
  (enemy_ai_boss_gator.c:1849-1850 `fress_skip_mask=0; no_draw=0`) und ersetzt sie
  durch Mode 4. Der Reset-Pfad dazu steht noch in main.c:3591-3593
  (`no_draw=0; fress_skip_mask=0` beim Finisher-Reset).
- **Kontrast Zombie-Devour (das Vorbild):** `re15_player_victim_devour` setzt Mode 2
  und laesst hp≥0 bzw. verankert die Ausnahme; `re15_player_is_dead` gibt fuer
  re2z/Hund **0 zurueck, solange `victim_state!=0 && state!=7`** (re15_damage.c:284-290).
  Der **Handoff** (hp=-1 + `state=7`) faellt erst am Kollaps-Clip-Ende
  (enemy_ai_common.c:1640-1642 / 1666-1671) — dann erst startet Tod/Gameover. Genau
  diese Verzoegerung fehlt dem Gator.

**FAZIT Q3:** Der Gator-Biss macht hp sofort negativ, und weil 0x23 nicht in der
is_dead-Ausnahme steht, startet die komplette Tod-/Gameover-Kette (inkl. victim_reset
und cmd3-Tod-Clip) **waehrend** des Finishers statt danach. Mode 4 wird erst 13 Frames
zu spaet gesetzt. Der alte no_draw/fress_skip_mask-Riegel ist ersatzlos entfernt.

---

## 4. PATCH-PLAN (minimal-invasiv, spiegelt den Zombie-Devour)

Ziel: der Finisher "besitzt" Leon wie der Zombie-Grab (Mode 1/2) — (a) Victim-Render
Mode 4 greift vom ersten Frame, (b) keine Spieler-FSM ueberschreibt Clip/Position,
(c) die Game-Over-Kette startet erst NACH der Sequenz.

**FIX A — Tod/Gameover bis zum Sequenz-Ende zurueckstellen (Kern-Hebel).**
Datei: `engine/src/re15_damage.c`, `re15_player_is_dead()`, Ausnahme-Block **Zeile 284-290**.
Ergaenzen (VOR dem `re15_ai_re2_for_type`-Zweig): wenn `re15_player_victim_state()==4`
(Mode 4 ist EXKLUSIV der Gator-Fress, nur `re15_player_victim_force` setzt ihn —
enemy_ai_common.c:807, einziger Aufrufer boss_gator.c:1867) **und** `player.state!=7`,
`return 0;`. Wirkung: kein Blackbg / YOU-DIED / Death-Cam / cmd3, solange der Finisher
laeuft — exakt wie der Zombie-Devour. Handoff: der Finisher-Schwanz
(enemy_ai_boss_gator.c:1917-1922) macht `re15_player_victim_force_end()` (Mode→0) +
`g->phase=GBP_CHASE`; danach ist hp<0 → is_dead true → Tod/Gameover laeuft normal.
Empfehlung: im Finisher-Schwanz zusaetzlich `pl->state=7` setzen, damit der Uebergang
sauber flippt (byte-true-Analogon zu enemy_ai_common.c:1642/1671).

**FIX B — Finisher durch den PIN-Zweig fuehren (keine FSM re-posiert Leon).**
Datei: `engine/src/enemy_ai_common.c`, `re15_player_is_grabbed()` **Zeile 578**.
`|| vs == 4` ergaenzen. Wirkung: mit Fix A (is_dead=false) erreicht die else-if-Kette
den GRAB-Zweig (game_step_common.c:1129) → `player_tick` + Kollision + Steuerung
uebersprungen (Leon "gehalten"). Zusaetzlich blockt es den cmd3-Arm in
game_step_common.c:1013 (`!re15_player_is_grabbed()`), damit
`re15_player_victim_reset()` (game_step_common.c:177) den Mode 4 **nicht mehr nullt**.

**FIX C — Victim-Mode 4 am FINISHER-START armen, nicht erst bei Frame 13.**
Datei: `engine/src/enemy_ai_boss_gator.c`, GBP_FRESSEN **P0-Block Zeile 1847-1858**
(`if (g->timer == 1)`). Direkt in P0 `re15_player_victim_force(e->type, <intro-clip>, 0)`
rufen, sodass der Render-Gate (main.c:6931 `vs!=0`) ab Frame 1 zieht. Damit entfaellt
das 13-Frame-Fenster, in dem Mode 0 ist (heute: cmd3-Tod-Clip; mit Fix A/B: NORMAL →
player_tick → STEHEN).
⚠️ Klippen-Detail: P0/P1 setzen aktuell `pl->motion=2` (Leons EIGENER Strampel-Clip,
gedacht fuer PL00-Render bei Mode 0). Bei Mode 4 zeichnet main.c `motion` aus der
**Opfer-Bank** (`p_clip_override=pl->motion`, main.c:6952). Also entweder das
Strampeln auf einen Opfer-Bank-Clip mappen (die authored RE2-Sequenz treibt Leon aus
Paar-3) und die P1-Platzierung ebenfalls per `re15_clip_root_motion_abs_pub` auf der
Opfer-Bank fahren (wie P2 es ab Zeile 1888 schon tut), ODER Fix C so bauen, dass P1
weiter aus PL00 rendert und Fix B trotzdem `player_tick` sperrt. **Minimal & kohaerent:
gesamten Finisher ab P0 aus der Opfer-Bank treiben** (Clip 1 durchgehend, P3 parkt
Leon ohnehin schon auf Opfer-Clip 1/Frame 119, boss_gator.c:1906-1909) — dann ist der
Gator wie der Zombie-Devour komplett victim-bank-gefuehrt. Den konkreten Intro-Clip
der EM23-Opfer-Bank (Layout `anim_victim`) mit `probe_gator_fress` bestaetigen, bevor
die Zahl in den Code geht (kein Raten — Beleg = Clip-Statistik der geladenen Bank).

Reihenfolge/Abhaengigkeit: A ist Voraussetzung (sonst blackt die Praesentation den
Raum weg); B verhindert das Nullen + das Stehen; C schliesst das Startfenster. Alle
drei sind noetig fuer das saubere "Leon im Maul, im sichtbaren Raum, Gameover erst
danach".

---

## 5. FAZIT (Kurz)

Der Render-Gate selbst (main.c:6931) ist korrekt und greift, sobald `g_player_victim==4`
gesetzt ist; `victim_ok` der EM23-Bank ist im echten Spiel identisch zur Probe
(gleiche CDEMD0.EMS, gleicher `re2_ems_load_bank`-Pfad) — das ist NICHT die Ursache.
Die Divergenz Probe↔Spiel liegt ausschliesslich in der `re15_game_step`-Orchestrierung,
die die Probe (direkter `re15_gator_boss_tick`) komplett umgeht. Kern: der Gator-Biss
macht hp sofort negativ (`take_damage` -50), und `re15_player_is_dead()` nimmt 0x23
NICHT aus (re15_damage.c:284-290 deckt nur Zombie-Familie + Hund) — dadurch startet
die volle Tod-/Gameover-Kette waehrend des Finishers: `re15_player_death_cmd3()`
(game_step_common.c:1013) nullt via `re15_player_victim_reset()` (:177) den Victim-Mode
und posiert Leon aus PL00 (Tod-Clip 7), und die Gameover-FSM blendet Raum + Death-Cam
ueber die Szene. Verschaerfend wird Mode 4 erst bei Gator-Clip-4-Frame 13
(boss_gator.c:1867) gesetzt, nicht in P0 — 13 Frames lang gehoert Leon damit der
Tod-FSM (oder, faellt is_dead je auf false, dem NORMAL-Zweig → aufrecht stehen). Der
alte no_draw/fress_skip_mask-Riegel wurde ersatzlos entfernt (P0 setzt beide 0).
Fix: den Finisher wie den Zombie-Devour bauen — is_dead-Ausnahme fuer Mode 4
(re15_damage.c:284), `vs==4` in `re15_player_is_grabbed()` (enemy_ai_common.c:578),
und Mode 4 in P0 armen (boss_gator.c:1847). ⛔ INTEGRITAET: die gelieferten Marken
F286/F432 zeigen Leon lebend (hp=25, Opfer-FSM=0), der Gator lauert mit hp=3000, und
der HP-Verlust waren -20-Spinnentreffer — der Finisher lief in DIESEM Lauf nicht. Die
Diagnose ist daher code-mechanisch belegt; eine Verifikation an einem echten
Gator-Kill-Capture steht als naechster Schritt aus (OFFEN).
