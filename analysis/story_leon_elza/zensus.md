# Varianten-Zensus Leon (xxx0) <-> Elza (xxx1)

Erzeugt von `zensus_varianten.py` aus `re15_port/shared_assets/PSX/STAGE1..6/ROOM*.RDT`. Alle Offsets = RDT-Datei-Offset; `mainNN`/`subNN` = SCD-Region.

Raumpaare: 120; byte-identisch: 71; mit Story-relevantem Unterschied: 44.

Kennzeichen: NPC = NPC-Spawns (Typ 0x40-0x4F) unterscheiden sich; Gegner = Anzahl/Typen der Gegner; Items; Tueren (Ziel/sce); Objekte (Obj_model_set); Effekte (Sce_espr_on); Texte* = abweichende Texte mit Story-Stichwort; CHAR-CK = Raum fragt Ck(1,0) = Elza-Bit ab.

## Uebersicht (nur Raeume mit Kennzeichen)

| Raum | Name | Kennzeichen | EM L/E | Items L/E | Tueren L/E | Texte L/E | MZD-README Z. |
|---|---|---|---|---|---|---|---|
| ROOM100 | BATH-LOCKERS | Items | 0/0 | 3/3 | 3/3 | 1/1 | 137 |
| ROOM101 | INTERROG. ROOM | NPC, Items, Objekte, Texte* | 4/6 | 3/3 | 2/2 | 3/24 | 49 138 164 277 348 |
| ROOM102 | MAIN OFFICE | NPC, Texte* | 8/10 | 2/2 | 3/3 | 6/15 | 101 204 265 357 |
| ROOM103 | LOBBY | Tueren, Texte* | 20/20 | 0/0 | 3/5 | 18/24 | 32 161 307 332 |
| ROOM104 | WEST CORRIDOR | Texte* | 40/40 | 0/0 | 4/4 | 2/4 | 276 |
| ROOM105 | EAST CORRIDOR | NPC, Items, Texte* | 1/0 | 1/2 | 6/6 | 9/6 | 105 139 169 247 271 333 |
| ROOM109 | REAR EXTERIOR | NPC, Gegner, Tueren, Objekte, Texte* | 12/4 | 0/0 | 3/2 | 10/0 | 104 109 117 187 209 266 279 337 358 |
| ROOM10B | FRONT EXTERIOR | NPC, Texte* | 4/5 | 0/0 | 2/2 | 0/6 | 26 27 359 |
| ROOM10D | 2F CORRIDOR | NPC, Texte* | 3/2 | 0/0 | 5/5 | 28/12 | 52 142 170 198 |
| ROOM113 | 3F CORRIDOR | Objekte | 0/0 | 0/0 | 4/4 | 6/6 | 147 171 256 270 |
| ROOM114 | BRIEFING ROOM | NPC, Gegner, Items, Texte* | 10/5 | 1/0 | 3/3 | 6/15 | 53 148 221 249 269 285 338 |
| ROOM115 | CHIEF OFFICE | Texte* | 1/1 | 0/0 | 1/1 | 15/4 | 216 |
| ROOM117 | HELIPORT | NPC, Gegner, Tueren, Objekte, Texte* | 8/10 | 0/0 | 7/6 | 17/5 | 103 160 |
| ROOM11A | SEWER EXIT | NPC | 1/2 | 0/0 | 5/5 | 3/3 | 56 253 281 301 310 |
| ROOM11B | GARAGE | Tueren, Objekte, Texte* | 3/3 | 2/2 | 5/4 | 16/15 | 25 172 193 222 223 287 303 334 361 362 |
| ROOM11C | PARKING LOT | NPC, Gegner, Texte* | 6/5 | 0/0 | 1/1 | 10/0 | 39 208 284 |
| ROOM121 | DETENTI. AREA | NPC, Objekte, Texte* | 10/11 | 2/2 | 6/6 | 0/11 | 278 |
| ROOM124 | OPENING | Tueren | 0/0 | 0/0 | 1/1 | 6/6 |  |
| ROOM126 | SEWER PASSAGE | NPC | 4/3 | 0/0 | 5/5 | 0/0 | 60 |
| ROOM200 | L TUNNEL | NPC, Texte* | 4/5 | 0/0 | 5/5 | 12/15 | 199 210 217 222 223 296 363 |
| ROOM201 | BREAK ROOM | NPC | 0/1 | 3/3 | 1/1 | 5/5 | 61 152 283 |
| ROOM300 | FACT. ENTRANCE | NPC, Texte* | 9/11 | 0/0 | 2/2 | 9/10 | 300 |
| ROOM301 | DOWNLOAD AREA | NPC | 10/11 | 1/1 | 6/6 | 2/2 | 335 |
| ROOM302 | PARKING AREA | NPC, Tueren, Texte* | 1/2 | 0/0 | 2/1 | 7/5 |  |
| ROOM306 | FACT. BACKYARD | NPC, Gegner, Tueren, Objekte, Texte* | 7/6 | 0/0 | 4/4 | 11/11 |  |
| ROOM307 | WAREHOUSE | NPC, Tueren, Objekte, Texte* | 4/4 | 0/0 | 2/2 | 6/16 | 36 365 |
| ROOM308 | WAREHOUSE LIFT | NPC, Gegner, Texte* | 3/2 | 0/0 | 1/1 | 2/0 |  |
| ROOM309 | T CORRIDOR | NPC, Objekte, Texte* | 0/2 | 0/0 | 5/5 | 4/33 | 38 114 224 304 339 340 366 368 |
| ROOM30C | LOBBY | NPC, Texte* | 0/1 | 0/0 | 2/2 | 2/11 | 224 367 370 |
| ROOM30E | FORKLIFT-FAC.L | NPC, Tueren, Objekte, Texte* | 5/4 | 2/2 | 5/4 | 16/19 | 370 371 372 |
| ROOM400 | LAB ENTRANCE | NPC, Gegner, Tueren, Objekte, Texte* | 7/6 | 0/0 | 5/4 | 10/15 | 203 218 336 373 374 |
| ROOM401 | SECURITY ROOM | NPC, Items, Objekte, Texte* | 2/0 | 3/2 | 1/1 | 44/8 | 176 213 311 336 375 |
| ROOM403 | MAIN SHAFT | NPC, Gegner, Texte* | 1/3 | 0/0 | 3/3 | 5/9 | 376 |
| ROOM501 | SECURITY ROOM | NPC, Texte* | 0/2 | 2/2 | 1/1 | 7/46 | 176 342 377 |
| ROOM502 | CARGO STORAGE | NPC, Gegner, Items, Tueren, Texte* | 1/1 | 1/0 | 1/2 | 0/2 | 346 378 |
| ROOM503 | MONITOR ROOM | NPC, Gegner, Texte* | 0/2 | 0/0 | 4/4 | 3/4 | 35 162 |
| ROOM506 | P-4 LABORATORY | Texte | 5/5 | 1/1 | 2/2 | 15/15 | 31 202 220 379 |
| ROOM509 | TRAIN CAR ABC | NPC | 2/1 | 0/0 | 6/6 | 2/2 | 158 |
| ROOM50B | C-2 HANGAR | NPC, Gegner, Objekte, Texte* | 4/0 | 0/0 | 2/2 | 19/1 | 328 |
| ROOM50E | B3 POWER ROOM | Gegner | 2/0 | 0/0 | 1/1 | 0/0 | 380 |
| ROOM50F | B4 POWER ROOM | Gegner | 0/2 | 0/0 | 1/1 | 0/0 | 309 |
| ROOM600 | GATE PLATFORM | NPC, Texte* | 2/0 | 0/0 | 4/4 | 8/3 | 111 305 329 382 |
| ROOM602 | SHELTER 01-02 | NPC | 2/2 | 0/0 | 2/2 | 1/1 | 75 |
| ROOM603 | TRAIN MAIN CAR | NPC, Texte* | 5/5 | 0/0 | 4/4 | 21/12 | 30 383 385 |

Deutung der Story-Spuren, Zeitleisten-Bedingungen und Gegenpruefung: `A_zensus.md` (gleicher Ordner).
Achtung Herkunft: Der Auslieferungsstand ist laut `info/Re1.5/README.TXT` Z.3/7 der fan-gemachte "Magic Zombie Door"-Umbau (MartinBiohazard) eines Team-IGAS-Builds; die Spalte MZD-README nennt die Zeilen, in denen der Mod diesen Raum erwaehnt.

## Szenario-Abfragen innerhalb der RDTs: Ck/Set auf Bank 1 Bit 0 (Elza-Bit)

Ck(1,0,x) (= Abfrage): 0 Stellen. Set(1,0,x) (= Umschalten): 12 Stellen.
- ROOM3071 sub00@0x03430 Set(1,0,0)
- ROOM3071 sub00@0x03448 Set(1,0,1)
- ROOM3071 sub00@0x03476 Set(1,0,0)
- ROOM3071 sub00@0x0348E Set(1,0,1)
- ROOM5080 sub00@0x00742 Set(1,0,1)
- ROOM5080 sub00@0x0075A Set(1,0,0)
- ROOM5090 sub00@0x01246 Set(1,0,1)
- ROOM5090 sub00@0x0125E Set(1,0,0)
- ROOM50F1 sub00@0x009A8 Set(1,0,0)
- ROOM50F1 sub00@0x009C0 Set(1,0,1)
- ROOM50F1 sub00@0x009D0 Set(1,0,0)
- ROOM50F1 sub00@0x009E8 Set(1,0,1)

## Je Raum

### ROOM100  "BATH-LOCKERS"  [Items]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 137.

Groesse L 177032 / E 177008 B; Kopf L [0 9 2 0 0 0 0] / E [0 9 2 0 0 0 0]; Skripte L 3 / E 3; Texte L 1 / E 1; Abdeckung L 340/342 / E 316/318

- **ITEM** nur Leon: sub00@0x00C24 Slot 3 Item 0x31 x1 Taken-Bit (9,134) Prop 255 sce 9
- **ITEM** nur Elza: sub00@0x00C24 Slot 3 Item 0x15 x30 Taken-Bit (9,134) Prop 255 sce 9
- **CK** nur Leon: sub01@0x00D00 Ck(9,134,1)
- **CK** nur Leon: sub01@0x00D08 Ck(3,133,0)
- **SET** nur Leon: sub01@0x00D0C Set(3,133,1)

### ROOM101  "INTERROG. ROOM"  [NPC, Items, Objekte, Texte*]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 49, 138, 164, 277, 348.

Groesse L 168808 / E 210440 B; Kopf L [0 9 3 0 0 0 0] / E [0 9 3 0 0 0 0]; Skripte L 3 / E 9; Texte L 3 / E 24; Abdeckung L 400/402 / E 1478/1478

- **EM** nur Elza: sub00@0x009F6 Typ 0x47 NPC 0x47 (Elliot/Roy?) grid 0x40 kill - (-2000,0,7450) dir 3998
- **EM** nur Elza: sub00@0x00A3E Typ 0x47 NPC 0x47 (Elliot/Roy?) grid 0x40 kill - (-2000,0,7450) dir 3998
- **ITEM** nur Leon: sub00@0x00996 Slot 2 Item 0x22 x1 Taken-Bit (9,140) Prop 0 sce 9
- **ITEM** nur Elza: main00@0x00954 Slot 2 Item 0x39 x1 Taken-Bit (9,138) Prop 0 sce 9
- **OBJ** nur Leon: sub00@0x00930 obj 0 typ 0 band 1 (200,-1600,5500) ry 3084
- **OBJ** nur Elza: main00@0x008EE obj 0 typ 0 band 1 (200,-1600,5500) ry 457
- **AOT** nur Leon: main00@0x008EE Slot 5 sce 1 flags 0x31 Text 0
- **AOT** nur Leon: main00@0x00902 Slot 6 sce 1 flags 0x31 Text 1
- **AOT** nur Leon: main00@0x00916 Slot 7 sce 1 flags 0x31 Text 2
- **AOT** nur Elza: main00@0x00996 Slot 6 sce 1 flags 0x31 Text 21
- **AOT** nur Elza: main00@0x009AA Slot 7 sce 1 flags 0x31 Text 22
- **AOT** nur Elza: main00@0x009BE Slot 8 sce 1 flags 0x31 Text 23
- **AOT** nur Elza: sub00@0x00A0A Slot 5 sce 0 flags 0x31 inert
- **AOT** nur Elza: sub00@0x00A52 Slot 5 sce 3 flags 0x31 sub06
- **AOTR** nur Elza: sub00@0x00A78 Aot_reset Slot 5 -> sce 3 sub07
- **AOTR** nur Elza: sub02@0x00DC0 Aot_reset Slot 5 -> sce 3 sub06
- **AOTR** nur Elza: sub06@0x00E0A Aot_reset Slot 5 -> sce 0 inert
- **AOTR** nur Elza: sub06@0x00E42 Aot_reset Slot 5 -> sce 3 sub07
- **AOTR** nur Elza: sub07@0x00E4E Aot_reset Slot 5 -> sce 0 inert
- **AOTR** nur Elza: sub07@0x00E78 Aot_reset Slot 5 -> sce 3 sub07
- **MSGON** nur Leon: 
- **MSGON** nur Elza: (0,) x1, (1,) x1, (2,) x1, (3,) x1, (4,) x1, (5,) x1, (6,) x1, (7,) x1, (8,) x1, (9,) x1, (10,) x1, (11,) x1, (12,) x1, (13,) x1, (14,) x1, (15,) x1, (16,) x1, (17,) x1, (18,) x1, (19,) x1, (20,) x1
- **EVT** nur Elza: sub00@0x00A1E Evt_exec sub02
- **EVT** nur Elza: sub00@0x00A22 Evt_exec sub03
- **EVT** nur Elza: sub00@0x00A66 Evt_exec sub05
- **CK** nur Elza: sub00@0x009E8 Ck(4,241,0)
- **CK** nur Elza: sub00@0x00A30 Ck(4,250,1)
- **CK** nur Elza: sub00@0x00A74 Ck(3,120,1)
- **CK** nur Elza: sub00@0x00A88 Ck(3,186,1)
- **CK** nur Elza: sub04@0x00DF2 Ck(5,0,0)
- **SET** nur Elza: sub02@0x00AFE Set(4,241,1)
- **SET** nur Elza: sub02@0x00B02 Set(4,250,1)
- **SET** nur Elza: sub02@0x00B06 Set(3,116,1)
- **SET** nur Elza: sub02@0x00B0A Set(2,7,1)
- **SET** nur Elza: sub02@0x00B0E Set(1,27,1)
- **SET** nur Elza: sub02@0x00DCA Set(2,7,0)
- **SET** nur Elza: sub02@0x00DCE Set(1,27,0)
- **SET** nur Elza: sub04@0x00DEA Set(5,0,0)
- **SET** nur Elza: sub06@0x00E2C Set(4,236,1)
- **SET** nur Elza: sub06@0x00E30 Set(3,120,1)
- **TEXT** nur Elza id 0: "Officer: groans"
- **TEXT** nur Elza id 1: "Elza: Hang in there! Are you the / only officer left in the building?"
- **TEXT** nur Elza id 2: "Officer: Who... who are you?"
- **TEXT** nur Elza id 3: "Elza: My name's Elza. / What's yours?"
- **TEXT** nur Elza id 4: "Officer: Elza, huh? I'm Roy."
- **TEXT** nur Elza id 5: "Elza: That wound... did one of / those things get you? How horrible..."
- **TEXT** nur Elza id 6: "Roy: This precinct isn't safe anymore."
- **TEXT** nur Elza id 7: "Roy: Those things broke through the / rear shutters and got inside."
- **TEXT** nur Elza id 8: "Roy: Those bastards at Umbrella / caused all of this..."
- **TEXT** nur Elza id 9: "Elza: ...What do you mean?"
- **TEXT** nur Elza id 10: "Roy: groans"
- **TEXT** nur Elza id 11: "Elza: Are you okay?"
- **TEXT** nur Elza id 12: "Roy: We don't have time to talk / right now."
- **TEXT** nur Elza id 13: "Roy: You have to rescue the survivors / in the other rooms."
- **TEXT** nur Elza id 14: "Elza: But..."
- **TEXT** nur Elza id 15: "Roy: My friend John should still be / in the underground cell block."
- **TEXT** nur Elza id 16: "Roy: Please, you have to help him..."
- **TEXT** nur Elza id 17: "Elza: Roy, wake up! Please wake up!"
- **TEXT** nur Elza id 18: "He has passed out from his injuries.He is holding a slip.The numbers 0513" are / printed on the slip."
- **TEXT** nur Elza id 19: "He is holding something..."
- **TEXT** nur Elza id 20: "You've taken the Prison key."

### ROOM102  "MAIN OFFICE"  [NPC, Texte*]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 101, 204, 265, 357.

Groesse L 228992 / E 274480 B; Kopf L [0 13 7 0 0 0 0] / E [0 13 7 0 0 0 0]; Skripte L 9 / E 13; Texte L 6 / E 15; Abdeckung L 908/910 / E 1534/1538

- **EM** nur Elza: main00@0x01DF6 Typ 0x49 NPC 0x49 (Kendo?) grid 0x40 kill - (-4274,0,-6590) dir 1024
- **EM** nur Elza: main00@0x01E0A Typ 0x47 NPC 0x47 (Elliot/Roy?) grid 0x40 kill - (-4614,0,-16919) dir 3072
- **AOT** nur Leon: main00@0x01DDA Slot 5 sce 3 flags 0x31 sub02
- **AOT** nur Leon: main00@0x01E18 Slot 6 sce 3 flags 0x41 sub03
- **AOT** nur Leon: main00@0x01ED2 Slot 7 sce 1 flags 0x31 Text 1
- **AOT** nur Leon: main00@0x01EE6 Slot 8 sce 1 flags 0x31 Text 1
- **AOT** nur Leon: main00@0x01EFA Slot 9 sce 1 flags 0x31 Text 2
- **AOT** nur Leon: main00@0x01F0E Slot 10 sce 1 flags 0x31 Text 3
- **AOT** nur Leon: main00@0x01F22 Slot 11 sce 1 flags 0x31 Text 5
- **AOT** nur Leon: main00@0x01F36 Slot 12 sce 1 flags 0x31 Text 5
- **AOT** nur Elza: main00@0x01DE2 Slot 5 sce 3 flags 0x41 sub02
- **AOT** nur Elza: main00@0x01E2C Slot 6 sce 3 flags 0x31 sub06
- **AOT** nur Elza: main00@0x01E74 Slot 7 sce 3 flags 0x41 sub07
- **AOT** nur Elza: main00@0x01F30 Slot 8 sce 1 flags 0x31 Text 10
- **AOT** nur Elza: main00@0x01F44 Slot 9 sce 1 flags 0x31 Text 10
- **AOT** nur Elza: main00@0x01F58 Slot 10 sce 1 flags 0x31 Text 11
- **AOT** nur Elza: main00@0x01F6C Slot 11 sce 1 flags 0x31 Text 12
- **AOT** nur Elza: main00@0x01F80 Slot 12 sce 1 flags 0x31 Text 14
- **AOT** nur Elza: main00@0x01F94 Slot 13 sce 1 flags 0x31 Text 14
- **AOTR** nur Leon: sub02@0x01F9A Aot_reset Slot 5 -> sce 3 sub02
- **AOTR** nur Elza: sub06@0x02216 Aot_reset Slot 6 -> sce 3 sub06
- **AOTR** nur Elza: sub07@0x02226 Aot_reset Slot 7 -> sce 0 inert
- **MSGON** nur Elza: sub02@0x02038 Message_on 1
- **MSGON** nur Elza: sub02@0x0205E Message_on 2
- **MSGON** nur Elza: sub02@0x02082 Message_on 3
- **MSGON** nur Elza: sub02@0x02122 Message_on 4
- **MSGON** nur Elza: sub02@0x0213A Message_on 5
- **MSGON** nur Elza: sub02@0x0215A Message_on 6
- **MSGON** nur Elza: sub02@0x02162 Message_on 7
- **MSGON** nur Elza: sub02@0x021A4 Message_on 8
- **MSGON** nur Elza: sub06@0x02202 Message_on 9
- **EVT** nur Leon: sub00@0x01F5C Evt_exec sub04
- **EVT** nur Leon: sub00@0x01F60 Evt_exec sub05
- **EVT** nur Leon: sub00@0x01F64 Evt_exec sub06
- **EVT** nur Leon: sub00@0x01F68 Evt_exec sub07
- **EVT** nur Elza: main00@0x01E1E Evt_exec sub03
- **EVT** nur Elza: sub00@0x01FC4 Evt_exec sub08
- **EVT** nur Elza: sub00@0x01FC8 Evt_exec sub09
- **EVT** nur Elza: sub00@0x01FCC Evt_exec sub10
- **EVT** nur Elza: sub00@0x01FD0 Evt_exec sub11
- **CK** nur Elza: main00@0x01DDE Ck(4,96,1)
- **CK** nur Elza: main00@0x01E28 Ck(3,95,1)
- **CK** nur Elza: main00@0x01E68 Ck(3,201,0)
- **CK** nur Elza: sub04@0x021D8 Ck(5,32,0)
- **CK** nur Elza: sub05@0x021E6 Ck(5,33,0)
- **SET** nur Elza: sub02@0x01FD8 Set(4,96,0)
- **SET** nur Elza: sub02@0x01FDC Set(3,201,0)
- **SET** nur Elza: sub02@0x01FE0 Set(4,240,1)
- **SET** nur Elza: sub02@0x01FE4 Set(3,95,1)
- **SET** nur Elza: sub02@0x01FF2 Set(2,7,1)
- **SET** nur Elza: sub02@0x01FF6 Set(1,27,1)
- **SET** nur Elza: sub02@0x021B4 Set(2,7,0)
- **SET** nur Elza: sub02@0x021B8 Set(1,27,0)
- **TEXT** nur Leon id 0: "It's the RPD officer, Roy.He appears to have died recently."
- **TEXT** nur Elza id 0: "John: No way, this can't be... Roy, this / is a joke, right?"
- **TEXT** nur Elza id 1: "Roy: John... Help me... Please kill me."
- **TEXT** nur Elza id 2: "Elza: John, shoot! That thing isn't / Roy anymore!"
- **TEXT** nur Elza id 3: "John: Roy, it's me... You do recognize / me, right?"
- **TEXT** nur Elza id 4: "Elza: John, I couldn't call for help / using the radio."
- **TEXT** nur Elza id 5: "Elza: The building's electric power / is starting to fail..."
- **TEXT** nur Elza id 6: "John: What am I supposed to do alone? / I don't get it!"
- **TEXT** nur Elza id 7: "Elza: We're the only ones left. If we / don't do anything, we'll also be..."
- **TEXT** nur Elza id 8: "John: Please... I... I'd like to be / alone now..."
- **TEXT** nur Elza id 9: "He's dead.Poor Roy..."

### ROOM103  "LOBBY"  [Tueren, Texte*]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 32, 161, 307, 332.

Groesse L 191528 / E 219120 B; Kopf L [0 13 1 0 0 0 0] / E [0 14 1 0 0 0 0]; Skripte L 13 / E 17; Texte L 18 / E 24; Abdeckung L 3092/3094 / E 3518/3522

