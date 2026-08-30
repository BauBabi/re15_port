# RE2-Retail Opening: TEXT/CAPTION- und AUDIO-Kopplung

Status: IN ARBEIT (inkrementell befüllt)
Datum: 2026-08-30
Teilauftrag: (a) Was zeigt die Opening-Sequenz als Erzähltext — .CPT-Untertitel oder TIM-Seiten?
(b) Einblend-Kadenz des Textes (Rate/Dauer/Position/Farbe). (c) XA-Cues 1..5 + BGM-Kopplung.
Alle Konstanten mit @0x…-Adresse. NICHTS einbauen — nur Analyse.

Basis: `analysis/preintro_re2/re2-sequenzer.md` (Overlay OPENING.BIN @0x801bfa18, Entry 0x801bfad8).
Adress-Umrechnung Overlay: `datei_offset = addr - 0x801bfa18`.

## Arbeitslog
- [ ] Overlay-Disassembler im Scratchpad + Gegenprobe am bekannten Skip-Check @0x801bfb1c
- [ ] Renderer 0x801c1a0c auseinandernehmen (welcher Element-Typ trägt Text)
- [ ] .CPT-Lader suchen (EXE + Overlay)
- [ ] .CPT-Format
- [ ] Kadenz-Konstanten
- [ ] XA-Cue-Tabelle @0x80010818
- [ ] BGM

---
## STATUS: UNVOLLSTÄNDIG (Agent-Abbruch 2026-08-30)

Dieser Teilauftrag kam über das Arbeitslog nicht hinaus (Agent-Stall). **Nichts hier ist
ein Befund.** Für die Übernahme war er nicht blockierend: unsere Montage nutzt das
RE1.5-eigene Message-/Voice-System (Texte + `VOICE/r1240_m00..m05`, byte-true), nicht RE2s
Caption-Kanal. Offen bleibt (für später, falls RE2s Textdarstellung übernommen werden soll):
das .CPT-Format (OPNL/OPNC/OPNW), die Einblend-Kadenz der Textzeilen und die XA-Cue-Tabelle
@0x80010818. Belegte Teilergebnisse dazu stehen in `re2-modes.md` (§ Text-Element: kein
Fade, feste Farbe 0x808080, opak) und `re2-renderer.md` (Typ-1-Elemente sind in diesem
Build tot — die Erzähltexte laufen als vorgerenderte Bildseiten).
