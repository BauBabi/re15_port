#!/usr/bin/env python3
"""Rebaut das RE1.5-(MZD)-CD-Image aus dem gerippten Baum in info/Re1.5/.

Quelle:  info/Re1.5/re15.cat   (psximager/psxrip-Katalog: Volume-Deskriptor + Dateibaum)
         info/Re1.5/           (der Dateibaum selbst, Disc-Root)
         info/Re1.5/re15.sys   (System-Area == Payload von PSYQ LICENSEJ.DAT, byte-identisch)

Der Katalog wird 1:1 in ein mkpsxiso-XML-Projekt uebersetzt (Reihenfolge = Katalog-
Reihenfolge) und mit mkpsxiso gebaut.  Ergebnis: <out>/re15.bin + re15.cue.

Aufruf:  python3 scripts/build_re15_iso.py [--out info/re15] [--xml-only]
"""
import argparse
import os
import re
import shutil
import subprocess
import sys
from xml.sax.saxutils import escape

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CAT = os.path.join(REPO, "info", "Re1.5", "re15.cat")
TREE = os.path.join(REPO, "info", "Re1.5")
# re15.sys ist der reine 2048-B/Sektor-Payload; mkpsxiso will die 2336-B/Sektor-Form
# -> PSYQ LICENSEJ.DAT, dessen Payload byte-identisch zu re15.sys ist (verifiziert).
LICENSE = os.path.join(
    REPO, "info", "Resident_Evil_und_Playstation_Information",
    "PSYQ_SDK", "psyq", "cdgen", "LCNSFILE", "LICENSEJ.DAT")


def parse_catalog(path):
    """-> (volume-dict, root-node).  Node = {'dirs': [(name, node)], 'files': [(name, kind)]}"""
    with open(path, "r", encoding="latin-1") as fh:
        lines = [ln.strip() for ln in fh]

    vol = {}
    root = None
    stack = []
    section = None
    for ln in lines:
        if not ln:
            continue
        if ln in ("system_area {", "volume {"):
            section = ln.split()[0]
            continue
        if section in ("system_area", "volume") and ln == "}":
            section = None
            continue
        if section == "volume":
            m = re.match(r"^(\w+)\s+(?:\[(.*)\]|(.*))$", ln)
            if m:
                vol[m.group(1)] = (m.group(2) if m.group(2) is not None else m.group(3)).strip()
            continue
        if section == "system_area":
            continue

        if ln == "dir {":                      # Disc-Root
            root = {"dirs": [], "files": []}
            stack = [root]
            continue
        m = re.match(r"^dir\s+(\S+)\s*\{$", ln)
        if m:
            node = {"dirs": [], "files": []}
            stack[-1]["dirs"].append((m.group(1), node))
            stack.append(node)
            continue
        if ln == "}":
            stack.pop()
            continue
        m = re.match(r"^(xafile|file)\s+(\S+)$", ln)
        if m:
            stack[-1]["files"].append((m.group(2), "xa" if m.group(1) == "xafile" else "data"))
            continue
        raise SystemExit(f"unbekannte Katalog-Zeile: {ln!r}")
    if root is None:
        raise SystemExit("kein Root-'dir {' im Katalog")
    return vol, root


def iso_date(s):
    """'2023-07-14 22:45:32.00 0' -> '2023071422453200'"""
    m = re.match(r"(\d{4})-(\d{2})-(\d{2})\s+(\d{2}):(\d{2}):(\d{2})\.(\d{2})", s or "")
    return "".join(m.groups()) if m else None


def emit(node, relpath, out, indent):
    pad = "  " * indent
    for name, kind in node["files"]:
        src = os.path.join(TREE, relpath, name)
        if not os.path.isfile(src):
            raise SystemExit(f"FEHLT im Baum: {os.path.relpath(src, REPO)}")
        t = ' type="xa"' if kind == "xa" else ""
        out.append(f'{pad}<file name="{escape(name)}"{t} source="{escape(src)}"/>')
    for name, child in node["dirs"]:
        out.append(f'{pad}<dir name="{escape(name)}">')
        emit(child, os.path.join(relpath, name), out, indent + 1)
        out.append(f"{pad}</dir>")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--out", default=os.path.join(REPO, "info", "re15"))
    ap.add_argument("--xml-only", action="store_true")
    args = ap.parse_args()

    outdir = os.path.abspath(args.out)
    os.makedirs(outdir, exist_ok=True)
    vol, root = parse_catalog(CAT)

    ids = {
        "system": vol.get("system_id", "PLAYSTATION"),
        "application": vol.get("application_id", "PLAYSTATION"),
        "volume": vol.get("volume_id", "MZD"),
        "volume_set": vol.get("volume_set_id", ""),
        "publisher": vol.get("publisher_id", ""),
        "data_preparer": vol.get("preparer_id", ""),
        "copyright": vol.get("copyright_file_id", ""),
    }
    for key in ("creation_date", "modification_date"):
        d = iso_date(vol.get(key))
        if d:
            ids[key] = d

    attrs = " ".join(f'{k}="{escape(v)}"' for k, v in ids.items() if v)
    xml = [
        '<?xml version="1.0" encoding="UTF-8"?>',
        f'<iso_project image_name="{escape(os.path.join(outdir, "re15.bin"))}"'
        f' cue_sheet="{escape(os.path.join(outdir, "re15.cue"))}">',
        '  <track type="data">',
        f"    <identifiers {attrs}/>",
        f'    <license file="{escape(LICENSE)}"/>',
        "    <directory_tree>",
    ]
    emit(root, "", xml, 3)
    xml += ["    </directory_tree>", "  </track>", "</iso_project>", ""]

    xml_path = os.path.join(outdir, "re15_iso.xml")
    with open(xml_path, "w", encoding="utf-8") as fh:
        fh.write("\n".join(xml))
    print(f"XML geschrieben: {xml_path}")
    if args.xml_only:
        return

    mk = shutil.which("mkpsxiso") or os.path.expanduser(
        "~/.local/PSn00bSDK-0.24-Linux/bin/mkpsxiso")
    if not os.path.isfile(mk):
        raise SystemExit("mkpsxiso nicht gefunden")
    sys.exit(subprocess.call([mk, "-y", xml_path]))


if __name__ == "__main__":
    main()
