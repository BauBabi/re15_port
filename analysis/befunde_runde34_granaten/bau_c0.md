# Runde 34 (Granaten) — Bau C0: Vertrags-Commit

Stand: 2026-09-29, Zweig `r34g/c0-vertrag`, Arbeitsbaum `.claude/worktrees/r34g_c0`, Basis master 1c0a02a7.
Auftrag: BAUPLAN §3.0 (V1, V2b, V3, V5) + §3.3 C0, mit der Orchestrator-Teilung C (Plattform) / D (RE2-FX-Maschine).
**Keine Verhaltensaenderung**: alle neuen Felder sind 0 (memset beim Spawn), alle neuen Zeiger NULL, alle neuen
Funktionen Stubs ohne Aufrufer. Einzige wirksame Aenderungen liegen in Debug-Harness-Pfaden (V5: nur mit
`RE15_STATE_LOG` / `RE15_FX_LOG` / `RE15_EQUIP` gesetzt).

STATUS: IN ARBEIT (Code + Assets geschrieben, Bau/Suite laufen).

---

## 1. Woertliche neue Deklarationen (alle neuen Header-Zeilen)

`re15_port/include/re15_esp.h` (V1, in `re15_esp_fx_t` am Ende angehaengt, Zeilen 211/220; frei stehend 232/239/250):

```c
    int16_t  wpos[3];
    uint8_t  granate_art;
extern uint8_t g_re15_licht_latch;
extern void (*re15_esp_se_hook)(uint32_t code, const int32_t pos[3]);
extern void (*re15_esp_aufschlag_hook)(int re2_art, const int32_t q[3], int16_t gier);
```

`re15_port/include/re15_damage.h` (V2b, Zeile 345):

```c
int     re15_re2_gl_apply(const int32_t p[3], int16_t gier, const int16_t box[4], uint32_t hitcode);
```

`re15_port/include/re2_fx.h` (V3, NEU; Zeilen 32/35/42/47/54/60):

```c
int  re2fx_register_core(const uint8_t *raw, size_t size);
void re2fx_reset(void);
void re2fx_aufschlag(int re2_art, const int32_t q[3], int16_t gier);
void re2fx_tick(void);
extern int  (*re2fx_applier)(const int32_t p[3], int16_t gier, const int16_t box[4], uint32_t hitcode);
extern void (*re2fx_se_hook)(uint32_t code, const int32_t pos[3]);
```

`re15_port/platform/pc/src/re2fx_pc.h` (V3, NEU; Zeile 14):

```c
void re2fx_pc_draw(void);
```

Definitionen: `engine/src/re15_esp.c:748-750` (`= 0` / `= NULL` / `= NULL`), `engine/src/re15_damage.c:3683-3687`
(Stub `return 0`), `engine/src/re2_fx.c` (Zeiger `= NULL`; `re2fx_register_core` → `-1` = nichts registriert;
`re2fx_reset`/`re2fx_aufschlag`/`re2fx_tick` leer), `platform/pc/src/re2fx_pc.c` (`re2fx_pc_draw` leer).

---

## 2. Was gebaut (Datei:Zeile)

| Datei:Zeile | Inhalt | Vertrag |
|---|---|---|
| `include/re15_esp.h:202-220` | `re15_esp_fx_t` + `int16_t wpos[3]` (slot+0x28/2a/2c) + `uint8_t granate_art` (0/2/3/4) | V1a + Art |
| `include/re15_esp.h:223-250` | `g_re15_licht_latch`, `re15_esp_se_hook`, `re15_esp_aufschlag_hook` | V1b/c/d |
| `engine/src/re15_esp.c:742-750` | Definitionen 0/NULL | V1 |
| `include/re15_damage.h:329-345` | `re15_re2_gl_apply` (FUN_800470C0-Zwilling) | V2b |
| `engine/src/re15_damage.c:3671-3687` | Stub `return 0` (Spur B4) | V2b |
| `include/re2_fx.h` (neu) | RE2-FX-Schnittstelle | V3 |
| `engine/src/re2_fx.c` (neu) | Stubs, Zeiger NULL | V3 |
| `platform/pc/src/re2fx_pc.h/.c` (neu) | `re2fx_pc_draw` Stub | V3 |
| `platform/pc/main.c:7460-7474` | State-Log: je Gegner ` hp=<HP>` hinter `]` | V5 |
| `platform/pc/main.c:4266-4273` | RE15_EQUIP laedt die ARMS-Bank (`re15_audio_prime_weapon`) | V5 / P13 |
| `platform/pc/main.c:307-326` | FX-Log: Felder `wpos A B zuender zaehler fl art F` angehaengt | V5 |
| `shared_assets/RE2/CORE00.ESP`, `TEX.TIM` (neu) | unveraenderte Kopien aus `info/re2leon/COMMON/DATA` | C0 / K8 |
| `.gitattributes` (Ende) | `-text` fuer die beiden neuen Dateien | C0 |

