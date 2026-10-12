#!/usr/bin/env python3
"""port_ruhe_zensus.py - Runde 36 G2 (P1/Zensus): faehrt eine Auswahl von ctest-Tests EINZELN mit eigenem
RE15_PUSH_LOG (Messschrieb dachkiste_r36.c: je Bewegung des Ruhe-Zweigs eine Zeile "RUHE-AUSSCHIEBUNG" mit Raum,
Pool-Platz, obj, Lage vorher/nachher und den getroffenen Zellen) und fasst zusammen, in welchen Tests/Raeumen der
Ruhe-Zweig (zellschub_r36.c re15_zellschub_ruhe = LAB_8002c0b8 FUN_8003b558(obj,2) @0x8002c0d0) ein Objekt bewegt hat.
Kein Verhalten: RE15_PUSH_LOG ist reine Diagnose.
Aufruf: python port_ruhe_zensus.py <ctest.json (ctest --show-only=json-v1)> <regex> <ausgabeordner> [jobs]"""
import concurrent.futures, json, os, re, subprocess, sys, time


def main():
    meta = json.load(open(sys.argv[1]))
    rx = re.compile(sys.argv[2], re.I)
    out = os.path.abspath(sys.argv[3])
    jobs = int(sys.argv[4]) if len(sys.argv) > 4 else 4
    os.makedirs(out, exist_ok=True)
    sel = [t for t in meta["tests"] if rx.search(t["name"]) and t.get("command")]
    env0 = dict(os.environ)
    env0["PATH"] = r"C:\msys64\mingw64\bin;" + env0.get("PATH", "")
    env0["SDL_ASSERT"] = "always_ignore"

    def lauf(t):
        name = t["name"]
        props = {p["name"]: p["value"] for p in t.get("properties", [])}
        cwd = props.get("WORKING_DIRECTORY") or os.path.dirname(t["command"][0])
        env = dict(env0)
        for kv in props.get("ENVIRONMENT", []) or []:
            k, _, v = kv.partition("=")
            env[k] = v
        log = os.path.join(out, name + ".push.txt")
        if os.path.exists(log):
            os.remove(log)
        env["RE15_PUSH_LOG"] = log
        t0 = time.time()
        try:
            r = subprocess.run(t["command"], cwd=cwd, env=env, capture_output=True, timeout=900)
            rc = r.returncode
        except subprocess.TimeoutExpired:
            rc = "TIMEOUT"
        return name, rc, time.time() - t0, log

    ergebnisse = []
    with concurrent.futures.ThreadPoolExecutor(max_workers=jobs) as ex:
        for name, rc, dt, log in ex.map(lauf, sel):
            ruhe = []
            if os.path.exists(log):
                for l in open(log, encoding="utf-8", errors="replace"):
                    if "RUHE-AUSSCHIEBUNG" in l:
                        ruhe.append(l.strip())
            ergebnisse.append((name, rc, dt, ruhe))
            print("%-45s rc=%s %6.1fs Ruhe-Bewegungen %d" % (name, rc, dt, len(ruhe)), flush=True)
    with open(os.path.join(out, "zusammenfassung.txt"), "w", encoding="utf-8") as f:
        f.write("Tests: %d (Regex %s)\n" % (len(ergebnisse), sys.argv[2]))
        for name, rc, dt, ruhe in sorted(ergebnisse):
            f.write("%-45s rc=%s Ruhe-Bewegungen %d\n" % (name, rc, len(ruhe)))
            arten = {}
            for l in ruhe:
                m = re.search(r"room=(\w+) p=(\d+) obj=(\d+) RUHE-AUSSCHIEBUNG lage=\((-?\d+),(-?\d+)\) -> \((-?\d+),(-?\d+)\)", l)
                if m:
                    k = (m.group(1), m.group(3), m.group(6), m.group(7))
                    arten[k] = arten.get(k, 0) + 1
            for (room, obj, nx, nz), n in sorted(arten.items()):
                f.write("    ROOM%s obj %s -> Endlage (%s,%s)  %d x\n" % (room, obj, nx, nz, n))


if __name__ == "__main__":
    main()