- **DOOR** nur Elza: main00@0x02052 Slot 18 sce 2(Tuer) -> ROOM124 Cut 0
- **DOOR** nur Elza: main00@0x02082 Slot 19 sce 2(Tuer) -> ROOM103 Cut 6
- **AOT** nur Leon: main00@0x01D6A Slot 11 sce 1 flags 0x31 Text 8
- **AOT** nur Elza: main00@0x01DD8 Slot 11 sce 1 flags 0x31 Text 7
- **MSGON** nur Elza: sub13@0x02A08 Message_on 18
- **MSGON** nur Elza: sub13@0x02A24 Message_on 19
- **MSGON** nur Elza: sub15@0x02A7A Message_on 20
- **MSGON** nur Elza: sub15@0x02A82 Message_on 21
- **MSGON** nur Elza: sub15@0x02A8A Message_on 22
- **MSGON** nur Elza: sub15@0x02A92 Message_on 23
- **EVT** nur Elza: main00@0x02072 Evt_exec sub12
- **EVT** nur Elza: main00@0x020A2 Evt_exec sub15
- **EVT** nur Elza: main00@0x020B2 Evt_exec sub13
- **CK** nur Elza: main00@0x01CC6 Ck(3,125,1)
- **CK** nur Elza: main00@0x01E54 Ck(3,125,1)
- **CK** nur Elza: main00@0x0204E Ck(3,193,0)
- **CK** nur Elza: main00@0x0207E Ck(3,125,0)
- **CK** nur Elza: main00@0x020AE Ck(3,207,1)
- **CK** nur Elza: sub14@0x02A62 Ck(5,32,0)
- **SET** nur Leon: sub00@0x01FE8 Set(3,116,1)
- **SET** nur Elza: sub08@0x02850 Set(2,7,1)
- **SET** nur Elza: sub08@0x02854 Set(1,27,1)
- **SET** nur Elza: sub08@0x028BE Set(2,7,0)
- **SET** nur Elza: sub08@0x028C2 Set(1,27,0)
- **SET** nur Elza: sub12@0x02976 Set(3,193,1)
- **SET** nur Elza: sub13@0x02982 Set(3,207,0)
- **SET** nur Elza: sub13@0x02986 Set(2,7,1)
- **SET** nur Elza: sub15@0x02A68 Set(3,125,1)
- **SET** nur Elza: sub15@0x02A6C Set(3,207,1)
- **TEXT** nur Elza id 18: "Elza: That was close, this must / contain them for some time..."
- **TEXT** nur Elza id 19: "Elza: Hey, is anyone here?!"
- **TEXT** nur Elza id 20: "We barricaded ourselves / inside the police station..."
- **TEXT** nur Elza id 21: "Where we thought it would / be safe."
- **TEXT** nur Elza id 22: "It was no use, and so began a / life and death struggle..."
- **TEXT** nur Elza id 23: "To escape."

### ROOM104  "WEST CORRIDOR"  [Texte*]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 276.

Groesse L 213520 / E 213700 B; Kopf L [0 7 4 0 0 0 0] / E [0 7 4 0 0 0 0]; Skripte L 12 / E 13; Texte L 2 / E 4; Abdeckung L 4404/4404 / E 4528/4530

- **AOT** nur Elza: main00@0x01178 Slot 8 sce 3 flags 0x41 sub11
- **AOTR** nur Elza: sub11@0x021EE Aot_reset Slot 8 -> sce 0 inert
- **MSGON** nur Elza: sub11@0x02200 Message_on 3
- **MSGON** nur Elza: sub11@0x02224 Message_on 2
- **CK** nur Elza: main00@0x0116C Ck(4,241,0)
- **CK** nur Elza: main00@0x01174 Ck(3,173,0)
- **CK** nur Elza: sub11@0x0221C Ck(5,32,0)
- **SET** nur Elza: sub08@0x019D4 Set(1,27,1)
- **SET** nur Elza: sub08@0x019D8 Set(2,7,1)
- **SET** nur Elza: sub08@0x01A56 Set(1,27,0)
- **SET** nur Elza: sub08@0x01A5A Set(2,7,0)
- **SET** nur Elza: sub11@0x021EA Set(3,173,1)
- **TEXT** nur Elza id 2: "Elza: What was that!?"
- **TEXT** nur Elza id 3: "gunshot"

### ROOM105  "EAST CORRIDOR"  [NPC, Items, Texte*]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 105, 139, 169, 247, 271, 333.

Groesse L 297324 / E 243976 B; Kopf L [0 10 2 0 0 0 0] / E [0 10 2 0 0 0 0]; Skripte L 6 / E 5; Texte L 9 / E 6; Abdeckung L 854/856 / E 756/758

- **EM** nur Leon: sub00@0x00C8E Typ 0x42 NPC 0x42 (Ada) grid 0x40 kill - (18550,0,-22300) dir 3072
- **ITEM** nur Elza: main00@0x00C22 Slot 12 Item 0x04 x15 Taken-Bit (9,165) Prop 255 sce 9
- **AOT** nur Elza: main00@0x00C0E Slot 11 sce 3 flags 0x31 sub03
- **AOTR** nur Elza: sub01@0x00CBA Aot_reset Slot 11 -> sce 0 inert
- **MSGON** nur Leon: sub03@0x00DD6 Message_on 8
- **MSGON** nur Elza: sub03@0x00DB6 Message_on 5
- **EVT** nur Leon: sub00@0x00CA2 Evt_exec sub03
- **EVT** nur Leon: sub03@0x00DB6 Evt_exec sub04
- **CK** nur Leon: sub00@0x00C8A Ck(3,110,1)
- **CK** nur Leon: sub03@0x00DD2 Ck(5,32,0)
- **CK** nur Leon: sub04@0x00E14 Ck(5,33,0)
- **CK** nur Leon: sub04@0x00E28 Ck(5,33,0)
- **CK** nur Elza: sub01@0x00CB6 Ck(9,165,1)
- **SET** nur Leon: sub02@0x00CCC Set(1,27,1)
- **SET** nur Leon: sub02@0x00D78 Set(1,27,0)
- **SET** nur Leon: sub03@0x00D88 Set(3,110,0)
- **TEXT** nur Leon id 6 (UNBENUTZT): "Ada: Give me a moment, I need a / break."
- **TEXT** nur Leon id 7 (UNBENUTZT): "Leon: Relax, I'll take you to the / medical room."
- **TEXT** nur Leon id 8: "Leon: Hey, wait!"

### ROOM106  "UPPER STAIRS"  

Byte-identisch (149428 B).

### ROOM107  "LOBBY OFFICE"  

Byte-identisch (153328 B).

### ROOM108  "ELEVATOR"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 50, 140.

Byte-identisch (177696 B).

### ROOM109  "REAR EXTERIOR"  [NPC, Gegner, Tueren, Objekte, Texte*]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 104, 109, 117, 187, 209, 266, 279, 337, 358.

Groesse L 336440 / E 282404 B; Kopf L [0 16 4 0 0 0 0] / E [0 16 4 0 0 0 0]; Skripte L 9 / E 4; Texte L 10 / E 0; Abdeckung L 1558/1558 / E 292/296

- **EM** nur Leon: sub00@0x02214 Typ 0x26 0x26 (Feuer?) grid 0x00 kill - (2052,-1800,-1334) dir 1024
- **EM** nur Leon: sub00@0x02228 Typ 0x26 0x26 (Feuer?) grid 0x01 kill - (2375,-1800,-2333) dir 1024
- **EM** nur Leon: sub00@0x0223C Typ 0x26 0x26 (Feuer?) grid 0x02 kill - (3104,-1800,-3498) dir 1024
- **EM** nur Leon: sub00@0x02250 Typ 0x26 0x26 (Feuer?) grid 0x04 kill - (2974,-1800,-860) dir 1024
- **EM** nur Leon: sub00@0x02264 Typ 0x26 0x26 (Feuer?) grid 0x03 kill - (2874,-1800,440) dir 1024
- **EM** nur Leon: sub00@0x02278 Typ 0x26 0x26 (Feuer?) grid 0x03 kill - (2774,-1800,2040) dir 1024
- **EM** nur Leon: sub00@0x0228C Typ 0x26 0x26 (Feuer?) grid 0x04 kill - (1374,-1800,1340) dir 1024
- **EM** nur Leon: sub00@0x022CC Typ 0x42 NPC 0x42 (Ada) grid 0x40 kill - (-30000,0,-30000) dir -25608
- **DOOR** nur Leon: sub00@0x02352 Slot 3 sce 2(Tuer) -> ROOM109 Cut 6
- **OBJ** nur Leon: sub00@0x021F2 obj 3 typ 0 band 2 (3060,-2754,-1560) ry 1878
- **OBJ** nur Leon: sub00@0x022AA obj 2 typ 0 band 2 (3060,-2754,-1560) ry 1878
- **AOT** nur Leon: main00@0x0215A Slot 3 sce 3 flags 0x41 sub07
- **AOT** nur Leon: sub00@0x0230E Slot 2 sce 1 flags 0xB1 Text 7
- **AOT** nur Leon: sub00@0x02336 Slot 2 sce 3 flags 0xB1 sub06
- **AOT** nur Elza: main00@0x0215A Slot 2 sce 3 flags 0x41 sub02
- **AOTR** nur Leon: sub07@0x0272C Aot_reset Slot 3 -> sce 0 inert
- **AOTR** nur Elza: sub02@0x02232 Aot_reset Slot 2 -> sce 0 inert
- **BGM** nur Leon: sub00@0x022EE op54 0000017833
- **BGM** nur Leon: sub00@0x022FC op54 0001000000
- **BGM** nur Leon: sub02@0x02440 op54 0001000000
- **BGM** nur Leon: sub03@0x024DA op54 0000010141
- **BGM** nur Leon: sub03@0x024E0 op54 0002000000
- **MSGON** nur Leon: sub02@0x02438 Message_on 0
- **MSGON** nur Leon: sub02@0x0248A Message_on 1
- **MSGON** nur Leon: sub02@0x024B6 Message_on 2
- **MSGON** nur Leon: sub03@0x02502 Message_on 9
- **MSGON** nur Leon: sub03@0x02586 Message_on 3
- **MSGON** nur Leon: sub03@0x025E8 Message_on 4
- **MSGON** nur Leon: sub03@0x02640 Message_on 5
- **MSGON** nur Leon: sub03@0x02670 Message_on 6
- **MSGON** nur Leon: sub06@0x02702 Message_on 7
- **MSGON** nur Leon: sub06@0x02708 Message_on 8
- **EVT** nur Leon: sub00@0x022E0 Evt_exec sub03
- **EVT** nur Leon: sub01@0x02406 Evt_exec sub02
- **CK** nur Leon: sub00@0x021C8 Ck(3,129,0)
- **CK** nur Leon: sub00@0x021EE Ck(3,129,0)
- **CK** nur Leon: sub00@0x022A6 Ck(3,132,1)
- **CK** nur Leon: sub00@0x022EA Ck(3,129,0)
- **CK** nur Leon: sub00@0x022F8 Ck(3,128,1)
- **CK** nur Leon: sub00@0x0230A Ck(3,133,0)
- **CK** nur Leon: sub00@0x02332 Ck(3,129,0)
- **CK** nur Leon: sub00@0x0237A Ck(3,187,1)
- **CK** nur Leon: sub01@0x023F8 Ck(3,128,0)
- **CK** nur Leon: sub04@0x026EE Ck(5,32,0)
- **CK** nur Leon: sub05@0x026FC Ck(5,33,0)
- **CK** nur Leon: sub06@0x02712 Ck(12,31,0)
- **SET** nur Leon: (1, 27, 0) x2, (1, 27, 1) x2, (2, 7, 0) x2, (2, 7, 1) x3, (3, 110, 1) x1, (3, 128, 1) x1, (3, 129, 1) x1, (3, 132, 0) x1, (3, 132, 1) x1, (3, 187, 1) x1
- **SET** nur Elza: 
- **TEXT** nur Leon id 0: "Woman: Anyone! Can someone please / get me out of here!?"
- **TEXT** nur Leon id 1: "Leon: Hey! Who's that?!"
- **TEXT** nur Leon id 2: "Woman: Someone, anyone! If out there, / open the door!"
- **TEXT** nur Leon id 3: "Leon: There! Are you okay?"
- **TEXT** nur Leon id 4: "Woman: You saved me... Thank you, / you have my gratitude."
- **TEXT** nur Leon id 5: "Leon: We can talk later... It's not safe / here."
- **TEXT** nur Leon id 6: "Leon: Besides, you need to get that / wound treated, right? Let's go."
- **TEXT** nur Leon id 7: "I must hurry up and get something to / put out this fire to save that woman!"
- **TEXT** nur Leon id 8: "Will you use the Fire Extinguisher? "
- **TEXT** nur Leon id 9: "You've used the Fire Extinguisher."

### ROOM10A  "LOWER STAIRS"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 215.

Byte-identisch (101164 B).

### ROOM10B  "FRONT EXTERIOR"  [NPC, Texte*]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 26, 27, 359.

Groesse L 93616 / E 139080 B; Kopf L [0 9 0 0 0 0 0] / E [0 9 0 0 0 0 0]; Skripte L 3 / E 8; Texte L 0 / E 6; Abdeckung L 170/170 / E 974/976

- **EM** nur Elza: main00@0x01726 Typ 0x49 NPC 0x49 (Kendo?) grid 0x40 kill - (-30000,0,-30000) dir 1040
- **MSGON** nur Elza: sub04@0x0199E Message_on 0
- **MSGON** nur Elza: sub04@0x019B6 Message_on 1
- **MSGON** nur Elza: sub04@0x019DA Message_on 2
- **MSGON** nur Elza: sub04@0x019FE Message_on 3
- **MSGON** nur Elza: sub04@0x01A2A Message_on 4
- **MSGON** nur Elza: sub04@0x01A42 Message_on 5
- **EVT** nur Elza: sub00@0x017BA Evt_exec sub02
- **EVT** nur Elza: sub00@0x017EA Evt_exec sub03
- **EVT** nur Elza: sub01@0x01824 Evt_exec sub04
- **EVT** nur Elza: sub02@0x018AC Evt_exec sub03
- **CK** nur Elza: sub00@0x0179E Ck(3,128,1)
- **CK** nur Elza: sub00@0x017C6 Ck(3,129,1)
- **CK** nur Elza: sub00@0x017F6 Ck(3,130,1)
- **CK** nur Elza: sub01@0x01816 Ck(3,130,1)
- **CK** nur Elza: sub03@0x018D6 Ck(5,32,0)
- **CK** nur Elza: sub05@0x01A9A Ck(5,32,0)
- **CK** nur Elza: sub06@0x01AA8 Ck(5,33,0)
- **SET** nur Elza: sub02@0x0182E Set(3,128,0)
- **SET** nur Elza: sub02@0x01832 Set(3,129,1)
- **SET** nur Elza: sub03@0x01954 Set(3,129,0)
- **SET** nur Elza: sub04@0x0195A Set(3,130,0)
- **SET** nur Elza: sub04@0x0195E Set(3,131,1)
- **SET** nur Elza: sub04@0x01962 Set(3,65,1)
- **SET** nur Elza: sub04@0x01966 Set(2,7,1)
- **SET** nur Elza: sub04@0x0196A Set(1,27,1)
- **TEXT** nur Elza id 0: "Elza: John, wait!"
- **TEXT** nur Elza id 1: "Elza: There's still a little girl / trapped in there!"
- **TEXT** nur Elza id 2: "John: Say what!?"
- **TEXT** nur Elza id 3: "Elza: I found her in the basement / garage! We can't just leave her behind!"
- **TEXT** nur Elza id 4: "John: The garage in the basement, / huh...?"
- **TEXT** nur Elza id 5: "John: If that's the case, we can go / through the sewer tunnels. Follow me."

### ROOM10C  "2F WEST SIDE"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 51, 141, 159, 248, 389.

Byte-identisch (87376 B).

### ROOM10D  "2F CORRIDOR"  [NPC, Texte*]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 52, 142, 170, 198.

Groesse L 286320 / E 240364 B; Kopf L [0 15 2 0 0 0 0] / E [0 15 2 0 0 0 0]; Skripte L 24 / E 23; Texte L 28 / E 12; Abdeckung L 3576/3576 / E 2646/2646

- **EM** nur Leon: sub00@0x01256 Typ 0x40 NPC 0x40 (Marvin?) grid 0x40 kill - (5775,0,23625) dir 1142
- **MSGON** nur Leon: (12,) x1, (13,) x1, (14,) x1, (15,) x1, (16,) x1, (17,) x1, (18,) x1, (19,) x1, (20,) x1, (21,) x1, (22,) x1, (23,) x1, (24,) x1, (25,) x1, (26,) x1, (27,) x1
- **MSGON** nur Elza: 
- **EVT** nur Leon: sub21@0x01A24 Evt_exec sub22
- **TEXT** nur Leon id 11: "It's a shutter switch. / Will you push it? "
- **TEXT** nur Leon id 12: "???: Freeze!"
- **TEXT** nur Leon id 13: "Leon: Wait don't shoot!"
- **TEXT** nur Leon id 14: "Marvin: Leon... You're still alive!"
- **TEXT** nur Leon id 15: "Leon: I'm glad to see that you're / still alive Marvin."
- **TEXT** nur Leon id 16: "Leon: Where are the rest of the / officers?"
- **TEXT** nur Leon id 17: "Leon: Has everyone been eaten by / zombies? or have they turned into / zombies?"
- **TEXT** nur Leon id 18: "Marvin: You've already seen him, / I thought he was the only one left at"
- **TEXT** nur Leon id 19: "the station, tell me, are there other / survivors?"
- **TEXT** nur Leon id 20: "Leon: You are the only one who I found, / the chief is injured, one of those / things bit him."
- **TEXT** nur Leon id 21: "Marvin: Damn, the situation is terrible, / we have to find a way out of here."
- **TEXT** nur Leon id 22: "Leon: The chief told me about using the / communication system and asking for / outside help."
- **TEXT** nur Leon id 23: "Marvin: But the system is damaged, if / the other officers could not repair it / as only we can?"
- **TEXT** nur Leon id 24: "Leon: I don't know, it remains to try, / the helicopter abandoned us, the phones"
- **TEXT** nur Leon id 25: "are down and with the building besieged / by those things, I don't see any more / options."
- **TEXT** nur Leon id 26: "Marvin: Okay, I'll meet with the chief / first and we'll start looking for a way / to fix it."
- **TEXT** nur Leon id 27: "Leon: I agree, he's in his office."
- **TEXT** nur Elza id 11: "It's a shutter switch. / Will you push it?         /   ,      3   "

### ROOM10E  "MED. EXAM ROOM"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 143, 264.

Byte-identisch (157632 B).

### ROOM10F  "COMMUNIC. ROOM"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 144, 266.

Byte-identisch (225092 B).

### ROOM110  "EVID. CORRIDOR"  

Byte-identisch (181704 B).

### ROOM111  "EVIDENCE ROOM"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 107, 145, 165.

Byte-identisch (196360 B).

### ROOM112  "3F WEST SIDE"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 146.

Byte-identisch (86900 B).

### ROOM113  "3F CORRIDOR"  [Objekte]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 147, 171, 256, 270.

Groesse L 198108 / E 198104 B; Kopf L [0 10 1 0 0 0 0] / E [0 10 1 0 0 0 0]; Skripte L 4 / E 4; Texte L 6 / E 6; Abdeckung L 622/624 / E 618/620

- **OBJ** nur Leon: main00@0x0092A obj 0 typ 0 band 1 (-1240,0,16300) ry 3072
- **AOTR** nur Leon: sub01@0x00A1C Aot_reset Slot 3 -> sce 1 Text 1
- **CK** nur Leon: sub00@0x009D2 Ck(3,107,1)
- **CK** nur Elza: main00@0x00912 Ck(3,107,0)

### ROOM114  "BRIEFING ROOM"  [NPC, Gegner, Items, Texte*]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 53, 148, 221, 249, 269, 285, 338.

Groesse L 147176 / E 194568 B; Kopf L [0 10 1 0 0 0 0] / E [0 10 1 0 0 0 0]; Skripte L 7 / E 8; Texte L 6 / E 15; Abdeckung L 798/802 / E 1658/1660

