# reAi_v2 — Roadmap / Verifikations-TODO

Konkrete Punkte, die **implementiert + build-verifiziert** sind, aber der **Nutzer noch selbst
live durchspielen/gegen-die-PSX prüfen** will (z.B. weil die Verifikation hier gerade nicht gut
ging). Abhaken, sobald live bestätigt.

> Verwandt: `UNTESTED_IMPLEMENTATIONS.md` (Fixes ohne laufendes Test-Szenario). Diese Liste hier =
> „ist fertig + von mir per Screenshot pixel-verifiziert, aber Nutzer will selbst nochmal drüberschauen".

---

## 🟡 ZU VERIFIZIEREN (Nutzer)

### [ ] OPTIONS / Controller-CONFIG-Screen — komplett byte-true (Commits `8380fc2c`…`6f540448`)
Von mir per Screenshot **pixel-identisch** gegen die PSX verifiziert; bitte einmal **live durchspielen**
und mit dem Original vergleichen (Video-identisch = korrekt, sonst nicht).

**So kommst du rein + was prüfen:**
- Debug-Sprung in den Screen: `RE15_CONFIG_TEST=1 re15_pc.exe` (öffnet OPTIONS direkt).
- Einzel-Screenshots eines bestimmten Screens: `RE15_CONFIG_TEST=1 RE15_CONFIG_SHOT=out.png RE15_CONFIG_TAB=<n> [RE15_CONFIG_CUR=<i>] [RE15_CONFIG_CODE=<i>] re15_pc.exe`
  - TAB 0/1/2 = TOP-Menü Cursor CONFIG/SOUND/EXIT · 3 = Preset-Picker · 4 = SOUND · 5 = EDIT-Slot-Select · 6 = EDIT Panel A · 7 = EDIT Panel B
- PSX-Referenz-Savestates: `stage_saves/mzd_options.sav` (TOP-Menü), `stage_saves/mzd_edit_panelA.sav`, `stage_saves/mzd_edit_panelB.sav`.

**Checkliste (live vs Original):**
- [ ] **TOP-Menü** CONFIG/SOUND/EXIT — blaues Tab-Highlight, Left/Right-Nav, **Confirm = ○ (Kreis), Cancel = ✕ (Cross)** (JAP-Konvention!).
- [ ] **Preset-Picker** (CONFIG→) TYPE A/B/C/EDIT/EXIT — „TYPE A B C"/„EDIT"-Tiles über den Tabs, A/B/C wählen → Labels relabeln.
- [ ] **Funktionaler Remap in-game**: TYPE B/C tatsächlich testen (Buttons anders belegt) — im echten Gameplay, nicht nur im Menü.
- [ ] **SOUND-Screen** STEREO/MONO — aktuelle Einstellung hell, andere dim; Toggle mit Up/Down; **Mono-Downmix hörbar** (auf Kopfhörer: beide Kanäle gleich).
- [ ] **„Not set"-Dim** (die unbelegten Boxen sind dimmer als die belegten).
- [ ] **EDIT-Screen**: Slot-Grid-Nav (gelber Cursor springt korrekt), Panel A (ACT-Box + blauer Cursor), Panel B (KEY-Box + △□○✕/L1R1/L2R2-Icons + Debug-Text „R1 O8!?KQW-h"), Button zuweisen → Label ändert sich.

Details/Methode: Memory `reai-v2-options-config-screen`.

---

## Als Nächstes geplant
- [ ] **LOAD GAME** nochmal ansehen (vom Nutzer angekündigt).
