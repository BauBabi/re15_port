#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""re2_tuer_ton.py - Tonteil aller 55 RE2-Tuerarchive: Kopf, VH, VAGs, Dauer; welche Toene das Skript
wann ausloest (Se_on + Bild) und ob Door_exit den Schliesston spielt; Tonfamilien per Bytevergleich.

Runde 31, Teil T2 (analysis/befunde_runde31/tueren_02_re2.md, Abschnitt 4).
Liest NUR info/re2leon/PSX.EXE, info/re2leon/COMMON/DOOR/DOORxx.DO2 (Simulator tools/tor/tuerkatalog.py).
Schreibt NUR build/r31_tueren/t2/ton.json.

Belege (RE2 Leon PSX.EXE, selbst disassembliert bzw. analysis/tor_1170/08_re_ton.md):
  Tabelle @0x8009a520, 12 B je Tuer: u16 Tonteil-Groesse (@0x80014d94 lhu s4,0(s1)), u16 Modellteil-Groesse,
    u32 Sektor, u8 Pruefsumme Ton (XOR erstes Byte je 512 B), u8 Pruefsumme Modell.
  Tonteil: Kopf 16 B (4 x 4 B, ff ff ff ff = leer @0x8005baa0), VH ab Kopf + u32 @Nachspann (@0x80014ea4),
    Nachspann @0xC30 (@0x80014e48 addiu a3,s0,3120), VB ab 0xC38 (@0x80014f84 ori s1,s1,0x1c38).
  Kopfeintrag (FUN_8005ba28): b0 Bit7 VAB-Ersatz (@0x8005bab0), b1&0x7f Programm (@0x8005bae4), b2>>4 Ton
    (@0x8005badc), b2&0xf Prio-Nibble (@0x8005bb04), b3&0x1f SPU-Stimme (@0x8005bad8), b3>>5 Folgelagen (@0x8005bae0).
  Tonsatz VH + 0x820 + prog*0x200 + ton*0x20 (@0x8005baf8); Note = tone[6] (Satz+10 @0x8005bbec),
    Fein = tone[5] (Satz+12 @0x8005bbf8), Pegel = tone[2] (@0x8005bc04..18).
  note2pitch2 @0x80083010, Tabelle @0x800aba40 (12 x 16 u16).
  Se_on 0x36 (Handler 0x80056428): pc[1] Bank, pc[2] Satz, pc[3] Lagebyte (@0x80056518..34).
  Door_exit spielt Satz 1 (a0 = 0x00010000 @0x800141f4), nur wenn Merker +0x248 (Flag 0x800 eines
    Door_model_set, @0x80014c90..a8) gesetzt ist (@0x800141d8 lhu v0,584(v0) / @0x800141e0 beq).
  Bildtakt VSync(0): 59,826 Bilder/s (analysis/tor_1170/08_re_bildtakt.md 0).