- **EM** nur Leon: sub00@0x00BAA Typ 0x16 Zivil-Zombie grid 0x88 kill 211 (-800,0,-20600) dir 1024
- **EM** nur Leon: sub00@0x00BBE Typ 0x10 Polizei-Zombie grid 0x86 kill 212 (-1800,0,-19600) dir 512
- **EM** nur Leon: sub00@0x00BD2 Typ 0x10 Polizei-Zombie grid 0x86 kill 213 (-1800,0,-21600) dir 3584
- **EM** nur Leon: sub00@0x00BE6 Typ 0x11 Polizei-Zombie dick grid 0x86 kill 214 (200,0,-21600) dir 2560
- **EM** nur Leon: sub00@0x00BFA Typ 0x11 Polizei-Zombie dick grid 0x86 kill 215 (200,0,-19600) dir 1536
- **EM** nur Leon: sub00@0x00C12 Typ 0x16 Zivil-Zombie grid 0x02 kill 211 (2850,0,-10300) dir 0
- **EM** nur Leon: sub00@0x00C26 Typ 0x10 Polizei-Zombie grid 0x02 kill 212 (-6650,0,-10300) dir 1024
- **EM** nur Leon: sub00@0x00C3A Typ 0x10 Polizei-Zombie grid 0x02 kill 213 (-5000,0,-19750) dir 2560
- **EM** nur Leon: sub00@0x00C4E Typ 0x11 Polizei-Zombie dick grid 0x02 kill 214 (9200,0,-14450) dir 2048
- **EM** nur Leon: sub00@0x00C62 Typ 0x11 Polizei-Zombie dick grid 0x02 kill 215 (5800,0,-19150) dir 1024
- **EM** nur Elza: sub00@0x00B06 Typ 0x49 NPC 0x49 (Kendo?) grid 0x40 kill - (-6750,0,-13350) dir 0
- **EM** nur Elza: sub00@0x00B1A Typ 0x27 Affe/Gorilla grid 0x30 kill 203 (10150,0,8300) dir 2048
- **EM** nur Elza: sub00@0x00B2E Typ 0x27 Affe/Gorilla grid 0x30 kill 204 (10150,0,12200) dir 2048
- **EM** nur Elza: sub00@0x00B54 Typ 0x27 Affe/Gorilla grid 0x10 kill 203 (-6600,0,-9800) dir 1024
- **EM** nur Elza: sub00@0x00B68 Typ 0x27 Affe/Gorilla grid 0x10 kill 204 (-5550,0,-19750) dir 2048
- **ITEM** nur Leon: sub00@0x00B94 Slot 7 Item 0x15 x30 Taken-Bit (9,168) Prop 255 sce 9
- **AOT** nur Leon: main00@0x00A9A Slot 2 sce 3 flags 0x31 sub02
- **AOT** nur Leon: main00@0x00AD4 Slot 3 sce 1 flags 0x31 Text 2
- **AOT** nur Leon: main00@0x00AE8 Slot 4 sce 1 flags 0x31 Text 4
- **AOT** nur Leon: sub00@0x00B14 Slot 5 sce 3 flags 0x31 sub03
- **AOT** nur Leon: sub00@0x00B80 Slot 6 sce 3 flags 0x41 sub04
- **AOT** nur Elza: main00@0x00A9A Slot 2 sce 3 flags 0x31 sub05
- **AOT** nur Elza: main00@0x00AD4 Slot 4 sce 1 flags 0x31 Text 14
- **AOT** nur Elza: sub00@0x00B88 Slot 3 sce 3 flags 0x31 sub06
- **AOTR** nur Leon: sub01@0x00C7A Aot_reset Slot 5 -> sce 3 sub05
- **AOTR** nur Leon: sub03@0x00CB8 Aot_reset Slot 5 -> sce 0 inert
- **AOTR** nur Leon: sub04@0x00D52 Aot_reset Slot 6 -> sce 0 inert
- **AOTR** nur Elza: sub06@0x00FFA Aot_reset Slot 3 -> sce 0 inert
- **BGM** nur Elza: sub00@0x00B4E op54 0001000000
- **BGM** nur Elza: sub01@0x00C06 op54 0002000000
- **BGM** nur Elza: sub02@0x00E5C op54 0001000000
- **MSGON** nur Leon: sub03@0x00CA6 Message_on 1
- **MSGON** nur Elza: sub02@0x00D0A Message_on 2
- **MSGON** nur Elza: sub02@0x00D42 Message_on 3
- **MSGON** nur Elza: sub02@0x00D7A Message_on 4
- **MSGON** nur Elza: sub02@0x00DCA Message_on 6
- **MSGON** nur Elza: sub02@0x00E14 Message_on 7
- **MSGON** nur Elza: sub02@0x00E64 Message_on 8
- **MSGON** nur Elza: sub02@0x00EBC Message_on 9
- **MSGON** nur Elza: sub02@0x00EEC Message_on 10
- **MSGON** nur Elza: sub02@0x00EFC Message_on 11
- **MSGON** nur Elza: sub05@0x00FD6 Message_on 12
- **MSGON** nur Elza: sub06@0x00FE8 Message_on 13
- **EVT** nur Elza: sub00@0x00B02 Evt_exec sub02
- **CK** nur Leon: (3, 210, 0) x1, (12, 31, 0) x1
- **CK** nur Elza: (4, 240, 1) x1, (4, 242, 1) x2, (5, 0, 0) x1, (5, 1, 0) x1, (7, 203, 1) x1, (7, 204, 1) x1, (8, 203, 0) x2, (8, 204, 0) x2
- **SET** nur Leon: sub04@0x00D60 Set(3,210,1)
- **SET** nur Elza: sub01@0x00C0C Set(4,242,0)
- **SET** nur Elza: sub02@0x00C18 Set(4,240,0)
- **SET** nur Elza: sub02@0x00C1C Set(4,242,1)
- **SET** nur Elza: sub02@0x00C20 Set(3,194,1)
- **SET** nur Elza: sub02@0x00C26 Set(2,7,1)
- **SET** nur Elza: sub02@0x00C2A Set(1,27,1)
- **SET** nur Elza: sub02@0x00F4E Set(2,7,0)
- **SET** nur Elza: sub02@0x00F52 Set(1,27,0)
- **SET** nur Elza: sub03@0x00FA4 Set(5,0,0)
- **SET** nur Elza: sub04@0x00FB6 Set(5,1,0)
- **TEXT** nur Leon id 3 (UNBENUTZT): "The streets are infested with / zombies!"
- **TEXT** nur Leon id 4: "The streets are infested with / zombies!"
- **TEXT** nur Leon id 5: "The switch is not responding..."
- **TEXT** nur Elza id 0: "Elza: John, wait!"
- **TEXT** nur Elza id 1: "John: Elza! sorry, I'm glad to see / you're still alive..."
- **TEXT** nur Elza id 2: "Elza: Same here... are you okay? / what's happened?"
- **TEXT** nur Elza id 3: "John: Somehow... Elza, listen to me!"
- **TEXT** nur Elza id 4: "Elza: What is wrong?"
- **TEXT** nur Elza id 5: "John: Don't go to the rooftop! / ...there's something...a somewhat / dangerous monster!"
- **TEXT** nur Elza id 6: "Elza: What was that?!"
- **TEXT** nur Elza id 7: "John: Shit! that is what I was / talking about!"
- **TEXT** nur Elza id 8: "Elza: what the hell is that?!"
- **TEXT** nur Elza id 9: "John: Looks like some kind of gorilla, / that would explain how they managed / to enter through the windows!"
- **TEXT** nur Elza id 10: "Elza: John! what will we do now?!"
- **TEXT** nur Elza id 11: "John: Elza, I'll try to distract them, / while you try to get out of here, hurry!"

### ROOM115  "CHIEF OFFICE"  [Texte*]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 216.

Groesse L 194080 / E 202392 B; Kopf L [0 9 4 0 0 0 0] / E [0 9 4 0 0 0 0]; Skripte L 10 / E 9; Texte L 15 / E 4; Abdeckung L 1404/1408 / E 886/886

- **AOT** nur Leon: main00@0x00DEA Slot 6 sce 3 flags 0x41 sub08
- **AOTR** nur Leon: sub08@0x01106 Aot_reset Slot 6 -> sce 0 inert
- **MSGON** nur Leon: sub08@0x0117E Message_on 4
- **MSGON** nur Leon: sub08@0x01196 Message_on 5
- **MSGON** nur Leon: sub08@0x0119E Message_on 6
- **MSGON** nur Leon: sub08@0x011B6 Message_on 7
- **MSGON** nur Leon: sub08@0x011DE Message_on 8
- **MSGON** nur Leon: sub08@0x011FA Message_on 9
- **MSGON** nur Leon: sub08@0x01236 Message_on 10
- **MSGON** nur Leon: sub08@0x01252 Message_on 12
- **MSGON** nur Leon: sub08@0x01262 Message_on 13
- **MSGON** nur Leon: sub08@0x0127E Message_on 14
- **CK** nur Leon: main00@0x00DE6 Ck(3,94,0)
- **CK** nur Leon: sub08@0x01132 Ck(5,32,0)
- **CK** nur Leon: sub08@0x01146 Ck(5,32,0)
- **CK** nur Leon: sub08@0x0115A Ck(5,32,0)
- **SET** nur Leon: sub03@0x00EE4 Set(2,7,1)
- **SET** nur Leon: sub03@0x00EE8 Set(1,27,1)
- **SET** nur Leon: sub03@0x00F8C Set(2,7,0)
- **SET** nur Leon: sub03@0x00F90 Set(1,27,0)
- **SET** nur Leon: sub08@0x01110 Set(3,94,1)
- **TEXT** nur Leon id 4: "Leon: Sir... the helicopter is / gone... it was our only way out..."
- **TEXT** nur Leon id 5: "Irons: Calm down Rookie, there / must be a way."
- **TEXT** nur Leon id 6: "Leon: The building is besieged by / those things, I don't see any option."
- **TEXT** nur Leon id 7: "Irons: Maybe using the communication / system... you can ask for outside help."
- **TEXT** nur Leon id 8: "Leon: But the system is damaged."
- **TEXT** nur Leon id 9: "Irons: I know, I know... but some / officers were trying to repair it, / you may be luckier than them."
- **TEXT** nur Leon id 10: "Leon: Ok, I'll go see if I can / achieve something with that."
- **TEXT** nur Leon id 11 (UNBENUTZT): "Irons: Take this card... it will allow / you to enter the communications room... / now go..."
- **TEXT** nur Leon id 12: "Irons: And remember this Leon, never / let your guard down... uniform or not,"
- **TEXT** nur Leon id 13: "those things... are no longer what / they were, don't make my mistake..."
- **TEXT** nur Leon id 14: "Leon: Yes sir..."

### ROOM116  "KENNEL"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 54, 197, 360.

Byte-identisch (69944 B).

### ROOM117  "HELIPORT"  [NPC, Gegner, Tueren, Objekte, Texte*]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 103, 160.

Groesse L 303564 / E 161300 B; Kopf L [0 13 6 0 0 0 0] / E [0 13 2 0 0 0 0]; Skripte L 17 / E 9; Texte L 17 / E 5; Abdeckung L 1794/1794 / E 1460/1462

- **EM** nur Leon: sub15@0x017D2 Typ 0x47 NPC 0x47 (Elliot/Roy?) grid 0x40 kill - (1261,-7200,10091) dir 1199
- **EM** nur Elza: sub07@0x016C8 Typ 0x49 NPC 0x49 (Kendo?) grid 0x00 kill - (1250,-7200,-4900) dir 3000
- **EM** nur Elza: sub07@0x016DC Typ 0x21 Kraehe grid 0x40 kill - (-9970,-10000,-5680) dir 4000
- **EM** nur Elza: sub07@0x016F0 Typ 0x21 Kraehe grid 0x40 kill - (-9700,-8000,-7680) dir 0
- **DOOR** nur Leon: main00@0x01226 Slot 1 sce 0(inert) -> ROOM10B Cut 0
- **DOOR** nur Leon: main00@0x0129C Slot 2 sce 2(Tuer) -> ROOM124 Cut 0
- **DOOR** nur Leon: main00@0x012CC Slot 3 sce 2(Tuer) -> ROOM117 Cut 0
- **DOOR** nur Elza: main00@0x011FC Slot 1 sce 2(Tuer) -> ROOM10B Cut 0
- **DOOR** nur Elza: main00@0x0126C Slot 2 sce 2(Tuer) -> ROOM10B Cut 0
- **OBJ** nur Leon: sub15@0x017E6 obj 2 typ 0 band 5 (-1161,-7200,1991) ry 2048
- **OBJ** nur Leon: sub15@0x01808 obj 3 typ 0 band 5 (-1161,-12296,1991) ry 0
- **OBJ** nur Leon: sub15@0x0182A obj 4 typ 0 band 5 (10939,-11996,1991) ry 0
- **OBJ** nur Leon: sub15@0x0184C obj 5 typ 0 band 5 (-3461,-7200,4491) ry 3072
- **AOT** nur Leon: main00@0x01320 Slot 5 sce 1 flags 0x31 Text 12
- **AOT** nur Elza: main00@0x012B6 Slot 4 sce 1 flags 0x31 Text 4
- **BGM** nur Leon: sub02@0x01438 op54 0101000000
- **BGM** nur Leon: sub02@0x01502 op54 0102000000
- **BGM** nur Leon: sub02@0x0159A op54 0101000000
- **BGM** nur Leon: sub02@0x015AC op54 0102000000
- **MSGON** nur Leon: (2,) x1, (4,) x1, (5,) x1, (6,) x1, (7,) x1, (8,) x1, (9,) x1, (10,) x1, (11,) x1, (13,) x1, (14,) x1, (15,) x1, (16,) x1
- **MSGON** nur Elza: 
- **EVT** nur Leon: main00@0x012EC Evt_exec sub11
- **EVT** nur Leon: sub00@0x0141E Evt_exec sub14
- **EVT** nur Leon: sub02@0x014F2 Evt_exec sub06
- **EVT** nur Leon: sub02@0x014F6 Evt_exec sub07
- **EVT** nur Leon: sub02@0x014FA Evt_exec sub08
- **EVT** nur Leon: sub02@0x015A0 Evt_exec sub12
- **EVT** nur Leon: sub02@0x015A4 Evt_exec sub13
- **EVT** nur Leon: sub15@0x01876 Evt_exec sub05
- **CK** nur Leon: (3, 62, 0) x1, (3, 125, 0) x1, (3, 125, 1) x2, (3, 193, 0) x1, (4, 242, 1) x1, (5, 32, 0) x1, (5, 33, 0) x1
- **CK** nur Elza: (3, 194, 1) x1, (4, 233, 0) x1, (5, 0, 0) x1, (5, 1, 0) x1, (5, 28, 0) x1
- **SET** nur Leon: (1, 27, 0) x2, (1, 27, 1) x1, (2, 7, 0) x2, (2, 7, 1) x2, (3, 62, 1) x1, (3, 125, 1) x1, (3, 193, 1) x1, (4, 242, 0) x1, (4, 242, 1) x1
- **SET** nur Elza: (3, 128, 1) x1, (3, 130, 1) x1, (3, 194, 0) x1, (4, 233, 1) x1, (5, 0, 0) x1, (5, 1, 0) x1, (5, 20, 1) x1, (5, 28, 0) x1, (5, 29, 0) x1, (5, 29, 1) x1, (5, 30, 0) x1, (5, 30, 1) x1, (5, 31, 0) x1, (5, 31, 1) x2
- **TEXT** nur Leon id 0: "Elliot: We must go! There is no / time to lose!"
- **TEXT** nur Leon id 1: "Pilot: Is there no one else left / inside?"
- **TEXT** nur Leon id 2: "Elliot: There is no one left, we / must go!"
- **TEXT** nur Leon id 3: "Elliot: Those things are already / close, they almost caught me!"
- **TEXT** nur Leon id 4: "Pilot: Alright get on board we're / off."
- **TEXT** nur Leon id 5: "Leon: Hey! Hey wait! Down here!"
- **TEXT** nur Leon id 6: "Leon: Oh, that's just freaking great..."
- **TEXT** nur Leon id 7: "Leon: Now what am I gonna do?"
- **TEXT** nur Leon id 8: "We barricaded ourselves / inside the police station..."
- **TEXT** nur Leon id 9: "Where we thought it would / be safe."
- **TEXT** nur Leon id 10: "It was no use, and so began a / life and death struggle..."
- **TEXT** nur Leon id 11: "To escape."
- **TEXT** nur Leon id 13: "Leon: Damn it, there must be another / way out of here."
- **TEXT** nur Leon id 14: "Leon: If only I could contact the / outside."
- **TEXT** nur Leon id 15: "Leon: ..."
- **TEXT** nur Leon id 16: "Leon: That's right! I have to report / this to the chief."
- **TEXT** nur Elza id 0: "John: Elza, hurry up!"
- **TEXT** nur Elza id 1: "John: We need to get this thing / off of here! Help me out!"
- **TEXT** nur Elza id 2 (UNBENUTZT): "John: C'mon, give me a hand!"
- **TEXT** nur Elza id 3: "John: Alright, let's get down from here!"

### ROOM118  "B1 CORRIDOR"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 55, 149.

Byte-identisch (91848 B).

### ROOM119  "FIRING-ARMORY"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 163, 214, 253.

Groesse L 330532 / E 330532 B; Kopf L [0 15 17 0 0 0 0] / E [0 15 17 0 0 0 0]; Skripte L 17 / E 17; Texte L 6 / E 6; Abdeckung L 3006/3010 / E 3006/3010

- (keine Unterschiede in den gezaehlten Kategorien; Byte-Unterschied liegt in anderen Sektionen, z.B. Kamera/Kollision/Modelle/Offsets)

### ROOM11A  "SEWER EXIT"  [NPC]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 56, 253, 281, 301, 310.

Groesse L 253612 / E 253632 B; Kopf L [0 13 4 0 0 0 0] / E [0 13 4 0 0 0 0]; Skripte L 7 / E 7; Texte L 3 / E 3; Abdeckung L 1184/11610 / E 1204/11630

- **EM** nur Leon: sub00@0x0118C Typ 0x40 NPC 0x40 (Marvin?) grid 0x40 kill - (-1600,-1800,23450) dir 512
- **EM** nur Elza: sub00@0x0118C Typ 0x49 NPC 0x49 (Kendo?) grid 0x40 kill - (1800,-1800,23800) dir 1536
- **EM** nur Elza: sub00@0x011A0 Typ 0x4B NPC 0x4B (Sherry?) grid 0x40 kill - (2300,-1800,22600) dir 1536

### ROOM11B  "GARAGE"  [Tueren, Objekte, Texte*]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 25, 172, 193, 222, 223, 287, 303, 334, 361, 362.

Groesse L 232492 / E 253312 B; Kopf L [0 15 3 0 0 0 0] / E [0 15 3 0 0 0 0]; Skripte L 13 / E 8; Texte L 16 / E 15; Abdeckung L 2410/2410 / E 1502/1504

- **DOOR** nur Leon: main00@0x01010 Slot 5 sce 0(inert) -> ROOM126 Cut 7
- **OBJ** nur Leon: main00@0x010EA obj 0 typ 0 band 1 (-2168,0,-19368) ry 1024
- **AOT** nur Leon: main00@0x0106C Slot 7 sce 1 flags 0x31 Text 14
- **AOT** nur Elza: main00@0x01030 Slot 7 sce 1 flags 0x31 Text 13
- **AOT** nur Elza: sub00@0x0112A Slot 8 sce 1 flags 0x31 Text 12
- **AOTR** nur Leon: sub01@0x01216 Aot_reset Slot 1 -> sce 1 Text 15
- **AOTR** nur Leon: sub01@0x01220 Aot_reset Slot 4 -> sce 1 Text 15
- **AOTR** nur Leon: sub01@0x0122A Aot_reset Slot 7 -> sce 1 Text 15
- **AOTR** nur Leon: sub06@0x0172E Aot_reset Slot 5 -> sce 2 Tuer
- **AOTR** nur Elza: sub00@0x0113E Aot_reset Slot 5 -> sce 0 inert
- **AOTR** nur Elza: sub01@0x011C0 Aot_reset Slot 1 -> sce 1 Text 14
- **AOTR** nur Elza: sub01@0x011CA Aot_reset Slot 4 -> sce 1 Text 14
- **AOTR** nur Elza: sub01@0x011D4 Aot_reset Slot 7 -> sce 1 Text 14
- **AOTR** nur Elza: sub01@0x011DE Aot_reset Slot 8 -> sce 1 Text 14
- **MSGON** nur Leon: sub03@0x0126E Message_on 13
- **MSGON** nur Leon: sub03@0x01298 Message_on 13
- **MSGON** nur Leon: sub06@0x017AE Message_on 12
- **EVT** nur Leon: sub01@0x011BA Evt_exec sub04
- **EVT** nur Leon: sub01@0x011D2 Evt_exec sub06
- **EVT** nur Leon: sub04@0x01330 Evt_exec sub05
- **EVT** nur Leon: sub06@0x015D6 Evt_exec sub07
- **EVT** nur Leon: sub06@0x015DE Evt_exec sub08
- **EVT** nur Elza: sub01@0x0116E Evt_exec sub02
- **EVT** nur Elza: sub01@0x01196 Evt_exec sub03
- **CK** nur Leon: (3, 67, 1) x1, (3, 130, 0) x1, (3, 131, 0) x1, (4, 6, 1) x2, (4, 205, 0) x2, (4, 243, 1) x1, (5, 0, 0) x1, (5, 1, 0) x1, (5, 2, 0) x1, (12, 24, 1) x1
- **CK** nur Elza: (3, 70, 1) x1, (3, 132, 0) x1, (5, 33, 0) x1, (9, 10, 1) x1
- **SET** nur Leon: (3, 130, 1) x1, (3, 131, 1) x1, (4, 6, 1) x1, (4, 205, 1) x1, (4, 244, 1) x1, (5, 0, 0) x1, (5, 1, 0) x1, (5, 2, 0) x1, (5, 20, 1) x1, (5, 32, 0) x1
- **SET** nur Elza: (1, 27, 0) x1, (2, 7, 0) x1, (3, 68, 1) x1, (3, 70, 0) x1, (3, 132, 1) x1
- **TEXT** nur Leon id 0: "Leon: H-hey!"
- **TEXT** nur Leon id 1: "Marvin: Leon, you're okay! What / was the commotion up there?"
- **TEXT** nur Leon id 2: "Leon: Those things broke through the / shutters and started getting inside."
- **TEXT** nur Leon id 3: "Marvin: What about the chief?"
- **TEXT** nur Leon id 4: "Marvin: Oh, I see... Damn it!"
- **TEXT** nur Leon id 5: "Marvin: Well you saw it, right? / We can't get out using the patrol / cars anymore. What should we do?"
- **TEXT** nur Leon id 6: "Leon: Marvin, please come here!"
- **TEXT** nur Leon id 7: "Leon: There's a manhole here. How / about an underground escape route?"
- **TEXT** nur Leon id 8: "Marvin: We don't really have any other / option than to try. Okay, let's see if / we can move this!"
- **TEXT** nur Leon id 9: "Marvin: Leon! give me a hand!"
- **TEXT** nur Leon id 10: "Marvin: Okay, let's hurry!"
- **TEXT** nur Leon id 11: "Leon: Marvin, I'll climb down first, / you help her get down!"
- **TEXT** nur Leon id 12: "Marvin: Okay! Be careful down there!"
- **TEXT** nur Elza id 0: "Elza: Hey! What are you doing in a / place like this?"
- **TEXT** nur Elza id 1: "Elza: You got trapped in here, right?"
- **TEXT** nur Elza id 2: "Elza: What's your name?"
- **TEXT** nur Elza id 3: "Girl: ...Sherry."
- **TEXT** nur Elza id 4: "Elza: Sherry, huh? That's a cute name. / I'm Elza."
- **TEXT** nur Elza id 5: "Sherry: Elza..."
- **TEXT** nur Elza id 6: "Elza: That's right! Okay Sherry, listen / up..."
- **TEXT** nur Elza id 7: "Elza: Just wait there. / I'll come rescue you in a bit."
- **TEXT** nur Elza id 8: "Elza: So just hold tight and wait for / me quietly in that car over there. Okay?"
- **TEXT** nur Elza id 10: "Elza: Sherry!?"
- **TEXT** nur Elza id 11: "Elza: Where has she gone!?"
- **TEXT** nur Elza id 12: "The door is locked."

### ROOM11C  "PARKING LOT"  [NPC, Gegner, Texte*]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 39, 208, 284.

Groesse L 155552 / E 108020 B; Kopf L [0 14 1 0 0 0 0] / E [0 13 1 0 0 0 0]; Skripte L 9 / E 3; Texte L 10 / E 0; Abdeckung L 1366/1370 / E 172/174

