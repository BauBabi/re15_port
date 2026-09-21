#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""re2_dokumente_extrakt.py — extrahiert ALLE 25 Dokumente ("FILE") von Resident Evil 2
(Retail, Leon) aus `info/re2leon/` nach `extracted_re2_dokumente/`, damit man sie als
Vorlage fuer eigene RE1.5-Dokumente auswaehlen kann.

Was rauskommt:
  hintergruende/bilder_roh/   25 Original-TIMs, 8bpp 128x256 — das HINTERGRUNDBILD je
                              Dokument (vorgerenderter Gegenstand bzw., bei den Film-
                              Dokumenten, das Foto). Ein Bild je Dokument, im Titel-Slot.
                              Der Text wird im Spiel DARUEBER gezeichnet (Beleg unten).
  hintergruende/bilder_png/   dieselben 25 als PNG, volle 128x256-Leinwand
  hintergruende/bilder_band/  dieselben 25, beschnitten auf das Band, das der Leser
                              zeichnet: Zeilen page_h..255, also 128 x (256 - page_h)
  hintergruende/seiten_roh/   191 Original-TIMs, 4bpp 256 x page_h — die Textseiten
  hintergruende/seiten_png/   dieselben 191 als PNG (RGBA, Index 0 durchsichtig wie im Spiel)
  hintergruende/seiten_lesbar/ dieselben auf neutralem Grund — NUR Ansichtshilfe
  texte/                      Dokumentnamen je Dokument: Rohbytes (Hex) + dekodiert
  texte/transkription/        von Hand abgelesene Seitentexte (re2_dokumente_transkription.py)
  modelle/                    ST_FILE.TM2 / .TIM / .TS im Rohzustand + die Geometrie als
                              OBJ (re2_tm2_obj.py). Das ist das Moebelstueck des
                              FILE-Bildschirms, NICHT ein Modell je Dokument.
  toc.csv                     jede Zeile ein extrahiertes TIM, mit Byte-Offset in FILES.TIM
  dokumente.csv / .json       eine Zeile je Dokument (Name, Seiten, Hoehe, Item-Id)
  uebersicht.html             der Auswahlbogen: Bild + alle Seiten + Text je Dokument

⛔ BELEGE (RE2 PSX.EXE, PS-X-EXE-Kopf: t_addr @0x18 = 0x80010000,
   Datei-Offset = RAM-Adresse - 0x8000F800):

  Dokument -> erster Slot   25 x u8   @0x800A9AD0
      gelesen `lbu a0,-25904(at)` @0x8006D480; Tabelle endet mit 0x00 -> genau 25 Dokumente
  Dokument-Record           25 x 4 B  @0x800AA144  {u16 max_page; u8 y_off; u8 pad}
      max_page `lhu s4,-24252(at)` @0x80076224
      y_off    `lhu s3,-24250(at)` @0x8007603C, dann `addiu s1,zero,256` @0x80076040
               und `subu s5,s1,s3` @0x80076044  =>  Seitenhoehe H = 256 - y_off
  Slot-Record               191 x 8 B @0x800A94B4  {u32 groesse; u16 sektor_lo; u8 sektor_hi; u8 pad}
      groesse `lw v0,-27468(at)`  @0x8006D4D8
      sektor  `lhu a0,-27464(at)` @0x8006D518 / `lbu v0,-27462(at)` @0x8006D50C
      Byte-Offset in FILES.TIM = sektor * 0x800
  Seite -> Slot             Titelseite = first[doc], Folgeseite p = first[doc] + 1 + p
      `beq v1,zero` @0x8006D484 / `addiu v0,a0,1` @0x8006D488
  CD-Datei-Id von FILES.TIM = 166   (`sh` -> 0x800D531C @0x8006D4B4, `jal 0x80012FB8` @0x8006D530)

  Dokument-Index = Item-Id - 0x68:
      `jal FUN_800692DC` @0x80071D00 mit dem Verzoegerungsslot
      `addiu a0,a3,-0x68` @0x80071D04;  FUN_800692DC haengt den Wert an die FILE-Liste
      `DAT_800D4B68` an (`sb a0,0x0(v1)` @0x80069300, max 24 Eintraege).
      Der FILE-Bildschirm liest den Dokument-Record damit zurueck:
      `&DAT_800AA144 + (byte)(&DAT_800D4B68)[zeile*8 + spalte] * 4` (FUN_800724B4).
      => Item-Ids 0x68..0x80 sind die 25 Dokumente.

  Namensbaenke (FUN_80030B9C):
      Bank EN  ptr = 0x8009E550 + u16[0x8009EBAC + id*2]
               `lhu v1,-0x1454(at)` @0x80030C24 / `addiu v0,v0,-0x1AB0` @0x80030C2C
      Bank JP  ptr = 0x8009DF3C + u16[0x8009E438 + id*2]
               `lhu v1,-0x1BC8(at)` @0x80030C74 / `addiu v0,v0,-0x20C4` @0x80030C7C
      Umschalter `(DAT_800CFB74 & 0x840) == 0x40` @0x80030BC8-D0.
      Glyphen-Kodierung: 0x00 = Leerzeichen, 0x0C..0x5F = chr(code + 0x24),
      0xF7 = Ende. (Nachgerechnet ueber alle 129 Eintraege beider Baenke; die EN-Bank
      ergibt lesbares Englisch, die zweite Bank benutzt Bytes 0xA0..0xEF und damit einen
      zweiten, NICHT-lateinischen Zeichensatz -> die wird nur als Hex mitgegeben.)

