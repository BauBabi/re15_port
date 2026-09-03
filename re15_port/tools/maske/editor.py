"""Maskeneditor — die fehlenden Vordergrundbereiche von Hand nachzeichnen.

WOZU: Fuer 1468 Kamera-Cuts hat der Prototyp weder Maskengeometrie noch
Vordergrundgrafik hinterlassen. Automatisch fuellen laesst sich das nicht — gemessen
(analysis/esp_masken_2026-09-03/):
  projizierte Kollision       25.9 - 28.1 % Praezision  (SCA beschreibt vor allem Waende)
  gelernter Segmentierer      19.7 - 26.3 % Praezision  (Vordergrundanteil 20.6 %, also
                                                         kaum ueber der Ratequote)
Die vorher festgelegte Schwelle war 95 %. Eine automatische Fuellung wuerde also in drei
von vier Faellen Hintergrund ueber die Figur malen — schlechter als der heutige Zustand.

DESHALB HANDARBEIT — aber nicht mit dem Pinsel. Das Bild wird in Superpixel zerlegt
(Kanten folgen den echten Objektgrenzen), und markiert wird durch ANKLICKEN. Ein Cut
braucht damit ein paar Klicks statt einer ausgemalten Silhouette, und die Raender sitzen
genauer, als man sie von Hand zoege.

Alles Weitere kommt aus den Messungen: Rechteckzerlegung, Tiefe aus dem Bodenkontakt je
Bildspalte, Ausgabe im Original-Sektionsformat (geom.py), Atlas gepackt (atlas.py).

VORAUSSETZUNG: die Hintergruende muessen dekodiert vorliegen —
    re15_port/build/tests/unit/probe_bg_dump.exe <cd-wurzel> build/bg_ppm

BEDIENUNG
    linke Maus (ziehen)   Superpixel markieren
    rechte Maus (ziehen)  Markierung entfernen
    n / p                 naechster / voriger Cut
    c                     Cut leeren
    +/-                   Superpixel groeber / feiner
    s                     Raum speichern (.MSK + Atlas-TIMs)
    q                     beenden

Aufruf:
    python re15_port/tools/maske/editor.py ROOM1140 [--cd re15_port/shared_assets/PSX]
"""
import argparse
import os
import struct
import sys
import tkinter as tk

import numpy as np
from PIL import Image, ImageTk
from skimage.segmentation import slic

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import atlas as atlasmod
import geom
from geom import load_bg, load_rdt, original_has_masks

SCALE = 2                       # Anzeige 640x480 — 320x240 ist zum Klicken zu klein