- **EM** nur Leon: sub00@0x01770 Typ 0x42 NPC 0x42 (Ada) grid 0x40 kill - (-8965,0,-14347) dir 475
- **EM** nur Leon: sub00@0x01784 Typ 0x27 Affe/Gorilla grid 0x30 kill 96 (-1220,-20000,-21568) dir 2656
- **EM** nur Leon: sub00@0x01798 Typ 0x27 Affe/Gorilla grid 0x30 kill 97 (-554,-20000,-25423) dir 2656
- **EM** nur Leon: sub00@0x017D4 Typ 0x42 NPC 0x42 (Ada) grid 0x40 kill - (-18214,-20000,-7229) dir 475
- **EM** nur Leon: sub00@0x017E8 Typ 0x27 Affe/Gorilla grid 0x10 kill 96 (-9013,0,-15461) dir 2656
- **EM** nur Leon: sub00@0x017FC Typ 0x27 Affe/Gorilla grid 0x10 kill 97 (9434,0,2189) dir 2656
- **EM** nur Elza: sub00@0x016F8 Typ 0x20 Hund grid 0x00 kill 211 (-15000,0,9000) dir 3072
- **EM** nur Elza: sub00@0x0170C Typ 0x20 Hund grid 0x00 kill 212 (-16500,0,0) dir 0
- **EM** nur Elza: sub00@0x01720 Typ 0x20 Hund grid 0x00 kill 213 (-4500,0,-15000) dir 3584
- **EM** nur Elza: sub00@0x01734 Typ 0x20 Hund grid 0x00 kill 214 (10500,0,0) dir 2048
- **EM** nur Elza: sub00@0x01748 Typ 0x20 Hund grid 0x00 kill 215 (1500,0,12000) dir 3072
- **BGM** nur Leon: sub00@0x01810 op54 0001000000
- **BGM** nur Leon: sub02@0x01982 op54 0001000000
- **BGM** nur Leon: sub03@0x01B0E op54 0002000000
- **MSGON** nur Leon: sub02@0x018A0 Message_on 0
- **MSGON** nur Leon: sub02@0x018D4 Message_on 1
- **MSGON** nur Leon: sub02@0x018EC Message_on 2
- **MSGON** nur Leon: sub02@0x01904 Message_on 3
- **MSGON** nur Leon: sub02@0x01928 Message_on 4
- **MSGON** nur Leon: sub02@0x01A5E Message_on 5
- **MSGON** nur Leon: sub02@0x01A80 Message_on 6
- **MSGON** nur Leon: sub03@0x01B68 Message_on 7
- **MSGON** nur Leon: sub03@0x01B7A Message_on 8
- **MSGON** nur Leon: sub03@0x01B86 Message_on 9
- **EVT** nur Leon: sub01@0x01824 Evt_exec sub02
- **EVT** nur Leon: sub01@0x01842 Evt_exec sub03
- **EVT** nur Leon: sub02@0x01ABA Evt_exec sub07
- **EVT** nur Leon: sub03@0x01BE6 Evt_exec sub04
- **CK** nur Leon: sub00@0x0176C Ck(4,64,0)
- **CK** nur Leon: sub00@0x017D0 Ck(3,67,0)
- **CK** nur Leon: sub01@0x01820 Ck(4,64,0)
- **CK** nur Leon: sub01@0x0182E Ck(3,67,0)
- **CK** nur Leon: sub01@0x01836 Ck(7,96,1)
- **CK** nur Leon: sub01@0x0183E Ck(7,97,1)
- **CK** nur Leon: sub05@0x01C46 Ck(5,0,0)
- **CK** nur Leon: sub06@0x01C58 Ck(5,1,0)
- **SET** nur Leon: sub02@0x0184E Set(4,64,1)
- **SET** nur Leon: sub02@0x01852 Set(1,27,1)
- **SET** nur Leon: sub02@0x01856 Set(2,7,1)
- **SET** nur Leon: sub02@0x01ACA Set(1,27,0)
- **SET** nur Leon: sub02@0x01ACE Set(2,7,0)
- **SET** nur Leon: sub03@0x01B02 Set(3,67,1)
- **SET** nur Leon: sub03@0x01B06 Set(1,27,1)
- **SET** nur Leon: sub03@0x01B0A Set(2,7,1)
- **SET** nur Leon: sub05@0x01C3E Set(5,0,0)
- **SET** nur Leon: sub06@0x01C50 Set(5,1,0)
- **TEXT** nur Leon id 0: "Leon: Ada! Where's Marvin? Wasn't / he with you?"
- **TEXT** nur Leon id 1: "Ada: He turned around all of a sudden / to go look for you."
- **TEXT** nur Leon id 2: "Ada: There was this really loud noise / from upstairs earlier."
- **TEXT** nur Leon id 3: "Ada: You didn't run into him?"
- **TEXT** nur Leon id 4: "Leon: What!?"
- **TEXT** nur Leon id 5: "Leon: Ada, you hide inside that patrol / car."
- **TEXT** nur Leon id 6: "Ada: O-okay, I'll do that..."
- **TEXT** nur Leon id 7: "Ada: Okay, it's over now."
- **TEXT** nur Leon id 8: "Ada: ...There are some outrageous / monsters out there."
- **TEXT** nur Leon id 9: "Leon: Yeah... Anyway, let's get / out of here."

### ROOM11D  "KENNEL LIGHT"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 57, 108, 151.

Byte-identisch (130140 B).

### ROOM11E  "B2 CORRIDOR"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 102, 334.

Byte-identisch (209684 B).

### ROOM11F  "BOILER ROOM"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 58.

Byte-identisch (152588 B).

### ROOM120  "MORGUE"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 267.

Byte-identisch (167132 B).

### ROOM121  "DETENTI. AREA"  [NPC, Objekte, Texte*]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 278.

Groesse L 140596 / E 254616 B; Kopf L [0 9 1 0 0 0 0] / E [0 9 1 0 0 0 0]; Skripte L 4 / E 8; Texte L 0 / E 11; Abdeckung L 602/604 / E 1640/1644

- **EM** nur Elza: sub00@0x01E36 Typ 0x49 NPC 0x49 (Kendo?) grid 0x40 kill - (-26225,0,-25100) dir 0
- **OBJ** nur Leon: main00@0x01E4E obj 0 typ 0 band 1 (-21750,0,-20760) ry 1216
- **OBJ** nur Elza: main00@0x01D86 obj 0 typ 0 band 1 (-21750,0,-20760) ry 1225
- **AOT** nur Elza: sub00@0x01DEE Slot 8 sce 1 flags 0x31 Text 10
- **AOT** nur Elza: sub00@0x01E72 Slot 7 sce 3 flags 0x41 sub03
- **AOTR** nur Elza: sub03@0x022B0 Aot_reset Slot 7 -> sce 0 inert
- **MSGON** nur Elza: sub02@0x01FD2 Message_on 0
- **MSGON** nur Elza: sub02@0x02008 Message_on 1
- **MSGON** nur Elza: sub02@0x02028 Message_on 2
- **MSGON** nur Elza: sub02@0x02062 Message_on 3
- **MSGON** nur Elza: sub02@0x02098 Message_on 4
- **MSGON** nur Elza: sub02@0x0218C Message_on 5
- **MSGON** nur Elza: sub02@0x021C4 Message_on 6
- **MSGON** nur Elza: sub02@0x021E8 Message_on 7
- **MSGON** nur Elza: sub02@0x02224 Message_on 8
- **MSGON** nur Elza: sub02@0x0226A Message_on 9
- **EVT** nur Elza: sub00@0x01E4A Evt_exec sub06
- **CK** nur Leon: sub00@0x01EAA Ck(3,44,0)
- **CK** nur Elza: sub00@0x01DEA Ck(4,236,0)
- **CK** nur Elza: sub00@0x01E1A Ck(4,252,0)
- **CK** nur Elza: sub00@0x01E6E Ck(3,44,1)
- **CK** nur Elza: sub01@0x01F56 Ck(5,2,1)
- **CK** nur Elza: sub04@0x02314 Ck(5,0,0)
- **CK** nur Elza: sub05@0x02326 Ck(5,1,0)
- **SET** nur Leon: 
- **SET** nur Elza: (1, 27, 0) x1, (1, 27, 1) x1, (2, 7, 0) x1, (2, 7, 1) x1, (3, 44, 0) x1, (3, 120, 0) x1, (3, 186, 1) x1, (3, 201, 1) x1, (4, 96, 1) x1, (4, 250, 0) x1, (4, 252, 1) x1, (5, 0, 0) x1, (5, 1, 0) x1, (5, 2, 1) x1
- **TEXT** nur Elza id 0: "Elza: Hey! You must be John right? / Get up now!"
- **TEXT** nur Elza id 1: "John: What do you want? / I'm trying to sleep here!"
- **TEXT** nur Elza id 2: "John: Ughh... my head is killing me."
- **TEXT** nur Elza id 3: "John: Who the hell are you anyway?"
- **TEXT** nur Elza id 4: "Elza: My name's Elza. / Roy sent me to get you out of here."
- **TEXT** nur Elza id 5: "Elza: Listen. We're in serious trouble. / The whole station is crawling with / zombies!"
- **TEXT** nur Elza id 6: "John: Say what!?"
- **TEXT** nur Elza id 7: "Elza: Roy was injured by one of them. / I left him in the Interrogation Room, but..."
- **TEXT** nur Elza id 8: "John: The interrogation room? / Alright. I'm going there."
- **TEXT** nur Elza id 9: "Elza: John. Wait!"
- **TEXT** nur Elza id 10: "It's locked.It seems that there is someone / inside..."

### ROOM122  "DETENTI. CELLS"  

Byte-identisch (140456 B).

### ROOM123  "B1 CORR. LIGHT"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 59, 150, 163, 272.

Byte-identisch (166268 B).

### ROOM124  "OPENING"  [Tueren]

Groesse L 163744 / E 163748 B; Kopf L [0 9 0 0 0 0 0] / E [0 9 0 0 0 0 0]; Skripte L 5 / E 5; Texte L 6 / E 6; Abdeckung L 258/258 / E 262/262

- **DOOR** nur Leon: main00@0x0051A Slot 0 sce 2(Tuer) -> ROOM117 Cut 0
- **DOOR** nur Elza: main00@0x0051A Slot 0 sce 2(Tuer) -> ROOM103 Cut 0
- **SET** nur Leon: sub02@0x0055A Set(3,139,1)
- **SET** nur Elza: sub02@0x0055A Set(3,111,1)
- **SET** nur Elza: sub02@0x0055E Set(3,112,1)

### ROOM125  "LOBBY ERA-1"  

Byte-identisch (140056 B).

### ROOM126  "SEWER PASSAGE"  [NPC]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 60.

Groesse L 287920 / E 287712 B; Kopf L [0 16 2 0 0 0 0] / E [0 16 2 0 0 0 0]; Skripte L 7 / E 6; Texte L 0 / E 0; Abdeckung L 836/838 / E 630/632

- **EM** nur Leon: sub02@0x01254 Typ 0x42 NPC 0x42 (Ada) grid 0x40 kill - (26750,-1800,11800) dir 2048
- **EM** nur Leon: sub02@0x01268 Typ 0x40 NPC 0x40 (Marvin?) grid 0x40 kill - (25400,-1800,9850) dir 0
- **EM** nur Leon: sub02@0x0128C Typ 0x42 NPC 0x42 (Ada) grid 0x40 kill - (-7500,-1800,-6250) dir 2048
- **EM** nur Leon: sub02@0x012A0 Typ 0x40 NPC 0x40 (Marvin?) grid 0x40 kill - (-8900,-1800,-4600) dir 0
- **EM** nur Elza: sub03@0x01276 Typ 0x49 NPC 0x49 (Kendo?) grid 0x40 kill - (-21100,-1800,-8050) dir 0
- **EM** nur Elza: sub03@0x01296 Typ 0x49 NPC 0x49 (Kendo?) grid 0x40 kill - (20900,-1800,-5000) dir 2048
- **EM** nur Elza: sub03@0x012B2 Typ 0x49 NPC 0x49 (Kendo?) grid 0x40 kill - (20900,-1800,-5000) dir 2048
- **EVT** nur Leon: sub02@0x01280 Evt_exec sub05
- **CK** nur Leon: sub02@0x01246 Ck(4,244,1)
- **CK** nur Leon: sub05@0x013D4 Ck(5,34,0)
- **CK** nur Leon: sub05@0x013E8 Ck(5,34,0)
- **CK** nur Leon: sub05@0x013FC Ck(5,34,0)
- **CK** nur Leon: sub05@0x01410 Ck(5,34,0)
- **CK** nur Leon: sub05@0x01424 Ck(5,34,0)
- **CK** nur Leon: sub05@0x01438 Ck(5,34,0)
- **CK** nur Leon: sub05@0x0144C Ck(5,34,0)
- **CK** nur Leon: sub05@0x01460 Ck(5,34,0)
- **CK** nur Leon: sub05@0x01474 Ck(5,34,0)
- **CK** nur Elza: sub03@0x01268 Ck(3,65,1)

### ROOM127  "EMPTY"  

Byte-identisch (4 B).

### ROOM200  "L TUNNEL"  [NPC, Texte*]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 199, 210, 217, 222, 223, 296, 363.

Groesse L 359612 / E 360660 B; Kopf L [0 16 3 0 0 0 0] / E [0 16 3 0 0 0 0]; Skripte L 9 / E 10; Texte L 12 / E 15; Abdeckung L 1414/1418 / E 1968/1972

- **EM** nur Leon: sub00@0x017A0 Typ 0x42 NPC 0x42 (Ada) grid 0x40 kill - (550,-1800,-20800) dir 0
- **EM** nur Leon: sub00@0x017B4 Typ 0x40 NPC 0x40 (Marvin?) grid 0x40 kill - (2450,-1800,-20400) dir 512
- **EM** nur Elza: sub00@0x0179E Typ 0x49 NPC 0x49 (Kendo?) grid 0x40 kill - (-26003,-1800,-26062) dir 3072
- **EM** nur Elza: sub00@0x017D6 Typ 0x49 NPC 0x49 (Kendo?) grid 0x40 kill - (4997,-1800,-20962) dir 2048
- **EM** nur Elza: sub00@0x0180A Typ 0x4B NPC 0x4B (Sherry?) grid 0x40 kill - (1797,-1800,-20962) dir 2048
- **AOT** nur Elza: sub00@0x017B2 Slot 7 sce 0 flags 0x41 inert
- **AOT** nur Elza: sub00@0x017EA Slot 7 sce 3 flags 0x41 sub04
- **AOTR** nur Elza: sub03@0x01948 Aot_reset Slot 7 -> sce 3 sub04
- **AOTR** nur Elza: sub04@0x0195C Aot_reset Slot 7 -> sce 0 inert
- **MSGON** nur Elza: sub05@0x01C42 Message_on 12
- **MSGON** nur Elza: sub05@0x01C7E Message_on 13
- **MSGON** nur Elza: sub05@0x01CAE Message_on 14
- **EVT** nur Leon: main00@0x0177C Evt_exec sub07
- **EVT** nur Elza: main00@0x0177C Evt_exec sub08
- **EVT** nur Elza: sub00@0x0181E Evt_exec sub05
- **CK** nur Leon: sub00@0x01798 Ck(4,244,1)
- **CK** nur Leon: sub04@0x01AD8 Ck(5,0,0)
- **CK** nur Leon: sub05@0x01AEA Ck(5,1,0)
- **CK** nur Leon: sub06@0x01AFC Ck(5,2,0)
- **CK** nur Elza: sub00@0x0179A Ck(3,65,1)
- **CK** nur Elza: sub00@0x017D2 Ck(3,66,1)
- **CK** nur Elza: sub00@0x01806 Ck(3,68,1)
- **CK** nur Elza: sub00@0x0182A Ck(3,69,1)
- **CK** nur Elza: sub06@0x01D1E Ck(5,32,0)
- **CK** nur Elza: sub07@0x01D2C Ck(5,33,0)
- **SET** nur Leon: (4, 244, 0) x1, (5, 0, 0) x1, (5, 1, 0) x1, (5, 2, 0) x1
- **SET** nur Elza: (1, 27, 0) x1, (1, 27, 1) x2, (2, 7, 0) x1, (2, 7, 1) x2, (3, 65, 0) x1, (3, 66, 0) x1, (3, 66, 1) x1, (3, 68, 0) x1, (3, 69, 1) x1, (3, 70, 1) x1, (3, 74, 1) x1
- **TEXT** nur Leon id 0: "Leon: Ada, Marvin, hold on."
- **TEXT** nur Leon id 1: "Marvin: What's up Leon?"
- **TEXT** nur Leon id 2: "Leon: There is a little girl that / came in here, we have to find her!"
- **TEXT** nur Leon id 3: "Marvin: A little girl, are you sure?"
- **TEXT** nur Leon id 4: "Leon: Yes, before meeting Ada I saw / her coming in this way, if I hadn't"
- **TEXT** nur Leon id 5: "seen her maybe we wouldn't have / found this way out."
- **TEXT** nur Leon id 6: "Leon: She must have thought I was / a zombie."
- **TEXT** nur Leon id 7: "Ada: So there is no time to lose, / we do not know what things are in"
- **TEXT** nur Leon id 8: "the sewers, zombies are not the only / things we should worry about here."
- **TEXT** nur Leon id 9: "Leon: Okay, it will be better to / separate and cover more ground."
- **TEXT** nur Leon id 10: "Leon: Marvin sees Ada up front."
- **TEXT** nur Leon id 11: "Marvin: Understood. If something / happens, this area will be to meet."
- **TEXT** nur Elza id 0: "John: Elza, over here!"
- **TEXT** nur Elza id 1: "John: Through this staircase you can / access the basement garage."
- **TEXT** nur Elza id 2: "Elza: Okay, I'll go for her."
- **TEXT** nur Elza id 3: "John: Meanwhile I will take a look / around, maybe she is down here too."
- **TEXT** nur Elza id 4: "Elza: Good idea, but be careful."
- **TEXT** nur Elza id 5: "John: You too."
- **TEXT** nur Elza id 6: "Elza: Sherry... thank god you're / fine! I was so worried!"
- **TEXT** nur Elza id 7: "Elza: What happened? why didn't you / wait for me?"
- **TEXT** nur Elza id 8: "Sherry: It was because of my daddy, / I've heard him call me!"
- **TEXT** nur Elza id 9: "Sherry: I went down here to find him, / but I couldn't..."
- **TEXT** nur Elza id 10: "Elza: I understand, don't worry honey, / if he's here we'll find him, okay?"
- **TEXT** nur Elza id 11: "Elza: My, that's a nice pocket watch!"
- **TEXT** nur Elza id 12: "Sherry: My mom gave it to me, she says / it's for good luck."
- **TEXT** nur Elza id 13: "Elza: I see, I guess your mom is right."
- **TEXT** nur Elza id 14: "Elza: Come on Sherry, let's look for / John and get out of here!"

### ROOM201  "BREAK ROOM"  [NPC]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 61, 152, 283.

Groesse L 316824 / E 316856 B; Kopf L [0 6 5 0 0 0 0] / E [0 6 5 0 0 0 0]; Skripte L 5 / E 5; Texte L 5 / E 5; Abdeckung L 428/6982 / E 458/7014

- **EM** nur Elza: sub00@0x008C0 Typ 0x4B NPC 0x4B (Sherry?) grid 0x40 kill - (-3250,0,-5875) dir 3072
- **CK** nur Elza: sub00@0x008BC Ck(3,74,1)

### ROOM202  "ZOMBIE TUNNEL"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 62, 206.

Groesse L 322276 / E 322276 B; Kopf L [0 7 3 0 0 0 0] / E [0 7 3 0 0 0 0]; Skripte L 5 / E 5; Texte L 4 / E 4; Abdeckung L 778/778 / E 778/778

- (keine Unterschiede in den gezaehlten Kategorien; Byte-Unterschied liegt in anderen Sektionen, z.B. Kamera/Kollision/Modelle/Offsets)

### ROOM203  "TREATMENT POOL"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 63, 106, 153, 344.

Byte-identisch (282076 B).

### ROOM204  "SEALED TUNNEL"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 64, 154.

Byte-identisch (255800 B).

### ROOM205  "SEPTIC POOL"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 41, 65, 91, 155.

Byte-identisch (342712 B).

### ROOM206  "ENGINE ROOM"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 66, 156, 263, 345.

Byte-identisch (272112 B).

### ROOM207  "TUN. FLOOD DRY"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 76, 181.

Byte-identisch (174036 B).

### ROOM208  "CONTROL ROOM"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 67, 252.

Byte-identisch (173608 B).

### ROOM209  "ALLIGATOR POOL"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 68, 99, 178, 246, 291.

Byte-identisch (311024 B).

### ROOM20A  "SERVICE TUNNEL"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 69, 292.

Byte-identisch (300176 B).

### ROOM20B  "L TUNNEL FLOOD"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 33, 113, 118, 119, 200, 211, 356.

Byte-identisch (201460 B).

### ROOM20C  "EMPTY"  

Byte-identisch (4 B).

### ROOM20D  "EMPTY"  

Byte-identisch (4 B).

### ROOM20E  "EMPTY"  

Byte-identisch (4 B).

### ROOM20F  "EMPTY"  

Byte-identisch (4 B).

### ROOM300  "FACT. ENTRANCE"  [NPC, Texte*]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 300.

Groesse L 158784 / E 159180 B; Kopf L [0 4 0 0 0 0 0] / E [0 4 0 0 0 0 0]; Skripte L 5 / E 5; Texte L 9 / E 10; Abdeckung L 946/950 / E 960/962

- **EM** nur Leon: sub00@0x0156C Typ 0x40 NPC 0x40 (Marvin?) grid 0x40 kill - (-24458,0,-16261) dir -248
- **EM** nur Leon: sub00@0x0158E Typ 0x40 NPC 0x40 (Marvin?) grid 0x00 kill - (-8250,0,-17350) dir 2048
- **EM** nur Elza: sub00@0x0156C Typ 0x49 NPC 0x49 (Kendo?) grid 0x40 kill - (-23363,0,-14359) dir 640
- **EM** nur Elza: sub00@0x01580 Typ 0x4B NPC 0x4B (Sherry?) grid 0x40 kill - (-23600,0,-15799) dir 352
- **EM** nur Elza: sub00@0x015A2 Typ 0x49 NPC 0x49 (Kendo?) grid 0x00 kill - (-8250,0,-17350) dir 2048
- **EM** nur Elza: sub00@0x015B6 Typ 0x4B NPC 0x4B (Sherry?) grid 0x00 kill - (-9350,0,-18150) dir 2048
- **MSGON** nur Elza: sub03@0x017BE Message_on 9
- **SET** nur Elza: sub03@0x016AE Set(4,244,1)
- **SET** nur Elza: sub03@0x016B2 Set(3,160,1)
- **TEXT** nur Leon id 2: "Leon: Marvin! Aren't these... RPD / officers!?"
- **TEXT** nur Leon id 3: "Marvin: Oh, man... They must be from the / squad investigating on the Umbrella / incident."
- **TEXT** nur Leon id 4: "Marvin: We don't know what's up ahead... / Stay alert, Leon!"
- **TEXT** nur Leon id 5: "Leon: Marvin, what about that woman?"
- **TEXT** nur Leon id 6: "Marvin: Ada...? She went on ahead."
- **TEXT** nur Leon id 7: "Marvin: She didn't seem to be at all / disturbed by all this massacre."
- **TEXT** nur Leon id 8: "Leon: There's a ton of things I need / to ask her the next time I see her..."
- **TEXT** nur Elza id 2: "Elza: !!"
- **TEXT** nur Elza id 3: "Elza: Wh-what is this place...?"
- **TEXT** nur Elza id 4: "John: These corpses are definitively / police officers."
- **TEXT** nur Elza id 5: "John: This used to be a chemical plant, / but it was supposedly not in use anymore..."
- **TEXT** nur Elza id 6: "Elza: What happened here?"
- **TEXT** nur Elza id 7: "Sherry: Elza..."
- **TEXT** nur Elza id 8: "Elza: It'll be okay, just stick close to / me."
- **TEXT** nur Elza id 9: "John: Let's go. We should find / somewhere to hide for now."

### ROOM301  "DOWNLOAD AREA"  [NPC]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 335.

Groesse L 166992 / E 167044 B; Kopf L [0 10 0 0 0 0 0] / E [0 10 0 0 0 0 0]; Skripte L 4 / E 4; Texte L 2 / E 2; Abdeckung L 822/824 / E 874/876

- **EM** nur Leon: sub00@0x02212 Typ 0x40 NPC 0x40 (Marvin?) grid 0x00 kill - (-22900,0,10850) dir 0
- **EM** nur Elza: sub00@0x02212 Typ 0x49 NPC 0x49 (Kendo?) grid 0x00 kill - (-22900,0,10850) dir 0
- **EM** nur Elza: sub00@0x02226 Typ 0x4B NPC 0x4B (Sherry?) grid 0x00 kill - (-22900,0,9700) dir 0

### ROOM302  "PARKING AREA"  [NPC, Tueren, Texte*]

Groesse L 162568 / E 118140 B; Kopf L [0 9 0 0 0 0 0] / E [0 9 0 0 0 0 0]; Skripte L 8 / E 3; Texte L 7 / E 5; Abdeckung L 708/1024 / E 188/454

