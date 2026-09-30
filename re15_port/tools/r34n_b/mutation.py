#!/usr/bin/env python3
"""Spur B (Runde 34 Nacht, BAU): MUTATIONSPROBEN der Riegel unit_r34n_b_* — "Fix raus -> Test rot".

Je Mutation: Quelltext gezielt verfaelschen (exakter Textersatz, sonst Abbruch), NUR das Ziel
probe_r34n_b_cursor bauen, den betroffenen Teil-Riegel laufen lassen, Ergebnis notieren, die Datei
per `git checkout --` zuruecksetzen. Am Ende wird der unveraenderte Stand noch einmal gebaut und
ALLE Teil-Riegel muessen wieder gruen sein.

    C:/Python310/python.exe re15_port/tools/r34n_b/mutation.py > analysis/befunde_runde34_nacht/B_belege/bau_mutationen.txt
"""
import os
import subprocess
import sys

HIER = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HIER, "..", "..", ".."))
BUILD = os.path.join(REPO, "re15_port", "build")
PROBE = os.path.join(BUILD, "tests", "unit", "probe_r34n_b_cursor.exe")
ENV = dict(os.environ)
ENV["PATH"] = r"C:\msys64\mingw64\bin;C:\msys64\usr\bin;" + ENV.get("PATH", "")

MOD = "re15_port/engine/src/hebetisch_cursor_1150.c"
VM = "re15_port/engine/src/scd_vm.c"
INC = "re15_port/engine/src/gen/hebetisch_cursor.inc"

MUTATIONEN = [
    ("M1 Halt-Haken in op_for entfernt", VM,
     "      if (hc == RE15_HC_HALT) return SCD_R_YIELD;",
     "      (void)0; /* MUTATION */",
     "halt"),
    ("M2 Treffertest invertiert", MOD,
     "        if (re15_hebetisch_cursor_in_kuppel(sx, sy)) {",
     "        if (!re15_hebetisch_cursor_in_kuppel(sx, sy)) {",
     "ablauf"),
    ("M3 Huellenecke (151,171) -> (140,171)", MOD,
     "    151, 171,  163, 161,",
     "    140, 171,  163, 161,",
     "kuppel"),
    ("M4 Armieren auch ueber scd_event_fire (Harness bekaeme den Cursor)", VM,
     "            if (scd_thread_start(slot, pc) == 0) return slot;",
     "            if (scd_thread_start(slot, pc) == 0) { re15_hebetisch_cursor_aktion(event_id, slot); return slot; }",
     "harness"),
    ("M5 Vorschalt statt Halt: Port zeigt Cut 4 schon vor sub04 (work_vars[0x0A] = 4)", MOD,
     "    s_zustand = RE15_HC_VERLANGT;\n",
     "    s_zustand = RE15_HC_VERLANGT; g_scd.work_vars[0x0A] = 4; /* MUTATION */\n",
     "cut_old"),
    ("M6 Abbruch springt 4 Byte zu weit (ohne Work_set @0x109A)", MOD,
     "        const uint8_t *ziel = halt + RE15_HC_AUFRAEUM_ABSTAND;",
     "        const uint8_t *ziel = halt + RE15_HC_AUFRAEUM_ABSTAND + 4;",
     "abbruch"),
    ("M7 Abbruch auf der GEHALTENEN Taste statt der Flanke (Schliess-CROSS sickert durch)", MOD,
     "    if (edge & RE15_HC_TASTE_ABBRUCH) {",
     "    if ((edge | held) & RE15_HC_TASTE_ABBRUCH) { /* MUTATION */",
     "kreuztext"),
    ("M8 ein Byte der eingebackenen TIM geaendert", INC,
     "static const unsigned char re15_hc_cursor_tim[33312] = {\n    0x10,",
     "static const unsigned char re15_hc_cursor_tim[33312] = {\n    0x11,",
     "bytes"),
]


def lauf(cmd, **kw):
    return subprocess.run(cmd, cwd=REPO, env=ENV, capture_output=True, text=True, **kw)


def bauen():
    r = lauf(["cmake", "--build", BUILD, "--target", "probe_r34n_b_cursor"])
    return r.returncode, (r.stdout + r.stderr)[-1500:]


def riegel(teil):
    r = lauf([PROBE, teil], timeout=300)
    zeilen = [z for z in r.stdout.splitlines() if "[FEHL]" in z or "BESTANDEN" in z or "GERISSEN" in z]
    return r.returncode, zeilen


def main():
    ergebnisse = []
    for name, datei, alt, neu, teil in MUTATIONEN:
        pfad = os.path.join(REPO, datei)
        roh = open(pfad, "rb").read()
        text = roh.decode("utf-8")
        crlf = "\r\n" in text
        alt_t = alt.replace("\n", "\r\n") if crlf else alt
        neu_t = neu.replace("\n", "\r\n") if crlf else neu
        if text.count(alt_t) != 1:
            print("%s: Ankertext %d-mal gefunden -> ABBRUCH" % (name, text.count(alt_t)))
            sys.exit(2)
        open(pfad, "wb").write(text.replace(alt_t, neu_t, 1).encode("utf-8"))
        try:
            rc_b, log_b = bauen()
            if rc_b != 0:
                ergebnisse.append((name, teil, "BAU FEHLGESCHLAGEN", log_b.splitlines()[-3:]))
            else:
                rc, zeilen = riegel(teil)
                ergebnisse.append((name, teil, "ROT (rc=%d)" % rc if rc != 0 else "GRUEN (!)", zeilen))
        finally:
            lauf(["git", "checkout", "--", datei])
    rc_b, log_b = bauen()
    ok_alle = rc_b == 0
    rest = []
    if ok_alle:
        for teil in ("halt", "kuppel", "ablauf", "cut_old", "harness", "abbruch", "kreuztext", "bytes"):
            rc, zeilen = riegel(teil)
            rest.append((teil, rc))
            ok_alle = ok_alle and rc == 0
    for name, teil, erg, zeilen in ergebnisse:
        print("%-75s Riegel unit_r34n_b_%-10s -> %s" % (name, teil if teil != "bytes" else "cursor_bytes", erg))
        for z in zeilen:
            print("      " + z.strip())
    print("\nZurueckgesetzt und neu gebaut: %s" % ("alle Teil-Riegel GRUEN " + str(rest) if ok_alle
                                                   else "FEHLER " + str(rest) + " " + log_b[-500:]))
    return 0 if ok_alle and all(e[2].startswith("ROT") for e in ergebnisse) else 1


if __name__ == "__main__":
    sys.exit(main())
