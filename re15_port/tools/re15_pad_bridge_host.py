#!/usr/bin/env python3
"""Pad-Bruecke, HOST-Seite (Windows, dort wo re15_pc.exe laeuft).

WOZU: RDP leitet Tastatur, Maus, Audio und Laufwerke weiter — generische HID-Gamepads NICHT.
Wer den Port ueber Remotedesktop bedient (z.B. vom Steam Deck aus), hat sein Pad am CLIENT
stecken; SDL auf dem Host sieht gar kein Geraet. Diese Bruecke nimmt den Pad-Zustand ueber TCP
entgegen und legt ihn hier auf ein VIRTUELLES Xbox-360-Pad (ViGEm/vgamepad). Das sieht SDL wie
einen echten Controller, und die vorhandene Zuordnung in input_pc.c greift unveraendert.

UEBER DIE LEITUNG geht das PSX-Pad-Wort (16 Bit, u16 little-endian) — das kanonische Format
dieses Projekts. Bitbelegung == PSX-Hardware, psx-spx controllersandmemorycards.md:406-421:
    Bit 0 Select | 1 L3 | 2 R3 | 3 Start | 4 Hoch | 5 Rechts | 6 Runter | 7 Links
        8 L2 | 9 R2 | 10 L1 | 11 R1 | 12 Dreieck | 13 Kreis | 14 Kreuz | 15 Quadrat

START:
    python re15_port/tools/re15_pad_bridge_host.py            # lauscht auf 0.0.0.0:27615
    python re15_port/tools/re15_pad_bridge_host.py --port 5000

Dann auf dem Client re15_pad_bridge_client.py starten. Danach re15_pc.exe starten (Reihenfolge
egal, das Pad wird per Hotplug erkannt) und mit RE15_PAD_DEBUG=1 gegenpruefen.

Voraussetzung: ViGEm-Bus-Treiber + `pip install vgamepad`.
"""
import argparse
import socket
import struct
import sys

PSX = {                      # Name -> Bit, exakt die Hardware-Belegung
    "SELECT": 0x0001, "L3": 0x0002, "R3": 0x0004, "START": 0x0008,
    "UP": 0x0010, "RIGHT": 0x0020, "DOWN": 0x0040, "LEFT": 0x0080,
    "L2": 0x0100, "R2": 0x0200, "L1": 0x0400, "R1": 0x0800,
    "TRIANGLE": 0x1000, "CIRCLE": 0x2000, "CROSS": 0x4000, "SQUARE": 0x8000,
}
DEFAULT_PORT = 27615


def build_maps(vg):
    b = vg.XUSB_BUTTON
    # POSITIONSGLEICHE Rueckabbildung: SDL macht daraus wieder genau dieselben PSX-Bits
    # (A unten = Kreuz, B rechts = Kreis, X links = Quadrat, Y oben = Dreieck).
    btn = {
        PSX["CROSS"]:    b.XUSB_GAMEPAD_A,
        PSX["CIRCLE"]:   b.XUSB_GAMEPAD_B,
        PSX["SQUARE"]:   b.XUSB_GAMEPAD_X,
        PSX["TRIANGLE"]: b.XUSB_GAMEPAD_Y,
        PSX["SELECT"]:   b.XUSB_GAMEPAD_BACK,
        PSX["START"]:    b.XUSB_GAMEPAD_START,
        PSX["L1"]:       b.XUSB_GAMEPAD_LEFT_SHOULDER,
        PSX["R1"]:       b.XUSB_GAMEPAD_RIGHT_SHOULDER,
        PSX["L3"]:       b.XUSB_GAMEPAD_LEFT_THUMB,
        PSX["R3"]:       b.XUSB_GAMEPAD_RIGHT_THUMB,
        PSX["UP"]:       b.XUSB_GAMEPAD_DPAD_UP,
        PSX["DOWN"]:     b.XUSB_GAMEPAD_DPAD_DOWN,
        PSX["LEFT"]:     b.XUSB_GAMEPAD_DPAD_LEFT,
        PSX["RIGHT"]:    b.XUSB_GAMEPAD_DPAD_RIGHT,
    }
    return btn


def names(word):
    return "+".join(n for n, bit in sorted(PSX.items(), key=lambda kv: kv[1]) if word & bit) or "-"


def serve(port, verbose):
    try:
        import vgamepad as vg
    except ImportError:
        sys.exit("vgamepad fehlt. Installieren: pip install vgamepad "
                 "(braucht den ViGEm-Bus-Treiber)")

    pad = vg.VX360Gamepad()
    btn_map = build_maps(vg)
    print("[bridge-host] virtuelles Xbox-360-Pad angelegt (ViGEm)")

    srv = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    srv.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    srv.bind(("0.0.0.0", port))
    srv.listen(1)
    print("[bridge-host] warte auf Client an Port %d ..." % port)

    while True:
        conn, addr = srv.accept()
        conn.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
        print("[bridge-host] verbunden: %s:%d" % addr)
        last = None
        try:
            buf = b""
            while True:
                chunk = conn.recv(256)
                if not chunk:
                    break
                buf += chunk
                # nur das ZULETZT empfangene Wort anwenden — bei Stau nicht hinterherhinken
                while len(buf) >= 2:
                    word = struct.unpack_from("<H", buf, 0)[0]
                    buf = buf[2:]
                    if word == last:
                        continue
                    last = word
                    pad.reset()
                    for bit, xb in btn_map.items():
                        if word & bit:
                            pad.press_button(button=xb)
                    pad.left_trigger(value=255 if word & PSX["L2"] else 0)
                    pad.right_trigger(value=255 if word & PSX["R2"] else 0)
                    pad.update()
                    if verbose:
                        print("[bridge-host] %04X %s" % (word, names(word)))
        except (ConnectionResetError, OSError) as e:
            print("[bridge-host] Verbindung weg: %s" % e)
        finally:
            conn.close()
            pad.reset()
            pad.update()
            print("[bridge-host] Pad neutralisiert, warte erneut ...")


if __name__ == "__main__":
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--port", type=int, default=DEFAULT_PORT)
    ap.add_argument("-v", "--verbose", action="store_true", help="jede Aenderung ausgeben")
    a = ap.parse_args()
    try:
        serve(a.port, a.verbose)
    except KeyboardInterrupt:
        print("\n[bridge-host] beendet")
