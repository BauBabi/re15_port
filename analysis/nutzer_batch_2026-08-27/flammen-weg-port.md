# ROOM1090 — "die Flammen verschwinden nach dem Loeschen nicht" (PORT-Seite)

Auftrag 2 von zweien. Auftrag 1 klaert die ORIGINAL-Seite (welcher Mechanismus loescht das
Feuer im Original). Dieses Dossier klaert ausschliesslich: **was tut der PORT heute**, gemessen,
und welche Teile gibt es im Port ueberhaupt nicht.

Status: IN ARBEIT (wird inkrementell fortgeschrieben).

---

## 0. Ergebnis in einem Satz

Der Port hat **keinen einzigen Code-Pfad, der eine brennende Flamme in ROOM1090 beendet** —
weder fuer den Aktor (Typ 0x26) noch fuer den ESP-Partikel-Effekt noch fuer die Schadenszone.
Der Fix ist damit ein **NEUBAU**, kein Nachziehen einer nicht ausgeloesten Bedingung.
(Belege unten, §2/§3; Messung §5.)

---

## 1. Der Port-Pfad — wo die Flammen entstehen

### 1a. Spawn: 7x `Sce_em_set` Typ 0x26 aus ROOM1090 sub00

`re15_port/shared_assets/PSX/STAGE1/ROOM1090.RDT`, SUB-Block base = Datei `0x21B4`
(RDT+0x44 = sub_scd_start, `rdt_common.c:239`), Sub-Tabelle
`[0x21c4, 0x23f4, 0x2414, 0x24ce, 0x26e6, 0x26f4, 0x2702, 0x272c]`.

sub00 (Datei 0x21C4..0x23F4), selbst disassembliert
(`analysis/nutzer_batch_2026-08-27/tools/re15_scd_dis.py`, Laengen aus
`re15_port/engine/src/scd_vm.c:166 s_opcode_sizes[]`):

```
  0x021EA  06 Ifel_ck          00 B4 00        ; if(...) ueberspringt 0xB4 Bytes
  0x021EE  21 Ck               03 81 00        ; <-- Flagbank 3, Bit 0x81, erwartet 0
  0x021F2  2D Obj_model_set    03 00 00 02 ...
  0x02214  44 Sce_em_set       00 26 00 01 ...  ; Typ 0x26, grid_id 0
  0x02228  44 Sce_em_set       01 26 01 01 ...  ;               grid_id 1
  0x0223C  44 Sce_em_set       02 26 02 01 ...  ;               grid_id 2
  0x02250  44 Sce_em_set       03 26 04 01 ...  ;               grid_id 4
  0x02264  44 Sce_em_set       04 26 03 01 ...  ;               grid_id 3
  0x02278  44 Sce_em_set       05 26 03 01 ...  ;               grid_id 3
  0x0228C  44 Sce_em_set       06 26 04 01 ...  ;               grid_id 4
  0x022A0  08 Endif            00
```

Der Feuer-Zustand haengt also an **Flagbank 3 / Bit 0x81**. Ist das Bit 0, brennt es.
Dasselbe Bit gated weiter unten in sub00:

```
  0x022E6  06 Ifel_ck          00 1C 00
  0x022EA  21 Ck               03 81 00
  0x022EE  54 Sce_bgm_control  00 00 01 78 33   ; (0x54 ist in RE1.5 Sce_bgm_control,
                                                ;  NICHT Sce_espr3d_on — scd_vm.c:4080 ff.)
  0x022F4  06 Ifel_ck          00 0C 00
  0x022F8  21 Ck               03 80 01
  0x022FC  54 Sce_bgm_control  00 01 00 00 00
  ...
  0x0232A  07 Else_ck          00 4C 00
  0x0232E  06 Ifel_ck          00 42 00
  0x02332  21 Ck               03 81 00
  0x02336  2C Aot_set          02 03 B1 01 00 64 00 14 05 3A 07 66 08 DE 0D 30 F8 08 07
```

### 1b. Der Setzer des Bits: ROOM1090 sub06 (Datei 0x2702..0x272C)

```
  0x02702  2B Message_on       07 FF FF
  0x02706  02 Evt_next
  0x02708  2B Message_on       08 FF FF
  0x0270C  02 Evt_next
  0x0270E  06 Ifel_ck          00 18 00
  0x02712  21 Ck               0C 1F 00
  0x02716  22 Set              02 07 01
  0x0271A  09 Sleep            0A 0A 00
  0x0271E  22 Set              03 81 01      ; <-- DAS LOESCH-FLAG wird gesetzt
  0x02722  22 Set              03 84 01
  0x02726  47 Aot_on           03            ; (aot_common.c:614 nennt genau diese Stelle)
  0x02728  08 Endif            00
  0x0272A  01 Evt_end          00
```

`Set(3,0x81,1)` ist die einzige Stelle im gesamten ROOM1090-Skript, die dieses Bit setzt.

