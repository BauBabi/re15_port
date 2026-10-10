# Runde 36 — Spur C "schuss" (2026-10-10)

Baum: `.claude/worktrees/r36_schuss`, Zweig `r36/schuss`, Basis master 5a9dbb7b (v0.8.23 + Auftrag).

## 0. Auftrag (Wortlaut, AUFTRAG.md / VERTRAG.md Spur C)

1. Grenze 1 (v0.8.22/23, vom Nutzer bestellt): "Alle Schusswaffen treffen ueber den Konferenztisch und treffen
   fressende Zombies. Im Original geht beides nicht." -> beheben: Sichtlinie (RE1.5 FUN_80011f50 -> FUN_8001b9b4 ->
   FUN_8003dcc4) + Fresser-Sperre (RE2 +0x1D3 / +0x10E im Resolver FUN_800410CC @0x80041270-9c) fuer ALLE Schusswaffen.
2. "Bei den Zombies habe ich manchmal das Gefuehl, dass das Blut nicht an der richtigen Stelle auftaucht, wenn sie
   getroffen werden." -> Original-Mechanismus des Trefferpunkts belegen, Port angleichen, Pin mit Lage-Differenz.
3. "Mit der Super Redhawk zerplatzen/verschwinden die Kraehen komplett — ist das in Resident Evil 2 auch so?" ->
   RE2-Kraehen-Handler (ENEMY07/EM_TYPE?) auf die Magnum-Todesreaktion disassemblieren, mit dem Port vergleichen,
   Antwort ins Dossier, Abweichung beheben.

Vorrunde: `analysis/befunde_runde35/B_werfer.md` §8.1/§8.4/§8.7 OFFEN 16/17 (Belege fuer Punkt 1 bereits
disassembliert, dort zitiert — hier selbst nachgeprueft, s. §1.2).

## 1. Punkt 1 — Sichtlinie + Fresser-Sperre fuer alle Schusswaffen

### 1.1 Messung vorher
(folgt)

### 1.2 RE-Belege
(folgt)

### 1.3 Umsetzung
(folgt)

### 1.4 Messung nachher
(folgt)

## 2. Punkt 2 — Blut-Anker bei getroffenen Zombies

### 2.1 Messung vorher
(folgt)

### 2.2 RE-Belege
(folgt)

### 2.3 Umsetzung
(folgt)

### 2.4 Messung nachher
(folgt)

## 3. Punkt 3 — Super Redhawk gegen Kraehen (RE2-Vergleich)

### 3.1 Messung vorher
(folgt)

### 3.2 RE-Belege (RE2 Retail)
(folgt)

### 3.3 Umsetzung
(folgt)

### 3.4 Messung nachher
(folgt)

## 4. Tests
(folgt)

## 5. OFFEN
(folgt)

## 6. Fuer den Nutzer
(folgt)
