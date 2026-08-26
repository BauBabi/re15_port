# Die Welt-Modelle der RE2-Dokumente (2026-08-26)

Nutzer-Einwand: *"man sieht die Dokument auch im Spiel bevor man sie anklickt in Resident
Evil 2, also muss es auch Modelle dafuer geben."*

Der Einwand ist berechtigt. Ich hatte im vorherigen Durchgang nur die SEITENBILDER
(FILES.TIM, 216 TIMs) geliefert und die Welt-Seite gar nicht behandelt. Hier ist sie.

## DER MECHANISMUS — byte-belegt

Das sichtbare Objekt am Boden ist KEIN globales Item-Modell, sondern ein **Raum-Prop**.
Der Port fuehrt das bereits fuer RE1.5, und RE2 macht es identisch:

* `Item_aot_set` installiert nur den AUFHEBE-Ausloeser.
* Das sichtbare Modell ist ein `Obj_model_set`-Prop desselben Raum-Skripts.
* Beim Aufheben loescht der Installer das Zeichen-Bit des Pool-Eintrags (RE1.5:
  @0x800406f8-718); der Port bildet das ueber `s_prop_taken_hidden` ab
  (`scd_vm.c`, Kommentarblock vor `scd_vm_init`).

**Woher das Mesh kommt:** aus dem RDT SELBST. Tabelle bei `RDT+0x30`, `2 * nOmodel`
u32-Offsets, abwechselnd TIM und MD1; `nOmodel` = Header-Byte `RDT+0x02`. Der Port liest
das in `rdt_common.c:51-79` als `prop_tim[]`/`prop_md1[]` und schneidet die Koerper bis
zur naechsten Grenze.

## GEMESSEN — RE2 fuehrt dieselbe Struktur

Zensus ueber alle 250 RE2-RDTs und 206 RE1.5-RDTs (Header-Byte 0x02 + Tabelle 0x30):

```
RE1.5: nOmodel 0..17, alle 206 Tabellen gueltig
RE2  : nOmodel 0..17, alle 250 Tabellen gueltig
       -> 381 VERSCHIEDENE Prop-MD1s in 893 Platzierungen
```

Die haeufigsten Modelle (md5-Kurzform, Groesse des MD1, Zahl der Raeume):

```
5cce83325695   49x    612 B   Textur-Atlas traegt "JONY-S ARMS"  -> das GENERISCHE Item
15f1e94f99fe   29x   1396 B
9e2117d6ee8b   25x   2396 B
e2baafec348a   23x    484 B
aa1e4d0cae89   20x   1812 B
11f139fa0703   17x    484 B   Textur zeigt Brett/Papier -> KANDIDAT Dokument
```

`5cce83325695` steht mehrfach im selben Raum (ROOMA120 achtmal, ROOMB030 sechsmal,
ROOMF0D0 viermal) — genau das Bild eines Raums mit mehreren Aufhebe-Gegenstaenden. Seine
Textur ist ein Atlas mit vielen Item-Motiven (beigelegt: item_atlas_5cce83325695.png).

## DIE ITEM-ID DER DOKUMENTE

`FUN_800692dc` haengt eine Dokument-Id an die FILE-Liste an. Es hat GENAU EINEN Aufrufer,
und der rechnet die Id aus der Item-Id:

```
80071d00: jal   0x800692dc
80071d04: addiu a0,a3,-104        ; docid = item_id - 104
```
Mit 25 Dokumenten liegen die Datei-Item-Ids also bei 104..128 (0x68..0x80).

## ⛔ WAS NOCH OFFEN IST

**Welches der 381 Modelle das Dokument ist, ist NICHT bewiesen.** `11f139fa0703` (484 B,
17 Raeume) hat eine Papier-/Brett-Textur und ist der Kandidat, aber das ist eine Aussage
ueber das AUSSEHEN, kein Beleg. Der saubere Beweis ist die Korrelation:

1. RE2s `Item_aot_set`-Opcode und dessen Item-Id-Feld aus dem Handler lesen (nicht aus
   RE1.5 unterstellen — bei `Sce_em_set` unterscheiden sich die beiden Spiele bereits in
   Laenge UND Feldlage, s. Commit 5d770b48).
2. Alle Raeume finden, deren Item-AOT eine Id in 104..128 fuehrt.
3. Pruefen, welches Prop-Modell genau in diesen Raeumen liegt.
Erst wenn Schritt 3 ein Modell zeigt, das in den Datei-Raeumen steht und sonst nicht, ist
die Frage beantwortet.

⛔ Ausdruecklich NICHT getan: ein Roh-Byte-Scan nach Opcode 0x50. Derselbe Ansatz hat bei
der Endraum-Frage hunderte Falschtreffer geliefert; ohne opcode-exakten Walker ueber die
RE2-SCD ist das wertlos, und eine RE2-Opcode-Laengentabelle existiert im EXE-Image nicht
als Bytetabelle (jeder der 143 Handler ab 0x800A74C8 schiebt den PC selbst).

## FOLGE FUER DEN PORT

Gute Nachricht: der Port braucht fuer Welt-Dokumente KEINE neue Modell-Ebene. Er liest
Prop-Modelle bereits aus dem RDT (`pc_load_room_prop_set`, `main.c:819-840`). Ein eigenes
Dokument im Spiel bedeutet:
  * ein `Obj_model_set`-Prop mit MD1+TIM im RDT des Raums (oder portseitig eingespeist),
  * ein `Item_aot_set` mit der Datei-Item-Id daneben,
  * die Seitenbilder aus `shared_assets/RE2/FILES/` bzw. eigene, gleich aufgebaute.