- **EM** nur Leon: sub00@0x00BF6 Typ 0x40 NPC 0x40 (Marvin?) grid 0x00 kill - (-25500,0,-19500) dir 0
- **EM** nur Elza: sub00@0x00BB8 Typ 0x49 NPC 0x49 (Kendo?) grid 0x00 kill - (-25500,0,-19500) dir 0
- **EM** nur Elza: sub00@0x00BCC Typ 0x4B NPC 0x4B (Sherry?) grid 0x00 kill - (-24000,0,-21000) dir 0
- **DOOR** nur Leon: main00@0x00B6E Slot 2 sce 2(Tuer) -> ROOM30C Cut 0
- **AOT** nur Leon: main00@0x00B5A Slot 1 sce 1 flags 0x31 Text 6
- **AOT** nur Leon: main00@0x00B8E Slot 4 sce 1 flags 0x31 Text 4
- **AOT** nur Leon: main00@0x00BB6 Slot 6 sce 1 flags 0x31 Text 5
- **AOT** nur Leon: sub00@0x00BE2 Slot 3 sce 3 flags 0x41 sub02
- **AOT** nur Elza: main00@0x00B6E Slot 3 sce 1 flags 0x31 Text 1
- **AOT** nur Elza: main00@0x00B82 Slot 4 sce 1 flags 0x31 Text 1
- **AOT** nur Elza: main00@0x00B96 Slot 5 sce 1 flags 0x31 Text 3
- **AOT** nur Elza: sub00@0x00BE0 Slot 2 sce 1 flags 0x31 Text 2
- **AOTR** nur Leon: sub02@0x00C14 Aot_reset Slot 3 -> sce 0 inert
- **MSGON** nur Leon: sub02@0x00CB8 Message_on 0
- **MSGON** nur Leon: sub02@0x00D12 Message_on 1
- **MSGON** nur Leon: sub02@0x00D32 Message_on 2
- **EVT** nur Leon: sub02@0x00C28 Evt_exec sub05
- **EVT** nur Leon: sub02@0x00D0E Evt_exec sub06
- **CK** nur Leon: sub03@0x00D8A Ck(5,0,0)
- **CK** nur Leon: sub04@0x00D9C Ck(5,1,0)
- **SET** nur Leon: sub02@0x00C10 Set(4,199,0)
- **SET** nur Leon: sub02@0x00C1E Set(2,7,1)
- **SET** nur Leon: sub02@0x00C22 Set(1,27,1)
- **SET** nur Leon: sub03@0x00D82 Set(5,0,0)
- **SET** nur Leon: sub04@0x00D94 Set(5,1,0)
- **TEXT** nur Leon id 0: "Leon: Dammit! the door does not open!"
- **TEXT** nur Leon id 1: "Marvin: There's an open window, maybe / we could enter through there?"
- **TEXT** nur Leon id 2: "Marvin: Leon give me an hand!"
- **TEXT** nur Elza id 2: "It's an open window. / I could boost Sherry in there.Not a good idea.Too dark, and there may be zombies / inside."

### ROOM303  "FACT. CORRIDOR"  

Byte-identisch (81004 B).

### ROOM304  "FACTORY LINE A"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 185, 280.

Byte-identisch (152352 B).

### ROOM305  "FACTORY LINE B"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 257, 330.

Byte-identisch (262652 B).

### ROOM306  "FACT. BACKYARD"  [NPC, Gegner, Tueren, Objekte, Texte*]

Groesse L 129900 / E 144536 B; Kopf L [0 7 1 0 0 0 0] / E [0 7 1 0 0 0 0]; Skripte L 5 / E 12; Texte L 11 / E 11; Abdeckung L 616/618 / E 1338/1340

- **EM** nur Leon: sub00@0x00F58 Typ 0x40 NPC 0x40 (Marvin?) grid 0x00 kill - (-25750,0,-25150) dir 0
- **EM** nur Leon: sub00@0x00F78 Typ 0x20 Hund grid 0x41 kill 80 (-14000,0,-24000) dir 499
- **EM** nur Leon: sub00@0x00F8C Typ 0x20 Hund grid 0x41 kill 81 (-8039,0,-22041) dir 499
- **EM** nur Leon: sub00@0x00FA0 Typ 0x20 Hund grid 0x41 kill 82 (-1313,0,-19980) dir 499
- **EM** nur Elza: sub00@0x00F44 Typ 0x4B NPC 0x4B (Sherry?) grid 0x40 kill - (-26427,0,-23682) dir 0
- **EM** nur Elza: sub00@0x00F58 Typ 0x49 NPC 0x49 (Kendo?) grid 0x40 kill - (-26227,0,-25082) dir 0
- **EM** nur Elza: sub00@0x00F6C Typ 0x20 Hund grid 0x41 kill - (-13350,0,-22000) dir 499
- **DOOR** nur Leon: main00@0x00EF8 Slot 3 sce 2(Tuer) -> ROOM302 Cut 6
- **DOOR** nur Elza: main00@0x00EF8 Slot 3 sce 2(Tuer) -> ROOM30C Cut 10
- **OBJ** nur Elza: sub00@0x00F80 obj 0 typ 0 band 1 (-19095,-495,-27045) ry 3072
- **AOTR** nur Leon: sub02@0x0103E Aot_reset Slot 1 -> sce 2 Tuer
- **AOTR** nur Elza: sub02@0x01036 Aot_reset Slot 1 -> sce 0 inert
- **MSGON** nur Elza: sub02@0x01040 Message_on 0
- **MSGON** nur Elza: sub02@0x010A8 Message_on 1
- **MSGON** nur Elza: sub02@0x010FC Message_on 2
- **MSGON** nur Elza: sub02@0x01104 Message_on 3
- **MSGON** nur Elza: sub02@0x0110C Message_on 4
- **MSGON** nur Elza: sub02@0x0113C Message_on 5
- **MSGON** nur Elza: sub02@0x01144 Message_on 6
- **MSGON** nur Elza: sub02@0x0117C Message_on 7
- **MSGON** nur Elza: sub02@0x01184 Message_on 8
- **EVT** nur Elza: sub00@0x00FA2 Evt_exec sub09
- **EVT** nur Elza: sub00@0x00FA6 Evt_exec sub10
- **EVT** nur Elza: sub02@0x01138 Evt_exec sub04
- **EVT** nur Elza: sub04@0x012D8 Evt_exec sub05
- **CK** nur Leon: sub00@0x00F28 Ck(3,160,0)
- **CK** nur Leon: sub00@0x00F3E Ck(3,161,0)
- **CK** nur Leon: sub00@0x00F54 Ck(4,199,1)
- **CK** nur Leon: sub00@0x00F74 Ck(3,160,0)
- **CK** nur Leon: sub00@0x0101E Ck(4,196,1)
- **CK** nur Leon: sub03@0x010CA Ck(3,117,0)
- **CK** nur Elza: sub00@0x00F36 Ck(4,244,1)
- **CK** nur Elza: sub06@0x0135E Ck(5,0,0)
- **CK** nur Elza: sub07@0x01370 Ck(5,1,0)
- **CK** nur Elza: sub08@0x01382 Ck(5,2,0)
- **SET** nur Leon: sub00@0x01026 Set(3,160,1)
- **SET** nur Leon: sub02@0x0103A Set(3,161,1)
- **SET** nur Leon: sub03@0x0104A Set(4,8,1)
- **SET** nur Elza: sub02@0x01026 Set(4,244,0)
- **SET** nur Elza: sub02@0x01032 Set(4,199,0)
- **SET** nur Elza: sub06@0x01356 Set(5,0,0)
- **SET** nur Elza: sub07@0x01368 Set(5,1,0)
- **SET** nur Elza: sub08@0x0137A Set(5,2,0)
- **TEXT** nur Leon id 7 (UNBENUTZT): "Sherry: I'll unlock the back door from / the other side!Just wait by the door, okay?"
- **TEXT** nur Leon id 9: "For some reason the door isn't / locked anymore..."
- **TEXT** nur Elza id 7: "Sherry: I'll unlock the back door from / the other side!"
- **TEXT** nur Elza id 8: "Sherry: Just wait by the door, okay?"

### ROOM307  "WAREHOUSE"  [NPC, Tueren, Objekte, Texte*]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 36, 365.

Groesse L 243040 / E 243908 B; Kopf L [0 13 3 0 0 0 0] / E [0 13 4 0 0 0 0]; Skripte L 14 / E 19; Texte L 6 / E 16; Abdeckung L 1626/1628 / E 2805/2806

- **EM** nur Leon: sub00@0x033CE Typ 0x30 Birkin grid 0x33 kill - (30000,0,30000) dir 2048
- **EM** nur Leon: sub00@0x0343C Typ 0x42 NPC 0x42 (Ada) grid 0x00 kill - (-13350,0,-16780) dir 30
- **EM** nur Leon: sub00@0x03450 Typ 0x40 NPC 0x40 (Marvin?) grid 0x00 kill - (-13250,0,-15410) dir 40
- **EM** nur Elza: sub00@0x03434 Typ 0x30 Birkin grid 0x21 kill - (-29000,0,-11850) dir 0
- **EM** nur Elza: sub00@0x034AE Typ 0x49 NPC 0x49 (Kendo?) grid 0x40 kill - (-13470,0,-16820) dir 260
- **EM** nur Elza: sub00@0x034C2 Typ 0x4B NPC 0x4B (Sherry?) grid 0x40 kill - (-13810,0,-15900) dir 300
- **DOOR** nur Leon: main00@0x03322 Slot 1 sce 2(Tuer) -> ROOM30E Cut 1
- **DOOR** nur Elza: main00@0x0332A Slot 1 sce 2(Tuer) -> ROOM30E Cut 5
- **OBJ** nur Elza: main00@0x03352 obj 0 typ 0 band 1 (-26650,-1150,-13810) ry 0
- **OBJ** nur Elza: main00@0x03374 obj 1 typ 0 band 1 (-26650,-2250,-12000) ry 0
- **AOT** nur Leon: sub00@0x03464 Slot 3 sce 3 flags 0x41 sub03
- **AOT** nur Elza: sub00@0x034D6 Slot 3 sce 3 flags 0x41 sub11
- **AOTR** nur Leon: sub00@0x033F6 Aot_reset Slot 0 -> sce 1 Text 5
- **AOTR** nur Leon: sub00@0x03420 Aot_reset Slot 0 -> sce 1 Text 5
- **AOTR** nur Leon: sub00@0x03478 Aot_reset Slot 0 -> sce 1 Text 5
- **AOTR** nur Elza: sub00@0x03460 Aot_reset Slot 0 -> sce 1 Text 15
- **AOTR** nur Elza: sub00@0x03492 Aot_reset Slot 0 -> sce 1 Text 15
- **AOTR** nur Elza: sub00@0x034EA Aot_reset Slot 0 -> sce 1 Text 15
- **AOTR** nur Elza: sub01@0x0351E Aot_reset Slot 1 -> sce 3 sub17
- **AOTR** nur Elza: sub17@0x03E15 Aot_reset Slot 1 -> sce 2 Tuer
- **MSGON** nur Elza: sub11@0x03AA9 Message_on 5
- **MSGON** nur Elza: sub11@0x03AC9 Message_on 6
- **MSGON** nur Elza: sub11@0x03B49 Message_on 7
- **MSGON** nur Elza: sub11@0x03B6F Message_on 8
- **MSGON** nur Elza: sub11@0x03BB5 Message_on 9
- **MSGON** nur Elza: sub11@0x03BF3 Message_on 10
- **MSGON** nur Elza: sub11@0x03C07 Message_on 11
- **MSGON** nur Elza: sub11@0x03C23 Message_on 12
- **MSGON** nur Elza: sub11@0x03C4B Message_on 13
- **MSGON** nur Elza: sub11@0x03C67 Message_on 14
- **EVT** nur Elza: sub02@0x03635 Evt_exec sub03
- **EVT** nur Elza: sub02@0x03639 Evt_exec sub04
- **EVT** nur Elza: sub02@0x03679 Evt_exec sub09
- **EVT** nur Elza: sub02@0x0367D Evt_exec sub10
- **EVT** nur Elza: sub11@0x03A03 Evt_exec sub12
- **EVT** nur Elza: sub11@0x03A0B Evt_exec sub13
- **EVT** nur Elza: sub11@0x03B53 Evt_exec sub03
- **EVT** nur Elza: sub11@0x03D39 Evt_exec sub10
- **CK** nur Leon: sub00@0x033CA Ck(4,215,0)
- **CK** nur Leon: sub00@0x03408 Ck(4,236,0)
- **CK** nur Elza: main00@0x0334E Ck(4,75,0)
- **CK** nur Elza: sub00@0x0342C Ck(4,75,0)
- **CK** nur Elza: sub00@0x03472 Ck(4,239,0)
- **CK** nur Elza: sub00@0x034F8 Ck(3,215,0)
- **CK** nur Elza: sub01@0x03534 Ck(5,3,0)
- **CK** nur Elza: sub02@0x03651 Ck(5,31,0)
- **CK** nur Elza: sub02@0x03669 Ck(5,30,0)
- **SET** nur Leon: (4, 194, 1) x1, (4, 215, 1) x1, (4, 236, 1) x1
- **SET** nur Elza: (1, 0, 0) x2, (1, 0, 1) x2, (1, 27, 0) x1, (1, 27, 1) x2, (2, 7, 0) x1, (2, 7, 1) x2, (3, 181, 1) x1, (3, 215, 1) x1, (4, 75, 1) x1, (4, 239, 1) x1, (5, 3, 1) x1
- **TEXT** nur Leon id 0: "Leon: Get on the lift! It's our only / option..."
- **TEXT** nur Leon id 1: "Marvin: This thing...? / But where will it take us?"
- **TEXT** nur Leon id 2: "Ada: Underground... Though we might / be better off staying here and get / eaten by those things..."
- **TEXT** nur Leon id 3: "Marvin: What?"
- **TEXT** nur Leon id 4: "Leon: Hurry up! They're coming!"
- **TEXT** nur Elza id 0: "Elza: I know it has to be somewhere...!"
- **TEXT** nur Elza id 1: "John: Okay, Elza! I've got your back!"
- **TEXT** nur Elza id 2: "Elza: This lift, this is where it / came from."
- **TEXT** nur Elza id 3: "John: ...! That's a damn huge lift..."
- **TEXT** nur Elza id 4: "John: They probably used it to move / some heavy equipment."
- **TEXT** nur Elza id 5: "John: ...Can we get it to work?"
- **TEXT** nur Elza id 6: "Elza: No... It looks like it can be / operated only from the inside..."
- **TEXT** nur Elza id 7: "Sherry: I know how to make it work!"
- **TEXT** nur Elza id 8: "Elza: Sherry? What are you talking / about...?"
- **TEXT** nur Elza id 9: "John: Save the discussion for later! / Those things are trying to get in here...!"
- **TEXT** nur Elza id 10: "John: Let's get this lift moving and go / down there!"
- **TEXT** nur Elza id 11: "Elza: Are you insane!?"
- **TEXT** nur Elza id 12: "Elza: I told you that weird monster / came from here!"
- **TEXT** nur Elza id 13: "John: If we stay here, we'll be eaten / for sure!"
- **TEXT** nur Elza id 14: "John: We've got no choice but to take / our chances down there! Let's go!"

### ROOM308  "WAREHOUSE LIFT"  [NPC, Gegner, Texte*]

Groesse L 240472 / E 168560 B; Kopf L [0 4 2 0 0 0 0] / E [0 4 2 0 0 0 0]; Skripte L 11 / E 5; Texte L 2 / E 0; Abdeckung L 1216/1218 / E 308/310

- **EM** nur Leon: sub00@0x007AC Typ 0x42 NPC 0x42 (Ada) grid 0x40 kill - (-25000,0,-24180) dir 914
- **EM** nur Leon: sub00@0x007C0 Typ 0x40 NPC 0x40 (Marvin?) grid 0x40 kill - (-26040,0,-25120) dir 0
- **EM** nur Leon: sub00@0x007D4 Typ 0x36 Birkin Oberkoerper grid 0x24 kill - (-20000,0,-26000) dir 2048
- **EM** nur Elza: sub00@0x007A0 Typ 0x4B NPC 0x4B (Sherry?) grid 0x40 kill - (-25000,0,-24180) dir 914
- **EM** nur Elza: sub00@0x007B4 Typ 0x49 NPC 0x49 (Kendo?) grid 0x40 kill - (-26040,0,-25120) dir 0
- **BGM** nur Leon: sub02@0x00824 op57 000008
- **BGM** nur Leon: sub02@0x0082C op57 000000
- **BGM** nur Leon: sub02@0x00834 op57 000008
- **BGM** nur Leon: sub02@0x0083C op57 000000
- **BGM** nur Leon: sub02@0x0087A op57 00000a
- **BGM** nur Leon: sub02@0x00882 op57 000000
- **BGM** nur Leon: sub02@0x0088A op57 000009
- **BGM** nur Leon: sub02@0x00892 op57 000000
- **BGM** nur Leon: sub02@0x0089A op57 000007
- **BGM** nur Leon: sub02@0x008A2 op57 000000
- **BGM** nur Leon: sub02@0x008AA op57 000008
- **BGM** nur Leon: sub02@0x008B2 op57 000000
- **BGM** nur Leon: sub02@0x008BA op57 00000a
- **BGM** nur Leon: sub02@0x009A8 op57 000000
- **BGM** nur Leon: sub02@0x009B0 op57 000009
- **BGM** nur Leon: sub02@0x009B8 op57 000000
- **BGM** nur Leon: sub02@0x009C0 op57 000007
- **BGM** nur Leon: sub02@0x009C8 op57 000000
- **MSGON** nur Leon: sub02@0x009E4 Message_on 0
- **MSGON** nur Leon: sub02@0x00A1A Message_on 1
- **EVT** nur Leon: sub00@0x007F0 Evt_exec sub09
- **EVT** nur Leon: sub02@0x008C6 Evt_exec sub04
- **EVT** nur Leon: sub02@0x00924 Evt_exec sub05
- **EVT** nur Leon: sub02@0x00928 Evt_exec sub06
- **EVT** nur Leon: sub02@0x00994 Evt_exec sub07
- **EVT** nur Leon: sub02@0x00998 Evt_exec sub08
- **CK** nur Leon: sub00@0x007A8 Ck(4,96,0)
- **CK** nur Elza: sub00@0x0079C Ck(4,253,0)
- **SET** nur Leon: sub02@0x007FA Set(4,96,1)
- **SET** nur Elza: sub02@0x007DA Set(4,253,1)
- **TEXT** nur Leon id 0: "Ada: You just don't give up..."
- **TEXT** nur Leon id 1: "Ada: That's... got to be..."

### ROOM309  "T CORRIDOR"  [NPC, Objekte, Texte*]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 38, 114, 224, 304, 339, 340, 366, 368.

Groesse L 184132 / E 272500 B; Kopf L [0 16 1 0 0 0 0] / E [0 16 2 0 0 0 0]; Skripte L 6 / E 15; Texte L 4 / E 33; Abdeckung L 456/456 / E 2852/2854

- **EM** nur Elza: sub00@0x014E0 Typ 0x49 NPC 0x49 (Kendo?) grid 0x40 kill - (-24523,0,4047) dir -2960
- **EM** nur Elza: sub00@0x014F4 Typ 0x42 NPC 0x42 (Ada) grid 0x40 kill - (-24523,0,4047) dir -2960
- **OBJ** nur Elza: sub00@0x01508 obj 1 typ 4 band 1 (-15072,950,-1010) ry 1024
- **AOT** nur Elza: main00@0x013D8 Slot 4 sce 5 flags 0x31 Marke
- **AOTR** nur Leon: sub00@0x01442 Aot_reset Slot 1 -> sce 1 Text 3
- **AOTR** nur Elza: sub00@0x0149E Aot_reset Slot 0 -> sce 1 Text 8
- **AOTR** nur Elza: sub00@0x014A8 Aot_reset Slot 1 -> sce 5 Marke
- **AOTR** nur Elza: sub00@0x014B2 Aot_reset Slot 2 -> sce 1 Text 8
- **AOTR** nur Elza: sub00@0x014BC Aot_reset Slot 3 -> sce 1 Text 8
- **AOTR** nur Elza: sub00@0x014C6 Aot_reset Slot 7 -> sce 0 inert
- **AOTR** nur Elza: sub00@0x014D4 Aot_reset Slot 1 -> sce 5 Marke
- **AOTR** nur Elza: sub05@0x0169C Aot_reset Slot 7 -> sce 5 Marke
- **AOTR** nur Elza: sub07@0x01D68 Aot_reset Slot 0 -> sce 2 Tuer
- **AOTR** nur Elza: sub07@0x01D72 Aot_reset Slot 1 -> sce 2 Tuer
- **AOTR** nur Elza: sub07@0x01D7C Aot_reset Slot 2 -> sce 2 Tuer
- **AOTR** nur Elza: sub07@0x01D86 Aot_reset Slot 3 -> sce 2 Tuer
- **BGM** nur Elza: sub05@0x0163C op57 000008
- **BGM** nur Elza: sub05@0x01644 op57 000000
- **BGM** nur Elza: sub05@0x0164C op57 000008
- **BGM** nur Elza: sub05@0x01654 op57 000000
- **BGM** nur Elza: sub05@0x01670 op57 000008
- **BGM** nur Elza: sub05@0x01678 op57 000000
- **BGM** nur Elza: sub05@0x01680 op57 000008
- **BGM** nur Elza: sub05@0x01688 op57 000000
- **MSGON** nur Leon: 
- **MSGON** nur Elza: (3,) x1, (4,) x1, (5,) x1, (6,) x1, (9,) x1, (10,) x1, (11,) x1, (12,) x1, (13,) x1, (14,) x1, (15,) x1, (16,) x1, (17,) x1, (18,) x1, (19,) x1, (20,) x1, (21,) x1, (22,) x1, (23,) x1, (24,) x1, (25,) x1, (26,) x1, (27,) x1, (28,) x1, (29,) x1, (30,) x1, (31,) x1, (32,) x1
- **EVT** nur Elza: sub01@0x01558 Evt_exec sub05
- **EVT** nur Elza: sub01@0x01570 Evt_exec sub06
- **EVT** nur Elza: sub02@0x015A4 Evt_exec sub07
- **EVT** nur Elza: sub07@0x0174A Evt_exec sub13
- **EVT** nur Elza: sub07@0x01760 Evt_exec sub08
- **EVT** nur Elza: sub07@0x01CCE Evt_exec sub12
- **CK** nur Elza: sub00@0x01492 Ck(4,16,0)
- **CK** nur Elza: sub01@0x01562 Ck(4,8,1)
- **CK** nur Elza: sub02@0x0158E Ck(4,16,0)
- **CK** nur Elza: sub02@0x01596 Ck(9,21,0)
- **CK** nur Elza: sub05@0x01602 Ck(12,31,0)
- **CK** nur Elza: sub06@0x016B8 Ck(9,21,0)
- **CK** nur Elza: sub07@0x01D94 Ck(3,117,0)
- **CK** nur Elza: sub09@0x01DCA Ck(5,32,0)
- **CK** nur Elza: sub10@0x01DD8 Ck(5,33,0)
- **CK** nur Elza: sub11@0x01DE6 Ck(5,34,0)
- **SET** nur Elza: sub05@0x01606 Set(2,7,1)
- **SET** nur Elza: sub05@0x016A6 Set(2,7,0)
- **SET** nur Elza: sub05@0x016AA Set(4,8,1)
- **SET** nur Elza: sub06@0x016C2 Set(9,21,1)
- **SET** nur Elza: sub07@0x016D0 Set(4,16,1)
- **SET** nur Elza: sub07@0x016D4 Set(3,57,1)
- **SET** nur Elza: sub07@0x016D8 Set(2,7,1)
- **SET** nur Elza: sub07@0x016DC Set(1,27,1)
- **TEXT** nur Elza id 1: "The power is off... Has a circuit breaker / tripped?"
- **TEXT** nur Elza id 3: " / I can't reach the breaker switch..."
- **TEXT** nur Elza id 5: "This breaker switch is flipped up... Flip / it back down? "
- **TEXT** nur Elza id 6: "The door is latched shut and won't open.I'd need a key to open this..."
- **TEXT** nur Elza id 8: " / I need to hurry up and save the others..."
- **TEXT** nur Elza id 9: "John: That was a close shave! / I almost got my ass handed to me!"
- **TEXT** nur Elza id 10: "John: Elza, are you okay?"
- **TEXT** nur Elza id 11: "Elza: Yes, somehow! I thought I was / done for!"
- **TEXT** nur Elza id 12: "John: Hey, you really saved my butt / there!"
- **TEXT** nur Elza id 13: "Elza: You really did save us, / thank you!"
- **TEXT** nur Elza id 14: "John: What's your name, kid?"
- **TEXT** nur Elza id 15: "Sherry: It's Sherry..."
- **TEXT** nur Elza id 16: "John: Sherry, huh? I'm John, and you / can rest assured that I'll keep you / safe from now on!"
- **TEXT** nur Elza id 17: "Elza: You seem to have taken a / liking to her."
- **TEXT** nur Elza id 18: "John: Well, of course, she saved my / life, you know? That's why I'll protect / her with my life!"
- **TEXT** nur Elza id 19: "Elza: How simplistic... Anyway, Sherry, / how did you know about that entrance?"
- **TEXT** nur Elza id 20: "Sherry: This is... where my dad used to / be..."
- **TEXT** nur Elza id 21: "Elza: Your... dad...?"
- **TEXT** nur Elza id 22: "Sherry: What was that?"
- **TEXT** nur Elza id 23: "John: I dunno. Sounds like somebody's / in there."
- **TEXT** nur Elza id 24: "Sherry: There wasn't anybody before."
- **TEXT** nur Elza id 25: "Elza: Let me deal with it. / John, take Sherry somewhere safe."
- **TEXT** nur Elza id 26: "John: Will do!"
- **TEXT** nur Elza id 27: "John: You stay with me now, you hear?"
- **TEXT** nur Elza id 28: "Sherry: Yes sir."
- **TEXT** nur Elza id 29: "Sherry: Good luck!"
- **TEXT** nur Elza id 30: "Elza: Thanks!"
- **TEXT** nur Elza id 31: "You've taken the Latch Key."
- **TEXT** nur Elza id 32: "You've used the Latch Key."