⛔ Der Dokument-TEXT selbst ist in RE2 KEIN Zeichenstrom, sondern in die 4bpp-Bilder
   gerastert (gemessen: FILE00_p01_page zeigt "August 8th / I talked to the chief today
   ..." als Pixel; die 16-Farben-CLUT hat Index 0 = 0x0000 = durchsichtig und Index 9 =
   0x7FFF = weiss, also Glyphen auf durchsichtigem Grund; PSX-GPU: Texel 0x0000 ist
   vollstaendig durchsichtig, psx-spx "Texture Color 0000h"). Als Zeichenketten
   existieren nur die Dokument-NAMEN (oben). Die Transkriptionen in texte/transkription/
   sind von Hand abgelesen und ausdruecklich KEINE Originalbytes.

⛔ Der HINTERGRUND unter der durchsichtigen Textseite ist das 8bpp-Bild AUS DEMSELBEN
   SLOT — nicht ein Pergament aus einer anderen Datei. Belegt am Oeffnen-Zustand
   (caseD_b): beide TIMs des Titel-Slots gehen in DENSELBEN VRAM-Ausschnitt,
       8006CF2C  li   v0,0x917   ; Slot 0x17, CLUT-Cursor 9  -> CLUT-Y 480+9  = 489
       8006CF38  jal  FUN_80076A40 / _lui a0,0x801a          ; 0x801A0000 = 8bpp-Bild
       8006CF44  li   v0,0xa17   ; derselbe Slot, Cursor 10  -> CLUT-Y 480+10 = 490
       8006CF50  jal  FUN_80076A40
       8006CF54  _ori a0=>DAT_801a8220,a0,0x8220             ; +33312 = 4bpp-Textseite
   FUN_80076A40 rechnet die Lage aus dem Slot: `sll v0,v1,0x6` @0x80076A6C (x = slot*64),
   `sll v0,v0,0x8` @0x80076AA4 (y = 0 oder 256), `addiu v0,v0,0x1e0` @0x80076B08
   (CLUT-Y = 480 + Cursor). Slot 0x17 = VRAM (448,256).
   Gezeichnet wird mit zwei SPRT (`li v0,0x66` @0x80076050 und @0x800760B0), CLUT
   (0,490) fuer den Text (`li a1,0x1ea` @0x8007604C) und (0,489) fuer das Bild
   (`li a1,0x1e9` @0x800760AC). Eingereiht in FUN_800761B8: Textseite an Bildschirm-
   (25,30) aus DAT_800D5C4C/4E (`lhu` @0x8007623C / `sh v0,0x8(s0)` @0x80076244), Bild an
   (100,60) (`li v0,0x64` @0x80076288 / `li v0,0x3c` @0x80076290). AddPrim haengt vorn
   an, also wird zuerst das Bild und darueber der Text gezeichnet.
   ⛔ ST_FILE.TIM ist NICHT dieser Hintergrund: die EXE laedt CD-Id 221/222 nie. Der
   Speicherblock "FILE TIM TM2" laedt Id 0xdf = 223 = ST_FILE.TS (`li a0,0xdf`
   @0x8006C75C mit `addiu a3,a3,0x1c20` = der String) — das ist die Grafik der
   FILE-LISTE (3D-Karteikarten), nicht der Seitenhintergrund.

⛔ Layout, gemessen (nicht modelliert): ein Dokument belegt einen 64-VRAM-Wort breiten
   Block von 256 Zeilen. Zeilen 0..page_h-1 nimmt die 4bpp-Textseite ein (256 px breit
   = 64 VRAM-Worte), Zeilen page_h..255 das 8bpp-Hintergrundbild (128 px breit = ebenfalls
   64 VRAM-Worte). Beweis aus den Daten (Bandzensus, laeuft bei jeder Extraktion mit):
   oberhalb des Bandes, also in den Zeilen 0..page_h-1 des 8bpp-Bildes, steht bei allen
   25 Dokumenten KEINE Kunst — 21 Dokumente sind dort vollstaendig durchsichtig, 4
   (12/15/16/17) sind mit opakem Schwarz gefuellt, hellster Kanalwert 8 von 255. Genau
   diese Zeilen ueberschreibt die Textseite beim Laden. Der sichtbare Gegenstand liegt
   immer im Band. Der Leser zeichnet zwei Sprites, Textseite u=0/v=0/w=256/h=page_h
   (`sb zero,-2(s0)` @0x80076068, `sb zero,-1(s0)` @0x8007606C, `sh s1,2(s0)` @0x80076070,
   `sh s5,4(s0)` @0x80076078) und Bild v=page_h (`subu v0,zero,s3` @0x800760B8,
   `sb v0,-1(s0)` @0x800760D0).