class Editor:
    def __init__(self, root, cd, ppm, out, room):
        self.cd, self.ppm, self.out, self.room = cd, ppm, out, room
        self.room_id = int(room[4:], 16)
        self.rdt, self.stage = load_rdt(cd, room)
        if not self.rdt:
            raise SystemExit("RDT nicht gefunden: %s" % room)
        self.n_cut = self.rdt[1]
        self.cam = struct.unpack_from("<I", self.rdt, 0x24)[0]
        # Nur Cuts anbieten, in denen das ORIGINAL nichts hat — dessen Arbeit
        # bleibt unangetastet byte-true.
        self.cuts = [c for c in range(self.n_cut)
                     if not original_has_masks(self.rdt, self.cam, c)]
        if not self.cuts:
            raise SystemExit("%s: alle Cuts haben Original-Masken, nichts zu tun" % room)
        self.i = 0
        self.n_seg = 400
        self.sel = {}                 # cut -> bool-Maske (240,320)
        self.root = root
        self.canvas = tk.Canvas(root, width=320 * SCALE, height=240 * SCALE,
                                highlightthickness=0)
        self.canvas.pack()
        self.status = tk.Label(root, anchor="w", font=("Consolas", 10))
        self.status.pack(fill="x")
        self.canvas.bind("<B1-Motion>", lambda e: self.paint(e, True))
        self.canvas.bind("<Button-1>", lambda e: self.paint(e, True))
        self.canvas.bind("<B3-Motion>", lambda e: self.paint(e, False))
        self.canvas.bind("<Button-3>", lambda e: self.paint(e, False))
        root.bind("<Key>", self.key)
        self.load()

    # ---------------- Daten ----------------
    def cut(self):
        return self.cuts[self.i]

    def load(self):
        c = self.cut()
        self.bg = load_bg(self.ppm, self.room_id, c)
        if self.bg is None:
            self.segments = None
            self.say("Hintergrund fehlt — probe_bg_dump laufen lassen")
            self.canvas.delete("all")
            return
        self.segments = slic(self.bg, n_segments=self.n_seg, compactness=12,
                             start_label=0, channel_axis=2)
        if c not in self.sel:
            self.sel[c] = np.zeros((240, 320), bool)
        self.draw()

    def paint(self, ev, on):
        if self.segments is None:
            return
        x, y = ev.x // SCALE, ev.y // SCALE
        if not (0 <= x < 320 and 0 <= y < 240):
            return
        s = self.segments[y, x]
        self.sel[self.cut()][self.segments == s] = on
        self.draw()

    # ---------------- Anzeige ----------------
    def draw(self):
        m = self.sel[self.cut()]
        img = self.bg.astype(np.int16).copy()
        img[m] = (img[m] * 0.45 + np.array([255, 40, 200]) * 0.55).astype(np.int16)
        pil = Image.fromarray(np.clip(img, 0, 255).astype(np.uint8))
        pil = pil.resize((320 * SCALE, 240 * SCALE), Image.NEAREST)
        self.tk_img = ImageTk.PhotoImage(pil)
        self.canvas.delete("all")
        self.canvas.create_image(0, 0, anchor="nw", image=self.tk_img)
        self.say("%s Cut %d (%d/%d)  markiert %.1f %%  Superpixel %d"
                 % (self.room, self.cut(), self.i + 1, len(self.cuts),
                    100.0 * m.mean(), self.n_seg))

    def say(self, s):
        self.status.config(text="  " + s)

    # ---------------- Tasten ----------------
    def key(self, ev):
        k = ev.keysym.lower()
        if k == "q":
            self.root.destroy()
        elif k == "n":
            self.i = (self.i + 1) % len(self.cuts); self.load()
        elif k == "p":
            self.i = (self.i - 1) % len(self.cuts); self.load()
        elif k == "c":
            self.sel[self.cut()][:] = False; self.draw()
        elif k in ("plus", "equal"):
            self.n_seg = min(2000, int(self.n_seg * 1.5)); self.load()
        elif k == "minus":
            self.n_seg = max(60, int(self.n_seg / 1.5)); self.load()
        elif k == "s":
            self.save()

    # ---------------- Speichern ----------------
    def save(self):
        os.makedirs(self.out, exist_ok=True)
        sections = {}
        written_atlas = 0
        for c, m in self.sel.items():
            if m.sum() < 64:
                continue
            bg = load_bg(self.ppm, self.room_id, c)
            if bg is None:
                continue
            dep = geom.depth_map(self.rdt, self.cam, c, m)
            if dep is None:
                continue
            boxes = geom.rects_from_mask(m, geom.MAX_MASKS_PER_CUT)
            if not boxes:
                continue
            tim, place, boxes = atlasmod.build(bg, m, boxes)
            if tim is None:
                continue
            # Eine Gruppe je Kasten: Anker = Bildposition - Atlasposition, dann ist
            # dst = Anker + src und das u8-Feld reicht immer.
            groups, masks = [], []
            for i, (x, y, w, h) in enumerate(boxes):
                if i not in place:
                    continue                      # passte nicht ins Blatt
                ax, ay = place[i]
                win = dep[y:y + h, x:x + w]
                if not (win > 0).any():
                    continue
                d = int(np.median(win[win > 0]))
                groups.append((1, x - ax, y - ay))
                masks.append((ax, ay, x, y, w, h, d))
            if not masks:
                continue
            sections[c] = geom.pack_section(groups, masks)
            open(os.path.join(self.out, "ROOM%04X_PRI%02d.TIM"
                              % (self.room_id, c)), "wb").write(tim)
            written_atlas += 1

        if not sections:
            self.say("nichts zu speichern")
            return
        blob = geom.pack_container(sections, self.n_cut)
        path = os.path.join(self.out, "%s.MSK" % self.room)
        open(path, "wb").write(blob)
        self.say("gespeichert: %s (%d Cuts, %d Atlanten)"
                 % (os.path.basename(path), len(sections), written_atlas))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("room")
    ap.add_argument("--cd", default="re15_port/shared_assets/PSX")
    ap.add_argument("--ppm", default="build/bg_ppm")
    ap.add_argument("--out", default="re15_port/shared_assets/PSX/MASKS")
    a = ap.parse_args()
    root = tk.Tk()
    root.title("RE1.5 — Vordergrundmasken nachzeichnen")
    Editor(root, a.cd, a.ppm, a.out, a.room.upper())
    root.mainloop()


if __name__ == "__main__":
    main()