---

## 3. Belege (in DIESER Sitzung selbst disassembliert, `re15_disasm.py` / `re2_disasm.py`)

Jede Adresse in den neuen Kommentaren wurde hier nachgelesen; zwei Stellen dabei korrigiert (siehe 3.9).

### 3.1 wpos = slot+0x28/+0x2a/+0x2c, s16 (RE1.5 PSX.EXE)
```
8001a1a0: jal 0x80068098     8001a1e4: jal 0x800661c0
8001a1fc: sh v0,40(a0)       8001a210: sh v0,42(a0)       8001a220: sh v0,44(a0)
8001a260..8001a2a4: lhu/sh 40/42/44(a0)   (Anker-Addition)
80018594: lh v0,40(v1)   800185a0: lh v0,42(v1)   800185a8: addiu v0,v0,-500   800185b0: lh v0,44(v1)
80018330: lh t1,42(t0)   80018338: blez t1,0x8001842c          (Routine 29 Bodentest)
```

### 3.2 granate_art = Resolver-Art a2 (RE1.5)
```
800185b4: ori a2,zero,0x2      800185b8: jal 0x80012d60      (Art 2 fest)
read 0x8006f418 11 --w 2 --signed = [10, 20, 1000, 1000, 1000, 50, 100, 200, 300, 1000, 0]
read 0x8006f430 11                = [3, 3, 9, 10, 11, 14, 15, 16, 17, 18, 20]
bytes 0x8006f41c = e8 03 e8 03 e8 03      bytes 0x8006f432 = 09 0a 0b
80033684: lbu v1,-13731(v1)   80033688: ori v0,zero,0x9   8003368c: bne v1,v0,0x800337ac   (Spawn-Gate)
800197d4: sb t8,112(t0)   800197d8: sb t7,113(t0)   800197dc: sh s1,114(t0)   800197e4: sh s2,46(t0)
```

### 3.3 g_re15_licht_latch = 0x800b5358 (RE1.5)
```
80018574: ori v0,zero,0x1   80018578: lui at,0x800b   8001857c: sb v0,21336(at)   (Routine 31)
8001768c: ori v0,zero,0x1   80017690: lui at,0x800b   80017694: sb v0,21336(at)   (Routine 9)
8001ce5c: lui v0,0x800b     8001ce60: lbu v0,21336(v0)   8001ce68: beq v0,zero,0x8001d088   (Leser)
8001d16c: lui s0,0x800b     8001d170: addiu s0,s0,21336   ...   8001d1b4: sb zero,0(s0)   (Loeschung)
```

### 3.4 re15_esp_se_hook = FUN_80045024-Analogon (RE1.5)
```
80018410: lui v1,0x10a  80018418: lhu a0,38(t0)  8001841c: ori v1,v1,0x1  80018420: sll a0,a0,8
80018424: jal 0x80045024   80018428: or a0,a0,v1                         (Abprall 0x010A0001|(n<<8))
80018350: lui a0,0x10a   80018354: ori a0,a0,0x1   80018358: jal 0x80045024   8001835c: addiu a1,sp,16  (Liegen)
800185e4: lui a0,0x408   800185e8: ori a0,a0,0x1   800185ec: jal 0x80045024                          (Explosion)
```

