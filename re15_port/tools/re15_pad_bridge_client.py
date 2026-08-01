#!/usr/bin/env python3
"""Pad-Bruecke, CLIENT-Seite (Linux / Steam Deck, dort wo der Controller steckt).

Liest das lokale Gamepad und schickt den Zustand als PSX-Pad-Wort (u16 little-endian) an
re15_pad_bridge_host.py auf dem Windows-Rechner, der re15_pc.exe ausfuehrt. Hintergrund: RDP
leitet Gamepads nicht weiter, ein am Client steckendes Pad ist auf dem Host unsichtbar.

BEWUSST OHNE FREMDPAKETE: Standardweg ist die Legacy-Joystick-Schnittstelle /dev/input/js*
(8-Byte-Ereignisse, in jedem Linux-Kernel vorhanden, auf SteamOS ohne Installation nutzbar).
Wer lieber SDL nimmt: --backend pygame (dann ist `pip install pygame` noetig).

START (Beispiel):
    python3 re15_pad_bridge_client.py --host 192.168.1.50
    python3 re15_pad_bridge_client.py --host 192.168.1.50 --device /dev/input/js1 --list

STEAM DECK: In den Eigenschaften der Verknuepfung "Steam Input" fuer das RDP-Fenster AUSschalten
bzw. das Layout auf "Gamepad" stellen — sonst uebersetzt Steam die Deck-Tasten schon vorher in
Maus/Tastatur und /dev/input/js* sieht nichts mehr.

TASTEN-ZUORDNUNG (Xbox-Layout am Client -> PSX, positionsgleich wie im Port):
    A unten -> Kreuz | B rechts -> Kreis | X links -> Quadrat | Y oben -> Dreieck
    LB/RB -> L1/R1 | LT/RT -> L2/R2 | Back/Start -> Select/Start | Stickklicks -> L3/R3
    D-Pad und linker Stick -> die vier Richtungen
Stimmt die Reihenfolge deiner Tasten nicht, zeigt --list die rohen Nummern; dann per
--btnmap "0=CROSS,1=CIRCLE,..." korrigieren.
"""
import argparse
import os
import socket
import struct
import sys
import time

PSX = {
    "SELECT": 0x0001, "L3": 0x0002, "R3": 0x0004, "START": 0x0008,
    "UP": 0x0010, "RIGHT": 0x0020, "DOWN": 0x0040, "LEFT": 0x0080,
    "L2": 0x0100, "R2": 0x0200, "L1": 0x0400, "R1": 0x0800,
    "TRIANGLE": 0x1000, "CIRCLE": 0x2000, "CROSS": 0x4000, "SQUARE": 0x8000,
}
DEFAULT_PORT = 27615

# Reihenfolge der Tasten im Linux-Joystick-Treiber fuer XInput-artige Pads (xpad/hid-steam).
DEFAULT_BTNMAP = {
    0: "CROSS", 1: "CIRCLE", 2: "SQUARE", 3: "TRIANGLE",
    4: "L1", 5: "R1", 6: "SELECT", 7: "START",
    8: "L3", 9: "R3",          # manche Treiber legen hier stattdessen "Guide" ab
}
# Achsen: 0/1 linker Stick, 2 LT, 5 RT, 6/7 D-Pad (xpad-Standard)
AX_LX, AX_LY, AX_LT, AX_RT, AX_DX, AX_DY = 0, 1, 2, 5, 6, 7
DEADZONE = 13000            # wie im Port (input_pc.c)
TRIGGER_ON = 0              # LT/RT liegen bei -32768..32767, Mitte 0 = halb gedrueckt


def names(word):
    return "+".join(n for n, b in sorted(PSX.items(), key=lambda kv: kv[1]) if word & b) or "-"


def parse_btnmap(s):
    m = dict(DEFAULT_BTNMAP)
    if not s:
        return m
    for part in s.split(","):
        part = part.strip()
        if not part:
            continue
        k, _, v = part.partition("=")
        v = v.strip().upper()
        if v not in PSX:
            sys.exit("unbekannter Tastenname in --btnmap: %r" % v)
        m[int(k)] = v
    return m