### ROOM30A  "NORTH OFFICE"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 262, 335.

Byte-identisch (208760 B).

### ROOM30B  "SOUTH OFFICE"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 70, 341.

Byte-identisch (175620 B).

### ROOM30C  "LOBBY"  [NPC, Texte*]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 224, 367, 370.

Groesse L 117896 / E 163416 B; Kopf L [0 15 0 0 0 0 0] / E [0 15 0 0 0 0 0]; Skripte L 3 / E 8; Texte L 2 / E 11; Abdeckung L 130/178 / E 824/1384

- **EM** nur Elza: sub00@0x00BEE Typ 0x40 NPC 0x40 (Marvin?) grid 0x40 kill - (-12450,0,-1000) dir 2048
- **AOT** nur Leon: main00@0x00B9A Slot 2 sce 1 flags 0x31 Text 1
- **AOT** nur Elza: main00@0x00B9A Slot 2 sce 1 flags 0x31 Text 10
- **AOTR** nur Elza: sub01@0x00C12 Aot_reset Slot 2 -> sce 1 Text 9
- **AOTR** nur Elza: sub01@0x00C1C Aot_reset Slot 3 -> sce 1 Text 9
- **MSGON** nur Elza: sub03@0x00CB2 Message_on 1
- **MSGON** nur Elza: sub03@0x00CD6 Message_on 2
- **MSGON** nur Elza: sub03@0x00D0E Message_on 3
- **MSGON** nur Elza: sub03@0x00D46 Message_on 4
- **MSGON** nur Elza: sub03@0x00D7E Message_on 5
- **MSGON** nur Elza: sub03@0x00DA2 Message_on 6
- **MSGON** nur Elza: sub03@0x00DC6 Message_on 7
- **MSGON** nur Elza: sub03@0x00DFE Message_on 8
- **EVT** nur Elza: sub00@0x00C02 Evt_exec sub03
- **EVT** nur Elza: sub03@0x00E32 Evt_exec sub06
- **CK** nur Elza: sub00@0x00BEA Ck(3,57,1)
- **CK** nur Elza: sub01@0x00C0E Ck(4,16,0)
- **CK** nur Elza: sub04@0x00E72 Ck(5,32,0)
- **CK** nur Elza: sub05@0x00E80 Ck(5,33,0)
- **SET** nur Elza: sub03@0x00C2C Set(3,57,0)
- **SET** nur Elza: sub03@0x00C30 Set(3,71,1)
- **SET** nur Elza: sub03@0x00C34 Set(2,7,1)
- **SET** nur Elza: sub03@0x00C38 Set(1,27,1)
- **SET** nur Elza: sub03@0x00E4E Set(2,7,0)
- **SET** nur Elza: sub03@0x00E52 Set(1,27,0)
- **TEXT** nur Elza id 1: "Elza: Who are you?"
- **TEXT** nur Elza id 2: "Officer: I'm Marvin Branagh, RPD. / I'm here with Leon Kennedy."
- **TEXT** nur Elza id 3: "Elza: Leon?! How is he? / We got separated back at the RPD."
- **TEXT** nur Elza id 4: "Marvin: He's all right and he's helping / me and Ada escape the city."
- **TEXT** nur Elza id 5: "Elza: Ada?"
- **TEXT** nur Elza id 6: "Marvin: Someone we rescued from the / RPD."
- **TEXT** nur Elza id 7: "Elza: Will you tell him that me and John / and Sherry are here and we're all right?"
- **TEXT** nur Elza id 8: "Marvin: Will do. See you soon!"
- **TEXT** nur Elza id 9: " / I need to hurry up and save the others..."

### ROOM30D  "LOB.PAR-MARVIN"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 369.

Byte-identisch (119948 B).

### ROOM30E  "FORKLIFT-FAC.L"  [NPC, Tueren, Objekte, Texte*]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 370, 371, 372.

Groesse L 295380 / E 263116 B; Kopf L [0 10 3 0 0 0 0] / E [0 10 3 0 0 0 0]; Skripte L 13 / E 5; Texte L 16 / E 19; Abdeckung L 1716/1718 / E 1250/1250

- **EM** nur Leon: sub11@0x01068 Typ 0x42 NPC 0x42 (Ada) grid 0x40 kill - (-22190,0,-23670) dir 2030
- **EM** nur Leon: sub11@0x0108C Typ 0x42 NPC 0x42 (Ada) grid 0x40 kill - (-22490,0,-25870) dir 3470
- **EM** nur Leon: sub11@0x010A0 Typ 0x40 NPC 0x40 (Marvin?) grid 0x40 kill - (-20020,0,-23340) dir 1940
- **EM** nur Elza: sub03@0x00EAC Typ 0x49 NPC 0x49 (Kendo?) grid 0x40 kill - (-20340,0,-23400) dir 2060
- **EM** nur Elza: sub03@0x00EC0 Typ 0x4B NPC 0x4B (Sherry?) grid 0x40 kill - (-20040,0,-23850) dir 2360
- **DOOR** nur Leon: main00@0x00A32 Slot 2 sce 2(Tuer) -> ROOM30E Cut 5
- **OBJ** nur Leon: main00@0x00A96 obj 0 typ 4 band 0 (-19554,0,22684) ry 0
- **AOT** nur Leon: main00@0x00A5A Slot 3 sce 3 flags 0x31 sub09
- **AOT** nur Leon: main00@0x00A6E Slot 4 sce 5 flags 0x44 Marke
- **AOT** nur Leon: main00@0x00A82 Slot 5 sce 5 flags 0x44 Marke
- **AOTR** nur Leon: sub03@0x00E72 Aot_reset Slot 0 -> sce 1 Text 14
- **AOTR** nur Leon: sub04@0x00FE0 Aot_reset Slot 3 -> sce 0 inert
- **AOTR** nur Elza: sub02@0x00B6A Aot_reset Slot 0 -> sce 1 Text 18
- **MSGON** nur Elza: sub02@0x00C98 Message_on 6
- **MSGON** nur Elza: sub02@0x00DE4 Message_on 14
- **MSGON** nur Elza: sub02@0x00E2A Message_on 16
- **MSGON** nur Elza: sub02@0x00E62 Message_on 17
- **EVT** nur Leon: sub01@0x00BFC Evt_exec sub05
- **EVT** nur Leon: sub01@0x00C14 Evt_exec sub06
- **EVT** nur Leon: sub01@0x00C2C Evt_exec sub07
- **EVT** nur Leon: sub01@0x00C44 Evt_exec sub08
- **EVT** nur Leon: sub01@0x00C6A Evt_exec sub10
- **EVT** nur Leon: sub01@0x00C92 Evt_exec sub04
- **EVT** nur Leon: sub11@0x010B4 Evt_exec sub03
- **CK** nur Leon: main00@0x00A56 Ck(3,156,0)
- **CK** nur Leon: sub01@0x00BF0 Ck(5,0,1)
- **CK** nur Leon: sub01@0x00C08 Ck(5,1,1)
- **CK** nur Leon: sub01@0x00C20 Ck(5,2,1)
- **CK** nur Leon: sub01@0x00C38 Ck(5,3,1)
- **CK** nur Leon: sub01@0x00C50 Ck(5,4,1)
- **CK** nur Leon: sub01@0x00C78 Ck(5,5,1)
- **CK** nur Leon: sub01@0x00CA0 Ck(5,6,1)
- **CK** nur Leon: sub11@0x01064 Ck(4,194,1)
- **CK** nur Leon: sub11@0x01088 Ck(3,75,1)
- **CK** nur Elza: sub03@0x00EA8 Ck(3,181,1)
- **SET** nur Leon: (1, 27, 0) x1, (1, 27, 1) x1, (2, 0, 0) x1, (2, 0, 1) x1, (2, 2, 0) x1, (2, 2, 1) x1, (2, 7, 0) x2, (2, 7, 1) x2, (3, 71, 1) x1, (3, 75, 0) x1, (3, 75, 1) x1, (3, 156, 1) x1, (4, 194, 0) x1, (5, 0, 0) x1, (5, 0, 1) x1, (5, 1, 0) x1, (5, 1, 1) x1, (5, 2, 0) x1, (5, 2, 1) x1, (5, 3, 0) x1, (5, 3, 1) x1, (5, 4, 0) x1, (5, 4, 1) x1, (5, 5, 0) x1, (5, 5, 1) x1, (5, 6, 0) x1, (5, 6, 1) x1
- **SET** nur Elza: (3, 181, 0) x1
- **TEXT** nur Leon id 0: "Ada: What was that noise?"
- **TEXT** nur Leon id 1: "Leon: Y-you...!"
- **TEXT** nur Leon id 2: "Ada: ...? Where's that other guy? / Wasn't he with you?"
- **TEXT** nur Leon id 3: "Leon: Y-yeah, Marvin is out in the / hall. He can't get out because of / all the monsters outside..."
- **TEXT** nur Leon id 4: "Leon: Ada... What have you been doing, / anyway!?"
- **TEXT** nur Leon id 5: "Leon: And what happened in that / tunnel, what's with all the police / officer corpses?"
- **TEXT** nur Leon id 6 (UNBENUTZT): " / Manhole...?"
- **TEXT** nur Leon id 7: "Ada: That's really bad... Those things / should already be trying to bust in / there."
- **TEXT** nur Leon id 8: "Ada: That glass won't hold up if they / keep pushing at it!"
- **TEXT** nur Leon id 9: "Leon: Marvin!! Are you okay!?"
- **TEXT** nur Leon id 10: "Leon: That wound... Did those things / get you? How careless..."
- **TEXT** nur Leon id 11: "Marvin: The hall is no good...! Those / things broke through the main gate / and got inside!"
- **TEXT** nur Leon id 12: "Leon: Say what!?"
- **TEXT** nur Leon id 13: "Marvin: The entire place is overrun! / And if they're already all the way out / here... the surface might just be done for..."
- **TEXT** nur Leon id 15: "There's a Flamethrower here.This weapon is not available / in this preview."
- **TEXT** nur Elza id 0: "Elza: John!"
- **TEXT** nur Elza id 1: "John: Elza! So this is where you were?"
- **TEXT** nur Elza id 2: "John: This is bad... one of those monsters / bit me as I tried to get away!"
- **TEXT** nur Elza id 3: "John: ...And there doesn't seem to be a / place with any medical supplies here..."
- **TEXT** nur Elza id 4: "John: Elza, how are things on your end? / Did you find any tasty food or something?"
- **TEXT** nur Elza id 5: "Elza: No, nothing like that..."
- **TEXT** nur Elza id 6: "John: Wh-what's wrong? Are those / walking corpses over there as well..."
- **TEXT** nur Elza id 7: "Elza: No... not at all... That's..."
- **TEXT** nur Elza id 8: "John: What is that...?"
- **TEXT** nur Elza id 9: "John: What are you talking about? / Is there something in there?"
- **TEXT** nur Elza id 10: "Elza: I'm not sure what it is, but... / That can't be... human, can it...?"
- **TEXT** nur Elza id 11: "John: Elza, is that thing still in there?"
- **TEXT** nur Elza id 12: "Elza: Yes, I think so... But I believe / it got scared and hid when it was / attacked..."
- **TEXT** nur Elza id 13: "Elza: I don't know where it could've / gone, though."
- **TEXT** nur Elza id 14: "Elza: ...Anyway, there are too many / walking corpses outside. We can't / really turn back."
- **TEXT** nur Elza id 15: "Elza: We have no choice but to go / inside..."
- **TEXT** nur Elza id 16: "John: But... that beast might still be in / there, right?"
- **TEXT** nur Elza id 17: "Elza: We can probably deal with it if / we stay alert... Let's go!"

### ROOM30F  "EMPTY"  

Byte-identisch (4 B).

### ROOM400  "LAB ENTRANCE"  [NPC, Gegner, Tueren, Objekte, Texte*]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 203, 218, 336, 373, 374.

Groesse L 288632 / E 328240 B; Kopf L [0 8 7 0 0 0 0] / E [0 8 4 0 0 0 0]; Skripte L 16 / E 19; Texte L 10 / E 15; Abdeckung L 2826/2828 / E 3092/3094

- **EM** nur Leon: main00@0x01262 Typ 0x29 Affe grid 0x00 kill 154 (-11500,0,-5700) dir 0
- **EM** nur Leon: sub00@0x012A2 Typ 0x42 NPC 0x42 (Ada) grid 0x40 kill - (-1603,0,-22311) dir 3096
- **EM** nur Leon: sub00@0x012B6 Typ 0x40 NPC 0x40 (Marvin?) grid 0x40 kill - (-1603,0,-22311) dir 3096
- **EM** nur Leon: sub00@0x012E0 Typ 0x42 NPC 0x42 (Ada) grid 0x40 kill - (-1603,0,-22311) dir 3096
- **EM** nur Leon: sub00@0x012F4 Typ 0x40 NPC 0x40 (Marvin?) grid 0x40 kill - (-1603,0,-22311) dir 3096
- **EM** nur Leon: sub00@0x01318 Typ 0x29 Affe grid 0x61 kill 197 (-14100,0,-21200) dir 3072
- **EM** nur Leon: sub00@0x0133A Typ 0x29 Affe grid 0x00 kill 197 (-12800,0,-10900) dir 3072
- **EM** nur Elza: sub00@0x01194 Typ 0x49 NPC 0x49 (Kendo?) grid 0x40 kill - (-4606,-3500,-2571) dir 1840
- **EM** nur Elza: sub00@0x011A8 Typ 0x4B NPC 0x4B (Sherry?) grid 0x40 kill - (-4952,-3500,-4725) dir 2032
- **EM** nur Elza: sub00@0x0122C Typ 0x2B Spinnenmann grid 0x41 kill 145 (-8280,0,-16110) dir 3072
- **EM** nur Elza: sub00@0x0126A Typ 0x2B Spinnenmann grid 0x00 kill 145 (-13880,0,-12560) dir 3072
- **EM** nur Elza: sub00@0x012BE Typ 0x2B Spinnenmann grid 0x41 kill 175 (-20160,0,-11070) dir 3072
- **EM** nur Elza: sub00@0x012FC Typ 0x2B Spinnenmann grid 0x00 kill 175 (-17560,0,-8770) dir 0
- **DOOR** nur Leon: main00@0x0109C Slot 1 sce 2(Tuer) -> ROOM401 Cut 0
- **DOOR** nur Leon: main00@0x0114C Slot 7 sce 2(Tuer) -> ROOM400 Cut 5
- **DOOR** nur Elza: main00@0x01084 Slot 1 sce 0(inert) -> ROOM401 Cut 0
- **OBJ** nur Leon: main00@0x011F4 obj 4 typ 0 band 1 (0,0,0) ry 3072
- **OBJ** nur Leon: main00@0x01216 obj 5 typ 0 band 1 (0,0,0) ry 3072
- **OBJ** nur Leon: main00@0x01238 obj 6 typ 0 band 1 (0,0,0) ry 3072
- **OBJ** nur Elza: sub00@0x011BC obj 1 typ 0 band 1 (-8280,-630,-16110) ry 3072
- **OBJ** nur Elza: sub00@0x011DE obj 2 typ 0 band 1 (-20160,-630,-11070) ry 3072
- **OBJ** nur Elza: sub00@0x0127E obj 1 typ 0 band 1 (-8060,0,-13239) ry 3369
- **OBJ** nur Elza: sub00@0x01310 obj 2 typ 0 band 1 (-20160,0,-9039) ry 3312
- **AOT** nur Leon: main00@0x01062 Slot 0 sce 1 flags 0x31 Text 9
- **AOT** nur Leon: main00@0x0112C Slot 6 sce 1 flags 0x31 Text 0
- **AOT** nur Elza: main00@0x0104A Slot 0 sce 1 flags 0x31 Text 13
- **AOT** nur Elza: main00@0x0110C Slot 8 sce 1 flags 0x31 Text 14
- **AOT** nur Elza: sub00@0x01218 Slot 4 sce 3 flags 0x41 sub05
- **AOT** nur Elza: sub00@0x012AA Slot 5 sce 3 flags 0x41 sub07
- **AOTR** nur Leon: main00@0x01140 Aot_reset Slot 2 -> sce 0 inert
- **AOTR** nur Leon: sub00@0x0134E Aot_reset Slot 2 -> sce 0 inert
- **AOTR** nur Leon: sub13@0x01AB8 Aot_reset Slot 2 -> sce 0 inert
- **AOTR** nur Elza: sub05@0x015AC Aot_reset Slot 4 -> sce 0 inert
- **AOTR** nur Elza: sub07@0x016E2 Aot_reset Slot 5 -> sce 0 inert
- **MSGON** nur Leon: sub02@0x0143A Message_on 0
- **MSGON** nur Leon: sub02@0x01440 Message_on 1
- **MSGON** nur Leon: sub02@0x0144A Message_on 2
- **MSGON** nur Elza: sub10@0x01A24 Message_on 9
- **MSGON** nur Elza: sub10@0x01A3C Message_on 10
- **MSGON** nur Elza: sub10@0x01A54 Message_on 11
- **MSGON** nur Elza: sub10@0x01AC8 Message_on 12
- **EVT** nur Leon: sub00@0x012CA Evt_exec sub05
- **EVT** nur Leon: sub05@0x01564 Evt_exec sub07
- **EVT** nur Elza: sub00@0x01262 Evt_exec sub02
- **EVT** nur Elza: sub00@0x012F4 Evt_exec sub03
- **EVT** nur Elza: sub01@0x01380 Evt_exec sub10
- **EVT** nur Elza: sub10@0x01874 Evt_exec sub11
- **EVT** nur Elza: sub10@0x01878 Evt_exec sub12
- **CK** nur Leon: (3, 32, 0) x1, (3, 32, 1) x1, (3, 63, 1) x1, (3, 101, 0) x1, (3, 101, 1) x1, (3, 231, 1) x1, (3, 232, 1) x1, (4, 38, 0) x1, (12, 31, 0) x1
- **CK** nur Elza: (3, 33, 0) x1, (3, 145, 0) x1, (3, 175, 0) x1, (4, 20, 0) x1
- **SET** nur Leon: (1, 27, 1) x2, (2, 7, 1) x2, (3, 32, 1) x1, (3, 63, 0) x1, (3, 72, 1) x1, (3, 231, 1) x1, (3, 232, 0) x1, (3, 232, 1) x1, (4, 20, 1) x1, (4, 38, 1) x1
- **SET** nur Elza: (3, 33, 1) x1, (3, 145, 1) x1, (3, 175, 1) x1
- **TEXT** nur Leon id 3: "Leon: Ada, what is this place...?"
- **TEXT** nur Leon id 4: "Ada: Just hurry up, okay?! / Let's get him to this room here!"
- **TEXT** nur Leon id 5: "Leon: Go on ahead! / I'll take care of this!"
- **TEXT** nur Leon id 6: "Ada: Leon...!"
- **TEXT** nur Leon id 7: "Leon: Just get going already!"
- **TEXT** nur Leon id 8: "Leon: Take care of Marvin!"
- **TEXT** nur Elza id 3: "Elza: What? What is this place...!?"
- **TEXT** nur Elza id 4: "John: Wow, this sure is something..."
- **TEXT** nur Elza id 5: "Elza: Sherry, what is this place?"
- **TEXT** nur Elza id 6: "John: Do you know anything about / this place?"
- **TEXT** nur Elza id 7: "Sherry: This is dad's research center. / He should be around here somewhere."
- **TEXT** nur Elza id 8: "Sherry: I'll go look for him!"
- **TEXT** nur Elza id 9: "John: Sherry, hold on!"
- **TEXT** nur Elza id 10: "John: Don't go alone!"
- **TEXT** nur Elza id 11: "Elza: Sherry!"
- **TEXT** nur Elza id 12: "John: Elza, let's go after her!"
- **TEXT** nur Elza id 14: "Security Room"It's locked."

### ROOM401  "SECURITY ROOM"  [NPC, Items, Objekte, Texte*]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 176, 213, 311, 336, 375.

Groesse L 297112 / E 194208 B; Kopf L [0 6 4 0 0 0 0] / E [0 6 4 0 0 0 0]; Skripte L 10 / E 5; Texte L 44 / E 8; Abdeckung L 2762/2764 / E 372/374