### 3.5 re2_art (Aufschlag-Hook, RE2 PSX.EXE)
```
8001f1a8: lbu v0,0(a1)   8001f1b4: addiu v0,v0,-9   8001f1b8: sb v0,27(v1)   (+0x1B := Id - 9)
Optab 0x8009D868: [40] = 0x80020758, [48] = 0x80020F3C, [49] = 0x800215C8
```
→ re2_art 1 = Brand (Op 48), 2 = Saeure (Op 49); Zuordnung 0x0A → 2, 0x0B → 1 explizit (Saeure-GP §15).

### 3.6 re15_re2_gl_apply / re2fx_applier (RE2 PSX.EXE, Aufrufer Op 40)
```
80020768: lui a2,0x8001 / 8002076c: addiu a2,a2,2320   (0x80010910)
80020770..8002078c: lwl/lwr/swl/swr  (8-Byte-Box -> sp+32, je Aufruf frisch)
80020790: lh v0,52(v1)  8002079c: lh v0,54(v1)  800207a4: addiu v0,v0,-100  800207ac: lh v0,56(v1)
80020794: lui a3,0x2002   800207a0: ori a3,a3,0xa   800207b0: addiu a0,sp,16   800207b8: lh a1,34(v1)
800207bc: jal 0x800470c0   800207c0: addiu a2,sp,32   800207c4: beq v0,zero,0x800207d4   800207cc: jal 0x80021970
read 0x80010910 4 --w 2 --signed = [-600, 0, 300, 150]
800470d0: addu s4,a0,zero   800470e0: addu s3,a2,zero   800470e8: addu s5,a3,zero
Radius-Erweiterung: 800471bc..800471ec  sh v0,6(s3) / sh v0,4(s3)  (in den Puffer des Aufrufers)
Ruecknahme:         800473dc..80047408  — erreicht NUR ueber 800471f0: beq v0,zero,0x800473dc (Nicht-Treffer);
                    Treffer: 80047210 beq v0,zero,0x80047434 (erster) bzw. 800473d4 j 0x8004740c (alle)
```
→ Signatur (p, gier, box, hitcode) = (a0, a1, a2, a3). `box` ist im Vertrag `const` (Orchestrator-Vorgabe):
fuer den einzigen Aufrufer Op 40 gleichwertig, weil er die Box vor jedem Aufruf neu kopiert (Hinweis an Spur B4).

### 3.7 re2fx_se_hook = FUN_8005ba28-Analogon (RE2)
```
80021678: lui a0,0x113   8002167c: ori a0,a0,0x1   80021680: addiu a1,sp,16   800216ac: jal 0x8005ba28   (Op 49)
80021020: lui a1,0x800e  80021024: lw a1,-13360(a1)  80021028: ori a0,a0,0x1  8002102c: jal 0x8005ba28
80021030: addiu a1,a1,96                                                                          (Op 48, a1 = Platz+0x60)
```

### 3.8 RE2-FX-Maschine (Header-Kopf re2_fx.h / re2fx_pc.h)
```
8001bd08: srl a0,v0,16  8001bd0c: andi v0,v0,0xffff  8001bd10: sll v0,v0,1  8001bd14: addu  8001bd18: addiu v0,v0,2  8001bd1c: sll v0,v0,2
8001cbe8: addiu t2,zero,96 ; Platz = 0x800D8CF0 + t0, t0 = 95*0x7C abwaerts (8001cc10/44/50: lhu v0,-29432(at) = +0x18)
8001d6b8: lw v0,-10136(at)   (0x8009D868)
80026980: jal 0x8001d300     (nach der Gegner-Schleife 800267c0 .. 80026930: bne s2,v0,0x800267c0)
8002b8cc: addiu v0,zero,28 / 8002b8d4: sh v0,-1040(at)  (0x800cfbf0)
80076a64: lbu v1,-1040(v1)  80076a6c: sll v0,v1,6  80076a80: addiu v0,v0,-1024  80076a9c-a4: sltiu/xori/sll 8
80076b00: lbu v0,-1039(v0)  80076b08: addiu v0,v0,480
TEX.TIM Kopf: 10 00 00 00 08 00 00 00 cc 04 00 00 00 01 e0 01 20 00 13 00  (4 bpp, CLUT (256,480) 32x19)
CORE00.ESP Kopf: 03 05 00 01 02 06 07 04
```

