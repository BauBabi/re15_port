# Abstürze v0.5.0 – v0.5.8 — was Windows tatsächlich protokolliert hat

Nutzer 2026-09-04: *„Irgendwie crashed mir jetzt am laufenden Bande die Anwendung weg"*,
dann *„Aber er stürzt jetzt auch teilweise im intro schon ab"*, dann — nach dem
v0.5.8-Hotfix — *„Es stürzt noch immer teilweise einfach so im Intro ab"* und die
Rückfrage *„Oder ist es irgendwas im Hintergrund was die Applikation killt?"*

## 0. Methode

Nicht modelliert, sondern **gemessen**: Windows Error Reporting protokolliert jeden
Absturz mit fehlerhaftem Modul, Ausnahmecode und Fehleroffset. Abgefragt über das
Anwendungs-Ereignisprotokoll (Provider `Application Error`, ID 1000). Die Offsets sind
gegen **genau die abgestürzten Binaries** aufgelöst (`addr2line`, ImageBase
`0x140000000`), nicht gegen den aktuellen Baum — ein Offset aus Version A gegen Version B
aufzulösen ergäbe eine falsche Funktion.

Gegenprobe auf Modul-Identität: PE-Zeitstempel des Binaries in `Downloads` =
`0x6a9ac99b` = 2026-09-04 13:37:31 UTC = der Wert `Sig[5]` im WER-Bericht. Es ist
dasselbe Binary.

**Die Frage nach dem Hintergrund ist damit beantwortet: nein.** In allen Fällen ist das
fehlerhafte Modul `re15_pc.exe` selbst — kein fremdes Modul, kein Virenscanner, kein
Fremdprozess. Ausnahmecode durchgehend `0xc0000005` (Zugriffsverletzung).

## 1. Die drei Abstürze

| Zeit | Version | Offset | Modul | Ursache | Status |
|---|---|---|---|---|---|
| 04.09. 00:43 | v0.5.0 | `0x132c0` | `audio_pc.c` | §3 | ✅ v0.5.9 |
| 04.09. 00:56 | v0.5.0 | `0x32282` | `emd_common.c` | §4 | ❌ offen |
| 04.09. 15:31 | v0.5.7 | `0x1c1cd` | `inv_render_pc.c` | §2 | ✅ v0.5.8 |
| 04.09. 15:31 | v0.5.7 | `0x13300` | `audio_pc.c` | §3 | ✅ v0.5.9 |
| 04.09. 15:48 | **v0.5.8** | `0x13300` | `audio_pc.c` | §3 | ✅ v0.5.9 |

Es waren **drei verschiedene Fehler**, nicht einer. Der v0.5.8-Hotfix war richtig und
nötig — er hat den Karten-Absturz (§2) beseitigt —, aber er war **nicht der einzige**.
Der Audio-Absturz (§3) ist älter als die gesamte Kartenarbeit: er steckt schon in v0.5.0.

## 2. `inv_render_pc.c` @0x1c1cd — die Rechteck-Tabelle aus dem falschen Blob

Behoben in v0.5.8 (`19f8354a`). `RE15_INV_PTR(0x80076840)` liest `0x22C` Bytes hinter dem
Ende von `re15_inv_ui_blob` [`0x80074A8C`, `0x80076614`); die Tabelle liegt im
`re15_inv_map_blob` ab `0x800762A0`. Siehe Memory `reai-v2-falscher-blob`.

## 3. `audio_pc.c` @0x13300 — der Mixer las aus freigegebenem Speicher

Aufgelöst gegen das ausgelieferte v0.5.8-Binary:

```
140013300: 41 0f bf 04 4b   movswl (%r11,%rcx,2),%eax   <audio_callback+0x1e0>
140013305: 41 83 f8 1f      cmp    $0x1f,%r8d
140013309: 7f 0f            jg     14001331a
```

Das ist Zeile-für-Zeile:

```c
int32_t smp = s_xa.pcm[s_xa.pos];                        /* <- 0x13300, der Zugriff */
int rem = s_xa.pcm_len - s_xa.pos;
if (rem < MIXER_TAIL_FADE_SAMPLES) ...                   /* 32 = 0x1f + 1           */
```

**Besitzverhältnis.** `s_xa.pcm` ist *geliehen*: `re15_voice_play` reicht
`re15_xa_read_s(s_voice_clip[id].pcm, ...)` weiter — der Puffer gehört aber dem
Sprach-Cache in `re15_voice_load_clip`. Dessen Verwerfen bei Raumwechsel gab ihn frei,
**ohne Sperre und ohne den Stream zu lösen**, während der SDL-Mixer-Thread weiterlas.

Das erklärt beide Beobachtungen des Nutzers exakt:

* **„teilweise"** — es kracht nur, wenn der Allokator die Seite inzwischen zurückgenommen
  hat. Sonst liest der Mixer stillschweigend Datenmüll.
* **„im Intro"** — dort laufen Sprachzeilen *und* die Raumkette ROOM1240 → ROOM1170.
  Das längste Fenster entsteht, wenn der neue Raum für die angefragte Zeile gar keinen
  Clip hat: dann kehrt `re15_voice_play` still zurück und `s_xa` zeigt bis zum Ende der
  **alten** Clip-Länge auf freigegebenen Speicher.

**Gegenprobe auf weitere Stellen.** Alle übrigen Freigaben von Puffern, aus denen der
Callback liest, waren bereits korrekt gesperrt: `s_fmv_audio.pcm` (Zeilen 1430/1443,
beide zwischen Lock/Unlock) und `ss_free_bank` (Vertrag „Aufrufer sperrt", Aufrufstellen
nachgeprüft — u.a. Zeile 2921 zwischen Lock 2900 und Unlock 2941). Die Cache-Freigabe war
die **einzige** ungesperrte.

**Warum 267 grüne Tests das nicht sahen.** Die Testschiene linkt ausschließlich
`re15_engine`; `audio_pc.c` gehört zur PC-Plattform und ist von keinem Testbinary
erreichbar. Deshalb prüft `tests/unit/test_audio_sperren.c` jetzt den **Sperr-Vertrag am
Quelltext** und gibt seine Abdeckung aus (5 Freigaben, davon 2 per Aufrufer-Vertrag).

## 4. `emd_common.c` @0x32282 — OFFEN

`re15_emd_get_keyframe_position`, einmal beobachtet (v0.5.0, 04.09. 00:56), seither nicht
wieder. Die Disassembly zeigt, dass **alle Schranken greifen**:

```
14003225e: cmp %edx,0x108(%rcx)      /* keyframe_index < keyframe_count      */
140032266: imul 0x104(%rcx),%edx     /* frame_base = index * size            */
140032271: cmp 0x118(%rcx),%r11d     /* frame_base + 5 < keyframe_data_size  */
140032282: movzwl (%r10,%rcx,1),%ecx /* <- hier krachte es                   */
```

Der Index war also geprüft — mithin war der **Zeiger** faul, oder `skel` selbst zeigte
ins Leere (die Feldwerte `0x104/0x108/0x118` kommen alle aus `%rcx`). `keyframe_data`
zeigt immer *in* einen größeren EMD/EMR/RBJ-Puffer (`emd + off`), ist also ebenfalls
geliehen — dieselbe Fehlerklasse wie §3.

⛔ **Nicht geraten, nicht „gefixt".** Ohne Reproduktion wäre jede Änderung hier ein
Rate-Defekt. Nächste Wege, wenn es wieder auftritt: Besitz von `skel` über den
Raumwechsel verfolgen (wer hält `re15_emd_skeleton_t` über ein Nachladen hinweg?),
`enemy_ai_common.c:1022` prüfen (kopiert `keyframe_data` aus einer Greifer-Bank), und
WER so einstellen, dass das Speicherabbild erhalten bleibt (die `.dmp` waren gelöscht,
der archivierte `Report.wer` enthält weder Stack noch Threads).
