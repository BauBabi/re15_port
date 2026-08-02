#!/usr/bin/env python3
"""Generiert die Gesamt-Coverage-Uebersicht RE15_ADDRESS_COVERAGE.md (Repo-Root)
plus die Rohdaten analysis/coverage/coverage_result.json.

Eingaben (alle unter analysis/coverage/):
  cited_addresses.json           -- Zensus der @0x80xxxxxx-Zitate im Port-Quellcode
                                    (erzeugt von extract_cited_addresses.py)
  original_exe_inventory.json    -- exe_functions (FUN_-Starts der PSX.EXE-Decompiles),
                                    named_functions (PsyQ-/benannte Symbole),
                                    catalog (Adresse -> Subsystem-Kontext aus RE15_FUN_CATALOG.md)
  original_overlay_inventory.json-- stage1..stage6: FUN_-Starts der Stage-Overlay-Decompiles

Zuordnungslogik (bewusst einfach und dokumentiert, damit reproduzierbar):
  * Eine zitierte Adresse mit kind 'dat' (mindestens ein DAT_-Zitat) gilt als
    Daten-Global: sie wird SEPARAT gezaehlt und NICHT auf Funktionen gemappt --
    auch dann, wenn dieselbe Adresse zusaetzlich als rohes 0x-Literal zitiert wird.
  * Alle uebrigen zitierten Adressen (fun/lab/raw) werden auf Funktionsintervalle
    gemappt: Funktion mit Start S gilt als ZITIERT, wenn eine zitierte Adresse A
    S <= A < min(naechster_Start, S + 0x4000) erfuellt. Der 0x4000-Deckel schuetzt
    gegen Riesen-Luecken zwischen den Decompile-Starts (sonst wuerde eine Funktion
    Zitate aus kilometerweit entferntem, nie decompiliertem Code "erben").
  * EXE-Mapping nur fuer Adressen < 0x80100000, Overlay-Mapping nur >= 0x80100000.
  * Overlay: die Stages teilen sich den Adressraum @0x80100000 -- eine zitierte
    Adresse wird fuer JEDE Stage gezaehlt, deren Funktionsliste sie abdeckt
    (Mehrdeutigkeit, im Dokument explizit ausgewiesen).

Aufruf:  python analysis/coverage/generate_address_coverage.py
"""

import bisect
import datetime
import json
import os

HERE = os.path.dirname(os.path.abspath(__file__))
REPO_ROOT = os.path.normpath(os.path.join(HERE, "..", ".."))

CITED_PATH = os.path.join(HERE, "cited_addresses.json")
EXE_INV_PATH = os.path.join(HERE, "original_exe_inventory.json")
OVL_INV_PATH = os.path.join(HERE, "original_overlay_inventory.json")
OUT_MD = os.path.join(REPO_ROOT, "RE15_ADDRESS_COVERAGE.md")
OUT_JSON = os.path.join(HERE, "coverage_result.json")

OVERLAY_BASE = 0x80100000
FUNC_SPAN_CAP = 0x4000  # Schutz gegen Riesen-Luecken zwischen Decompile-Starts

# Woertliches Zitat aus lesson.txt (Repo-Root):
LESSON_QUOTE = (
    "Verlange für jede Zahl/jedes Verhalten, das ich in den Port schreibe, "
    "die @0x…-Adresse."
)


def load_json(path):
    with open(path, "r", encoding="utf-8") as f:
        return json.load(f)


def normalize_subsystem(context):
    """Kuerzt einen Katalog-Kontext auf sein Subsystem-Label.

    Schneidet am ersten ' (' (Verifikations-/Datums-Klammer) oder ' — '
    (Gedankenstrich-Zusatz) ab: 'Combat / player damage (verified ...)' ->
    'Combat / player damage'; 'Enemy AI — STAGE1 zombie ...' -> 'Enemy AI'.
    """
    cut = len(context)
    for sep in (" — ", " ("):
        i = context.find(sep)
        if i != -1:
            cut = min(cut, i)
    return context[:cut].strip()