Aufruf: python re15_port/tools/tueren/re2_tuer_ton.py [--tabelle]
"""
import argparse
import hashlib
import json
import os
import struct
import sys
from collections import defaultdict

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(REPO, "re15_port", "tools", "tor"))
sys.path.insert(0, HERE)
import tuerkatalog as tk  # noqa: E402
import re2_tuer_varianten as rv  # noqa: E402

OUT = os.path.join(REPO, "build", "r31_tueren", "t2")
BILDRATE = 59.826          # VSync(0) je Bild, analysis/tor_1170/08_re_bildtakt.md


def note2pitch2(lut, note, fine, center, shift):
    """RE2 @0x80083010: fine = (a1 + tone[5]) >> 3, Uebertrag ab 16; sem = note + 60 - center + Uebertrag;
    Oktave = sem/12 - 5; Tabelle @0x800aba40."""
    s = ((fine & 0xFFFF) + (shift & 0xFF)) >> 3
    frac, carry = s, 0
    if s > 15:
        frac, carry = s - 16, 1
    sem = carry + note + 0x3C - center
    octv = int(sem / 12) - 5
    rem = sem - int(sem / 12) * 12
    p = lut[rem * 16 + frac]
    if octv < 0:
        p >>= (-octv) & 0x1F
    elif octv >= 1:
        p = None
    return p


def vag_bloecke(b):
    """Anzahl 16-B-Bloecke bis zum ersten Endflag (Flag & 1, wie vab_common.c re15_vag_adpcm_decode)."""
    n = 0
    for f in range(len(b) // 16):
        n += 1
        if b[f * 16 + 1] & 0x01:
            break
    return n


def zerlege(idx, lut):
    door = tk.Door(idx)
    d = door.data
    ent = tk.exe_rd(tk.DOOR_TABLE + idx * 12, 12)
    ton_gr, mod_gr, sektor = struct.unpack_from("<HHI", ent, 0)
    ck = 0
    for o in range(0, ton_gr, 512):
        ck ^= d[o]
    ton = d[:ton_gr]
    kopf = ton[:16]
    nach = struct.unpack_from("<I", ton, 0xC30)[0]
    vh = ton[nach:0xC30]
    vb = ton[0xC38:]
    rec = {"archiv": door.name, "tabelle": ent.hex(" "), "tonteil": ton_gr, "sektor": sektor,
           "pruefsumme_ok": ck == ent[8], "sha1_tonteil": hashlib.sha1(ton).hexdigest(),
           "kopf": kopf.hex(" "), "vh_bei": nach, "vh_kennung_ok": vh[:4] == b"pBAV"}
    nprog, ntone, nvag = struct.unpack_from("<HHH", vh, 18)
    rec.update({"ps": nprog, "ts": ntone, "vs": nvag, "vh_groesse_ok": 32 + 2048 + nprog * 512 + 512 == len(vh)})
    groessen = [struct.unpack_from("<H", vh, 2080 + nprog * 512 + 2 * i)[0] * 8 for i in range(256)]
    vag_off, pos = {}, 0
    for i, g in enumerate(groessen):
        if g:
            vag_off[i] = (pos, g)
            pos += g
    rec["vb_summe_ok"] = pos == len(vb)
    eintraege = []
    for n in range(4):
        e = kopf[n * 4:n * 4 + 4]
        if struct.unpack("<I", e)[0] == 0xFFFFFFFF:
            eintraege.append({"n": n, "leer": True})
            continue
        prog, tone, prio, stimme, folge = e[1] & 0x7F, e[2] >> 4, e[2] & 0xF, e[3] & 0x1F, e[3] >> 5
        base = 2080 + prog * 512 + tone * 32
        t = vh[base:base + 32]
        vol, pan, center, shift, note = t[2], t[3], t[4], t[5], t[6]
        vag = struct.unpack_from("<h", t, 22)[0]
        pitch = note2pitch2(lut, note, shift, center, shift)
        rate = 44100.0 * pitch / 4096.0 if pitch else None
        vo, g = vag_off.get(vag, (0, 0))
        vagb = vb[vo:vo + g]
        bl = vag_bloecke(vagb)
        samples = bl * 28
        eintraege.append({"n": n, "leer": False, "roh": e.hex(" "), "prog": prog, "ton": tone, "prio_nib": prio,
                          "stimme": stimme, "folgelagen": folge, "vol": vol, "pan": pan, "note": note,
                          "center": center, "fein": shift, "pitch": pitch, "rate_hz": round(rate, 1) if rate else None,
                          "vag": vag, "vag_bytes": g, "vag_sha1": hashlib.sha1(vagb).hexdigest()[:12],
                          "dauer_s": round(samples / rate, 3) if rate else None,
                          "dauer_bilder": round(samples / rate * BILDRATE, 1) if rate else None})
    rec["eintraege"] = eintraege
    # Skript: Se_on und Schliesston je Variante (Simulation ohne Ladewarten)
    var = []
    for v, tgt in rv.varianten(door):
        vm, summ = tk.analyse_variant(door, v)
        var.append({"variante": v, "bilder": len(vm.frames),
                    "se_on": [{"bild": s["tick"], "bank": s["vab"], "satz": s["se"] & 0xFF,
                               "lage": (s["se"] >> 8) & 0xFF, "bezug": s["work"] & 0xFF} for s in vm.sounds],
                    "schliesston_door_exit": bool(vm.global248)})
    rec["varianten"] = var
    return rec


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--tabelle", action="store_true")
    a = ap.parse_args()
    lut = list(struct.unpack("<192H", tk.exe_rd(0x800ABA40, 384)))
    alle = [zerlege(i, lut) for i in range(tk.N_DOORS)]
    fam = defaultdict(list)
    fam_e = [defaultdict(list), defaultdict(list)]
    for r in alle:
        fam[r["sha1_tonteil"]].append(r["archiv"])
        for n in (0, 1):
            e = r["eintraege"][n]
            if not e["leer"]:
                fam_e[n][e["vag_sha1"]].append(r["archiv"])
    json.dump({"archive": alle, "familien_tonteil": list(fam.values()),
               "familien_ton0": list(fam_e[0].values()), "familien_ton1": list(fam_e[1].values())},
              open(os.path.join(OUT, "ton.json"), "w"), indent=1)
    print("Pruefungen: Pruefsumme %d/55, VH-Kennung %d/55, VH-Groesse %d/55, VB-Summe %d/55, VH @0x10: %d/55" % (
        sum(r["pruefsumme_ok"] for r in alle), sum(r["vh_kennung_ok"] for r in alle),
        sum(r["vh_groesse_ok"] for r in alle), sum(r["vb_summe_ok"] for r in alle),
        sum(r["vh_bei"] == 0x10 for r in alle)))
    print("Tonteile bytegleich (Familien mit >1 Archiv):")
    for v in fam.values():
        if len(v) > 1:
            print("  ", v)
    for n in (0, 1):
        print("Ton %d (VAG bytegleich):" % n)
        for v in fam_e[n].values():
            if len(v) > 1:
                print("  ", v)
    if a.tabelle:
        for r in alle:
            es = r["eintraege"]
            t = " | ".join("E%d %s st%d vag%d %s %.2fs" % (e["n"], e["roh"], e["stimme"], e["vag"], e["vag_sha1"][:6],
                                                        e["dauer_s"] or 0) for e in es if not e["leer"])
            vs = "; ".join("V%d:%s%s" % (v["variante"], ",".join("S%d@%d" % (s["satz"], s["bild"]) for s in v["se_on"]) or "-",
                                         "+zu" if v["schliesston_door_exit"] else "") for v in r["varianten"])
            print("%s %6d B  %s  || %s" % (r["archiv"], r["tonteil"], t, vs))


if __name__ == "__main__":
    main()
