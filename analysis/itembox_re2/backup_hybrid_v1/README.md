# SICHERUNG: Item-Box „Hybrid v1" (Stand vor der RE2-Vollportierung)

Nutzer-Auftrag 2026-08-30: „sichere die aktuelle Item Box implementierung, aber eigentlich
wäre mir doch lieb, wenn du die Itembox mit dem Kompletten Mechanismus aus Resident Evil 2
übernimmst und portierst."

Das hier ist die **Sicherung des bisherigen Standes**, bevor die Box auf RE2s vollständigen
Mechanismus umgestellt wird.

## Wie man zurückkommt

- **Git-Tag:** `itembox-hybrid-v1` (auf dem Commit direkt vor der Umstellung)
- **Git-Branch:** `backup/itembox-hybrid-v1`
- **Datei-Kopien:** dieses Verzeichnis (unabhängig von Git lesbar)

Alle drei sind auch auf GitHub gepusht.

Wiederherstellen einzelner Dateien:
```
git checkout itembox-hybrid-v1 -- re15_port/engine/src/re15_itembox.c \
                                  re15_port/include/re15_itembox.h
```
Oder schlicht die Kopien hier zurückkopieren.

## Was in diesem Stand steckt (und was daran NICHT RE2 war)

| Teil | Herkunft |
|---|---|
| Transfer-Mechanik (Ablegen/Entnehmen, Munitions-Tausch, Stapel, Verdichtung, Auto-Ablegen der Waffe) | **RE2-portiert** — `FUN_800703b8`-Familie, 14-Quirk-Katalog |
| Munitions-Erkennung, Kapazitäten, „breite" 2-Zellen-Waffen | RE1.5-adaptiert (RE1.5-Ids/Prop-Tabelle statt RE2s) |
| **Speicher-Struktur: 4 Seiten × 8 Plätze mit L1/R1-Blättern** | **DESIGN, nicht kanonisch** — ersetzte RE2s **64-Platz-Scroll-Ring** |
| Ablage-Form (4 dormante 8er-Arrays im RE1.5-Save) | RE1.5-dormant, Zweck unbelegt |
| Öffnungs-Trigger an den 16 Box-Räumen | Port-Erfindung (Vorbild: Save-Telefon); `RE15_BOX_PREVIEW_MSG=1` = byte-true Original-Meldung |

Genau der fett markierte Teil ist der Grund für die Neufassung: der Nutzer will RE2s
**kompletten** Mechanismus, also auch dessen Speicher- und Bedienmodell.

Die zugehörigen Spezifikations-Dokumente (`itembox_spec.md`, `itembox_verdict.md`) liegen
hier mit, weil sie den Hybrid-Stand beschreiben und nach der Umstellung teilweise
überholt sind.
