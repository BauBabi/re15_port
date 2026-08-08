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
import subprocess, time, os, shutil

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


def konfig_zuruecksetzen():
    """Jeden Lauf mit einer BEKANNT GUTEN pcsx.json beginnen.

    ⛔ Ein hartes Beenden von pcsx-redux hinterlaesst eine pcsx.json, mit der der Emulator beim
    naechsten Mal in einem Fenster mit dem Titel "Error" haengenbleibt: kein Lua, keine
    Ausgabedatei, keine Fehlermeldung. Da der Treiber am Ende selbst hart beendet, war das ein
    selbstgebauter Kreislauf — mehrfach erlebt. Deshalb hier deterministisch zuruecksetzen.
    """
    gut = os.path.join(os.path.dirname(os.path.abspath(__file__)), "pcsx.json.good")
    ziel = os.path.join(os.environ.get("APPDATA", ""), "pcsx-redux", "pcsx.json")
    try:
        if os.path.exists(gut) and os.path.isdir(os.path.dirname(ziel)):
            shutil.copyfile(gut, ziel)
            print("pcsx.json auf den bekannt guten Stand gesetzt", flush=True)
    except Exception as e:
        print("Konfig-Reset fehlgeschlagen:", e, flush=True)


def aufraeumen():
    """Erst hoeflich, dann hart — und den RICHTIGEN Prozessnamen treffen."""
    for name in ("pcsx-redux.main", "pcsx-redux.exe"):
        subprocess.run(["taskkill", "/IM", name, "/F"],
                       stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)


aufraeumen()          # Leichen frueherer Laeufe blockieren sonst den Start
konfig_zuruecksetzen()

gp = vg.VX360Gamepad(); gp.update(); time.sleep(1.0)
print("vgamepad erzeugt", flush=True)
proc = subprocess.Popen([EXE, "-bios", BIOS, "-iso", ISO, "-interpreter", "-debugger",
                         "-run", "-dofile", LUA, "-stdout", "-lua_stdout"],
                        stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
print("pcsx-redux gestartet, pid", proc.pid, flush=True)

t0 = time.time()
time.sleep(BOOT)

def spieler_da():
    # BEIDE Schreibweisen erkennen. Vorher stand hier nur "SPIELER existiert"; Sonden, die
    # "SPIELER ab Bild" schreiben, liessen die Warteschleife bis zum Zeitlimit laufen — und die
    # tippt alle 18 s weiter "Neues Spiel" (HOCH/A). Diese Tastendruecke liefen dann MITTEN in
    # die Messung hinein und haben sie verfaelscht.
    try:
        with open(OUT, encoding="utf-8", errors="replace") as f:
            t = f.read()
            return "SPIELER existiert" in t or "SPIELER ab Bild" in t
    except Exception:
        return False


def menue_offen():
    # ⛔ GROSS-/KLEINSCHREIBUNG EGAL. Vorher stand hier nur "MENUE offen"; eine Sonde, die
    # "MENUE OFFEN" schreibt, wurde nicht erkannt -> der Treiber tippte weiter BACK (= SELECT),
    # und JEDE dieser Flanken setzt DAT_8008f618 = 2 (FUN_8001443c.c Zeile 23), was das Menue
    # ueber @0x80014… wieder SCHLIESST. Genau daran ist der Sprung nach ROOM1030 gescheitert.
    try:
        with open(OUT, encoding="utf-8", errors="replace") as f:
            t = f.read().upper()
            return "MENUE OFFEN" in t or "*** OFFEN" in t
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

# ⛔ proc.terminate() beendet nur den winzigen STARTER pcsx-redux.exe. Der eigentliche Emulator
# heisst "pcsx-redux.main" — OHNE .exe — und ueberlebt. Ueber mehrere Laeufe haben sich so sechs
# Leichen angesammelt (je ~128 KB, direkt beim Start blockiert); danach startete gar nichts mehr
# und PCSX zeigte nur noch ein Fenster mit dem Titel "Error". Deshalb hier hart nachraeumen.
# Gnadenfrist: die Lua-Sonden beenden sich selbst per PCSX.quit(0) und schreiben dabei eine
# saubere pcsx.json. Nur wer dann noch lebt, wird hart beendet.
for _ in range(6):
    time.sleep(1)
    if subprocess.run(["tasklist", "/FI", "IMAGENAME eq pcsx-redux.main", "/NH"],
                      capture_output=True, text=True).stdout.find("pcsx-redux") < 0:
        break
aufraeumen()
print("Ausgabe:", OUT, flush=True)
