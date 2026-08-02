#!/usr/bin/env python3
"""Zensus der Original-Adress-Zitate im Port-Quellcode (reproduzierbar).

Scannt rekursiv re15_port/{engine,platform,include,tools,tests} (nur *.c/*.h,
re15_port/build/ komplett ausgeschlossen) nach Zitaten der Formen
  0x80xxxxxx, FUN_80xxxxxx, DAT_80xxxxxx, LAB_80xxxxxx  (xxxxxx = 6 Hex-Ziffern),
normalisiert auf Kleinbuchstaben, filtert auf 0x80010000..0x801fffff und
schreibt analysis/coverage/cited_addresses.json.

Aufruf:  python analysis/coverage/extract_cited_addresses.py
"""

import json
import os
import re
import sys

REPO_ROOT = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
SCAN_DIRS = [
    os.path.join("re15_port", "engine"),
    os.path.join("re15_port", "platform"),
    os.path.join("re15_port", "include"),
    os.path.join("re15_port", "tools"),
    os.path.join("re15_port", "tests"),
]
EXCLUDE_DIR = os.path.normpath(os.path.join("re15_port", "build"))
OUT_PATH = os.path.join(REPO_ROOT, "analysis", "coverage", "cited_addresses.json")

ADDR_LO = 0x80010000
ADDR_HI = 0x801FFFFF

# Ein Muster pro Zitat-Form. \b-Grenzen verhindern, dass laengere Hex-Literale
# (z.B. 0x80012d60ff) oder laengere Bezeichner faelschlich matchen.
PATTERN = re.compile(
    r"\b(?:(FUN)|(DAT)|(LAB))_(80[0-9a-fA-F]{6})\b"  # FUN_/DAT_/LAB_80xxxxxx
    r"|\b0[xX](80[0-9a-fA-F]{6})\b"                   # rohes 0x80xxxxxx
)


def scan_file(abspath):
    """Liefert set von (addr_lowercase, kind)-Tupeln der Datei."""
    hits = set()
    try:
        with open(abspath, "r", encoding="utf-8", errors="replace") as f:
            text = f.read()
    except OSError as e:
        print(f"WARN: kann {abspath} nicht lesen: {e}", file=sys.stderr)
        return hits
    for m in PATTERN.finditer(text):
        if m.group(5):  # rohes 0x-Literal
            addr, kind = m.group(5), "raw"
        else:
            addr = m.group(4)
            kind = "fun" if m.group(1) else ("dat" if m.group(2) else "lab")
        addr = addr.lower()
        val = int(addr, 16)
        if ADDR_LO <= val <= ADDR_HI:
            hits.add((addr, kind))
    return hits


def main():
    addresses = {}  # addr -> {"kinds": set, "files": set, "count": int}
    per_file = {}   # relpath -> unique addr count

    for scan_dir in SCAN_DIRS:
        root_abs = os.path.join(REPO_ROOT, scan_dir)
        if not os.path.isdir(root_abs):
            print(f"WARN: Verzeichnis fehlt: {root_abs}", file=sys.stderr)
            continue
        for dirpath, dirnames, filenames in os.walk(root_abs):
            rel_dir = os.path.relpath(dirpath, REPO_ROOT)
            # re15_port/build/ komplett ausschliessen
            dirnames[:] = [
                d for d in dirnames
                if not os.path.normpath(os.path.join(rel_dir, d)).lower().startswith(EXCLUDE_DIR.lower())
            ]
            if os.path.normpath(rel_dir).lower().startswith(EXCLUDE_DIR.lower()):
                continue
            for fname in sorted(filenames):
                if not fname.lower().endswith((".c", ".h")):
                    continue
                abspath = os.path.join(dirpath, fname)
                relpath = os.path.relpath(abspath, REPO_ROOT).replace("\\", "/")
                hits = scan_file(abspath)
                if not hits:
                    continue
                uniq_addrs = set()
                for addr, kind in hits:
                    uniq_addrs.add(addr)
                    entry = addresses.setdefault(addr, {"kinds": set(), "files": set(), "count": 0})
                    entry["kinds"].add(kind)
                    entry["files"].add(relpath)
                per_file[relpath] = len(uniq_addrs)

    # count = Anzahl Dateien, in denen die Adresse zitiert wird
    out_addresses = {}
    for addr in sorted(addresses):
        e = addresses[addr]
        out_addresses[addr] = {
            "kinds": sorted(e["kinds"]),
            "files": sorted(e["files"]),
            "count": len(e["files"]),
        }

    result = {
        "addresses": out_addresses,
        "per_file": {k: per_file[k] for k in sorted(per_file)},
        "total_unique": len(out_addresses),
    }

    os.makedirs(os.path.dirname(OUT_PATH), exist_ok=True)
    with open(OUT_PATH, "w", encoding="utf-8") as f:
        json.dump(result, f, indent=2)
        f.write("\n")

    top5 = sorted(per_file.items(), key=lambda kv: (-kv[1], kv[0]))[:5]
    print(f"total_unique: {result['total_unique']}")
    print(f"files_with_citations: {len(per_file)}")
    print("top5_files_by_unique:")
    for relpath, n in top5:
        print(f"  {n:5d}  {relpath}")
    print(f"output: {OUT_PATH}")


if __name__ == "__main__":
    main()