def map_to_functions(starts_sorted, cited_ints):
    """Mappt zitierte Adressen auf Funktionsintervalle.

    starts_sorted: aufsteigend sortierte Funktionsstarts (ints).
    cited_ints:    iterierbare zitierte Adressen (ints).
    Rueckgabe: (per_func: {start: sorted list zitierter Adressen}, unmapped: sorted list)
    Intervall je Funktion i: [start_i, min(start_{i+1}, start_i + FUNC_SPAN_CAP)).
    """
    per_func = {}
    unmapped = []
    n = len(starts_sorted)
    for a in sorted(cited_ints):
        idx = bisect.bisect_right(starts_sorted, a) - 1
        if idx < 0:
            unmapped.append(a)
            continue
        start = starts_sorted[idx]
        nxt = starts_sorted[idx + 1] if idx + 1 < n else None
        end = start + FUNC_SPAN_CAP if nxt is None else min(nxt, start + FUNC_SPAN_CAP)
        if a < end:
            per_func.setdefault(start, []).append(a)
        else:
            unmapped.append(a)
    return per_func, unmapped


def hex8(v):
    return f"{v:08x}"


def compact_hex_block(addrs, per_line=8):
    lines = []
    for i in range(0, len(addrs), per_line):
        lines.append("  ".join(hex8(a) for a in addrs[i : i + per_line]))
    return lines


