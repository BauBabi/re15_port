# -*- coding: utf-8 -*-
"""NB3 Bauer (so angewandt, CRLF-erhaltend): Querbezug integration_r41_re2gator Lauf kanister - RE15_EXIT_AT 1500 -> 1700 + Kopfkommentar (f)."""
import io
p = r"C:/workspace/git/reAi_v2/.claude/worktrees/r41_gatorfix/re15_port/tests/integration/test_r41_re2gator.cmake"
raw = io.open(p, "rb").read()
crlf = b"\r\n" in raw
t = raw.decode("utf-8").replace("\r\n", "\n")
a = '    "RE15_EXIT_AT=1500#2090"   # Querbezug Runde 41 Spur F (Kopf): ohne Spinnen Aufnahme F1227 statt F1119 -> Fenster +100\n'
b = '    "RE15_EXIT_AT=1700#2090"   # Querbezug Runde 41 Spur F (Kopf): ohne Spinnen Aufnahme F1227 statt F1119 -> Fenster +100;\n' \
    '                               # Spur G2 NB3 (f): Aufnahme F1447 statt F1227 -> Fenster +200 (Schranke >= 200 Bilder, Abweichung 0 bleibt)\n'
assert t.count(a) == 1, t.count(a)
t = t.replace(a, b)
h_alt = ('#   420 -> 440 (gemessen F41n4 Laeufe N_L/N_E: Folge C F423). Pruefungen unveraendert.\n')
h_neu = h_alt + (
'#   (f) Querbezug Runde 41 Spur G2 NB3 (Nutzerentscheid 2026-11-06 F4 "Er darf nicht zu lange haengen" -> Vorgabe B, enemy_ai_boss_gator.c\n'
'#   gb_punkt_im_band / Zweig 7): steht die Figur im Koerper-Band vor der Plattform-/Rampenkante, faehrt die Verfolgung sie direkt an statt\n'
'#   ueber den Westumlauf. Im Lauf "kanister" fuehrt der Autopilot Leon durch das Rampenband ((5600,-19698) F1115: zw=7 statt zw=6), die\n'
'#   Bahn des Gators liegt danach anders zum liegenden Kanister: Zielmodus 1/0 viermal (F1129/1141, 1252/1261, 1327/1336, 1427; laengs\n'
'#   < 8880 ohne Aufnahme-Fenster, quer > Koerper + 267) und die Aufnahme erst im vierten Vorbeilauf F1435 (quer 1040) / angehaengt F1447\n'
'#   statt F1215 (quer -478) / F1227 - bis EXIT_AT 1500 nur 30 angehaengte Bilder (zweimal einzeln nachgefahren, bitgleich; alter Stand\n'
'#   a661eb8aa dreimal F1227). RE15_EXIT_AT 1500 -> 1700 (+200 = Verschiebung 220 auf die naechsten 100 gerundet, Regel (c)); die\n'
'#   Schranke >= 200 Bilder bei Abweichung 0 bleibt. Belege: analysis/befunde_runde41/G2_belege/nb3/bau/re2gator/.\n')
assert t.count(h_alt) == 1
t = t.replace(h_alt, h_neu)
out = t.replace("\n", "\r\n") if crlf else t
io.open(p, "wb").write(out.encode("utf-8"))
print("gepatcht, crlf=%d" % crlf)