### 3.9 V5-Belege (RE1.5) und Korrekturen
```
HP-Store Resolver:   80013000: sh v1,154(s1)   80013004: lh v1,154(s1)
Menue-Commit:        80046688: sb v0,-13731(at)  800466c0: lbu a0,-13731(a0)  800466c4: jal 0x80043d8c  800466c8: ori a1,a1,0x8000
Zuender (R30):       80018474: ori v0,zero,0x2a   8001847c: sh v0,30(v1)
Zaehler:             8001850c: sh v0,38(a0) (R30)   800183d4: sh v1,38(t0) (R29)
```
Korrigiert beim Nachlesen: HP-Store steht @0x80013000 (nicht @0x80013008 = `ori v0,v0,0x1`); der Zaehler-Store
der Routine 30 @0x8001850c (Adresse im ersten Kommentarentwurf fehlte).

---

## 4. Assets (C0 / K8)

| Datei | Groesse | md5 | sha1 | git-Blob |
|---|---|---|---|---|
| `re15_port/shared_assets/RE2/CORE00.ESP` | 8572 B (= K8) | `c0b0a7f46862698b7eccbf445d86048c` | `21ef5d23c3257e492acfa98bc0a779f782355897` | `e030f7908d970ddf0833ef94d40079dc595b26f6` |
| `re15_port/shared_assets/RE2/TEX.TIM` | 132320 B (= K8) | `7472e1a871a480b5e6e7e7d50b7761b5` | `4b5ccf3a24edb01032656a6f85d80a381c7e2f75` | `e2f3e1b32153487e99bc20af38e5f14c572559a1` |

Quelle `info/re2leon/COMMON/DATA/{CORE00.ESP,TEX.TIM}` — md5 der Quelle identisch, git-Blob identisch mit dem schon
versionierten Original (kein Zuwachs im Repo). `.gitattributes`: beide Dateien `-text` (wie `RE2/DOOR/**`,
`RE15DOOR/**`); vorher schuetzte sie nur die NUL-Heuristik (CORE00.ESP Byte 2 = 00, TEX.TIM Byte 1 = 00;
`core.autocrlf = true`).