- **EM** nur Leon: main00@0x00664 Typ 0x42 NPC 0x42 (Ada) grid 0x40 kill - (-16557,0,-23410) dir 0
- **EM** nur Leon: main00@0x00678 Typ 0x40 NPC 0x40 (Marvin?) grid 0x40 kill - (-19267,-720,-19865) dir 3072
- **ITEM** nur Leon: main00@0x00592 Slot 9 Item 0x47 x1 Taken-Bit (9,6) Prop 1 sce 9
- **OBJ** nur Leon: main00@0x005F6 obj 1 typ 0 band 1 (-25754,-1260,-19510) ry 0
- **AOT** nur Leon: main00@0x004F2 Slot 1 sce 3 flags 0x31 sub08
- **AOT** nur Leon: main00@0x00506 Slot 2 sce 1 flags 0x31 Text 36
- **AOT** nur Leon: main00@0x0051A Slot 3 sce 1 flags 0x31 Text 37
- **AOT** nur Leon: main00@0x0052E Slot 4 sce 1 flags 0x31 Text 38
- **AOT** nur Leon: main00@0x00542 Slot 5 sce 1 flags 0x31 Text 39
- **AOT** nur Leon: main00@0x00556 Slot 6 sce 1 flags 0x31 Text 40
- **AOT** nur Leon: main00@0x0056A Slot 7 sce 1 flags 0x31 Text 41
- **AOT** nur Leon: main00@0x0057E Slot 8 sce 3 flags 0x31 sub07
- **AOT** nur Leon: main00@0x0068C Slot 12 sce 1 flags 0x31 Text 31
- **AOT** nur Elza: main00@0x004F2 Slot 1 sce 1 flags 0x31 Text 0
- **AOT** nur Elza: main00@0x00506 Slot 2 sce 1 flags 0x31 Text 1
- **AOT** nur Elza: main00@0x0051A Slot 3 sce 1 flags 0x31 Text 2
- **AOT** nur Elza: main00@0x0052E Slot 4 sce 1 flags 0x31 Text 3
- **AOT** nur Elza: main00@0x00542 Slot 5 sce 1 flags 0x31 Text 4
- **AOT** nur Elza: main00@0x00556 Slot 6 sce 1 flags 0x31 Text 5
- **AOT** nur Elza: main00@0x00596 Slot 9 sce 3 flags 0x31 sub02
- **AOT** nur Elza: main00@0x005AA Slot 10 sce 3 flags 0x31 sub03
- **AOTR** nur Leon: sub04@0x00DB8 Aot_reset Slot 12 -> sce 0 inert
- **MSGON** nur Leon: (0,) x1, (1,) x1, (2,) x1, (3,) x1, (4,) x1, (5,) x1, (8,) x1, (9,) x1, (10,) x1, (11,) x1, (12,) x1, (13,) x1, (14,) x1, (15,) x1, (16,) x1, (17,) x1, (18,) x1, (19,) x1, (20,) x1, (21,) x1, (22,) x1, (23,) x1, (24,) x1, (25,) x1, (26,) x1, (27,) x1, (28,) x1, (29,) x1, (30,) x1, (32,) x1, (33,) x1, (34,) x1, (35,) x1, (42,) x1, (43,) x1
- **MSGON** nur Elza: 
- **EVT** nur Leon: main00@0x006A0 Evt_exec sub03
- **EVT** nur Leon: sub00@0x006C2 Evt_exec sub02
- **EVT** nur Leon: sub00@0x006D2 Evt_exec sub04
- **CK** nur Leon: main00@0x00660 Ck(3,72,0)
- **CK** nur Leon: sub00@0x006BE Ck(4,34,0)
- **CK** nur Leon: sub00@0x006CE Ck(4,250,1)
- **CK** nur Leon: sub01@0x006E0 Ck(9,6,1)
- **CK** nur Leon: sub01@0x006E8 Ck(3,101,0)
- **CK** nur Leon: sub05@0x00F7E Ck(5,32,0)
- **CK** nur Leon: sub06@0x00F8C Ck(5,33,0)
- **SET** nur Leon: (1, 27, 0) x2, (1, 27, 1) x2, (2, 7, 0) x2, (2, 7, 1) x2, (3, 63, 1) x1, (3, 76, 1) x1, (3, 79, 1) x1, (3, 101, 1) x1, (4, 20, 0) x1, (4, 34, 1) x1, (4, 250, 0) x1
- **SET** nur Elza: 
- **TEXT** nur Leon id 0: "Ada: Leon, that wound..."
- **TEXT** nur Leon id 1: "Leon: Yeah, we need to hurry."
- **TEXT** nur Leon id 2: "Leon: Ada... Just who are you, anyway?"
- **TEXT** nur Leon id 3: "Leon: Don't tell me..."
- **TEXT** nur Leon id 4: "Ada: That's right. / I'm with Umbrella."
- **TEXT** nur Leon id 5: "Ada: Leon... There's no time to talk / right now."
- **TEXT** nur Leon id 6: "Ada: If we don't hurry, he's going / to die."
- **TEXT** nur Leon id 7: "Leon: What should we do?"
- **TEXT** nur Leon id 8: "Ada: It's a virus that makes people / turn into those things. If we can / get him the vaccine, it won't break out."
- **TEXT** nur Leon id 9: "Leon: And where's this vaccine?"
- **TEXT** nur Leon id 10: "Ada: The vaccine itself isn't here, / but we can make one from a strain of / the virus."
- **TEXT** nur Leon id 11: "Ada: However..."
- **TEXT** nur Leon id 12: "Leon: Ada? What's wrong?"
- **TEXT** nur Leon id 13: "Ada: Leon, I can't help you."
- **TEXT** nur Leon id 14: "Leon: Wait, what!?"
- **TEXT** nur Leon id 15: "Ada: There's something I have to do."
- **TEXT** nur Leon id 16: "Ada: You've helped me out a lot, but / I have to do this myself! "
- **TEXT** nur Leon id 17: "Leon: I'll..."
- **TEXT** nur Leon id 18: "Leon: ...do it on my own if I have to!"
- **TEXT** nur Leon id 19: "Leon: So, please tell me! / What should I do!?"
- **TEXT** nur Leon id 20: "Ada: The viral strain should be stored / in this research center's ultra-low / temperature laboratory."
- **TEXT** nur Leon id 21: "Ada: As long as you can start up the / program for it, it should handle the / rest automatically."
- **TEXT** nur Leon id 22: "Ada: However, the Virus Inhibitor is / very delicate."
- **TEXT** nur Leon id 23: "Ada: If you don't handle it with / extreme care, it'll all be for naught."
- **TEXT** nur Leon id 24: "Leon: Well, I have to give it a go."
- **TEXT** nur Leon id 25: "Leon: Marvin... I'll save you!"
- **TEXT** nur Leon id 26: "Ada: Leon. When all this is over, / head to the underground shelter!"
- **TEXT** nur Leon id 27: "Ada: If you don't, you probably won't / survive."
- **TEXT** nur Leon id 28: "Ada: And keep in mind that this is an / Umbrella research facility. Be careful."
- **TEXT** nur Leon id 29: "Leon: Ada, wait!"
- **TEXT** nur Leon id 30: "Leon: I probably won't survive...?"
- **TEXT** nur Leon id 31: "I need to hurry up and get that / vaccine!"
- **TEXT** nur Leon id 32: "Leon: What's wrong, Marvin? Open your / eyes!!"
- **TEXT** nur Leon id 33: "Leon: Ada!"
- **TEXT** nur Leon id 34: "Ada: Leon! What are you doing!? The / virus is on the loose here as well!"
- **TEXT** nur Leon id 35: "Ada: Hurry to the shelter!"
- **TEXT** nur Leon id 38: "This equipment seems to be unusable."
- **TEXT** nur Elza id 2: " / This equipment seems to be unusable."

### ROOM402  "A-2 ELEVATOR"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 71, 245.

Byte-identisch (178440 B).

### ROOM403  "MAIN SHAFT"  [NPC, Gegner, Texte*]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 376.

Groesse L 142824 / E 187740 B; Kopf L [0 6 1 0 0 0 0] / E [0 6 1 0 0 0 0]; Skripte L 9 / E 12; Texte L 5 / E 9; Abdeckung L 684/686 / E 1104/1108

- **EM** nur Elza: main00@0x00556 Typ 0x49 NPC 0x49 (Kendo?) grid 0x40 kill - (-24275,0,-20772) dir 979
- **EM** nur Elza: main00@0x0056A Typ 0x2D Sicherungsgeraet grid 0x00 kill - (-23040,-450,-23040) dir 512
- **MSGON** nur Elza: sub08@0x007BE Message_on 5
- **MSGON** nur Elza: sub08@0x007C6 Message_on 6
- **MSGON** nur Elza: sub08@0x007F4 Message_on 7
- **MSGON** nur Elza: sub08@0x00818 Message_on 8
- **EVT** nur Elza: main00@0x0057E Evt_exec sub08
- **CK** nur Elza: main00@0x00552 Ck(3,88,0)
- **CK** nur Elza: sub09@0x008B2 Ck(5,32,0)
- **CK** nur Elza: sub10@0x008C0 Ck(5,33,0)
- **SET** nur Elza: sub06@0x006D2 Set(1,27,1)
- **SET** nur Elza: sub06@0x006FC Set(1,27,0)
- **SET** nur Elza: sub08@0x0075C Set(3,88,1)
- **SET** nur Elza: sub08@0x00764 Set(2,7,1)
- **SET** nur Elza: sub08@0x0089C Set(2,7,0)
- **TEXT** nur Elza id 5: "John: Sherry!"
- **TEXT** nur Elza id 6: "John: What the? / Where did she go!?"
- **TEXT** nur Elza id 7: "Elza: Let's split up and look for / her!"
- **TEXT** nur Elza id 8: "John: Okay! I'll go this way, you / go that way, Elza!"

### ROOM404  "EAST PASSAGE"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 167, 260.

Byte-identisch (156844 B).

### ROOM405  "PRIVATE ROOMS"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 72.

Byte-identisch (221128 B).

### ROOM406  "EMPTY"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 282.

Byte-identisch (4 B).

### ROOM407  "EMERGE. STAIRS"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 73, 244.

Byte-identisch (102320 B).

### ROOM408  "WEST PASSAGE"  

Byte-identisch (145212 B).

### ROOM409  "FREEZING ROOM"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 168.

Byte-identisch (118896 B).

### ROOM40A  "CT SCAN"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 29, 110, 115, 116.

Groesse L 97496 / E 97496 B; Kopf L [0 8 0 0 0 0 0] / E [0 8 0 0 0 0 0]; Skripte L 8 / E 8; Texte L 14 / E 14; Abdeckung L 846/848 / E 846/848

- (keine Unterschiede in den gezaehlten Kategorien; Byte-Unterschied liegt in anderen Sektionen, z.B. Kamera/Kollision/Modelle/Offsets)

### ROOM40B  "CELL CULTIVE"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 312.

Byte-identisch (196460 B).

### ROOM40C  "EMPTY"  

Byte-identisch (4 B).

### ROOM40D  "EMPTY"  

Byte-identisch (4 B).

### ROOM40E  "EMPTY"  

Byte-identisch (4 B).

### ROOM40F  "EMPTY"  

Byte-identisch (4 B).

### ROOM500  "A-2 ENTRANCE"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 74, 157.

Groesse L 128700 / E 128712 B; Kopf L [0 9 2 0 0 0 0] / E [0 9 2 0 0 0 0]; Skripte L 3 / E 3; Texte L 1 / E 1; Abdeckung L 322/322 / E 332/334

- **CK** nur Elza: sub00@0x008D0 Ck(3,47,1)

### ROOM501  "SECURITY ROOM"  [NPC, Texte*]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 176, 342, 377.

Groesse L 173880 / E 360020 B; Kopf L [0 5 3 0 0 0 0] / E [0 5 3 0 0 0 0]; Skripte L 5 / E 10; Texte L 7 / E 46; Abdeckung L 352/354 / E 2710/2712

- **EM** nur Elza: main00@0x004E0 Typ 0x49 NPC 0x49 (Kendo?) grid 0x40 kill - (-24132,0,-22479) dir 2048
- **EM** nur Elza: main00@0x004F4 Typ 0x4B NPC 0x4B (Sherry?) grid 0x40 kill - (-25911,-50,-22700) dir 0
- **AOT** nur Leon: main00@0x003BA Slot 1 sce 1 flags 0x31 Text 0
- **AOT** nur Leon: main00@0x003CE Slot 2 sce 1 flags 0x31 Text 1
- **AOT** nur Leon: main00@0x003F6 Slot 4 sce 1 flags 0x31 Text 3
- **AOT** nur Leon: main00@0x0040A Slot 5 sce 1 flags 0x31 Text 4
- **AOT** nur Leon: main00@0x0044A Slot 8 sce 3 flags 0x31 sub02
- **AOT** nur Leon: main00@0x0045E Slot 9 sce 3 flags 0x31 sub03
- **AOT** nur Elza: main00@0x003BA Slot 1 sce 3 flags 0x31 sub08
- **AOT** nur Elza: main00@0x003CE Slot 2 sce 1 flags 0x31 Text 38
- **AOT** nur Elza: main00@0x003E2 Slot 3 sce 1 flags 0x31 Text 39
- **AOT** nur Elza: main00@0x0040A Slot 5 sce 1 flags 0x31 Text 41
- **AOT** nur Elza: main00@0x0041E Slot 6 sce 1 flags 0x31 Text 42
- **AOT** nur Elza: main00@0x0045E Slot 10 sce 3 flags 0x31 sub07
- **AOT** nur Elza: main00@0x00508 Slot 7 sce 1 flags 0x31 Text 43
- **AOTR** nur Elza: sub04@0x00958 Aot_reset Slot 7 -> sce 0 inert
- **MSGON** nur Leon: 
- **MSGON** nur Elza: (0,) x1, (1,) x1, (2,) x1, (3,) x1, (4,) x1, (7,) x1, (8,) x1, (9,) x1, (10,) x1, (11,) x1, (12,) x1, (13,) x1, (14,) x1, (15,) x1, (16,) x1, (17,) x1, (18,) x1, (19,) x1, (20,) x1, (21,) x1, (22,) x1, (23,) x1, (24,) x1, (25,) x1, (26,) x1, (27,) x1, (28,) x1, (29,) x1, (30,) x1, (31,) x1, (32,) x1, (33,) x1, (34,) x1, (35,) x1, (36,) x1, (37,) x1, (44,) x1, (45,) x1
- **EVT** nur Elza: main00@0x0051C Evt_exec sub03
- **EVT** nur Elza: sub00@0x0053E Evt_exec sub02
- **EVT** nur Elza: sub00@0x0054E Evt_exec sub04
- **CK** nur Elza: main00@0x004DC Ck(3,58,1)
- **CK** nur Elza: sub00@0x0053A Ck(4,237,1)
- **CK** nur Elza: sub00@0x0054A Ck(4,248,1)
- **CK** nur Elza: sub05@0x00E12 Ck(5,32,0)
- **CK** nur Elza: sub06@0x00E20 Ck(5,33,0)
- **SET** nur Elza: sub02@0x0055A Set(4,237,0)
- **SET** nur Elza: sub02@0x0055E Set(3,79,1)
- **SET** nur Elza: sub02@0x00562 Set(2,7,1)
- **SET** nur Elza: sub02@0x00566 Set(1,27,1)
- **SET** nur Elza: sub02@0x00930 Set(2,7,0)
- **SET** nur Elza: sub02@0x00934 Set(1,27,0)
- **SET** nur Elza: sub04@0x00948 Set(4,248,0)
- **SET** nur Elza: sub04@0x0094C Set(3,76,1)
- **SET** nur Elza: sub04@0x00950 Set(2,7,1)
- **SET** nur Elza: sub04@0x00954 Set(1,27,1)
- **SET** nur Elza: sub04@0x00E00 Set(2,7,0)
- **SET** nur Elza: sub04@0x00E04 Set(1,27,0)
- **TEXT** nur Elza id 0: "Elza: Sherry! Wake up!"
- **TEXT** nur Elza id 1: "John: It's no good... / She's passed out."
- **TEXT** nur Elza id 2: "Elza: Ahh... Sherry..."
- **TEXT** nur Elza id 3: "Elza: John. I read some files in this / research center, and I found out / some terrifying things."
- **TEXT** nur Elza id 4: "Elza: She's been infected by what / they were working on here."
- **TEXT** nur Elza id 5: "John: Elza, have a look at this."
- **TEXT** nur Elza id 6: "Elza: What is this...?"
- **TEXT** nur Elza id 7: "John: If we can't get the vaccine, / Sherry will..."
- **TEXT** nur Elza id 8: "Elza: Okay. I'll go get it."
- **TEXT** nur Elza id 9: "John: What?"
- **TEXT** nur Elza id 10: "Elza: While we're talking, the virus / is spreading inside Sherry's body."
- **TEXT** nur Elza id 11: "Elza: Even if it's just a chance, / I have to take it."
- **TEXT** nur Elza id 12: "John: Elza..."
- **TEXT** nur Elza id 13: "Elza: John, keep a close eye on her."
- **TEXT** nur Elza id 14: "Elza: Sherry needs you right now."
- **TEXT** nur Elza id 15: "John: Elza, please..."
- **TEXT** nur Elza id 16: "John: ...Please save this girl, even / if it's just her."
- **TEXT** nur Elza id 17: "Elza: John, I've got the vaccine!"
- **TEXT** nur Elza id 18: "Elza: How's Sherry?"
- **TEXT** nur Elza id 19: "John: Quick! She doesn't have long!"
- **TEXT** nur Elza id 20: "Elza: Sherry! Wake up!"
- **TEXT** nur Elza id 21: "John: Sherry, what's wrong!? / Talk to me!"
- **TEXT** nur Elza id 22: "Elza: Sherry, please... Wake up...!"
- **TEXT** nur Elza id 23: "John: What's wrong...?"
- **TEXT** nur Elza id 24: "Elza: Sherry! You're awake!"
- **TEXT** nur Elza id 25: "Elza: What a relief..."
- **TEXT** nur Elza id 26: "John: Yes, I was really worried!"
- **TEXT** nur Elza id 27: "Elza: John, did you hear that / announcement?"
- **TEXT** nur Elza id 28: "John: Yeah. What's going on!?"
- **TEXT** nur Elza id 29: "Elza: It seems like that beast broke / the place's power core."
- **TEXT** nur Elza id 30: "Elza: And fire's starting to spread / all over the place."
- **TEXT** nur Elza id 31: "John: What!? That means we're also / in danger!"
- **TEXT** nur Elza id 32: "Elza: That's right! We can't stay / here, we need to hurry to the shelter!"
- **TEXT** nur Elza id 33: "John: Sherry, do you know where the / shelter is?"
- **TEXT** nur Elza id 34: "Sherry: Y-yes."
- **TEXT** nur Elza id 35: "Sherry: The second shelter should be / open."
- **TEXT** nur Elza id 36: "John: Alright, let's get going!"
- **TEXT** nur Elza id 37: "Elza: Lead the way, Sherry!"
- **TEXT** nur Elza id 43: " / I need to hurry up and get that vaccine!"

### ROOM502  "CARGO STORAGE"  [NPC, Gegner, Items, Tueren, Texte*]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 346, 378.

Groesse L 136380 / E 164720 B; Kopf L [0 13 1 0 0 0 0] / E [0 13 1 0 0 0 0]; Skripte L 5 / E 8; Texte L 0 / E 2; Abdeckung L 384/386 / E 552/556

- **EM** nur Leon: sub00@0x012D6 Typ 0x18 Labor-Zombie grid 0xAE kill - (-2000,0,-14150) dir 0
- **EM** nur Elza: sub00@0x01316 Typ 0x4B NPC 0x4B (Sherry?) grid 0x40 kill - (-1500,0,-14200) dir 0
- **ITEM** nur Leon: sub00@0x012C0 Slot 3 Item 0x17 x12 Taken-Bit (9,135) Prop 255 sce 9
- **DOOR** nur Elza: main00@0x011FE Slot 5 sce 2(Tuer) -> ROOM501 Cut 0
- **AOT** nur Leon: main00@0x012A0 Slot 6 sce 3 flags 0x41 sub03
- **AOT** nur Elza: main00@0x012AC Slot 7 sce 3 flags 0x41 sub05
- **AOT** nur Elza: main00@0x012C0 Slot 8 sce 3 flags 0x41 sub06
- **AOT** nur Elza: sub00@0x01302 Slot 6 sce 1 flags 0x31 Text 0
- **AOTR** nur Leon: sub02@0x0130C Aot_reset Slot 5 -> sce 0 inert
- **AOTR** nur Leon: sub02@0x0132E Aot_reset Slot 6 -> sce 3 sub03
- **AOTR** nur Leon: sub03@0x0133A Aot_reset Slot 6 -> sce 0 inert
- **AOTR** nur Leon: sub03@0x0135C Aot_reset Slot 5 -> sce 3 sub02
- **AOTR** nur Elza: sub02@0x0136A Aot_reset Slot 3 -> sce 0 inert
- **AOTR** nur Elza: sub05@0x013BA Aot_reset Slot 7 -> sce 0 inert
- **AOTR** nur Elza: sub05@0x013DC Aot_reset Slot 8 -> sce 3 sub06
- **AOTR** nur Elza: sub06@0x013E8 Aot_reset Slot 8 -> sce 0 inert
- **AOTR** nur Elza: sub06@0x0140A Aot_reset Slot 7 -> sce 3 sub05
- **MSGON** nur Elza: sub02@0x01392 Message_on 1
- **EVT** nur Elza: sub00@0x0132A Evt_exec sub03
- **CK** nur Elza: sub00@0x012EA Ck(4,192,0)
- **CK** nur Elza: sub04@0x013B4 Ck(5,32,0)
- **SET** nur Elza: sub02@0x01352 Set(4,192,1)
- **SET** nur Elza: sub02@0x01356 Set(4,237,1)
- **SET** nur Elza: sub02@0x0135A Set(3,58,1)
- **SET** nur Elza: sub02@0x0135E Set(3,47,1)
- **SET** nur Elza: sub02@0x01362 Set(2,7,1)
- **SET** nur Elza: sub02@0x01366 Set(1,27,1)
- **TEXT** nur Elza id 0: "Sherry must have crawled through here, / but the hole is too small for me.There must be another way to / get back there."
- **TEXT** nur Elza id 1: "Elza: Sherry..."

### ROOM503  "MONITOR ROOM"  [NPC, Gegner, Texte*]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 35, 162.

Groesse L 158772 / E 229300 B; Kopf L [0 12 12 0 0 0 0] / E [0 12 12 0 0 0 0]; Skripte L 4 / E 7; Texte L 3 / E 4; Abdeckung L 616/620 / E 974/978

- **EM** nur Elza: sub00@0x010B6 Typ 0x4B NPC 0x4B (Sherry?) grid 0x40 kill - (14100,0,17050) dir 0
- **EM** nur Elza: sub00@0x010CA Typ 0x29 Affe grid 0x61 kill - (18200,0,17050) dir 2048
- **AOT** nur Elza: sub00@0x010E8 Slot 6 sce 3 flags 0xC1 sub03
- **AOTR** nur Elza: sub03@0x01138 Aot_reset Slot 6 -> sce 0 inert
- **MSGON** nur Elza: sub03@0x011EC Message_on 3
- **CK** nur Elza: sub00@0x010B2 Ck(3,47,0)
- **CK** nur Elza: sub00@0x010E4 Ck(3,47,0)
- **CK** nur Elza: sub04@0x0122E Ck(5,32,0)
- **CK** nur Elza: sub05@0x0123C Ck(5,33,0)
- **SET** nur Elza: sub03@0x01134 Set(3,47,1)
- **SET** nur Elza: sub03@0x01142 Set(2,7,1)
- **SET** nur Elza: sub03@0x01146 Set(1,27,1)
- **SET** nur Elza: sub03@0x011A8 Set(1,30,1)
- **SET** nur Elza: sub03@0x011E6 Set(1,30,0)
- **SET** nur Elza: sub03@0x0120C Set(2,7,0)
- **SET** nur Elza: sub03@0x01210 Set(1,27,0)
- **TEXT** nur Elza id 3: "Elza: Sherry!"

### ROOM504  "B-2 CORRIDOR"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 219.

Byte-identisch (209404 B).

### ROOM505  "SERVER ROOM"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 312.

Byte-identisch (195312 B).

### ROOM506  "P-4 LABORATORY"  [Texte]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 31, 202, 220, 379.

Groesse L 270976 / E 270976 B; Kopf L [0 13 9 0 0 0 0] / E [0 13 9 0 0 0 0]; Skripte L 8 / E 8; Texte L 15 / E 15; Abdeckung L 1988/1992 / E 1988/1992

