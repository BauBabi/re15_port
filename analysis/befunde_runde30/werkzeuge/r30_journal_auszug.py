#!/usr/bin/env python3
"""Zieht die Agenten-Ergebnisse aus einem Workflow-Journal (journal.jsonl) und legt je
Thema eine JSON- und eine Textfassung unter build/r30_journal/ ab.

Aufruf:  python r30_journal_auszug.py <journal.jsonl> [<journal2.jsonl> ...]
"""
import json
import os
import sys

OUT = "build/r30_journal"
BS = chr(92)  # Rueckstrich — nicht als Literal, die Shell frisst ihn im Heredoc


def schluessel(r):
    d = r.get("dossier", "") or ""
    k = d.replace(BS, "/").split("/")[-1].replace(".md", "")
    if not k:
        k = (r.get("thema", "") or "unbekannt").split()[0]
    linse = r.get("linse")
    if linse:
        k = "sk_%s_%s" % ((r.get("thema", "") or "x").replace(BS, "/").split("/")[-1], linse)
    return k


def text(r, key):
    o = ["=" * 70, "THEMA: " + key]
    if "linse" in r:
        o.append("LINSE: %s   URTEIL: %s" % (r.get("linse"), r.get("urteil")))
        o.append("WIDERLEGT:")
        for w in r.get("widerlegte_punkte", []):
            o.append("  x %s\n      warum: %s\n      beleg: %s" % (w.get("aussage"), w.get("warum"), w.get("beleg")))
        o.append("BESTAETIGT:")
        for b in r.get("bestaetigte_punkte", []):
            o.append("  + " + b)
        o.append("LUECKEN:")
        for b in r.get("luecken", []):
            o.append("  ? " + b)
        o.append("EMPFEHLUNG: " + str(r.get("empfehlung")))
        return "\n".join(o)
    o.append("ZUSAMMENFASSUNG: " + str(r.get("zusammenfassung")))
    o.append("URSACHE: " + str(r.get("ursache_des_symptoms")))
    o.append("PLAN:")
    for s in r.get("umsetzungsplan", []):
        o.append("  - %s\n      Dateien: %s\n      Konstanten: %s" % (
            s.get("schritt"), ", ".join(s.get("dateien", [])), s.get("konstanten", "")))
    o.append("OFFEN:")
    for s in r.get("offene_fragen", []):
        o.append("  ? " + s)
    o.append("NUTZERENTSCHEIDUNG: " + str(r.get("nutzer_entscheidung_noetig")))
    for k in r.get("kernbefunde", []):
        if k.get("status") == "NICHT BELEGT":
            o.append("  ! NICHT BELEGT: " + k.get("aussage", ""))
    o.append("ARTEFAKTE:")
    for a in r.get("artefakte", []):
        o.append("  * " + a)
    return "\n".join(o)


def main():
    os.makedirs(OUT, exist_ok=True)
    for pfad in sys.argv[1:]:
        for zeile in open(pfad, encoding="utf-8"):
            try:
                o = json.loads(zeile)
            except Exception:
                continue
            if o.get("type") != "result":
                continue
            r = o.get("result")
            if not isinstance(r, dict):
                continue
            # verschachtelte Rueckgabe des Workflows selbst ueberspringen
            if "ermittlung" in r:
                continue
            key = schluessel(r)
            json.dump(r, open(os.path.join(OUT, key + ".json"), "w", encoding="utf-8"),
                      ensure_ascii=False, indent=1)
            t = text(r, key)
            open(os.path.join(OUT, key + ".txt"), "w", encoding="utf-8").write(t)
            print("%-40s %6d Zeichen" % (key, len(t)))


if __name__ == "__main__":
    main()
