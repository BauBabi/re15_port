---
name: ghidra-mapping
description: Ordnet eine Ghidra-Adresse (FUN_/LAB_/DAT_80xxxxxx oder 0x80xxxxxx) per RE15_FUN_CATALOG.md ihrem Subsystem, Zweck und Confidence-Level zu; bei unbekannter Adresse werden die nächsten katalogisierten Nachbarn im ±0x1000-Bereich gelistet. Verwenden beim Durcharbeiten von Stage-Overlay-Decompilation oder vor manuellem Reverse-Engineering einer Adresse.
---

# Ghidra-Mapping Skill

Dieser Skill ordnet eine gegebene Ghidra-Funktionsadresse (FUN_80xxxxxx / LAB_80xxxxxx / DAT_80xxxxxx) dem zugehörigen Subsystem, Zweck und Confidence-Level zu — basierend auf dem RE15_FUN_CATALOG.md Katalog.

## Kontext

Lade den gesamten Inhalt von `RE15_FUN_CATALOG.md` (im Repo-Root) als Wissensgrundlage. Dieser Katalog enthält alle bisher code-verifizierten Funktionsadressen mit:
- Adresse (FUN_/LAB_/DAT_ Präfix + 8-stellige Hex-Adresse)
- Subsystem-Zuordnung (Player/Animation, Camera, SCD VM, Lighting, RVD/AOT, Render, Audio, Sprite/BG, Save/Item-Box, Stairs, Prop/Fixture)
- Zweck/Purpose (einzeilige Beschreibung)
- Evidence (Quellverweis: ghidra1:Zeile, .c-Datei, etc.)
- Confidence (high/medium/low)

## Eingabe

Eine Funktionsadresse im Format:
- `FUN_80xxxxxx` (Funktionen)
- `LAB_80xxxxxx` (Labels/Code-Positionen)
- `DAT_80xxxxxx` (Daten-Adressen)
- Oder nur die nackte Hex-Adresse `0x80xxxxxx`

## Ausgabe

### Fall 1: Adresse ist im Katalog vorhanden

Gib eine strukturierte Antwort zurück:

```
Adresse:    FUN_80xxxxxx
Subsystem:  [Subsystem-Name aus der Tabellen-Überschrift]
Zweck:      [Purpose-Spalte aus dem Katalog]
Confidence: [high | medium | low]
Evidence:   [Evidence-Spalte]
```

Falls die Adresse unter "HIGH-VALUE CORRECTIONS" gelistet ist, ergänze eine Warnung:
```
⚠️ KORREKTUR: [Zusammenfassung der Korrektur aus dem Corrections-Abschnitt]
```

### Fall 2: Adresse ist NICHT im Katalog vorhanden

Gib folgende Meldung aus:

```
❌ Adresse 0x80xxxxxx ist NICHT katalogisiert.

Nächste Nachbarn im ±0x1000-Bereich:
  1. [Adresse] — [Zweck] (Confidence: [level])
  2. [Adresse] — [Zweck] (Confidence: [level])
  3. [Adresse] — [Zweck] (Confidence: [level])
```

Die Nachbarn werden nach Abstand zur angefragten Adresse sortiert (nächster zuerst). Es werden maximal 3 Nachbarn ausgegeben, die innerhalb von ±0x1000 Bytes der angefragten Adresse liegen.

Falls keine Nachbarn im ±0x1000-Bereich existieren, gib aus:
```
❌ Adresse 0x80xxxxxx ist NICHT katalogisiert.
   Keine katalogisierten Nachbarn im Bereich ±0x1000 gefunden.
   Nächster bekannter Eintrag: [Adresse] — [Zweck] (Abstand: 0xYYYY)
```

## Adress-Matching-Regeln

1. **Exakter Match**: Prüfe zuerst auf exakte Übereinstimmung der 8-stelligen Hex-Adresse (case-insensitiv)
2. **Präfix-unabhängig**: `FUN_80015850`, `LAB_80015850`, `0x80015850` matchen alle auf denselben Katalog-Eintrag
3. **Bereichs-Suche**: Für die Nachbar-Suche berechne `|addr_query - addr_catalog| <= 0x1000`
4. **Sortierung**: Nachbarn werden nach absolutem Abstand aufsteigend sortiert

## Adress-Bereiche im Katalog

- `0x8001xxxx – 0x8006xxxx`: Main EXE (PSX.EXE) — Engine-Kern, Rendering, Audio, I/O
- `0x8007xxxx`: Dispatch-Tabellen, Lookup-Tables
- `0x800Axxxx – 0x800Dxxxx`: BSS/RAM-Daten (Spielzustand, Puffer)
- `0x80100000+`: Stage-Overlays (dynamisch geladen)

## Subsystem-Klassifikation

Wenn die Adresse in einer der folgenden Tabellen-Sektionen des Katalogs vorkommt, verwende den entsprechenden Subsystem-Namen:

| Katalog-Sektion | Subsystem |
|-----------------|-----------|
| Player / animation | Player & Animation |
| Stairs / floor-band traversal | Stairs & Floor-Band |
| Camera | Camera |
| SCD VM | SCD Virtual Machine |
| Lighting / NCCT | Lighting |
| RVD / AOT | RVD & AOT (Zones/Triggers) |
| Render / fade / letterbox | Rendering & Visual FX |
| Sprite / BG / MDEC / TIM | Sprite & Background |
| Audio (SPU / VAB) | Audio |
| Save / item-box | Save & Inventory |
| Prop / fixture rendering | Props & Fixtures |
| RE2 RENDERING PIPELINE | RE2 Reference Pipeline |

## Beispiele

### Beispiel 1 — Katalogisierte Adresse
Eingabe: `FUN_80015850`

```
Adresse:    FUN_80015850
Subsystem:  Camera
Zweck:      Per-frame camera orbit+soft-track animator: dolly residuals, XZ via rcos/rsin(yaw),
            soft-track active actor XZ/60 & Y/20, +400 Y, rebuild via 0x80053ca4.
            Runs every frame incl. cinematics.
Confidence: high
Evidence:   ghidra1:81665-81811

⚠️ KORREKTUR: Seed sagte "dead code, not for static cuts" — FALSCH für PSX.
   Läuft JEDEN gerenderten Frame für alle Kamerazustände inkl. Cinematics.
```

### Beispiel 2 — Nicht katalogisierte Adresse
Eingabe: `FUN_80015000`

```
❌ Adresse 0x80015000 ist NICHT katalogisiert.

Nächste Nachbarn im ±0x1000-Bereich:
  1. 0x8001500c — Camera frame dispatcher (Abstand: +0x0C, Confidence: high)
  2. 0x80015850 — Per-frame camera orbit+soft-track animator (Abstand: +0x850, Confidence: high)
  3. 0x80014368 — Point-in-quad RVD test (Abstand: -0xC98, Confidence: high)
```

## Hinweise für den Einsatz

- Dieser Skill ist besonders nützlich beim Durcharbeiten von Ghidra-Decompilationen der Stage-Overlays
- Prüfe vor dem manuellen RE-Versuch IMMER ob die Adresse bereits katalogisiert ist
- Bei `high`-Confidence-Einträgen kann der dokumentierte Zweck als verlässlich angesehen werden
- Bei `medium`/`low`-Einträgen ist zusätzliche Verifikation empfohlen
- Beachte die HIGH-VALUE CORRECTIONS Sektion — dort sind frühere Fehlzuordnungen dokumentiert