- **SET** nur Leon: sub06@0x030E2 Set(4,250,1)
- **SET** nur Elza: sub06@0x030E2 Set(4,248,1)
- **TEXT** nur Leon id 13: "You've taken the T-Vaccine."
- **TEXT** nur Elza id 13: "You've taken the G-Vaccine."

### ROOM507  "D-2 CORRIDOR"  

Byte-identisch (204548 B).

### ROOM508  "FUEL CONTROL"  

Groesse L 101096 / E 101088 B; Kopf L [0 5 0 0 0 0 0] / E [0 5 0 0 0 0 0]; Skripte L 6 / E 6; Texte L 4 / E 4; Abdeckung L 364/364 / E 356/356

- **SET** nur Leon: sub00@0x00742 Set(1,0,1)
- **SET** nur Leon: sub00@0x0075A Set(1,0,0)

### ROOM509  "TRAIN CAR ABC"  [NPC]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 158.

Groesse L 187708 / E 191696 B; Kopf L [0 16 0 0 0 0 0] / E [0 16 0 0 0 0 0]; Skripte L 6 / E 6; Texte L 2 / E 2; Abdeckung L 762/764 / E 734/736

- **EM** nur Leon: sub00@0x0122A Typ 0x4D NPC 0x4D (Annette?) grid 0x40 kill - (600,0,-25300) dir 0
- **SET** nur Leon: sub00@0x01246 Set(1,0,1)
- **SET** nur Leon: sub00@0x0125E Set(1,0,0)

### ROOM50A  "C-2 CORRIDOR"  

Byte-identisch (131832 B).

### ROOM50B  "C-2 HANGAR"  [NPC, Gegner, Objekte, Texte*]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 328.

Groesse L 177296 / E 225512 B; Kopf L [0 4 0 0 0 0 0] / E [0 4 1 0 0 0 0]; Skripte L 10 / E 4; Texte L 19 / E 1; Abdeckung L 1718/1720 / E 178/180

- **EM** nur Leon: sub00@0x00512 Typ 0x42 NPC 0x42 (Ada) grid 0x40 kill - (-20350,0,-11600) dir 2048
- **EM** nur Leon: sub00@0x00526 Typ 0x29 Affe grid 0x61 kill 128 (-19500,0,10500) dir 0
- **EM** nur Leon: sub00@0x0053A Typ 0x29 Affe grid 0x61 kill 129 (-13500,0,10500) dir 0
- **EM** nur Leon: sub00@0x00566 Typ 0x42 NPC 0x42 (Ada) grid 0x40 kill - (-11800,0,-11600) dir 2048
- **OBJ** nur Elza: sub00@0x004F2 obj 0 typ 0 band 1 (-11800,0,-11600) ry 2048
- **AOT** nur Leon: sub00@0x004EA Slot 2 sce 0 flags 0x31 inert
- **AOT** nur Leon: sub00@0x004FE Slot 3 sce 3 flags 0x41 sub02
- **AOT** nur Leon: sub00@0x00552 Slot 2 sce 3 flags 0x31 sub08
- **AOT** nur Elza: sub00@0x004DE Slot 2 sce 3 flags 0x31 sub02
- **AOTR** nur Leon: sub02@0x005AA Aot_reset Slot 3 -> sce 0 inert
- **AOTR** nur Leon: sub02@0x005B4 Aot_reset Slot 0 -> sce 1 Text 17
- **AOTR** nur Leon: sub02@0x005BE Aot_reset Slot 1 -> sce 1 Text 17
- **AOTR** nur Leon: sub04@0x00AB4 Aot_reset Slot 0 -> sce 2 Tuer
- **AOTR** nur Leon: sub04@0x00ABE Aot_reset Slot 1 -> sce 2 Tuer
- **AOTR** nur Leon: sub04@0x00AE6 Aot_reset Slot 2 -> sce 3 sub08
- **AOTR** nur Leon: sub08@0x00B4A Aot_reset Slot 2 -> sce 3 sub08
- **AOTR** nur Elza: sub02@0x00542 Aot_reset Slot 2 -> sce 3 sub02
- **MSGON** nur Leon: (1,) x1, (2,) x1, (3,) x1, (4,) x1, (5,) x1, (6,) x1, (7,) x1, (8,) x1, (9,) x1, (10,) x1, (11,) x1, (12,) x1, (13,) x1, (14,) x1, (15,) x1, (16,) x1, (18,) x1
- **MSGON** nur Elza: 
- **EVT** nur Leon: sub00@0x0057A Evt_exec sub07
- **EVT** nur Leon: sub01@0x0059A Evt_exec sub04
- **EVT** nur Leon: sub02@0x0093E Evt_exec sub03
- **CK** nur Leon: sub00@0x004E6 Ck(3,98,0)
- **CK** nur Leon: sub01@0x00586 Ck(3,99,0)
- **CK** nur Leon: sub01@0x0058E Ck(8,128,1)
- **CK** nur Leon: sub01@0x00596 Ck(8,129,1)
- **CK** nur Leon: sub05@0x00AFA Ck(5,32,0)
- **CK** nur Leon: sub06@0x00B08 Ck(5,33,0)
- **SET** nur Leon: sub02@0x005A6 Set(3,98,1)
- **SET** nur Leon: sub02@0x005CC Set(2,7,1)
- **SET** nur Leon: sub02@0x005D0 Set(1,27,1)
- **SET** nur Leon: sub02@0x00984 Set(2,7,0)
- **SET** nur Leon: sub02@0x00988 Set(1,27,0)
- **SET** nur Leon: sub04@0x009EA Set(3,99,1)
- **SET** nur Leon: sub04@0x009EE Set(2,7,1)
- **SET** nur Leon: sub04@0x009F2 Set(1,27,1)
- **SET** nur Leon: sub04@0x00ACC Set(2,7,0)
- **SET** nur Leon: sub04@0x00AD0 Set(1,27,0)
- **TEXT** nur Leon id 0: "Ada: We're all set. Annette is waiting / for us below, and I left Marvin with her."
- **TEXT** nur Leon id 1: "Ada: Your other friends are down there / with her too."
- **TEXT** nur Leon id 2: "Leon: That's good. Who's Annette?"
- **TEXT** nur Leon id 3: "Ada: Dr. Birkin's wife and my boss. / She's going to help us with the train."
- **TEXT** nur Leon id 4: "Leon: Who's Dr. Birkin, and where is he?"
- **TEXT** nur Leon id 5: "Ada: He..."
- **TEXT** nur Leon id 6: "Ada: He's dead. Umbrella killed him. / That's why Annette is helping us."
- **TEXT** nur Leon id 7: "Leon: What's with those?"
- **TEXT** nur Leon id 8: "Ada: They're part of Umbrella's / experiments here."
- **TEXT** nur Leon id 9: "Ada: They were to be shipped out for / use as biological weapons."
- **TEXT** nur Leon id 10: "Leon: Ada, why did you set the reactor / to blow up?"
- **TEXT** nur Leon id 11: "Ada: So I can destroy this place. / You've seen what's in here."
- **TEXT** nur Leon id 12: "Ada: You've seen what they can do. / I had to do it, understand? It's..."
- **TEXT** nur Leon id 13: "Ada: Aaaaaaaagggghhh!"
- **TEXT** nur Leon id 14: "Leon: Ada..."
- **TEXT** nur Leon id 15: "Ada: I... I'm sorry, Leon. I'm not... / going... to make it... am I?"
- **TEXT** nur Leon id 16: "Ada: I... I so wanted... to escape... / you... will have... to do that... for me..."
- **TEXT** nur Leon id 17: "I can't leave Ada alone!"
- **TEXT** nur Leon id 18: "She's dead."
- **TEXT** nur Elza id 0: "It's Ada. She's dead.It looks like one of those / gorillas killed her."

### ROOM50C  "A-3 CORRIDOR"  

Byte-identisch (105908 B).

### ROOM50D  "STEAM ROOM"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 331.

Byte-identisch (207788 B).

### ROOM50E  "B3 POWER ROOM"  [Gegner]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 380.

Groesse L 101604 / E 101280 B; Kopf L [0 10 0 0 0 0 0] / E [0 10 0 0 0 0 0]; Skripte L 6 / E 3; Texte L 0 / E 0; Abdeckung L 356/356 / E 38/38

- **EM** nur Leon: sub00@0x00ACE Typ 0x30 Birkin grid 0x33 kill - (-15000,-5600,12200) dir 1024
- **EM** nur Leon: sub00@0x00AEE Typ 0x30 Birkin grid 0x10 kill - (-15000,0,-16300) dir 3072
- **AOT** nur Leon: sub00@0x00ABA Slot 1 sce 3 flags 0x41 sub02
- **AOTR** nur Leon: sub02@0x00B30 Aot_reset Slot 1 -> sce 0 inert
- **BGM** nur Leon: sub00@0x00B02 op54 0001000000
- **BGM** nur Leon: sub01@0x00B22 op54 0002000000
- **BGM** nur Leon: sub02@0x00B42 op54 0001000000
- **EVT** nur Leon: sub02@0x00B8E Evt_exec sub04
- **CK** nur Leon: sub00@0x00AB6 Ck(3,61,0)
- **CK** nur Leon: sub00@0x00AEA Ck(3,73,0)
- **CK** nur Leon: sub01@0x00B12 Ck(5,28,1)
- **CK** nur Leon: sub03@0x00BC8 Ck(5,32,0)
- **SET** nur Leon: sub01@0x00B16 Set(5,28,0)
- **SET** nur Leon: sub01@0x00B1A Set(3,73,1)
- **SET** nur Leon: sub01@0x00B1E Set(3,84,1)
- **SET** nur Leon: sub02@0x00B2C Set(3,61,1)
- **SET** nur Leon: sub02@0x00B3A Set(1,27,1)
- **SET** nur Leon: sub02@0x00B3E Set(2,7,1)
- **SET** nur Leon: sub02@0x00BB2 Set(1,27,0)
- **SET** nur Leon: sub02@0x00BB6 Set(2,7,0)

### ROOM50F  "B4 POWER ROOM"  [Gegner]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 309.

Groesse L 100988 / E 101292 B; Kopf L [0 10 0 0 0 0 0] / E [0 10 0 0 0 0 0]; Skripte L 3 / E 6; Texte L 0 / E 0; Abdeckung L 56/58 / E 354/356

- **EM** nur Elza: sub00@0x009AC Typ 0x30 Birkin grid 0x33 kill - (-12100,-5600,-31500) dir 3072
- **EM** nur Elza: sub00@0x009D4 Typ 0x30 Birkin grid 0x10 kill - (-12100,0,-15550) dir 1024
- **AOT** nur Elza: sub00@0x00994 Slot 1 sce 3 flags 0x41 sub02
- **AOTR** nur Elza: sub02@0x00A1A Aot_reset Slot 1 -> sce 0 inert
- **BGM** nur Elza: sub00@0x009EC op54 0001000000
- **BGM** nur Elza: sub01@0x00A0C op54 0002000000
- **BGM** nur Elza: sub02@0x00A2C op54 0001000000
- **EVT** nur Elza: sub02@0x00A54 Evt_exec sub04
- **CK** nur Elza: sub00@0x00990 Ck(3,61,0)
- **CK** nur Elza: sub00@0x009CC Ck(3,73,0)
- **CK** nur Elza: sub01@0x009FC Ck(5,28,1)
- **CK** nur Elza: sub03@0x00A8E Ck(5,32,0)
- **SET** nur Elza: sub00@0x009A8 Set(1,0,0)
- **SET** nur Elza: sub00@0x009C0 Set(1,0,1)
- **SET** nur Elza: sub00@0x009D0 Set(1,0,0)
- **SET** nur Elza: sub00@0x009E8 Set(1,0,1)
- **SET** nur Elza: sub01@0x00A00 Set(5,28,0)
- **SET** nur Elza: sub01@0x00A04 Set(3,73,1)
- **SET** nur Elza: sub01@0x00A08 Set(3,84,1)
- **SET** nur Elza: sub02@0x00A16 Set(3,61,1)
- **SET** nur Elza: sub02@0x00A24 Set(1,27,1)
- **SET** nur Elza: sub02@0x00A28 Set(2,7,1)
- **SET** nur Elza: sub02@0x00A78 Set(1,27,0)
- **SET** nur Elza: sub02@0x00A7C Set(2,7,0)

### ROOM510  "B-3 CORRIDOR"  

Byte-identisch (117452 B).

### ROOM511  "F MONITOR ROOM"  

Byte-identisch (220360 B).

### ROOM512  "F B-2 CORRIDOR"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 306.

Byte-identisch (165396 B).

### ROOM513  "F D-2 CORRIDOR"  

Byte-identisch (221440 B).

### ROOM514  "F C-2 CORRIDOR"  

Byte-identisch (188848 B).

### ROOM515  "EMPTY"  

Byte-identisch (4 B).

### ROOM516  "EMPTY"  

Byte-identisch (4 B).

### ROOM517  "EMPTY"  

Byte-identisch (4 B).

### ROOM600  "GATE PLATFORM"  [NPC, Texte*]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 111, 305, 329, 382.

Groesse L 237836 / E 184724 B; Kopf L [0 13 2 0 0 0 0] / E [0 13 2 0 0 0 0]; Skripte L 10 / E 5; Texte L 8 / E 3; Abdeckung L 1218/1220 / E 574/574

- **EM** nur Leon: sub00@0x00F44 Typ 0x4D NPC 0x4D (Annette?) grid 0x40 kill - (3150,-3600,1600) dir 1024
- **EM** nur Leon: sub00@0x00F68 Typ 0x4D NPC 0x4D (Annette?) grid 0x00 kill - (3150,-3600,1600) dir 1024
- **AOT** nur Leon: main00@0x00DA0 Slot 2 sce 1 flags 0x31 Text 6
- **AOT** nur Leon: main00@0x00E02 Slot 5 sce 1 flags 0x31 Text 7
- **AOT** nur Leon: main00@0x00E66 Slot 5 sce 3 flags 0x31 sub07
- **AOT** nur Leon: sub00@0x00F26 Slot 4 sce 3 flags 0x41 sub02
- **AOT** nur Elza: main00@0x00DA0 Slot 2 sce 1 flags 0x31 Text 1
- **AOT** nur Elza: main00@0x00E02 Slot 4 sce 1 flags 0x31 Text 2
- **AOT** nur Elza: main00@0x00E66 Slot 4 sce 3 flags 0x31 sub02
- **AOTR** nur Leon: sub07@0x011D4 Aot_reset Slot 5 -> sce 0 inert
- **MSGON** nur Leon: sub02@0x00FC4 Message_on 4
- **MSGON** nur Leon: sub06@0x010FA Message_on 1
- **MSGON** nur Leon: sub06@0x0113A Message_on 2
- **MSGON** nur Leon: sub06@0x01156 Message_on 3
- **MSGON** nur Leon: sub07@0x011A6 Message_on 5
- **EVT** nur Leon: sub00@0x00F58 Evt_exec sub06
- **EVT** nur Leon: sub07@0x011EC Evt_exec sub08
- **CK** nur Leon: sub00@0x00F22 Ck(3,239,0)
- **CK** nur Leon: sub00@0x00F40 Ck(3,100,0)
- **CK** nur Leon: sub00@0x00F64 Ck(3,239,0)
- **CK** nur Leon: sub04@0x0104E Ck(5,0,0)
- **CK** nur Leon: sub05@0x01060 Ck(5,1,0)
- **SET** nur Leon: sub02@0x00F8E Set(3,239,1)
- **SET** nur Leon: sub02@0x00F92 Set(2,7,1)
- **SET** nur Leon: sub02@0x00F96 Set(1,27,1)
- **SET** nur Leon: sub02@0x01012 Set(2,7,0)
- **SET** nur Leon: sub02@0x01016 Set(1,27,0)
- **SET** nur Leon: sub04@0x01046 Set(5,0,0)
- **SET** nur Leon: sub05@0x01058 Set(5,1,0)
- **SET** nur Leon: sub06@0x0106A Set(3,100,1)
- **SET** nur Leon: sub06@0x0106E Set(2,7,1)
- **SET** nur Leon: sub06@0x01072 Set(1,27,1)
- **SET** nur Leon: sub06@0x0118A Set(2,7,0)
- **SET** nur Leon: sub06@0x0118E Set(1,27,0)
- **TEXT** nur Leon id 0: "Annette: You must be that cop. / Your friends are already here."
- **TEXT** nur Leon id 1: "Annette: Where's Ada?"
- **TEXT** nur Leon id 2: "Annette: I see..."
- **TEXT** nur Leon id 3: "Annette: Come on. Let's join your / friends and get out of here."
- **TEXT** nur Leon id 4: "Annette: Let's see if we can get / this thing moving."

### ROOM601  "MAIN PLATFORM"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 347.

Byte-identisch (145260 B).

### ROOM602  "SHELTER 01-02"  [NPC]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 75.

Groesse L 201880 / E 201880 B; Kopf L [0 9 1 0 0 0 0] / E [0 9 1 0 0 0 0]; Skripte L 4 / E 4; Texte L 1 / E 1; Abdeckung L 134/136 / E 134/136

- **EM** nur Leon: sub00@0x00B96 Typ 0x40 NPC 0x40 (Marvin?) grid 0x40 kill - (-9000,0,9575) dir 400
- **EM** nur Leon: sub00@0x00BAA Typ 0x42 NPC 0x42 (Ada) grid 0x40 kill - (-8000,0,10575) dir 400
- **EM** nur Elza: sub00@0x00B96 Typ 0x49 NPC 0x49 (Kendo?) grid 0x40 kill - (1200,0,-8625) dir 1656
- **EM** nur Elza: sub00@0x00BAA Typ 0x4B NPC 0x4B (Sherry?) grid 0x40 kill - (200,0,-7525) dir 1656

### ROOM603  "TRAIN MAIN CAR"  [NPC, Texte*]

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 30, 383, 385.

Groesse L 368996 / E 363876 B; Kopf L [0 14 2 0 0 0 0] / E [0 14 2 0 0 0 0]; Skripte L 11 / E 10; Texte L 21 / E 12; Abdeckung L 1860/1862 / E 1250/1252

- **EM** nur Leon: sub06@0x01044 Typ 0x4B NPC 0x4B (Sherry?) grid 0x40 kill - (-24471,0,-25806) dir 3072
- **EM** nur Leon: sub07@0x010D4 Typ 0x4D NPC 0x4D (Annette?) grid 0x40 kill - (-1790,0,-22272) dir 24
- **EM** nur Leon: sub07@0x010F8 Typ 0x4D NPC 0x4D (Annette?) grid 0x40 kill - (-1790,0,-22272) dir 24
- **EM** nur Elza: sub06@0x00E6A Typ 0x4B NPC 0x4B (Sherry?) grid 0x40 kill - (-24026,0,-26208) dir 3072
- **EM** nur Elza: sub07@0x00EFA Typ 0x4B NPC 0x4B (Sherry?) grid 0x40 kill - (-1790,0,-22272) dir 24
- **EM** nur Elza: sub07@0x00F1E Typ 0x4B NPC 0x4B (Sherry?) grid 0x40 kill - (-1790,0,-22272) dir 24
- **AOTR** nur Leon: sub06@0x0106A Aot_reset Slot 3 -> sce 1 Text 10
- **AOTR** nur Leon: sub06@0x01098 Aot_reset Slot 2 -> sce 1 Text 20
- **AOTR** nur Elza: sub06@0x00E90 Aot_reset Slot 3 -> sce 1 Text 4
- **AOTR** nur Elza: sub06@0x00EBE Aot_reset Slot 2 -> sce 1 Text 11
- **MSGON** nur Leon: sub02@0x00EC6 Message_on 4
- **MSGON** nur Leon: sub08@0x01174 Message_on 11
- **MSGON** nur Leon: sub08@0x01198 Message_on 12
- **MSGON** nur Leon: sub08@0x011D0 Message_on 13
- **MSGON** nur Leon: sub08@0x01200 Message_on 14
- **MSGON** nur Leon: sub08@0x0125E Message_on 15
- **MSGON** nur Leon: sub08@0x01296 Message_on 16
- **MSGON** nur Leon: sub08@0x012C2 Message_on 17
- **MSGON** nur Leon: sub08@0x012E6 Message_on 18
- **MSGON** nur Leon: sub08@0x0130A Message_on 19
- **MSGON** nur Elza: sub08@0x0109E Message_on 10
- **EVT** nur Leon: sub07@0x0110C Evt_exec sub09
- **CK** nur Leon: sub02@0x00DA8 Ck(5,32,0)
- **CK** nur Leon: sub02@0x00DBC Ck(5,32,0)
- **CK** nur Leon: sub02@0x00E0A Ck(5,32,0)
- **TEXT** nur Leon id 0: "Leon: Hang on, Marvin. / We're getting out of here."
- **TEXT** nur Leon id 1: "Leon: Where's Elza?"
- **TEXT** nur Leon id 2: "Sherry: She's out there fighting / the monsters!"
- **TEXT** nur Leon id 3: "John: Yeah! She's trying to keep / the outside clear so we can escape!"
- **TEXT** nur Leon id 4: "Leon: Why aren't you out there?"
- **TEXT** nur Leon id 5: "John: She told me to stay and keep / an eye on Marvin here."
- **TEXT** nur Leon id 6: "Leon: Where's Annette?"
- **TEXT** nur Leon id 7: "John: Up front, trying to start / the train."
- **TEXT** nur Leon id 8: "Leon: I see. Thanks."
- **TEXT** nur Leon id 11: "Annette: Where's that biker girl?"
- **TEXT** nur Leon id 12: "Leon: Keeping the monsters at bay. / Can you start this thing?"
- **TEXT** nur Leon id 13: "Annette: Certainly."
- **TEXT** nur Leon id 14: "Annette: If I remember the startup / sequence correctly..."
- **TEXT** nur Leon id 15: "Leon: What's wrong?"
- **TEXT** nur Leon id 16: "Annette: Not enough power! / The train's batteries are too low!"
- **TEXT** nur Leon id 17: "Leon: I'll take care of it!"
- **TEXT** nur Leon id 18: "Annette: I'll be ready to start the / train when you do!"
- **TEXT** nur Leon id 19: "Leon: Good luck to both of us!"
- **TEXT** nur Elza id 0: "Elza: Where's Leon?"
- **TEXT** nur Elza id 1: "Sherry: He's fighting off monsters. / He wants you to start the train."
- **TEXT** nur Elza id 2: "Elza: I'll do it!"
- **TEXT** nur Elza id 5: "Sherry: I know how to start this! / I watched Mom and Dad do it."
- **TEXT** nur Elza id 6: "Sherry: Let's see, you push this / here and throw this switch there."
- **TEXT** nur Elza id 7: "Elza: What's wrong?"
- **TEXT** nur Elza id 8: "Sherry: It's doesn't have enough power!"
- **TEXT** nur Elza id 9: "Sherry: This says the train's main / batteries need to be fueled!"
- **TEXT** nur Elza id 10: "Elza: Let me take care of that. / You start the train once it's ready."

### ROOM604  "ENDING"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 195, 261, 289, 384, 386.

Byte-identisch (163048 B).

### ROOM605  "EMPTY"  

Byte-identisch (4 B).

### ROOM606  "EMPTY"  

Byte-identisch (4 B).

### ROOM607  "EMPTY"  

MZD-README (info/Re1.5/README.TXT) nennt diesen Raum in Zeile(n) 387.

Byte-identisch (4 B).

