# Elza-Zweig — Bau (Runde 35)

Vorarbeiten: `elza-original.md` (Ermittlung) und `elza-portzustand.md` (Messung),
beide aus den Schwesterbaeumen hierher uebernommen.

## Die zwei Original-Groessen (aus elza-original.md, selbst nachgeprueft)

| Original | Leon | Elza | steuert |
|---|---|---|---|
| `DAT_800ACA5C` (byte) | 0 | **4** | PLD-Index, CORE-Bank, Gore-Zweige |
| `DAT_800ACA3C` Bit 31 | 0 | 1 | **RDT-Dateivariante + Startraum** |

Regel der Raumwahl, @0x800397e4 `srl a0,a0,31` + @0x800397ec `addu a0,a0,v0`:
`datei_id = stage_tabelle[stage][raum] + elza_bit`. Im Port ist die Dateivariante
die niedrigste Hex-Ziffer der Raum-Id (`ROOM%04X`), also `room_id | elza_bit`.

## Log
- [start] Ergebnisdatei angelegt, beide Dossiers uebernommen.