**Paketbau / Android (nur GELESEN, nicht geaendert):**
* `release/make_package.sh:410-411` `copy_common`: `cp -r "$RE2" "$out/shared_assets/RE2"` — der ganze Baum geht mit.
  Das Paket-Gate (`:186-193`) prueft nur CDEMD0.EMS, ENEMSE.VBS, TORSE.VBS, DOOR/*.DO2 — NICHT CORE00.ESP/TEX.TIM.
* `platform/android/app/build.gradle:104` `stageAssets`: `from(new File(portRoot, "shared_assets/RE2")) { into "shared_assets/RE2" }`
  — ganzer Baum; Existenz-Gate `:113-115` ohne die neuen Dateien; `noCompress` sammelt Endungen dynamisch (ESP/TIM
  kommen schon aus `shared_assets/PSX`).
* Android-Quellen: `platform/android/jni/CMakeLists.txt:42` GLOB `platform/pc/src/*.c`, Engine ueber
  `re15_port/CMakeLists.txt` → die neuen `.c` werden erfasst, ABER der Android-Bau cacht den Configure in `app/.cxx`
  (Memory reai-v2-android-glob-cache) → vor dem naechsten Android-Bau neu konfigurieren.

---

## 5. Harness V5 (Format)

* **State-Log** (`RE15_STATE_LOG`): Gegnerklammer unveraendert, danach ` hp=<HP>`:
  `... [3 t=10 st=1 ss1=0 ss2=0 ss3=0 g=00 mo=1 af=5 stun=0 d=1299 @(100,200,r0)] hp=80 [4 ...`
  Grund fuer "hinter der Klammer": drei Auswerter verankern die Klammer bis `)\]`
  (`port_inventar_werkzeug/auswertung.py` RX_EN, `befunde_runde30/nachschliff-room5080_tools/birkin_frost_echtlauf.py`
  und `birkin_vor_folge.py`); `tools/parity_run.py`/`parity_diff.py` lesen nur den Klammeranfang. Spieler-HP stand
  schon in `PL(x,z,rot=..,hp=..)` (P30 war nur fuer die Gegner richtig).
  Regex fuer neue Auswerter: `\[(\d+) t=([0-9a-f]+) [^\]]*\] hp=(-?\d+)`.
* **RE15_EQUIP**: nach `re15_player_set_equipped_weapon` jetzt `re15_audio_prime_weapon(re15_player_equipped_weapon())`
  (vorher: Bank blieb ARMS01 aus `re15_audio_init` `audio_pc.c:1767`; Bestaetigung im Code-Kommentar `main.c:4520-4522`).
  Wirkt auch mit `RE15_NOAUDIO=1` (der Lader liest nur Dateien; `re15_audio_weapon_se` bleibt dann stumm, der
  Waffen-Log zeigt aber `bank=9(geladen=1)` statt `bank=-1(geladen=0)`).
* **FX-Log** (`RE15_FX_LOG`): alte Zeile bis `q=%d` unveraendert, angehaengt
  ` wpos=(x,y,z) A=<row+0x00> B=<row+0x02> zuender=<+0x1e> zaehler=<+0x26> fl=<+0x6c hex> art=<granate_art> F=<frame_count>`.
  Hinweis: die Zeile entsteht nur fuer GEZEICHNETE Plaetze (sichtbar, nicht gecullt, Blatt geladen) — ein Platz mit
  Flags 0x61/0x63 fehlt im FX-Log (Liegen/Explosion); Spur A misst das in der Unit-Sonde.

---

## 6. Sonden / Mutationsproben

Keine neuen Tests (Auftrag: "keine neuen Tests noetig"; C0 hat kein Verhalten). Messungen: siehe §7.

---

## 7. Messungen

(folgt: Bau, volle Suite, Harness-Lauf)

---

## INTEGRATIONSWUNSCH (fremde Dateien — NICHT geaendert)

1. `release/make_package.sh:186-189` — sobald Spur D CORE00.ESP/TEX.TIM liest: Gate-Zeile wie fuer CDEMD0.EMS/ENEMSE.VBS
   (`for f in CDEMD0.EMS ENEMSE.VBS CORE00.ESP TEX.TIM; do ...`), sonst waere der Saeure-/Brand-Aufschlag im Paket still tot.
2. `platform/android/app/build.gradle:113-115` — Existenz-Gate um `"shared_assets/RE2/CORE00.ESP"` ergaenzen (gleiche Begruendung).
3. Android-Bau (andere Sitzung): `app/.cxx` loeschen / neu konfigurieren, weil `engine/src/re2_fx.c` und
   `platform/pc/src/re2fx_pc.c` neu sind (GLOB-Cache) — sonst Linkfehler, sobald `main.c` `re2fx_*` ruft.
4. Bindung (Spur C, `platform/pc/main.c` / `audio_pc.c`): `re15_esp_se_hook`, `re15_esp_aufschlag_hook = re2fx_aufschlag`,
   `re2fx_applier = re15_re2_gl_apply` (nach Merge B), `re2fx_se_hook`, `re2fx_register_core` beim Boot mit
   `shared_assets/RE2/CORE00.ESP` (Lader `re15_pc_read_re2`), `re2fx_reset` an denselben Stellen wie `re15_esp_fx_reset`
   (Raumwechsel `room_pc.c:130`, `scd_room_setup.c:199`; Original @0x80019378), Latch-Leser + Loeschen (C3).

## OFFEN

* keiner fuer C0.
