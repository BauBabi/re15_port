"""drive_1030.py — bis ROOM1030 kommen und die Kriechbewegung messen.

Aufbauend auf dem erprobten Ablauf des Skills re15-pcsx-watchpoint (pcsx_drive.py):
vgamepad VOR dem Emulator, Start mit -interpreter -debugger, Disc als C:\\tmp\\mzd.cue
(Pfad OHNE Leerzeichen/Klammern — sonst bricht -iso still).

NEU (Nutzer-Hinweis 2026-08-07): Das DEBUG-MENUE oeffnet sich mit SELECT waehrend des INTROS,
also NACH der Spielerauswahl. Vorher gedrueckt passiert nichts — deshalb landete der Sprung
zuvor immer im Intro-Raum ROOM1240.

Ablauf:
  1. ~110 s Boot abwarten (Interpreter ist ~5x langsamer)
  2. Neues Spiel: Hoch x3 + A x3  (Menue + Spielerauswahl)
  3. ein paar A, um in das Intro zu kommen
  4. SELECT -> Debug-Menue
  5. ab hier uebernimmt das Lua-Skript: Raumindex schreiben und JUMP bestaetigen
"""
import vgamepad as vg
import subprocess, time, os

B = vg.XUSB_BUTTON
EXE  = os.getenv("PCSX_EXE", r"C:\Users\mjoedicke\AppData\Local\Microsoft\WinGet\Packages\GrumpyCoders.PCSX-Redux_Microsoft.Winget.Source_8wekyb3d8bbwe\pcsx-redux.exe")
BIOS = os.getenv("PCSX_BIOS", r"C:\tmp\scph1001.bin")
ISO  = os.getenv("PCSX_ISO",  r"C:\tmp\mzd.cue")
LUA  = os.getenv("PCSX_LUA",  r"C:\workspace\git\reAi_v2\tools\redux\crawl_1030.lua")
OUT  = os.getenv("PCSX_OUT",  r"C:\workspace\git\reAi_v2\tools\redux\crawl_out.txt")
MAXS = int(os.getenv("PCSX_MAX_SECS", "420"))
BOOT = int(os.getenv("PCSX_BOOT_SECS", "115"))


def tap(gp, btn, hold=0.12, gap=0.35):
    gp.press_button(button=btn); gp.update(); time.sleep(hold)
    gp.release_button(button=btn); gp.update(); time.sleep(gap)


def fertig():
    try:
        with open(OUT, encoding="utf-8", errors="replace") as f:
            return "fertig" in f.read()
    except Exception:
        return False


gp = vg.VX360Gamepad(); gp.update(); time.sleep(1.0)
print("vgamepad erzeugt", flush=True)
proc = subprocess.Popen([EXE, "-bios", BIOS, "-iso", ISO, "-interpreter", "-debugger",
                         "-run", "-dofile", LUA, "-stdout", "-lua_stdout"],
                        stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
print("pcsx-redux gestartet, pid", proc.pid, flush=True)

t0 = time.time()
time.sleep(BOOT)

def spieler_da():
    try:
        with open(OUT, encoding="utf-8", errors="replace") as f:
            return "SPIELER existiert" in f.read()
    except Exception:
        return False


def menue_offen():
    try:
        with open(OUT, encoding="utf-8", errors="replace") as f:
            return "MENUE offen" in f.read()
    except Exception:
        return False


def neues_spiel(gp):
    """Menue + Spielerauswahl. WIEDERHOLEN, nicht einmal druecken — die Zeitpunkte sind nicht
    verlaesslich; der erprobte pcsx_drive.py macht das alle ~18 s genauso."""
    for _ in range(3):
        tap(gp, B.XUSB_GAMEPAD_DPAD_UP, gap=0.45)
    for _ in range(3):
        tap(gp, B.XUSB_GAMEPAD_A, gap=0.8)
    for _ in range(4):
        tap(gp, B.XUSB_GAMEPAD_A, gap=0.5)


naechster = 0
while time.time() - t0 < MAXS and not spieler_da():
    el = time.time() - t0
    if el >= naechster:
        print("[%.0fs] Neues Spiel" % el, flush=True)
        neues_spiel(gp)
        naechster = (time.time() - t0) + 18
    time.sleep(2)
print("[%.0fs] Spieler da -> jetzt SELECT" % (time.time() - t0), flush=True)

for versuch in range(12):
    if menue_offen():
        print("[%.0fs] Menue offen nach %d SELECT" % (time.time() - t0, versuch), flush=True)
        break
    tap(gp, B.XUSB_GAMEPAD_BACK, hold=0.25, gap=1.5)
    time.sleep(2.0)

while time.time() - t0 < MAXS:
    time.sleep(5)
    if fertig():
        print("[%.0fs] Messung fertig" % (time.time() - t0), flush=True)
        break

try:
    proc.terminate()
except Exception:
    pass
print("Ausgabe:", OUT, flush=True)
