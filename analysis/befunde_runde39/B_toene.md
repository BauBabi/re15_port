# Runde 39 — Spur B "toene" (Dossier, fortlaufend)

Baum: `.claude/worktrees/r39_toene` (Zweig `r39/toene`, Basis 3e49ff87). Wortlaut: `AUFTRAG.md`, Zuteilung: `VERTRAG.md` §0 (Spur B).

## Punkte (Wortlaut des Nutzers)
1. "In Room 10F0 fehlt noch beim öffnen der Schublade der Sound. Den könnte man zum Beispiel aus Resident Evil 2 holen, wenn dort eine Schublade geöffnet wird."
2. "In Room 1090, wenn die Hunde in der Cutscene rein gesprungen wird, fehlt noch der Sound vom öffnenden Gitter, den könnte man von den öffnenden Gittern der Spinnen aus STAGE2 holen."
3. "In Room 1090 fehlt noch Sound für den bewegenden Schrank nach dem lösen des Rätsels. Den könnte man aus Resident Evil 2 holen von den bewegenden Regalen in der 2. Etage."

## Stand
- (laufend) Dossier angelegt, Lektuere AUFTRAG/VERTRAG erledigt.

## 0. Welche Raeume gemeint sind (gemessen, nicht vermutet)

### 0.1 Punkt 1 "Room 10F0 ... Schublade" = ROOM11F0 (Leon) / ROOM11F1 (Elza, bytegleich)
* ROOM10F0.RDT (= ROOM10F1.RDT, `cmp` 0 Abweichungen) hat KEINE Schublade: main00 @0x0F32..0x10D4 nur Tueren,
  Aot_set sce 1 (Texte), 4 Item_aot_set (Slot 11..14), Obj_model_set 1..3 (Item-Modelle); sub00/sub01 nur der
  Tritt obj 0 (Typ 4) und (3,102). Kein Cut_chg, kein Ereignis (`scd_dump_room.py`, Ausgabe im Messordner).
* ROOM11F0.RDT (Boiler Room, Generator) main00 @0x00CE6 `2c 0d 03 31 00 00 d8 27 b2 c1 dc 05 e8 03 ff 00 18 13 00 00`
  = AOT 13 sce 3 Rechteck (10200,-15950) 1500 x 1000 -> Evt_exec **sub19** @0x017B6:
  `29 0b` Cut_chg 11 / `22 02 07 01` Sperre / Work_set(1,0) + Member_set(1,-16200) (Leon aus dem Bild) / bei
  Ck(9,99)==0 Work_set(3,1) + Member_set(1,-1200) (Patronen obj 1 @0x00D10 in die Schublade) / Sleep 30 /
  `47 0e` Aot_on 14 = Item_aot_set @0x00CFA (H. Gun Bullets x30, Bit (9,99)) / Sleep 2 / zurueck / Cut_old /
  Cut_auto / `22 02 07 00` @0x0180A. Kein Se_on in sub19 -> RE1.5 STUMM.
* Bild (echte exe, RE15_DEBUG_JUMP=11F0, RE15_FIRE_AOT=13@400, Framedump F405): Cut 11 = die GEOEFFNETE
  SCHUBLADE mit der Patronenschachtel (`B_belege/b1_11f0_cut11_schublade.png`). Runde 34 Nacht C nennt sub19
  bereits "Schublade" (`C_generator.md` Z.186-187).
* Die Ziffernverschiebung 10xx -> 11xx ist dieselbe wie bei Punkt 2/3 (s. 0.2) und schon in Runde 36 belegt
  (`analysis/befunde_runde36/M_karte.md` R0: "ROOM 1090" mit Hunde-Luke + Weste = ROOM1190). Gegenprobe: die
  Leichenschublade ROOM1200 sub02 (obj 1 faehrt, ebenfalls ohne Se_on) ist eine Zombie-Szene, keine
  "geoeffnete Schublade" und zwei Ziffern entfernt -> nicht gemeint (im Abschnitt "Fuer den Nutzer" benannt).

### 0.2 Punkt 2 + 3 "Room 1090" = ROOM1190 (Schiessstand B1)
* ROOM1090.RDT hat weder Hunde noch einen Schrank, der sich nach einem Raetsel bewegt: Sce_em_set nur 0x26
  (Feuer), 0x42 (Ada), 0x10/0x11 (Zombies @0x023A0..0x023DC); Obj 0/1 sind Typ-4-Kisten (Spieler schiebt).
* ROOM1190.RDT sub10 @0x0276A (gestartet aus sub01 @0x02468/@0x024A8, wenn die vier Zielscheiben-Schalter
  stimmen: Ck(4,0xF3)=1 Strom, Bank-5-Bits 4..7) = DIE Raetsel-Szene:
  - `22 04 ea 01` Set(4,0xEA) geloest, Sleep 40, **@0x02790 `29 08` Cut 8**, Sleep 20,
    **@0x02796 `2e 03 05` Work_set(3,5) = SCHRANK obj 5** (@0x02A46 bei (3920,0,-25952)),
    @0x0279A `2f 00 9c ff` Speed_set(0,-100), @0x0279E `0d 00 06 00 14 00` For 20 {Add_speed, Evt_next, Evt_next}
    = 20 x -100 = -2000 in x ueber 40 Bilder (Endlage (1920,0,-25952) = sub13 @0x02AB8), danach
    Aot_reset Slot 5 sce 9 = die Red Keycard dahinter (Item_aot_set @0x02A30 Item 0x37).
  - Sleep 20, **@0x027E0 `29 0b` Cut 11**, je Hund (Sce_em_set 0x20 @0x02900/@0x02914/@0x02928 im Schacht):
    Work_set(2,n) + Member_set(0x0C,0x43) (+ Member_set(1,-3600)) + **Gosub 0x0B** @0x027EA/@0x027F8/@0x02806
    = sub11 @0x02854: Work_set(3,4) = LUKE/GITTER obj 4 (@0x02A06 bei (310,-4500,-26946)), Speed_set(5,150)
    Speed_set(0x0B,-6), For 10 {Add_speed, Add_aspeed, Evt_next} = aufklappen, dann Speed_set(5,-150)/(0x0B,6)
    For 10 = zuklappen. Dreimal (je Hund). Kein Se_on in sub10/sub11 -> RE1.5 STUMM.