def main():
    cited = load_json(CITED_PATH)
    exe_inv = load_json(EXE_INV_PATH)
    ovl_inv = load_json(OVL_INV_PATH)

    addresses = cited["addresses"]
    total_unique = cited["total_unique"]

    # --- Klassifikation der zitierten Adressen -------------------------------
    dat_addrs = []       # mind. ein DAT_-Zitat -> Daten-Global, nicht gemappt
    func_pool = []       # fun/lab/raw -> Kandidaten fuers Funktions-Mapping
    for a_hex, info in addresses.items():
        a = int(a_hex, 16)
        if "dat" in info["kinds"]:
            dat_addrs.append(a)
        else:
            func_pool.append(a)
    dat_addrs.sort()
    func_pool.sort()
    dat_exe = [a for a in dat_addrs if a < OVERLAY_BASE]
    dat_ovl = [a for a in dat_addrs if a >= OVERLAY_BASE]
    dat_mixed = sorted(
        int(a, 16)
        for a, info in addresses.items()
        if "dat" in info["kinds"] and len(info["kinds"]) > 1
    )
    cites_exe = [a for a in func_pool if a < OVERLAY_BASE]
    cites_ovl = [a for a in func_pool if a >= OVERLAY_BASE]

    # --- (1) EXE-Funktions-Coverage ------------------------------------------
    exe_starts = sorted(int(a, 16) for a in exe_inv["exe_functions"])
    exe_per_func, exe_unmapped = map_to_functions(exe_starts, cites_exe)
    exe_cited = sorted(exe_per_func.keys())
    exe_uncited = [s for s in exe_starts if s not in exe_per_func]

    # --- (2) Overlay-Coverage je Stage ---------------------------------------
    stage_names = sorted(ovl_inv.keys())  # stage1..stage6
    stage_results = {}
    addr_stage_hits = {}  # zitierte Overlay-Adresse -> Liste der Stages, die sie abdecken
    for st in stage_names:
        starts = sorted(int(a, 16) for a in ovl_inv[st])
        per_func, unmapped = map_to_functions(starts, cites_ovl)
        cited_starts = sorted(per_func.keys())
        uncited_starts = [s for s in starts if s not in per_func]
        mapped_addrs = sorted(a for lst in per_func.values() for a in lst)
        for a in mapped_addrs:
            addr_stage_hits.setdefault(a, []).append(st)
        stage_results[st] = {
            "functions_total": len(starts),
            "functions_cited": len(cited_starts),
            "functions_uncited": len(uncited_starts),
            "cited_addresses_mapped": len(mapped_addrs),
            "cited_starts": [hex8(s) for s in cited_starts],
            "uncited_starts": [hex8(s) for s in uncited_starts],
        }
    ovl_addrs_multi_stage = sorted(a for a, sts in addr_stage_hits.items() if len(sts) > 1)
    ovl_addrs_no_stage = sorted(a for a in cites_ovl if a not in addr_stage_hits)

    # --- (3) + (4) Subsystem-Sicht ueber den Katalog -------------------------
    catalog = {int(a, 16): ctx for a, ctx in exe_inv["catalog"].items()}
    # Katalog-Adresse -> EXE-Funktion (gleiche Intervallregel); exakter Start gewinnt.
    func_subsystem = {}
    cat_per_func, _cat_unmapped = map_to_functions(exe_starts, [a for a in catalog if a < OVERLAY_BASE])
    for start, cat_addrs in cat_per_func.items():
        exact = [a for a in cat_addrs if a == start]
        chosen = exact[0] if exact else min(cat_addrs)
        func_subsystem[start] = normalize_subsystem(catalog[chosen].split(" :: ")[0])

    NO_CTX = "(ohne Katalog-Kontext)"
    subsys_rows = {}
    for s in exe_starts:
        name = func_subsystem.get(s, NO_CTX)
        row = subsys_rows.setdefault(name, {"total": 0, "cited": 0, "uncited": 0})
        row["total"] += 1
        if s in exe_per_func:
            row["cited"] += 1
        else:
            row["uncited"] += 1

    # --- Rohdaten-JSON --------------------------------------------------------
    result = {
        "generated": datetime.date.today().isoformat(),
        "generator": "analysis/coverage/generate_address_coverage.py",
        "inputs": {
            "cited_addresses": os.path.relpath(CITED_PATH, REPO_ROOT).replace("\\", "/"),
            "original_exe_inventory": os.path.relpath(EXE_INV_PATH, REPO_ROOT).replace("\\", "/"),
            "original_overlay_inventory": os.path.relpath(OVL_INV_PATH, REPO_ROOT).replace("\\", "/"),
        },
        "rules": {
            "func_span_cap": hex(FUNC_SPAN_CAP),
            "overlay_base": hex(OVERLAY_BASE),
            "dat_rule": "kind 'dat' vorhanden -> Daten-Global, nicht auf Funktionen gemappt",
        },
        "totals": {
            "cited_unique_total": total_unique,
            "cited_dat_globals": len(dat_addrs),
            "cited_dat_globals_exe_space": len(dat_exe),
            "cited_dat_globals_overlay_space": len(dat_ovl),
            "cited_dat_also_other_kind": len(dat_mixed),
            "cited_func_pool_exe_space": len(cites_exe),
            "cited_func_pool_overlay_space": len(cites_ovl),
            "exe_functions_total": len(exe_starts),
            "exe_functions_cited": len(exe_cited),
            "exe_functions_uncited": len(exe_uncited),
            "exe_cited_addresses_mapped": sum(len(v) for v in exe_per_func.values()),
            "exe_cited_addresses_unmapped": len(exe_unmapped),
            "overlay_cited_addresses_multi_stage": len(ovl_addrs_multi_stage),
            "overlay_cited_addresses_no_stage": len(ovl_addrs_no_stage),
            "named_functions_in_inventory": len(exe_inv.get("named_functions", [])),
        },
        "exe": {
            "cited_starts": [hex8(s) for s in exe_cited],
            "uncited_starts": [hex8(s) for s in exe_uncited],
            "unmapped_cited_addresses": [hex8(a) for a in exe_unmapped],
            "per_function_cite_counts": {hex8(s): len(v) for s, v in sorted(exe_per_func.items())},
            "per_function_subsystem": {hex8(s): func_subsystem.get(s, NO_CTX) for s in exe_starts},
        },
        "overlays": stage_results,
        "overlay_ambiguity": {
            "multi_stage_addresses": [hex8(a) for a in ovl_addrs_multi_stage],
            "no_stage_addresses": [hex8(a) for a in ovl_addrs_no_stage],
        },
        "dat_globals": {
            "exe_space": [hex8(a) for a in dat_exe],
            "overlay_space": [hex8(a) for a in dat_ovl],
        },
        "subsystems": subsys_rows,
    }
    with open(OUT_JSON, "w", encoding="utf-8") as f:
        json.dump(result, f, indent=2, ensure_ascii=False)
        f.write("\n")

    # --- Markdown -------------------------------------------------------------
    t = result["totals"]
    exe_pct = 100.0 * t["exe_functions_cited"] / t["exe_functions_total"]
    md = []
    w = md.append
    w("# RE1.5-Port — Adress-Coverage-Übersicht (zitierte Original-Adressen)")
    w("")
    w(f"**Stand: {result['generated']}** — generiert von `analysis/coverage/generate_address_coverage.py`")
    w("aus `analysis/coverage/cited_addresses.json` (Zensus des Port-Quellcodes via")
    w("`extract_cited_addresses.py`), `original_exe_inventory.json` und `original_overlay_inventory.json`.")
    w("Rohdaten: `analysis/coverage/coverage_result.json`.")
    w("")
    w("> Arbeitsregel aus `lesson.txt` (wörtlich): *„" + LESSON_QUOTE + "“*")
    w(">")
    w("> Dieses Dokument ist die quantitative Gegenprobe zu dieser Regel: Es zählt, welche")
    w("> Original-Adressen der Port tatsächlich als Beleg zitiert — und welche Original-Funktionen")
    w("> noch ohne jedes Adress-Zitat sind.")
    w("")
    w("Verwandte Dokumente: [`RE15_FUN_COVERAGE.md`](RE15_FUN_COVERAGE.md) (qualitative Sicht:")
    w("was die Funktionen tun und wie gut sie verstanden sind) und")
    w("[`RE15_FUN_CATALOG.md`](RE15_FUN_CATALOG.md) (kuratierter Katalog code-verifizierter Adressen).")
    w("")
    w("## Methodik & Semantik — was die Zahlen bedeuten (und was NICHT)")
    w("")
    w("**ZITIERT** heißt: Der Port-Quellcode (`re15_port/{engine,platform,include,tools,tests}`,")
    w("nur `*.c`/`*.h`, ohne `build/`) referenziert mindestens eine Adresse im Funktionsintervall")
    w("als `0x80xxxxxx`-, `FUN_`-, `DAT_`- oder `LAB_`-Zitat. Das ist ein **Signal für RE-Abdeckung**")
    w("— die Stelle wurde disassembliert/gelesen und als Beleg in den Code geschrieben — aber")
    w("**KEIN Beweis vollständiger Byte-Treue** der Funktion: Ein einzelnes Zitat kann auch nur eine")
    w("Konstante, einen Teilaspekt oder einen Kommentar-Verweis abdecken.")
    w("")
    w("**NICHT ZITIERT** heißt umgekehrt **nicht zwingend unportiert**: Das Verhalten kann portiert")
    w("sein, ohne dass eine Adresse im Code steht (z.B. über RE2-Referenzen, Format-Doku oder alte")
    w("Commits) — dann fehlt allerdings der Adress-Beleg im Code, und genau das soll diese Liste")
    w("sichtbar machen. Viele nicht-zitierte Funktionen sind außerdem PsyQ-/Lib-Code, toter Code")
    w("oder schlicht noch nicht erreichte Subsysteme.")
    w("")
    w("Zuordnungsregeln (implementiert in `generate_address_coverage.py`):")
    w("")
    w(f"1. **EXE-Funktions-Coverage:** Die {t['exe_functions_total']} `exe_functions`-Starts werden sortiert;")
    w("   eine Funktion mit Start `S` gilt als zitiert, wenn mindestens eine zitierte Adresse `A`")
    w("   `S <= A < min(nächster_Start, S + 0x4000)` erfüllt. Der `0x4000`-Deckel schützt gegen")
    w("   Riesen-Lücken zwischen den Decompile-Starts (ohne ihn würde eine Funktion Zitate aus weit")
    w("   entferntem, nie decompiliertem Code „erben“).")
    w("2. **Overlay-Coverage je Stage:** analog über die Stage-Funktionslisten, nur für zitierte")
    w("   Adressen `>= 0x80100000`. ⚠️ **Mehrdeutigkeit:** Alle Stages teilen sich den Adressraum")
    w("   ab `0x80100000` — eine zitierte Overlay-Adresse kann in **mehreren** Stages in einem")
    w("   Funktionsintervall liegen und wird dann für **jede** dieser Stages gezählt")
    w(f"   ({t['overlay_cited_addresses_multi_stage']} von {t['cited_func_pool_overlay_space']} Overlay-Zitaten treffen mehr als eine Stage).")
    w("   Aus dem Zitat allein ist NICHT entscheidbar, welche Stage gemeint war; die Stage-Spalten")
    w("   sind daher obere Schranken je Stage, keine disjunkte Aufteilung.")
    w("3. **DAT_-Zitate (Daten-Globals):** Adressen, die mindestens einmal als `DAT_…` zitiert werden,")
    w("   zählen als Daten-Globals: separat gezählt, **nicht** auf Funktionen gemappt — auch wenn")
    w(f"   dieselbe Adresse zusätzlich als rohes `0x…`-Literal zitiert wird ({t['cited_dat_also_other_kind']} solcher Mischfälle).")
    w("4. **Subsystem-Sicht:** Katalog-Adressen aus `RE15_FUN_CATALOG.md` (via `original_exe_inventory.json`)")
    w("   werden mit derselben Intervallregel auf EXE-Funktionen gemappt (exakter Start gewinnt, sonst")
    w("   niedrigste Katalog-Adresse im Intervall); der Kontext-Text wird auf sein Subsystem-Label gekürzt.")
    w("")
    w("Nicht Teil des Nenners: die " + str(t["named_functions_in_inventory"]) + " benannten (PsyQ-/SDK-)Symbole aus")
    w("`named_functions` — sie sind Bibliothekscode, keine Spiel-Logik-Decompiles.")
    w("")
    w("## Gesamtzahlen")
    w("")
    w("| Metrik | Wert |")
    w("|---|---|")
    w(f"| Zitierte Original-Adressen im Port (unique) | **{t['cited_unique_total']}** |")
    w(f"| … davon DAT_-Daten-Globals (separat, nicht gemappt) | {t['cited_dat_globals']} (EXE-Raum {t['cited_dat_globals_exe_space']}, Overlay-Raum {t['cited_dat_globals_overlay_space']}) |")
    w(f"| … davon Funktions-Zitate im EXE-Raum (< 0x80100000) | {t['cited_func_pool_exe_space']} |")
    w(f"| … davon Funktions-Zitate im Overlay-Raum (>= 0x80100000) | {t['cited_func_pool_overlay_space']} |")
    w(f"| EXE-Funktionen (Decompile-Inventar) | {t['exe_functions_total']} |")
    w(f"| EXE-Funktionen ZITIERT | **{t['exe_functions_cited']}** ({exe_pct:.1f}%) |")
    w(f"| EXE-Funktionen NICHT zitiert | {t['exe_functions_uncited']} |")
    w(f"| EXE-Raum-Zitate auf Funktionen gemappt / ungemappt | {t['exe_cited_addresses_mapped']} / {t['exe_cited_addresses_unmapped']} (ungemappt = Datenraum oder > Start+0x4000) |")
    w(f"| Overlay-Zitate, die mehrere Stages treffen (Mehrdeutigkeit) | {t['overlay_cited_addresses_multi_stage']} |")
    w(f"| Overlay-Zitate ohne Stage-Funktions-Treffer | {t['overlay_cited_addresses_no_stage']} |")
    w("")
    w("### Overlay-Coverage je Stage")
    w("")
    w("⚠️ Spalten sind wegen des geteilten Adressraums **nicht disjunkt** (siehe Methodik Punkt 2).")
    w("")
    w("| Stage | Funktionen | zitiert | nicht zitiert | zitiert % | gemappte Zitate |")
    w("|---|---:|---:|---:|---:|---:|")
    for st in stage_names:
        r = stage_results[st]
        pct = 100.0 * r["functions_cited"] / r["functions_total"] if r["functions_total"] else 0.0
        w(
            f"| {st.upper()} | {r['functions_total']} | {r['functions_cited']} | "
            f"{r['functions_uncited']} | {pct:.1f}% | {r['cited_addresses_mapped']} |"
        )
    w("")
    w("## Subsystem-Sicht (EXE-Funktionen × Katalog-Kontext)")
    w("")
    w("Kontext aus `RE15_FUN_CATALOG.md`; Funktionen ohne Katalog-Treffer stehen unter")
    w("„(ohne Katalog-Kontext)“ — das ist der größte Block und zugleich die ehrliche Aussage,")
    w("dass der Katalog nur einen Teil des EXE-Inventars abdeckt.")
    w("")
    w("| Subsystem (Katalog-Kontext) | Funktionen | zitiert | nicht zitiert |")
    w("|---|---:|---:|---:|")
    for name in sorted(subsys_rows, key=lambda n: (-subsys_rows[n]["total"], n)):
        r = subsys_rows[name]
        w(f"| {name} | {r['total']} | {r['cited']} | {r['uncited']} |")
    w("")
    w("## Anhang A — nicht-zitierte EXE-FUN-Adressen")
    w("")
    w(f"{len(exe_uncited)} von {len(exe_starts)} EXE-Funktionen ohne Adress-Zitat im Port-Code")
    w("(Semantik siehe oben: nicht zwingend unportiert, aber ohne Beleg im Code):")
    w("")
    w("```")
    md.extend(compact_hex_block(exe_uncited))
    w("```")
    w("")
    w("## Anhang B — nicht-zitierte STAGE1-Overlay-Funktionen")
    w("")
    s1 = stage_results["stage1"]
    w(f"{s1['functions_uncited']} von {s1['functions_total']} STAGE1-Overlay-Funktionen ohne Adress-Zitat:")
    w("")
    w("```")
    md.extend(compact_hex_block([int(a, 16) for a in s1["uncited_starts"]]))
    w("```")
    w("")
    w("## Anhang C — STAGE2–6 (nur Zählwerte)")
    w("")
    w("Die vollständigen Lückenlisten für STAGE2–6 stehen in")
    w("`analysis/coverage/coverage_result.json` (`overlays.stageN.uncited_starts`);")
    w("die Funktionsinventare in `analysis/coverage/original_overlay_inventory.json`.")
    w("")
    w("| Stage | nicht zitiert / gesamt |")
    w("|---|---:|")
    for st in stage_names:
        if st == "stage1":
            continue
        r = stage_results[st]
        w(f"| {st.upper()} | {r['functions_uncited']} / {r['functions_total']} |")
    w("")
    with open(OUT_MD, "w", encoding="utf-8") as f:
        f.write("\n".join(md))
        f.write("\n")

    print(f"EXE:    {t['exe_functions_cited']}/{t['exe_functions_total']} zitiert ({exe_pct:.1f}%)")
    for st in stage_names:
        r = stage_results[st]
        print(f"{st}: {r['functions_cited']}/{r['functions_total']} zitiert")
    print(f"DAT-Globals: {t['cited_dat_globals']}  |  unique Zitate gesamt: {t['cited_unique_total']}")
    print(f"geschrieben: {OUT_MD}")
    print(f"geschrieben: {OUT_JSON}")


if __name__ == "__main__":
    main()