⛔ Titelbild == Seite 0: bei allen 25 Dokumenten ist das 4bpp-Bild im Titel-Slot
   byte-identisch mit dem der Seite 0 (md5-geprueft, siehe Abschlussmeldung). Die
   Dokumente 5 und 6 ("Mail to the chief") sind untereinander in allen 10 Slots
   byte-identisch.
"""
import csv
import json
import os
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
EXE = os.path.join(REPO, "info", "re2leon", "PSX.EXE")
SRC = os.path.join(REPO, "info", "re2leon", "COMMON", "DATA", "FILES.TIM")
OUT = os.path.join(REPO, "extracted_re2_dokumente")

DOC_TAB, CNT_TAB, REC_TAB = 0x800A9AD0, 0x800AA144, 0x800A94B4
NAME_EN_OFF, NAME_EN_BLOB = 0x8009EBAC, 0x8009E550
NAME_JP_OFF, NAME_JP_BLOB = 0x8009E438, 0x8009DF3C
FIRST_DOC_ITEM = 0x68
N_DOC, N_SLOT = 25, 191

# Lese-Hilfsgrund fuer hintergruende/lesbar/: die Textseiten sind durchsichtig, das Spiel
# legt sie ueber die Bildschirm-Grafik. Damit man sie ueberhaupt lesen kann, wird hier ein
# neutraler dunkler Grund untergelegt. Das ist eine ANSICHTSHILFE, kein Originalwert.
LESBAR_BG = (24, 26, 32)


def tim_parse(buf, off):
    """PSX-TIM-Kopf ab off -> Felder + Ende-Offset. Kein Raten: Feldfolge ist die des
    PSX-SDK (magic 0x10, flags{pmode 0-2, clut bit 3}, optionaler CLUT-Block, Bildblock)."""
    magic, flags = struct.unpack_from("<II", buf, off)
    if magic != 0x10:
        raise ValueError("kein TIM @0x%x (magic 0x%08x)" % (off, magic))
    pmode, has_clut = flags & 7, (flags >> 3) & 1
    p = off + 8
    cluts, cx, cy, cw, ch = [], 0, 0, 0, 0
    if has_clut:
        clen, cx, cy, cw, ch = struct.unpack_from("<IHHHH", buf, p)
        for k in range(ch):
            cluts.append([struct.unpack_from("<H", buf, p + 12 + 2 * (k * cw + i))[0]
                          for i in range(cw)])
        p += clen
    ilen, ix, iy, iw, ih = struct.unpack_from("<IHHHH", buf, p)
    pix = buf[p + 12:p + ilen]
    bpp = {0: 4, 1: 8, 2: 16, 3: 24}[pmode]
    return dict(off=off, end=p + ilen, bpp=bpp, w=iw * 16 // bpp, h=ih,
                vram=(ix, iy), clut_vram=(cx, cy), cluts=cluts, pix=pix)


def rgb555(c):
    return ((c & 0x1F) << 3, ((c >> 5) & 0x1F) << 3, ((c >> 10) & 0x1F) << 3)


def tim_rgba(t, clut_index=0):
    """TIM -> (w, h, RGBA-Bytes). Index 0 wird durchsichtig gesetzt, so zeichnet die
    PSX-GPU eine CLUT-Textur mit CLUT-Eintrag 0x0000 (hier gemessen: Eintrag 0 der
    Textseiten-CLUT ist 0x0000)."""
    w, h = t["w"], t["h"]
    cl = t["cluts"][clut_index] if t["cluts"] else []
    out = bytearray(w * h * 4)
    for y in range(h):
        row = y * w
        for x in range(w):
            if t["bpp"] == 4:
                b = t["pix"][row // 2 + (x >> 1)]
                idx = (b & 0x0F) if (x & 1) == 0 else (b >> 4)
            else:
                idx = t["pix"][row + x]
            r, g, bl = rgb555(cl[idx]) if idx < len(cl) else (255, 0, 255)
            p = (row + x) * 4
            out[p], out[p + 1], out[p + 2] = r, g, bl
            out[p + 3] = 0 if idx == 0 else 255
    return w, h, bytes(out)


def png_write(path, w, h, rgba, bg=None):
    """Schreibt PNG. bg != None -> Alpha wird auf bg gerechnet (Ansichtshilfe)."""
    import zlib
    if bg is not None:
        buf = bytearray(rgba)
        for i in range(0, len(buf), 4):
            if buf[i + 3] == 0:
                buf[i], buf[i + 1], buf[i + 2], buf[i + 3] = bg[0], bg[1], bg[2], 255
        rgba = bytes(buf)
    raw = b"".join(b"\x00" + rgba[y * w * 4:(y + 1) * w * 4] for y in range(h))

    def chunk(tag, data):
        return (struct.pack(">I", len(data)) + tag + data
                + struct.pack(">I", zlib.crc32(tag + data) & 0xFFFFFFFF))

    with open(path, "wb") as f:
        f.write(b"\x89PNG\r\n\x1a\n")
        f.write(chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 8, 6, 0, 0, 0)))
        f.write(chunk(b"IDAT", zlib.compress(raw, 9)))
        f.write(chunk(b"IEND", b""))


def glyph_decode(raw):
    """RE2-Glyphenstrom -> Text. 0x00 Leerzeichen, 0x0C..0x5F -> chr(code+0x24)."""
    s = ""
    for b in raw:
        if b == 0x00:
            s += " "
        elif 0x0C <= b < 0x60:
            s += chr(b + 0x24)
        else:
            s += "<%02X>" % b
    return s


HTML_KOPF = u"""<!DOCTYPE html>
<meta charset="utf-8">
<title>RE2-Dokumente</title>
<style>
:root{--bg:#f7f5ef;--fg:#1d1d1f;--card:#fff;--line:#d8d3c6;--muted:#6b6355;--acc:#8a2b2b}
@media (prefers-color-scheme:dark){:root:not([data-theme="light"]){
 --bg:#14151a;--fg:#e9e6df;--card:#1c1e25;--line:#31343d;--muted:#9a958a;--acc:#e0736f}}
:root[data-theme="dark"]{--bg:#14151a;--fg:#e9e6df;--card:#1c1e25;--line:#31343d;
 --muted:#9a958a;--acc:#e0736f}
*{box-sizing:border-box}
body{margin:0;background:var(--bg);color:var(--fg);
 font:15px/1.55 "Iowan Old Style",Georgia,serif;padding:0 16px 64px}
header,main{max-width:1180px;margin:0 auto}
h1{font-size:1.9rem;margin:2rem 0 .3rem}
h2{font-size:1.15rem;margin:0}
.sub{color:var(--muted);margin:0 0 1.2rem}
.zahlen{display:flex;flex-wrap:wrap;gap:10px;margin:1.2rem 0 1.6rem}
.zahlen div{background:var(--card);border:1px solid var(--line);border-radius:10px;
 padding:8px 14px}
.zahlen b{display:block;font-size:1.5rem;font-family:ui-monospace,monospace}
.zahlen span{color:var(--muted);font-size:.82rem}
.hinweis{background:var(--card);border:1px solid var(--line);border-left:4px solid var(--acc);
 border-radius:8px;padding:12px 16px;margin:0 0 1.6rem}
.hinweis p{margin:.4rem 0}
.toc{display:flex;flex-wrap:wrap;gap:6px;margin:0 0 2rem;padding:0;list-style:none}
.toc a{display:block;background:var(--card);border:1px solid var(--line);border-radius:6px;
 padding:4px 9px;text-decoration:none;color:var(--fg);font-size:.86rem}
.toc a:hover{border-color:var(--acc)}
.dok{background:var(--card);border:1px solid var(--line);border-radius:12px;
 padding:16px;margin:0 0 20px;scroll-margin-top:16px}
.kopf{display:flex;flex-wrap:wrap;gap:8px 16px;align-items:baseline;
 border-bottom:1px solid var(--line);padding-bottom:8px;margin-bottom:12px}
.nr{font-family:ui-monospace,monospace;color:var(--acc);font-weight:700}
.meta{color:var(--muted);font-size:.8rem;font-family:ui-monospace,monospace}
.spalten{display:grid;grid-template-columns:172px 1fr;gap:16px}
@media(max-width:820px){.spalten{grid-template-columns:1fr}}
.bild img{width:100%;max-width:140px;image-rendering:pixelated;background:#0d0e12;
 border:1px solid var(--line);border-radius:6px}
.bild figcaption{color:var(--muted);font-size:.76rem;margin-top:4px}
.seiten{display:flex;flex-wrap:wrap;gap:8px;margin:0 0 12px}
.seiten figure{margin:0}
.seiten img{width:256px;max-width:100%;image-rendering:pixelated;
 border:1px solid var(--line);border-radius:4px;display:block}
.seiten figcaption{color:var(--muted);font-size:.74rem;font-family:ui-monospace,monospace}
details{margin-top:6px}
summary{cursor:pointer;color:var(--acc);font-size:.88rem}
pre{white-space:pre-wrap;font:12.5px/1.5 ui-monospace,monospace;background:var(--bg);
 border:1px solid var(--line);border-radius:6px;padding:10px;overflow-x:auto;margin:6px 0 0}
a.roh{font-size:.74rem;font-family:ui-monospace,monospace;color:var(--muted)}
</style>
<header>
<h1>Resident&nbsp;Evil&nbsp;2 &mdash; alle 25 Dokumente</h1>
<p class="sub">Unveraendert aus <code>info/re2leon/</code> extrahiert, zur Auswahl als
Vorlage fuer die RE1.5-Dokumente.</p>
<div class="zahlen">
<div><b>25</b><span>Dokumente (Soll = Ist)</span></div>
<div><b>191</b><span>Seitenbilder 4bpp</span></div>
<div><b>25</b><span>Hintergrundbilder 8bpp</span></div>
<div><b>216</b><span>TIMs gesamt</span></div>
<div><b>166</b><span>davon verschieden</span></div>
</div>
<div class="hinweis">
<p><b>Aufbau eines Dokuments.</b> Links das <b>Hintergrundbild</b> (8bpp, 128&nbsp;px breit)
&mdash; der vorgerenderte Gegenstand, bei den Film-Dokumenten das Foto selbst. Rechts die
<b>Textseiten</b> (4bpp, 256&nbsp;px breit). Beide stecken im selben Slot derselben Datei,
und das Spiel legt sie im VRAM absichtlich uebereinander: das Hintergrundbild belegt
128&times;256, die Textseite ueberschreibt davon die oberen H&nbsp;Zeilen. Uebrig bleiben
genau die unteren 256&minus;H&nbsp;Zeilen des Bildes &mdash; das ist der Ausschnitt, der
hier gezeigt wird.</p>
<p><b>Auf dem Bildschirm liegt der Text ueber dem Bild.</b> Der Leser zeichnet zwei
Sprites: erst das Bild (128&times;(256&minus;H) an Position 100/60), dann darueber die
Textseite (256&times;H an 25/30). Weil die Textseite durchsichtigen Grund hat, scheint das
Bild hindurch. <b>Dieses 8bpp-Bild ist also der eigentliche &bdquo;Dokumenten-Hintergrund&ldquo;</b>
&mdash; nicht ein Pergament aus einer anderen Datei; die Status-Bildschirm-Grafik
<code>ST_FILE.TIM</code> laedt RE2 nie.</p>
<p><b>Der Text ist ein Bild.</b> RE2 speichert den Dokumenttext auf der PSX nicht als
Zeichen, sondern gerastert &mdash; die 16-Farben-Palette einer Seite hat Eintrag&nbsp;0 =
<code>0x0000</code> (durchsichtig) und Eintrag&nbsp;9 = <code>0x7FFF</code> (weiss). Als echte
Zeichenkette existiert nur der <b>Name</b>. Die aufklappbaren Texte unten sind daher eine
<b>von Hand abgelesene Transkription</b>, kein Originalbyte.</p>
<p><b>Ansichtshilfe.</b> Die gezeigten Seiten-PNGs haben einen neutralen dunklen Grund
untergelegt, weil die Originale durchsichtig sind. Byte-true liegen sie in
<code>hintergruende/seiten_roh/</code> (TIM) und <code>seiten_png/</code> (RGBA).</p>
<p><b>TITEL und S00 sind dasselbe Bild</b> &mdash; in allen 25 Dokumenten byte-identisch.
Das Titelbild liegt einmal im Titel-Slot und einmal als Seite&nbsp;0. Beide sind unten
gezeigt, damit nichts unterschlagen wirkt.</p>
<p><b>Zum Namen.</b> Der Balken zeigt den Namen doppelt: als <i>Anzeige</i> und als
<i>strenge Dekodierung</i> <code>chr(byte+0x24)</code> samt Rohbytes. Wo die strenge
Dekodierung <code>^</code> ergibt, zeichnet der RE2-Zeichensatz einen Apostroph &mdash;
gemessen an den Seitenbildern, die dasselbe Wort mit <code>'</code> zeigen
(&bdquo;CHRIS'S&nbsp;DIARY&ldquo;). Sonst ist nichts ersetzt.</p>
</div>
"""


def uebersicht(out, docs):
    """Schreibt uebersicht.html — der Bogen, auf dem ausgewaehlt wird."""
    import html as _h
    L = [HTML_KOPF, '<ul class="toc">']
    for d in docs:
        L.append('<li><a href="#d%d">%d&nbsp;%s</a></li>'
                 % (d["doc"], d["doc"], _h.escape(d["name_anzeige"])))
    L.append("</ul></header>\n<main>")
    for d in docs:
        tp = os.path.join(out, "texte", "transkription", "FILE%02d.txt" % d["doc"])
        txt = open(tp, encoding="utf-8").read() if os.path.isfile(tp) else \
            "(keine Transkription vorhanden — re2_dokumente_transkription.py laufen lassen)"
        L.append('<section class="dok" id="d%d">' % d["doc"])
        L.append('<div class="kopf"><span class="nr">%02d</span><h2>%s</h2>'
                 '<span class="meta">Item-Id 0x%02X &middot; streng: %s &middot; @%s %s</span>'
                 '<span class="meta">%d Slots &middot; %d Textseiten &middot; Seitenhoehe %d</span>'
                 '</div>'
                 % (d["doc"], _h.escape(d["name_anzeige"]), d["item_id"],
                    _h.escape(d["name"]), d["name_addr"], d["name_hex"],
                    d["slots"], d["textseiten"], d["page_h"]))
        L.append('<div class="spalten"><figure class="bild">'
                 '<a href="hintergruende/bilder_png/FILE%02d_title_paper.png">'
                 '<img src="%s" alt="Hintergrundbild Dokument %d"></a>'
                 '<figcaption>Hintergrundbild 128&times;%d<br>'
                 '<a class="roh" href="hintergruende/bilder_png/FILE%02d_title_paper.png">'
                 'ganzes Bild</a> &middot; '
                 '<a class="roh" href="hintergruende/bilder_roh/FILE%02d_title_paper.TIM">'
                 'roh (TIM)</a></figcaption></figure><div>'
                 % (d["doc"], d["icon"], d["doc"], 256 - d["page_h"], d["doc"], d["doc"]))
        L.append('<div class="seiten">')
        alle = [(d["titel"], "TITEL")] + [(p, "S%02d" % i)
                                          for i, p in enumerate(d["seiten"])]
        for p, tag in alle:
            roh = p.replace("seiten_lesbar", "seiten_roh").replace(".png", ".TIM")
            # kein loading="lazy": sonst fehlen die Bilder beim Drucken und in
            # Bildschirmfotos (gemessen — ein Kopfloser-Browser-Schnappschuss der
            # Dokument-19-Karte kam leer heraus).
            L.append('<figure><img src="%s" alt="%s">'
                     '<figcaption>%s &middot; <a class="roh" href="%s">TIM</a></figcaption>'
                     '</figure>' % (p, tag, tag, roh))
        L.append('</div>')
        L.append('<details><summary>Transkription anzeigen (abgelesen, keine Originalbytes)'
                 '</summary><pre>%s</pre></details>' % _h.escape(txt))
        L.append('</div></div></section>')
    L.append("</main>")
    p = os.path.join(out, "uebersicht.html")
    open(p, "w", encoding="utf-8", newline="\n").write("\n".join(L))
    return p


def main():
    out = sys.argv[1] if len(sys.argv) > 1 else OUT
    exe = open(EXE, "rb").read()
    taddr = struct.unpack_from("<I", exe, 0x18)[0]
    assert taddr == 0x80010000, "unerwartetes t_addr 0x%08x" % taddr
    fo = lambda a: 0x800 + (a - taddr)
    tim = open(SRC, "rb").read()

    # --- Tabellen ----------------------------------------------------------------
    first = [exe[fo(DOC_TAB) + i] for i in range(N_DOC)]
    assert exe[fo(DOC_TAB) + N_DOC] == 0, "Dokumenttabelle endet nicht nach 25 Eintraegen"
    doc_meta = []
    for i in range(N_DOC):
        o = fo(CNT_TAB + i * 4)
        mp = struct.unpack_from("<H", exe, o)[0]
        h = 256 - exe[o + 2]
        assert h in (112, 128, 144, 176), "unerwartete Seitenhoehe %d (Dokument %d)" % (h, i)
        doc_meta.append((mp, h))
    slots = []
    for i in range(N_SLOT):
        o = fo(REC_TAB + i * 8)
        size = struct.unpack_from("<I", exe, o)[0]
        sect = struct.unpack_from("<H", exe, o + 4)[0] | (exe[o + 6] << 16)
        slots.append((size, sect * 0x800))

    # --- Namen -------------------------------------------------------------------
    def name_raw(off_tab, blob, item_id):
        o = struct.unpack_from("<H", exe, fo(off_tab + item_id * 2))[0]
        p = fo(blob + o)
        q = p
        while exe[q] != 0xF7:
            q += 1
        return exe[p:q], blob + o

    # --- Verzeichnisse -----------------------------------------------------------
    dirs = {k: os.path.join(out, *k.split("/")) for k in
            ("hintergruende/seiten_roh", "hintergruende/seiten_png",
             "hintergruende/seiten_lesbar", "hintergruende/bilder_roh",
             "hintergruende/bilder_png", "hintergruende/bilder_band",
             "modelle", "texte", "texte/transkription")}
    for d in dirs.values():
        os.makedirs(d, exist_ok=True)

    rows, docs, band_zensus, leere = [], [], [], []
    n_title_dup = 0
    for di in range(N_DOC):
        item_id = FIRST_DOC_ITEM + di
        lo = first[di]
        hi = first[di + 1] if di + 1 < N_DOC else N_SLOT
        max_page, page_h = doc_meta[di]
        en_raw, en_addr = name_raw(NAME_EN_OFF, NAME_EN_BLOB, item_id)
        jp_raw, jp_addr = name_raw(NAME_JP_OFF, NAME_JP_BLOB, item_id)
        en = glyph_decode(en_raw)
        pages, icon, title_png = [], None, None
        for si in range(lo, hi):
            size, base = slots[si]
            assert base + size <= len(tim), "Slot %d liegt ausserhalb FILES.TIM" % si
            page = si - lo - 1                      # -1 = Titel-Slot
            t = tim_parse(tim, base)
            while True:
                role = "page" if t["bpp"] == 4 else "paper"
                tag = "title" if page < 0 else "p%02d" % page
                stem = "FILE%02d_%s_%s" % (di, tag, role)
                w, h, rgba = tim_rgba(t)
                if role == "paper":
                    open(os.path.join(dirs["hintergruende/bilder_roh"], stem + ".TIM"),
                         "wb").write(tim[t["off"]:t["end"]])
                    png_write(os.path.join(dirs["hintergruende/bilder_png"],
                                           stem + ".png"), w, h, rgba)
                    # Band, das der Leser zeichnet: Zeilen page_h..255
                    band = rgba[page_h * w * 4:]
                    png_write(os.path.join(dirs["hintergruende/bilder_band"],
                                           stem + ".png"), w, 256 - page_h, band)
                    icon = "hintergruende/bilder_band/" + stem + ".png"
                    # Bandzensus: oberhalb des Bandes darf keine Kunst stehen.
                    oben = rgba[:page_h * w * 4]
                    hell, n_opak = 0, 0
                    for q in range(0, len(oben), 4):
                        if oben[q + 3]:
                            n_opak += 1
                            hell = max(hell, oben[q], oben[q + 1], oben[q + 2])
                    band_zensus.append((di, page_h, n_opak, hell))
                else:
                    open(os.path.join(dirs["hintergruende/seiten_roh"], stem + ".TIM"),
                         "wb").write(tim[t["off"]:t["end"]])
                    png_write(os.path.join(dirs["hintergruende/seiten_png"],
                                           stem + ".png"), w, h, rgba)
                    png_write(os.path.join(dirs["hintergruende/seiten_lesbar"],
                                           stem + ".png"), w, h, rgba, bg=LESBAR_BG)
                    if not any(rgba[q] for q in range(3, len(rgba), 4)):
                        leere.append(stem)      # nur Index 0 -> nichts Sichtbares
                    if page < 0:
                        title_png = "hintergruende/seiten_lesbar/" + stem + ".png"
                    else:
                        pages.append("hintergruende/seiten_lesbar/" + stem + ".png")
                rows.append([di, si, page, role, "0x%06X" % t["off"], t["end"] - t["off"],
                             t["bpp"], w, h, "0x%04X" % t["cluts"][0][0] if t["cluts"] else "",
                             "%d,%d" % t["clut_vram"], stem + ".TIM"])
                if t["end"] >= base + size:
                    break
                t = tim_parse(tim, t["end"])
        # Gegenprobe: Titel-Slot-Bild == Bild von Seite 0?
        a = os.path.join(dirs["hintergruende/seiten_roh"], "FILE%02d_title_page.TIM" % di)
        b = os.path.join(dirs["hintergruende/seiten_roh"], "FILE%02d_p00_page.TIM" % di)
        dup = os.path.isfile(b) and open(a, "rb").read() == open(b, "rb").read()
        n_title_dup += 1 if dup else 0
        # Texte
        with open(os.path.join(dirs["texte"], "FILE%02d_name.txt" % di), "w",
                  encoding="utf-8", newline="\n") as f:
            f.write("Dokument %d   Item-Id 0x%02X\n" % (di, item_id))
            f.write("Name (Bank EN @0x%08X): %s\n" % (en_addr, en))
            f.write("  Rohbytes: %s F7\n" % en_raw.hex(" ").upper())
            f.write("Name (Bank 2 @0x%08X, nicht-lateinischer Zeichensatz, nur Hex):\n"
                    "  Rohbytes: %s F7\n" % (jp_addr, jp_raw.hex(" ").upper()))
        # Anzeigename: nur 0x3A -> Apostroph, weil der RE2-Zeichensatz dort einen
        # Apostroph zeichnet (gemessen an den Seitenbildern: "CHRIS'S DIARY").
        # Die strenge Dekodierung `en` und die Rohbytes bleiben unveraendert daneben.
        docs.append(dict(doc=di, item_id=item_id, name=en,
                         name_anzeige=en.replace("^", "'"),
                         name_hex=en_raw.hex(" ").upper(), name_addr="0x%08X" % en_addr,
                         name2_hex=jp_raw.hex(" ").upper(), name2_addr="0x%08X" % jp_addr,
                         slots=hi - lo, textseiten=len(pages) + 1, max_page=max_page,
                         page_h=page_h, titel_gleich_seite0=dup,
                         icon=icon, titel=title_png, seiten=pages))

    with open(os.path.join(out, "toc.csv"), "w", newline="", encoding="utf-8") as f:
        w = csv.writer(f)
        w.writerow(["doc", "slot", "seite", "rolle", "byteoff_in_FILES_TIM", "tim_bytes",
                    "bpp", "breite", "hoehe", "clut0", "clut_vram", "name"])
        w.writerows(rows)
    with open(os.path.join(out, "dokumente.csv"), "w", newline="", encoding="utf-8") as f:
        w = csv.writer(f)
        w.writerow(["doc", "item_id", "name_anzeige", "name_streng", "name_bytes_hex",
                    "name_addr", "slots", "textseiten", "max_page_startwert",
                    "seitenhoehe", "titelbild_gleich_seite0"])
        for d in docs:
            w.writerow([d["doc"], "0x%02X" % d["item_id"], d["name_anzeige"], d["name"],
                        d["name_hex"], d["name_addr"], d["slots"], d["textseiten"],
                        d["max_page"], d["page_h"],
                        "ja" if d["titel_gleich_seite0"] else "NEIN"])
    with open(os.path.join(out, "dokumente.json"), "w", encoding="utf-8") as f:
        json.dump(docs, f, indent=1, ensure_ascii=False)

    # --- Zugabe: die Terminal-Texte, die in RE2 als KLARTEXT in der EXE stehen ---
    # Beleg: der Block beginnt @0x80010BCC ("DOOR LOCK SERVICE]---...") und wird
    # referenziert von `addiu a1=>s_DOOR_LOCK_SERVICE...,a1,0xbcc` @0x8003AB98.
    # Gegenprobe der Offset-Rechnung: Datei 0x104C -> 0x8001084C, und Ghidra benennt
    # genau dort `s_DOOR_SOUND_8001084c`.
    # Im Block liegen zwischen den Texten Zeigertabellen (0x8003xxxx-Worte), daher wird
    # je Zeichenkette gefiltert: nur druckbares ASCII 0x20..0x7E wird ausgegeben.
    TERM_VON, TERM_BIS = 0x13CC, 0x1694
    tl = ["RE2 (Retail, Leon) — Klartext-Texte des Computer-Terminals aus PSX.EXE",
          "Block Datei 0x%04X..0x%04X  =  RAM 0x%08X..0x%08X" %
          (TERM_VON, TERM_BIS, taddr - 0x800 + TERM_VON, taddr - 0x800 + TERM_BIS),
          "Referenz: `addiu a1,a1,0xbcc` @0x8003AB98 laedt 0x80010BCC.",
          "",
          "Steuerzeichen, wie sie in den Bytes stehen (NICHT ersetzt):",
          "  ]        Zeilenumbruch",
          "  |<n>     Farbwechsel (|0 = zurueck, |1, |2 ...)",
          "  [0nn.    Pause um nn Einheiten, dann ein Punkt",
          ""]
    p = TERM_VON
    while p < TERM_BIS:
        if not 0x20 <= exe[p] <= 0x7E:
            p += 1
            continue
        q = p
        while q < TERM_BIS and 0x20 <= exe[q] <= 0x7E:
            q += 1
        if q - p >= 5:
            tl.append("@0x%08X  %s" % (taddr - 0x800 + p, exe[p:q].decode("ascii")))
        p = q
    open(os.path.join(dirs["texte"], "exe_terminaltexte.txt"), "w",
         encoding="utf-8", newline="\n").write("\n".join(tl) + "\n")

    # --- Roh-Beigaben des FILE-Bildschirms --------------------------------------
    import shutil
    for fn in ("ST_FILE.TIM", "ST_FILE.TM2", "ST_FILE.TS"):
        src = os.path.join(REPO, "info", "re2leon", "COMMON", "DATA", fn)
        if os.path.isfile(src):
            shutil.copyfile(src, os.path.join(dirs["modelle"], fn))

    uebersicht(out, docs)

    n_page = sum(1 for r in rows if r[3] == "page")
    n_paper = sum(1 for r in rows if r[3] == "paper")
    print("Dokumente     : %d  (Soll 25, Tabelle @0x800A9AD0 endet nach 25)" % len(docs))
    print("Slots benutzt : %d  (Soll 191, Tabelle @0x800A94B4)"
          % sum(d["slots"] for d in docs))
    print("TIMs geschnitten: %d  (%d Seitenbilder 4bpp + %d Ikonen 8bpp)"
          % (len(rows), n_page, n_paper))
    print("Titelbild == Seite 0: %d von %d Dokumenten" % (n_title_dup, N_DOC))
    hell = max(h for _, _, _, h in band_zensus)
    klar = [d for d, _, n, _ in band_zensus if n == 0]
    assert hell <= 16, ("Bandzensus verletzt: oberhalb des Bandes steht Kunst "
                        "(hellster Kanalwert %d von 255)" % hell)
    print("Bandzensus  : ueber dem Band keine Kunst — %d/%d Dokumente dort vollstaendig "
          "durchsichtig, die uebrigen %s mit opakem Schwarz gefuellt; hellster "
          "Kanalwert %d von 255"
          % (len(klar), N_DOC, [d for d, _, n, _ in band_zensus if n], hell))
    print("Leere Seiten: %d von %d Seitenbildern enthalten nur Index 0, also nichts "
          "Sichtbares: %s" % (len(leere), n_page, ", ".join(leere)))

    # --- Abdeckungszensus: bleibt in FILES.TIM etwas uebrig? --------------------
    # Drei Fragen, drei Antworten, alle als assert:
    #  (a) deckt sich das, was die 216 TIMs belegen, mit dem, was die Slot-Tabelle
    #      benennt (also kein Byte in einem Slot ohne TIM)?
    #  (b) ist alles, was KEIN Slot benennt, reine Nullfuellung bis zur naechsten
    #      Sektorgrenze (0x800)?  Wenn ja, enthaelt der Container nichts weiter.
    belegt = bytearray(len(tim))
    for r in rows:
        o, n = int(r[4], 16), r[5]
        belegt[o:o + n] = b"\x01" * n
    benannt = bytearray(len(tim))
    for size, base in slots:
        benannt[base:base + size] = b"\x01" * size
    in_slot_ohne_tim = sum(1 for i in range(len(tim)) if benannt[i] and not belegt[i])
    assert in_slot_ohne_tim == 0, \
        "%d Bytes liegen in einem Slot, aber in keinem TIM" % in_slot_ohne_tim
    rest_nz = sum(1 for i in range(len(tim)) if not benannt[i] and tim[i])
    assert rest_nz == 0, \
        "%d Bytes ausserhalb aller Slots sind NICHT 0x00 — da liegt noch Inhalt" % rest_nz
    n_belegt = sum(belegt)
    print("Abdeckung   : %d von %d Bytes in FILES.TIM (%.1f %%) von den 216 TIMs belegt; "
          "kein Byte in einem Slot ohne TIM; die restlichen %d Bytes sind ALLE 0x00 "
          "(Sektorfuellung) -> im Container bleibt nichts uebrig"
          % (n_belegt, len(tim), 100.0 * n_belegt / len(tim), len(tim) - n_belegt))
    print("->", out)


if __name__ == "__main__":
    main()