def run_js(dev, sock, btnmap, verbose, list_only):
    """Legacy-Joystick-API: je Ereignis 8 Byte = u32 time, i16 value, u8 type, u8 number."""
    EV_BUTTON, EV_AXIS, EV_INIT = 0x01, 0x02, 0x80
    btn_state, ax_state = {}, {}
    word = last_sent = 0
    with open(dev, "rb") as f:
        while True:
            data = f.read(8)
            if not data or len(data) < 8:
                break
            _t, value, etype, number = struct.unpack("<IhBB", data)
            etype &= ~EV_INIT                      # Init-Flut beim Oeffnen mitnehmen
            if etype == EV_BUTTON:
                btn_state[number] = value
                if list_only:
                    print("Taste %d = %d" % (number, value))
            elif etype == EV_AXIS:
                ax_state[number] = value
                if list_only and abs(value) > 20000:
                    print("Achse %d = %d" % (number, value))
            if list_only:
                continue

            word = 0
            for num, val in btn_state.items():
                if val and num in btnmap:
                    word |= PSX[btnmap[num]]
            lx, ly = ax_state.get(AX_LX, 0), ax_state.get(AX_LY, 0)
            if ly < -DEADZONE: word |= PSX["UP"]
            if ly > DEADZONE:  word |= PSX["DOWN"]
            if lx < -DEADZONE: word |= PSX["LEFT"]
            if lx > DEADZONE:  word |= PSX["RIGHT"]
            dx, dy = ax_state.get(AX_DX, 0), ax_state.get(AX_DY, 0)
            if dy < -DEADZONE: word |= PSX["UP"]
            if dy > DEADZONE:  word |= PSX["DOWN"]
            if dx < -DEADZONE: word |= PSX["LEFT"]
            if dx > DEADZONE:  word |= PSX["RIGHT"]
            if ax_state.get(AX_LT, -32768) > TRIGGER_ON: word |= PSX["L2"]
            if ax_state.get(AX_RT, -32768) > TRIGGER_ON: word |= PSX["R2"]

            if word != last_sent:
                last_sent = word
                sock.sendall(struct.pack("<H", word))
                if verbose:
                    print("%04X %s" % (word, names(word)))


def run_pygame(sock, verbose):
    import pygame
    pygame.init()
    pygame.joystick.init()
    if pygame.joystick.get_count() == 0:
        sys.exit("pygame sieht kein Joystick-Geraet")
    js = pygame.joystick.Joystick(0)
    js.init()
    print("[bridge-client] pygame: %s" % js.get_name())
    last = None
    clock = pygame.time.Clock()
    while True:
        pygame.event.pump()
        word = 0
        order = ["CROSS", "CIRCLE", "SQUARE", "TRIANGLE", "L1", "R1", "SELECT", "START", "L3", "R3"]
        for i, nm in enumerate(order):
            if i < js.get_numbuttons() and js.get_button(i):
                word |= PSX[nm]
        if js.get_numaxes() > 1:
            lx, ly = js.get_axis(0), js.get_axis(1)
            if ly < -0.4: word |= PSX["UP"]
            if ly > 0.4:  word |= PSX["DOWN"]
            if lx < -0.4: word |= PSX["LEFT"]
            if lx > 0.4:  word |= PSX["RIGHT"]
        if js.get_numhats() > 0:
            hx, hy = js.get_hat(0)
            if hy > 0:  word |= PSX["UP"]
            if hy < 0:  word |= PSX["DOWN"]
            if hx < 0:  word |= PSX["LEFT"]
            if hx > 0:  word |= PSX["RIGHT"]
        if word != last:
            last = word
            sock.sendall(struct.pack("<H", word))
            if verbose:
                print("%04X %s" % (word, names(word)))
        clock.tick(120)


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--host", required=False, help="IP des Windows-Rechners mit re15_pc.exe")
    ap.add_argument("--port", type=int, default=DEFAULT_PORT)
    ap.add_argument("--device", default="/dev/input/js0")
    ap.add_argument("--backend", choices=("js", "pygame"), default="js")
    ap.add_argument("--list", action="store_true",
                    help="nur die rohen Tasten-/Achsennummern zeigen (zum Zuordnen), nichts senden")
    ap.add_argument("--btnmap", help='z.B. "0=CROSS,1=CIRCLE,2=SQUARE,3=TRIANGLE"')
    ap.add_argument("-v", "--verbose", action="store_true")
    a = ap.parse_args()

    if a.list:
        if a.backend != "js":
            sys.exit("--list gibt es nur fuer das js-Backend")
        if not os.path.exists(a.device):
            sys.exit("kein %s — steckt das Pad? (ls /dev/input/js*)" % a.device)
        print("Tasten druecken; Abbruch mit Strg-C")
        run_js(a.device, None, {}, False, True)
        return

    if not a.host:
        sys.exit("--host fehlt (IP des Windows-Rechners)")

    while True:                                  # bei Abbruch neu verbinden
        try:
            s = socket.create_connection((a.host, a.port), timeout=5)
            s.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
            print("[bridge-client] verbunden mit %s:%d" % (a.host, a.port))
            if a.backend == "pygame":
                run_pygame(s, a.verbose)
            else:
                if not os.path.exists(a.device):
                    sys.exit("kein %s — steckt das Pad? (ls /dev/input/js*)\n"
                             "Auf dem Steam Deck: Steam Input fuer das RDP-Fenster auf 'Gamepad' "
                             "stellen, sonst werden die Tasten vorher zu Maus/Tastatur." % a.device)
                run_js(a.device, s, parse_btnmap(a.btnmap), a.verbose, False)
        except KeyboardInterrupt:
            print("\n[bridge-client] beendet")
            return
        except Exception as e:
            print("[bridge-client] %s — neuer Versuch in 2 s" % e)
            time.sleep(2)


if __name__ == "__main__":
    main()
