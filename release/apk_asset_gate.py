#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Volle Asset-Pruefung der Android-APK gegen den Quellbaum (Runde 34a, 2026-09-29).

WARUM (offener Punkt aus v0.8.19): Der Android-Bau pruefte nur Stichproben - von den 30 neuen
Port-Tuerarchiven (shared_assets/RE15DOOR) genau EINES (P07G.DO2, und das nur im Quellbaum).
Alle 30 wurden in v0.8.19 von Hand in der APK nachgemessen. Dieses Gate macht das fuer JEDE
Datei JEDES Asset-Baums, bei jedem Bau.

WAS GEPRUEFT WIRD
  0. Die Baumliste unten (BAEUME) ist die EINZIGE Liste des Gates. Sie wird gegen den
     Gradle-Task stageAssets (re15_port/platform/android/app/build.gradle) geprueft: jede
     from(new File(<basis>, "<quelle>")) { into "<ziel>"; include ... }-Anweisung. Weicht die
     build.gradle ab (neuer Baum wie RE15DOOR in Runde 33, anderes include, unbekannte
     Anweisung), bricht das Gate ab, statt still etwas anderes zu pruefen als gebaut wird.
  a. Jede Quelldatei jedes Baums liegt in der APK unter assets/<ziel>/<pfad>.
  b. Groesse UND sha256 sind gleich (Inhalt ueber zipfile gelesen; zipfile prueft dabei die
     CRC jedes Eintrags mit).
  c. Unter assets/ liegt nichts ausser diesen Dateien und re15_assets.txt (keine Zusatz-,
     Doppel- oder Verzeichniseintraege).
  d. re15_assets.txt - so gelesen wie der Leser auf dem Geraet
     (re15_port/platform/android/jni/android_glue.c, re15_android_bootstrap_assets):
     Zeilen an '\\n' getrennt, angehaengte '\\r' abgeschnitten (:200), leere und '#'-Zeilen
     uebersprungen (:201), Zeile = "<bytes>\\t<pfad>" (:202-206), Kopfzeile
     "# re15 assets <anzahl> <bytes>" am Pufferanfang (:163). Jede Zeile muss genau einen
     APK-Eintrag assets/<pfad> derselben Groesse treffen, und jede Asset-Datei der APK muss im
     Manifest stehen - sonst wird sie auf dem Geraet nie entpackt. Format des Schreibers:
     app/build.gradle writeAssetManifest (:131-141).
  e. Ausgabe: Zaehlung je Baum und ausdruecklich RE2/DOOR, RE15DOOR, TORSE.VBS.
  Pflichtinhalt zusaetzlich: kein Baum leer, RE2/DOOR und RE15DOOR mit *.DO2, TORSE.VBS
  vorhanden (make_package.sh check_tree verlangt dasselbe fuer die PC-Pakete).

RUECKGABE (fail closed)
  0 = APK und Quellbaum gleich
  1 = Abweichung (alle Befunde werden gelistet, je Art begrenzt mit "und N weitere")
  2 = Bedien-/Lesefehler, Konfigurationsabweichung zur build.gradle oder JEDER unerwartete
      Fehler - nie 0, wenn nicht wirklich alles verglichen wurde.

AUFRUF (reines Python >= 3.8, keine Fremdpakete; Windows + Linux)
  apk_asset_gate.py [--repo <repo>] <apk>    Pruefung; --repo Standard = Ordner ueber release/
  apk_asset_gate.py --selbsttest             baut in einem Temp-Ordner Mini-Quellbaum,
                                             Mini-APK und Manifest und prueft, dass das Gate
                                             die gute APK annimmt und JEDE Faelschung ablehnt
                                             (sonst bestaetigt sich das Gate nur selbst).
  Aufrufer: release/build_android.sh (Abschnitt Gates, auch --gate-only <apk>).
  Interpreter per release/python_finden.sh (nie den WindowsApps-Alias "python3").
"""
import argparse
import hashlib
import os
import re
import shutil
import subprocess
import sys
import tempfile
import time
import traceback
import warnings
import zipfile
import zlib

RC_GLEICH, RC_ABWEICHUNG, RC_FEHLER = 0, 1, 2

# ---------------------------------------------------------------------------------------------
# DIE Baumliste - einmal hier, gegen build.gradle stageAssets geprueft (gradle_baeume_pruefen).
# (Basis in build.gradle, Quellordner relativ zur Basis, Ziel unter assets/, include-Muster)
# ---------------------------------------------------------------------------------------------
BAEUME = (
    ("portRoot", "shared_assets/PSX",          "shared_assets/PSX",          ()),
    ("portRoot", "shared_assets/extracted_fx", "shared_assets/extracted_fx", ()),
    ("portRoot", "shared_assets/RE2",          "shared_assets/RE2",          ()),
    ("portRoot", "shared_assets/RE15DOOR",     "shared_assets/RE15DOOR",     ()),
    ("repoRoot", "synchro",                    "synchro",                    ("STAGE*/**",)),
)
# build.gradle: portRoot = <repo>/re15_port, repoRoot = <repo> (app/build.gradle:25-26)
BASIS_ORDNER = {"portRoot": "re15_port", "repoRoot": ""}
GRADLE_REL = "re15_port/platform/android/app/build.gradle"

MANIFEST = "re15_assets.txt"
MANIFEST_MAX = 64 << 20          # android_glue.c:80 read_apk_asset: sz > 64 MiB -> NULL
KOPF_RE = re.compile(r"# re15 assets (\d+) (\d+)")

# Pflichtinhalt (Pfade relativ zu assets/ bzw. zu re15_port/)
PFLICHT_ORDNER = (("shared_assets/RE2/DOOR", ".DO2"), ("shared_assets/RE15DOOR", ".DO2"))
PFLICHT_DATEI = ("shared_assets/RE2/TORSE.VBS",)
# ausdrueckliche Zaehlung in der Ausgabe (Praefix unter assets/)
ZAEHLEN = (("RE2/DOOR", "shared_assets/RE2/DOOR/"), ("RE15DOOR", "shared_assets/RE15DOOR/"),
           ("TORSE.VBS", "shared_assets/RE2/TORSE.VBS"))

BLOCK = 1 << 20


class Bedienfehler(Exception):
    """Gate kann keine gueltige Aussage treffen -> Rueckgabe 2."""


# =============================================================================================
# build.gradle: stageAssets-Block lesen
# =============================================================================================
def _zeichenkette_ende(text, i):
    """i zeigt auf ein Anfuehrungszeichen; Index hinter dem Ende der Zeichenkette."""
    q = text[i:i + 3] if text[i:i + 3] in ('"""', "'''") else text[i]
    j = i + len(q)
    while j < len(text):
        if text[j] == "\\":
            j += 2
            continue
        if text.startswith(q, j):
            return j + len(q)
        j += 1
    raise Bedienfehler("build.gradle: Zeichenkette ohne Ende ab Zeichen %d" % i)


def _ohne_kommentare(text):
    """Groovy-Code ohne // und /* */-Kommentare; Zeichenketten bleiben unberuehrt."""
    aus, i, n = [], 0, len(text)
    while i < n:
        c = text[i]
        if c in "\"'":
            j = _zeichenkette_ende(text, i)
            aus.append(text[i:j])
            i = j
        elif text.startswith("//", i):
            j = text.find("\n", i)
            i = n if j < 0 else j
        elif text.startswith("/*", i):
            j = text.find("*/", i + 2)
            if j < 0:
                raise Bedienfehler("build.gradle: Kommentar /* ohne Ende")
            aus.append("\n" * text.count("\n", i, j + 2))
            i = j + 2
        else:
            aus.append(c)
            i += 1
    return "".join(aus)


def _klammer_ende(code, i):
    """code[i] ist '{'/'('/'['; Index der passenden schliessenden Klammer.
    Zeichenketten und Kommentare zaehlen nicht (in build.gradle stehen Klammern in Kommentaren)."""
    paar = {"{": "}", "(": ")", "[": "]"}
    stapel, j = [], i
    while j < len(code):
        c = code[j]
        if c in "\"'":
            j = _zeichenkette_ende(code, j)
            continue
        if code.startswith("//", j):
            k = code.find("\n", j)
            j = len(code) if k < 0 else k
            continue
        if code.startswith("/*", j):
            k = code.find("*/", j + 2)
            if k < 0:
                raise Bedienfehler("build.gradle: Kommentar /* ohne Ende")
            j = k + 2
            continue
        if c in paar:
            stapel.append(paar[c])
        elif c in ")}]":
            if not stapel or stapel.pop() != c:
                raise Bedienfehler("build.gradle: Klammern passen nicht (Zeichen %d)" % j)
            if not stapel:
                return j
        j += 1
    raise Bedienfehler("build.gradle: Klammer ohne Ende")


def _anweisungen(code):
    """Anweisungen der obersten Ebene eines Blocks (an Zeilenende/';' getrennt, Klammern beachtet)."""
    aus, start, j, tiefe = [], 0, 0, 0
    while j < len(code):
        c = code[j]
        if c in "\"'":
            j = _zeichenkette_ende(code, j)
            continue
        if c in "({[":
            tiefe += 1
        elif c in ")}]":
            tiefe -= 1
            if tiefe < 0:
                raise Bedienfehler("build.gradle: Klammern passen nicht im stageAssets-Block")
        elif tiefe == 0 and c in "\n;":
            aus.append(code[start:j].strip())
            start = j + 1
        j += 1
    aus.append(code[start:].strip())
    return [a for a in aus if a]


_STR = r'"([^"$\\]+)"'
_FROM_RE = re.compile(r'from\s*\(\s*new\s+File\s*\(\s*(portRoot|repoRoot)\s*,\s*' + _STR +
                      r'\s*\)\s*\)\s*(?:\{(.*)\})?\s*$', re.S)
_INTO_RE = re.compile(r'into\s*(?:\(\s*' + _STR + r'\s*\)|' + _STR + r')\s*$')
_INCL_RE = re.compile(r'include\s*\(?\s*("[^"$\\]+"(?:\s*,\s*"[^"$\\]+")*)\s*\)?\s*$')


def gradle_baeume(gradle_text):
    """stageAssets-Block -> Liste (basis, quelle, ziel, includes). Unbekanntes -> Bedienfehler."""
    treffer = list(re.finditer(r'tasks\.register\(\s*"stageAssets"\s*,\s*Sync\s*\)\s*\{', gradle_text))
    if len(treffer) != 1:
        raise Bedienfehler('build.gradle: tasks.register("stageAssets", Sync) { ... } %s'
                           % ("nicht gefunden" if not treffer else "mehrfach vorhanden"))
    auf = treffer[0].end() - 1
    zu = _klammer_ende(gradle_text, auf)
    block = _ohne_kommentare(gradle_text[auf + 1:zu])

    baeume, into_stage, preserve = [], 0, 0
    for a in _anweisungen(block):
        kopf = re.match(r"[A-Za-z_]\w*", a)
        wort = kopf.group(0) if kopf else ""
        if wort == "from":
            m = _FROM_RE.match(a)
            if not m:
                raise Bedienfehler("build.gradle stageAssets: from-Anweisung nicht im bekannten "
                                   "Format 'from(new File(<portRoot|repoRoot>, \"<ordner>\")) { into ... }': %s"
                                   % " ".join(a.split()))
            basis, quelle, koerper = m.group(1), m.group(2), m.group(3) or ""
            ziel, incl = None, []
            for b in _anweisungen(koerper):
                mi, mc = _INTO_RE.match(b), _INCL_RE.match(b)
                if mi and ziel is None:
                    ziel = mi.group(1) or mi.group(2)
                elif mc:
                    incl += re.findall(r'"([^"]+)"', mc.group(1))
                else:
                    raise Bedienfehler("build.gradle stageAssets: unbekannte Anweisung im from-Block "
                                       "von '%s': %s" % (quelle, " ".join(b.split())))
            if ziel is None:
                raise Bedienfehler("build.gradle stageAssets: from '%s' ohne into" % quelle)
            baeume.append((basis, quelle, ziel, tuple(sorted(incl))))
        elif re.match(r"into\s*\(\s*assetStage\s*\)\s*$", a):
            into_stage += 1
        elif re.match(r'preserve\s*\{\s*include\s*\(?\s*"' + re.escape(MANIFEST) + r'"\s*\)?\s*\}\s*$', a):
            preserve += 1
        elif wort in ("description", "duplicatesStrategy", "doFirst", "doLast"):
            pass
        else:
            raise Bedienfehler("build.gradle stageAssets: unbekannte Anweisung (das Gate kennt ihre "
                               "Wirkung auf den APK-Inhalt nicht): %s" % " ".join(a.split())[:200])
    if into_stage != 1 or preserve != 1:
        raise Bedienfehler("build.gradle stageAssets: erwartet genau ein 'into(assetStage)' und ein "
                           "'preserve { include \"%s\" }' (gefunden %d / %d)" % (MANIFEST, into_stage, preserve))
    return baeume


def gradle_baeume_pruefen(repo):
    pfad = os.path.join(repo, GRADLE_REL)
    if not os.path.isfile(pfad):
        raise Bedienfehler("build.gradle fehlt: %s (zeigt --repo auf das Repo?)" % pfad)
    with open(pfad, "r", encoding="utf-8", newline=None) as f:
        text = f.read()
    ist = set(gradle_baeume(text))
    soll = set((b, q, z, tuple(sorted(i))) for b, q, z, i in BAEUME)
    if ist != soll:
        def fmt(t):
            return "%s/%s -> assets/%s%s" % (t[0], t[1], t[2], (" include %s" % list(t[3])) if t[3] else "")
        zeilen = ["build.gradle stageAssets weicht von der Baumliste des Gates ab (%s):" % GRADLE_REL]
        zeilen += ["  nur in build.gradle: " + fmt(t) for t in sorted(ist - soll)]
        zeilen += ["  nur im Gate:         " + fmt(t) for t in sorted(soll - ist)]
        zeilen.append("  -> BAEUME in release/apk_asset_gate.py nachziehen (und make_package.sh copy_common "
                      "gegenpruefen), erst dann ist die Pruefung wieder vollstaendig.")
        raise Bedienfehler("\n".join(zeilen))
    return len(ist)


# =============================================================================================
# Quellbaum
# =============================================================================================
def _ant_passt(muster, pfad):
    """Ant/Gradle-Muster ('*', '?', '**' als ganzes Segment), gross/klein-genau."""
    mt, pt = muster.split("/"), pfad.split("/")

    def seg(m, s):
        rx = "".join("[^/]*" if c == "*" else "[^/]" if c == "?" else re.escape(c) for c in m)
        return re.fullmatch(rx, s) is not None

    def geht(i, j):
        if i == len(mt):
            return j == len(pt)
        if mt[i] == "**":
            return any(geht(i + 1, k) for k in range(j, len(pt) + 1))
        return j < len(pt) and seg(mt[i], pt[j]) and geht(i + 1, j + 1)
    return geht(0, 0)


def _auslass_hinweis(rel):
    """Warum Gradle (Standardausschluesse) bzw. AAPT (ignoreAssetsPattern) die Datei weglaesst."""
    teile = rel.split("/")
    for d in teile[:-1]:
        if d.startswith("."):
            return "Ordner '%s' beginnt mit '.' (Gradle/AAPT lassen ihn aus)" % d
        if d.startswith("_"):
            return "Ordner '%s' beginnt mit '_' (AAPT-Muster '<dir>_*')" % d
    n = teile[-1]
    if n.startswith("."):
        return "Name beginnt mit '.' (Gradle/AAPT lassen ihn aus)"
    if n.endswith("~") or (n.startswith("#") and n.endswith("#")) or (n.startswith("%") and n.endswith("%")):
        return "Name passt auf einen Gradle-Standardausschluss"
    if n.lower() in ("thumbs.db", "picasa.ini") or n.lower().endswith(".scc"):
        return "Name passt auf das AAPT-Ignoriermuster"
    return ""


def quelldateien(repo, befund):
    """{apk_name: (abs_pfad, rel_zum_repo)} ueber alle Baeume + Zaehlung je Baum."""
    dateien, zahl = {}, {}

    def walk_fehler(e):
        raise Bedienfehler("Quellbaum nicht lesbar: %s" % e)

    for basis, quelle, ziel, muster in BAEUME:
        rel_baum = "/".join(x for x in (BASIS_ORDNER[basis], quelle) if x)
        wurzel = os.path.join(repo, *rel_baum.split("/"))
        zahl[ziel] = 0
        if not os.path.isdir(wurzel):
            befund("Quellbaum", "Quellbaum fehlt: %s (Gradle kopiert aus einem fehlenden Ordner "
                                "still NICHTS)" % rel_baum)
            continue
        for dp, dns, fns in os.walk(wurzel, onerror=walk_fehler, followlinks=True):
            dns.sort()
            for fn in sorted(fns):
                p = os.path.join(dp, fn)
                rel = os.path.relpath(p, wurzel).replace(os.sep, "/")
                if muster and not any(_ant_passt(m, rel) for m in muster):
                    continue
                dateien["assets/%s/%s" % (ziel, rel)] = (p, "%s/%s" % (rel_baum, rel))
                zahl[ziel] += 1
        if zahl[ziel] == 0:
            befund("Quellbaum", "Quellbaum leer: %s" % rel_baum)
    for ordner, endung in PFLICHT_ORDNER:
        n = sum(1 for a in dateien if a.startswith("assets/%s/" % ordner) and a.endswith(endung)
                and "/" not in a[len("assets/%s/" % ordner):])
        if n == 0:
            befund("Quellbaum", "Pflichtinhalt fehlt: re15_port/%s/*%s (0 Dateien)" % (ordner, endung))
    for datei in PFLICHT_DATEI:
        q = dateien.get("assets/" + datei)
        if not q or os.path.getsize(q[0]) == 0:
            befund("Quellbaum", "Pflichtdatei fehlt/leer: re15_port/%s" % datei)
    return dateien, zahl


# =============================================================================================
# APK
# =============================================================================================
def _sha_datei(pfad):
    h, n = hashlib.sha256(), 0
    with open(pfad, "rb") as f:
        while True:
            b = f.read(BLOCK)
            if not b:
                return h.hexdigest(), n
            h.update(b)
            n += len(b)


def _sha_eintrag(zf, info):
    h, n = hashlib.sha256(), 0
    with zf.open(info) as f:
        while True:
            b = f.read(BLOCK)
            if not b:
                return h.hexdigest(), n
            h.update(b)
            n += len(b)


def manifest_pruefen(roh, apk_dateien, befund):
    """roh = Bytes von assets/re15_assets.txt; apk_dateien = {rel: groesse} (ohne Manifest)."""
    if len(roh) > MANIFEST_MAX:
        befund("Manifest", "Manifest %d B > 64 MiB: das Geraet liest es nicht (android_glue.c:80)" % len(roh))
    if roh.startswith(b"\xef\xbb\xbf"):
        befund("Manifest", "Manifest beginnt mit BOM: Kopfzeile auf dem Geraet unlesbar "
                           "(sscanf am Pufferanfang, android_glue.c:163)")
    try:
        text = roh.decode("utf-8")
    except UnicodeDecodeError as e:
        befund("Manifest", "Manifest ist kein gueltiges UTF-8 (%s) - Pfade treffen die APK-Namen nicht" % e)
        text = roh.decode("utf-8", "replace")

    zeilen = text.split("\n")                       # android_glue.c:197 (strchr '\n')
    eintraege, zeile_von = {}, {}
    kopf = None
    for nr, z in enumerate(zeilen, 1):
        z = z.rstrip("\r")                          # :200 nur angehaengte '\r'
        if nr == 1:
            kopf = z
        if not z:
            continue                                # :201
        if z.startswith("#"):
            if nr != 1:
                befund("Manifest", "Manifest-Zeile %d: unerwartete Kommentarzeile '%s' (der Schreiber "
                                   "schreibt nur die Kopfzeile)" % (nr, z[:80]))
            continue
        if "\t" not in z:
            befund("Manifest", "Manifest-Zeile %d ohne Tab (das Geraet ueberspringt sie still, "
                               "android_glue.c:203): '%s'" % (nr, z[:120]))
            continue
        groesse, pfad = z.split("\t", 1)
        if not re.fullmatch(r"[0-9]+", groesse):
            befund("Manifest", "Manifest-Zeile %d: Groessenfeld '%s' ist keine Zahl (atoll, :205)" % (nr, groesse[:40]))
            continue
        teile = pfad.split("/")
        if (not pfad or pfad.startswith("/") or "\\" in pfad or any(t in ("", ".", "..") for t in teile)):
            befund("Manifest", "Manifest-Zeile %d: unzulaessiger Pfad '%s' (leer, absolut, '\\\\', "
                               "'.'/'..' oder '//': landet ausserhalb des Ankers)" % (nr, pfad[:120]))
            continue
        if pfad == MANIFEST:
            befund("Manifest", "Manifest-Zeile %d nennt das Manifest selbst" % nr)
            continue
        if pfad in eintraege:
            befund("Manifest", "Manifest nennt %s mehrfach (Zeilen %d und %d)" % (pfad, zeile_von[pfad], nr))
            continue
        eintraege[pfad] = int(groesse)
        zeile_von[pfad] = nr

    m = KOPF_RE.fullmatch(kopf or "")
    if not m:
        befund("Manifest", "Manifest-Kopfzeile fehlt/unlesbar: '%s' (erwartet '# re15 assets <anzahl> <bytes>', "
                           "build.gradle writeAssetManifest)" % (kopf or "")[:80])
    else:
        n_kopf, b_kopf = int(m.group(1)), int(m.group(2))
        n_ist, b_ist = len(eintraege), sum(eintraege.values())
        if (n_kopf, b_kopf) != (n_ist, b_ist):
            befund("Manifest", "Manifest-Kopfzeile passt nicht: nennt %d Dateien / %d Bytes, die Zeilen "
                               "ergeben %d / %d" % (n_kopf, b_kopf, n_ist, b_ist))

    for pfad in sorted(eintraege):
        if pfad not in apk_dateien:
            befund("Manifest: Zeile ohne APK-Eintrag",
                   "Manifest nennt %s (%d B), die APK hat keinen Eintrag assets/%s" % (pfad, eintraege[pfad], pfad))
        elif apk_dateien[pfad] != eintraege[pfad]:
            befund("Manifest: falsche Groesse",
                   "Manifest-Groesse falsch: %s Manifest %d B, APK %d B (Entpacken scheitert bei jedem "
                   "Start, android_glue.c:225)" % (pfad, eintraege[pfad], apk_dateien[pfad]))
    for pfad in sorted(apk_dateien):
        if pfad not in eintraege:
            befund("Manifest: Datei fehlt im Manifest",
                   "fehlt im Manifest: %s (wird auf dem Geraet nie entpackt)" % pfad)
    return len(eintraege), sum(eintraege.values())


def pruefen(repo, apk, max_zeilen):
    t0 = time.monotonic()
    befunde = {}
    reihenfolge = []

    def befund(art, text):
        if art not in befunde:
            befunde[art] = []
            reihenfolge.append(art)
        befunde[art].append(text)

    repo = os.path.abspath(repo)
    print("== APK-Asset-Gate (release/apk_asset_gate.py) ==")
    print("   APK:       %s" % os.path.abspath(apk))
    print("   Quellbaum: %s" % repo)

    n_baeume = gradle_baeume_pruefen(repo)
    print("   Baumliste: %d Baeume = build.gradle stageAssets (from/into/include geprueft)" % n_baeume)

    if not os.path.isfile(apk):
        raise Bedienfehler("APK fehlt: %s" % apk)
    try:
        zf = zipfile.ZipFile(apk)
    except (zipfile.BadZipFile, OSError) as e:
        raise Bedienfehler("APK nicht lesbar: %s (%s)" % (apk, e))

    quellen, zahl_quelle = quelldateien(repo, befund)

    with zf:
        infos = zf.infolist()
        namen = [i.filename for i in infos]
        je_name = {}
        for i in infos:
            je_name.setdefault(i.filename, []).append(i)
        asset_infos = {}
        n_komprimiert = {}
        for name, liste in je_name.items():
            if not name.startswith("assets/"):
                continue
            if len(liste) > 1:
                befund("APK: doppelt/Verzeichnis", "doppelter Eintrag in der APK (%dx): %s" % (len(liste), name))
            info = liste[-1]
            if info.is_dir():
                befund("APK: doppelt/Verzeichnis", "Verzeichniseintrag in der APK: %s" % name)
                continue
            asset_infos[name] = info

        # (a)+(b) jede Quelldatei in der APK, Groesse + sha256
        gleich = {}
        bytes_gleich = 0
        for name in sorted(quellen):
            pfad, rel = quellen[name]
            info = asset_infos.get(name)
            if info is None:
                hinweis = _auslass_hinweis(name[len("assets/"):])
                befund("fehlt in der APK", "fehlt in der APK: %s  (Quelle %s)%s"
                       % (name, rel, ("  - " + hinweis) if hinweis else ""))
                continue
            q_groesse = os.path.getsize(pfad)
            if info.file_size != q_groesse:
                befund("Groesse weicht ab", "Groesse weicht ab: %s  Quelle %d B, APK %d B"
                       % (name, q_groesse, info.file_size))
                continue
            try:
                a_sha, a_n = _sha_eintrag(zf, info)
            except (zipfile.BadZipFile, zlib.error, EOFError) as e:
                befund("APK: beschaedigt (CRC)", "APK-Eintrag beschaedigt (CRC/Entpacken): %s: %s" % (name, e))
                continue
            q_sha, q_n = _sha_datei(pfad)
            if a_n != q_n:
                befund("Groesse weicht ab", "Groesse weicht ab (gelesen): %s  Quelle %d B, APK %d B" % (name, q_n, a_n))
            elif a_sha != q_sha:
                befund("Inhalt weicht ab (sha256)", "Inhalt weicht ab (sha256, gleiche Groesse %d B): %s  "
                       "Quelle %s.., APK %s.." % (q_n, name, q_sha[:16], a_sha[:16]))
            else:
                gleich[name] = q_n
                bytes_gleich += q_n
            if info.compress_type != zipfile.ZIP_STORED:
                n_komprimiert[name] = info.compress_type

        # (c) nichts Zusaetzliches unter assets/
        man_name = "assets/" + MANIFEST
        for name in sorted(asset_infos):
            if name != man_name and name not in quellen:
                befund("zusaetzlich in der APK", "zusaetzlich in der APK (kein Asset-Baum liefert ihn): %s" % name)

        # (d) Manifest
        n_man = b_man = None
        if man_name not in asset_infos:
            befund("Manifest", "Manifest fehlt in der APK: %s (das Geraet entpackt dann NICHTS, "
                               "android_glue.c:153)" % man_name)
        else:
            try:
                roh = zf.read(asset_infos[man_name])
            except (zipfile.BadZipFile, zlib.error, EOFError) as e:
                befund("APK: beschaedigt (CRC)", "APK-Eintrag beschaedigt (CRC/Entpacken): %s: %s" % (man_name, e))
                roh = None
            if roh is not None:
                apk_dateien = {n[len("assets/"):]: i.file_size for n, i in asset_infos.items() if n != man_name}
                n_man, b_man = manifest_pruefen(roh, apk_dateien, befund)

    # (e) Zaehlung
    zahl_apk = {}
    for _b, _q, ziel, _m in BAEUME:
        zahl_apk[ziel] = sum(1 for n in asset_infos if n.startswith("assets/%s/" % ziel))
    print("   %-28s %7s %7s %8s %12s" % ("Baum (unter assets/)", "Quelle", "APK", "gleich", "Bytes gleich"))
    for _b, _q, ziel, _m in BAEUME:
        pre = "assets/%s/" % ziel
        g = [n for n in gleich if n.startswith(pre)]
        print("   %-28s %7d %7d %8d %12d" % (ziel, zahl_quelle.get(ziel, 0), zahl_apk[ziel], len(g),
                                            sum(gleich[n] for n in g)))
    print("   %-28s %7d %7d %8d %12d" % ("SUMME", len(quellen), sum(zahl_apk.values()), len(gleich), bytes_gleich))
    for label, pre in ZAEHLEN:
        name = "assets/" + pre
        if pre.endswith("/"):
            q = sum(1 for n in quellen if n.startswith(name))
            a = sum(1 for n in asset_infos if n.startswith(name))
            g = sum(1 for n in gleich if n.startswith(name))
            print("   %-10s Quelle %d, APK %d, sha256 gleich %d/%d" % (label + ":", q, a, g, q))
        else:
            q = os.path.getsize(quellen[name][0]) if name in quellen else None
            a = asset_infos[name].file_size if name in asset_infos else None
            print("   %-10s Quelle %s, APK %s, sha256 %s" % (
                label + ":", ("%d B" % q) if q is not None else "FEHLT", ("%d B" % a) if a is not None else "FEHLT",
                "gleich" if name in gleich else "NICHT gleich/nicht geprueft"))
    if n_man is not None:
        print("   Manifest:  %d Zeilen / %d Bytes (Kopfzeile + Zeilen gegen die APK geprueft)" % (n_man, b_man))
    if n_komprimiert:
        print("   WARNUNG: %d Asset-Eintraege sind komprimiert (noCompress-Liste pruefen), Inhalt trotzdem "
              "verglichen: %s" % (len(n_komprimiert), ", ".join(sorted(n_komprimiert)[:5])))
    print("   Laufzeit:  %.1f s" % (time.monotonic() - t0))

    n_befunde = sum(len(v) for v in befunde.values())
    if n_befunde == 0 and len(gleich) != len(quellen):
        raise Bedienfehler("interner Widerspruch: %d Quelldateien, %d gleich, aber keine Befunde"
                           % (len(quellen), len(gleich)))
    if n_befunde:
        print("--- Abweichungen: %d ---" % n_befunde)
        for art in reihenfolge:
            liste = befunde[art]
            print("   [%s] %d" % (art, len(liste)))
            for t in liste[:max_zeilen]:
                print("      " + t)
            if len(liste) > max_zeilen:
                print("      ... und %d weitere" % (len(liste) - max_zeilen))
        print("== APK-ASSET-GATE-ABWEICHUNG: %d Befunde ==" % n_befunde)
        return RC_ABWEICHUNG
    print("== APK-ASSET-GATE-OK: %d Dateien in %d Baeumen bytegleich, Manifest stimmt ==" % (len(gleich), len(BAEUME)))
    return RC_GLEICH


# =============================================================================================
# Selbsttest: gute APK annehmen, JEDE Faelschung ablehnen
# =============================================================================================
_GRADLE_MUSTER = r'''plugins {
    id 'com.android.application'
}
def repoRoot    = file("$projectDir/../../../..").canonicalFile
def portRoot    = new File(repoRoot, "re15_port")
def assetStage  = layout.buildDirectory.dir("re15_assets").get().asFile
def computeVersionCode = { String v ->
    def m = (v =~ /(\d+)\.(\d+)\.(\d+)/)
    return m.find() ? 1 : 0
}
def url = "https://example.invalid/release-${computeVersionCode('1.2.3')}/x.tar.gz"   // "{ keine Klammer
tasks.register("stageAssets", Sync) {
    description = "Asset-Baeume nach app/build/re15_assets/ spiegeln { (Klammer im Text)"
    into(assetStage)
    from(new File(portRoot, "shared_assets/PSX"))          { into "shared_assets/PSX" }
    from(new File(portRoot, "shared_assets/extracted_fx")) { into "shared_assets/extracted_fx" }
    from(new File(portRoot, "shared_assets/RE2"))          { into "shared_assets/RE2" }
    from(new File(portRoot, "shared_assets/RE15DOOR"))     { into "shared_assets/RE15DOOR" }   // Runde 33
    from(new File(repoRoot, "synchro")) {
        into "synchro"
        include "STAGE*/**"        // synchro/unused/ + README bleiben draussen }
    }
    /* Block-Kommentar mit from(new File(portRoot, "ALT")) { into "ALT" } */
    preserve { include "re15_assets.txt" }
    duplicatesStrategy = DuplicatesStrategy.FAIL
    doFirst {
        ["shared_assets/PSX/STAGE1"].each { rel ->
            if (!new File(portRoot, rel).exists()) throw new GradleException("Asset fehlt: ${rel}")
        }
    }
}
tasks.register("writeAssetManifest") {
    doLast { def lines = []; lines.sort() }
}
'''

_FIXTURE = (   # Pfad relativ zum Repo, Groesse (0 wie shared_assets/PSX/STAGE1/wincfg.bin)
    ("re15_port/shared_assets/PSX/STAGE1/ROOM1240.RDT", 3000),
    ("re15_port/shared_assets/PSX/STAGE1/wincfg.bin", 0),
    ("re15_port/shared_assets/PSX/DATA/TEX.TIM", 5000),
    ("re15_port/shared_assets/extracted_fx/effect0_blood.tim", 700),
    ("re15_port/shared_assets/RE2/CDEMD0.EMS", 1200),
    ("re15_port/shared_assets/RE2/TORSE.VBS", 900),
    ("re15_port/shared_assets/RE2/DOOR/DOOR07.DO2", 1500),
    ("re15_port/shared_assets/RE2/DOOR/DOOR13.DO2", 1600),
    ("re15_port/shared_assets/RE15DOOR/P07G.DO2", 1700),
    ("re15_port/shared_assets/RE15DOOR/P2DS.DO2", 1800),
    ("synchro/STAGE1/room1170/main00.wav", 2000),
    ("synchro/STAGE2/room2000/main00.wav", 2100),
    ("synchro/unused/STAGE1/room1170/alt.mp3", 800),     # ausserhalb include "STAGE*/**"
    ("synchro/README.md", 100),                          # ausserhalb include "STAGE*/**"
)


def _inhalt(name, groesse):
    b = b""
    k = 0
    while len(b) < groesse:
        b += hashlib.sha256(("%s#%d" % (name, k)).encode()).digest()
        k += 1
    return b[:groesse]


class _Fall:
    """Ein Selbsttest-Fall: Mini-Repo + Mini-APK aufbauen, dann faelschen."""

    def __init__(self, wurzel):
        self.repo = os.path.join(wurzel, "repo")
        self.apk = os.path.join(wurzel, "mini.apk")
        self.gradle = _GRADLE_MUSTER
        self.quelle = {rel: _inhalt(rel, n) for rel, n in _FIXTURE}
        # APK-Inhalt = was stageAssets liefern wuerde: (name, bytes, methode)
        self.eintraege = []
        for rel, b in sorted(self.quelle.items()):
            ziel = None
            if rel.startswith("re15_port/shared_assets/"):
                ziel = rel[len("re15_port/"):]
            elif rel.startswith("synchro/STAGE"):
                ziel = rel
            if ziel:
                self.eintraege.append(["assets/" + ziel, b, zipfile.ZIP_STORED])
        self.manifest_zeilen = None          # None = aus den Eintraegen erzeugen
        self.manifest_roh = None             # Bytes ueberschreiben alles
        self.ohne_manifest = False
        self.doppelt = []
        self.kippen = None                   # (name, offset) Byte im fertigen APK-Datenstrom kippen
        self.leere_ordner = []
        self.nach_schreiben = None           # Eingriff nach dem Schreiben (APK/build.gradle zerstoeren)

    def manifest_aus_eintraegen(self):
        return [(n[len("assets/"):], len(b)) for n, b, _m in self.eintraege]

    @staticmethod
    def manifest_text(zeilen):
        z = sorted("%d\t%s" % (g, p) for p, g in zeilen)           # wie writeAssetManifest (:131-141)
        return "# re15 assets %d %d\n" % (len(z), sum(g for _p, g in zeilen)) + "\n".join(z) + "\n"

    def schreiben(self):
        for rel, b in self.quelle.items():
            p = os.path.join(self.repo, *rel.split("/"))
            os.makedirs(os.path.dirname(p), exist_ok=True)
            with open(p, "wb") as f:
                f.write(b)
        for rel in self.leere_ordner:
            os.makedirs(os.path.join(self.repo, *rel.split("/")), exist_ok=True)
        g = os.path.join(self.repo, *GRADLE_REL.split("/"))
        os.makedirs(os.path.dirname(g), exist_ok=True)
        with open(g, "w", encoding="utf-8", newline="\n") as f:
            f.write(self.gradle)
        if self.manifest_roh is not None:
            man = self.manifest_roh
        else:
            zeilen = self.manifest_zeilen if self.manifest_zeilen is not None else self.manifest_aus_eintraegen()
            man = self.manifest_text(zeilen).encode("utf-8")
        with warnings.catch_warnings():
            warnings.simplefilter("ignore")          # "Duplicate name" beim Doppel-Fall
            with zipfile.ZipFile(self.apk, "w") as zf:
                for name, b in (("AndroidManifest.xml", b"<manifest/>"), ("classes.dex", b"dex\n035\0"),
                                ("lib/arm64-v8a/libmain.so", b"\x7fELF"), ("resources.arsc", b"\x02\x00")):
                    zf.writestr(name, b)
                if not self.ohne_manifest:
                    zf.writestr(zipfile.ZipInfo("assets/" + MANIFEST), man, zipfile.ZIP_STORED)
                for name, b, methode in self.eintraege + self.doppelt:
                    zf.writestr(zipfile.ZipInfo(name), b, methode)
        if self.kippen:
            name, off = self.kippen
            with zipfile.ZipFile(self.apk) as zf:
                info = zf.getinfo(name)
            with open(self.apk, "r+b") as f:
                f.seek(info.header_offset + 26)
                n_name = int.from_bytes(f.read(2), "little")
                n_extra = int.from_bytes(f.read(2), "little")
                pos = info.header_offset + 30 + n_name + n_extra + off
                f.seek(pos)
                alt = f.read(1)
                f.seek(pos)
                f.write(bytes([alt[0] ^ 0x5A]))

    def eintrag(self, name):
        for e in self.eintraege:
            if e[0] == name:
                return e
        raise AssertionError("Selbsttest-Fixture: kein Eintrag %s" % name)

    def gradle_ersetzen(self, alt, neu):
        if alt not in self.gradle:
            raise AssertionError("Selbsttest-Fixture: '%s' nicht im Gradle-Muster" % alt)
        self.gradle = self.gradle.replace(alt, neu, 1)


def _faelle():
    """(Titel, Faelschung(fall), erwartete Rueckgabe, Pflicht-Teilstrings der Ausgabe)"""
    D7 = "assets/shared_assets/RE2/DOOR/DOOR07.DO2"
    P07 = "assets/shared_assets/RE15DOOR/P07G.DO2"

    def nichts(f):
        pass

    def crlf(f):
        f.manifest_roh = _Fall.manifest_text(f.manifest_aus_eintraegen()).replace("\n", "\r\n").encode()

    def deflate(f):
        f.eintrag("assets/shared_assets/PSX/DATA/TEX.TIM")[2] = zipfile.ZIP_DEFLATED

    def ohne_p07(f):
        f.eintraege.remove(f.eintrag(P07))

    def ein_byte(f):
        e = f.eintrag(D7)
        e[1] = e[1][:700] + bytes([e[1][700] ^ 1]) + e[1][701:]

    def zusatz(f):
        f.eintraege.append(["assets/shared_assets/PSX/EXTRA.BIN", b"zusatz", zipfile.ZIP_STORED])

    def unused(f):
        rel = "synchro/unused/STAGE1/room1170/alt.mp3"
        f.eintraege.append(["assets/" + rel, f.quelle[rel], zipfile.ZIP_STORED])

    def man_groesse(f):
        f.manifest_zeilen = [(p, g + 1 if p == "shared_assets/RE2/TORSE.VBS" else g)
                             for p, g in f.manifest_aus_eintraegen()]

    def man_zeile_fehlt(f):
        f.manifest_zeilen = [(p, g) for p, g in f.manifest_aus_eintraegen()
                             if p != "shared_assets/RE15DOOR/P2DS.DO2"]

    def nur_quelle(f):
        f.quelle["re15_port/shared_assets/RE15DOOR/NEU.DO2"] = b"nur im Quellbaum"

    def kuerzer(f):
        e = f.eintrag("assets/shared_assets/PSX/DATA/TEX.TIM")
        e[1] = e[1][:-10]

    def crc(f):
        f.kippen = ("assets/shared_assets/extracted_fx/effect0_blood.tim", 100)

    def doppelt(f):
        e = f.eintrag(D7)
        f.doppelt.append([e[0], e[1], zipfile.ZIP_STORED])

    def kopf(f):
        t = _Fall.manifest_text(f.manifest_aus_eintraegen())
        n = len(f.eintraege)
        f.manifest_roh = t.replace("# re15 assets %d " % n, "# re15 assets %d " % (n + 1), 1).encode()

    def bom(f):
        f.manifest_roh = b"\xef\xbb\xbf" + _Fall.manifest_text(f.manifest_aus_eintraegen()).encode()

    def ohne_manifest(f):
        f.ohne_manifest = True

    def ohne_tab(f):
        t = _Fall.manifest_text(f.manifest_aus_eintraegen())
        z = "\t" + "shared_assets/RE2/CDEMD0.EMS"
        f.manifest_roh = t.replace(z, " shared_assets/RE2/CDEMD0.EMS", 1).encode()

    def punktpunkt(f):
        f.manifest_zeilen = f.manifest_aus_eintraegen() + [("shared_assets/../../boese.bin", 3)]

    def re15door_leer(f):
        for rel in [r for r in f.quelle if r.startswith("re15_port/shared_assets/RE15DOOR/")]:
            del f.quelle[rel]
        f.eintraege = [e for e in f.eintraege if not e[0].startswith("assets/shared_assets/RE15DOOR/")]
        f.leere_ordner.append("re15_port/shared_assets/RE15DOOR")

    def re15door_weg(f):
        for rel in [r for r in f.quelle if r.startswith("re15_port/shared_assets/RE15DOOR/")]:
            del f.quelle[rel]
        f.eintraege = [e for e in f.eintraege if not e[0].startswith("assets/shared_assets/RE15DOOR/")]

    def torse_weg(f):
        del f.quelle["re15_port/shared_assets/RE2/TORSE.VBS"]
        f.eintraege.remove(f.eintrag("assets/shared_assets/RE2/TORSE.VBS"))

    def kein_zip(f):
        def ueberschreiben():
            with open(f.apk, "wb") as h:
                h.write(b"kein ZIP, nur Text")
        f.nach_schreiben = ueberschreiben

    def apk_fehlt(f):
        f.nach_schreiben = lambda: os.remove(f.apk)

    def gradle_neu(f):
        f.gradle_ersetzen('    from(new File(repoRoot, "synchro")) {',
                          '    from(new File(portRoot, "shared_assets/NEU"))  { into "shared_assets/NEU" }\n'
                          '    from(new File(repoRoot, "synchro")) {')

    def gradle_ohne(f):
        f.gradle_ersetzen('    from(new File(portRoot, "shared_assets/RE15DOOR"))     { into "shared_assets/RE15DOOR" }'
                          '   // Runde 33\n', "")

    def gradle_include(f):
        f.gradle_ersetzen('include "STAGE*/**"', 'include "**"')

    def gradle_exclude(f):
        f.gradle_ersetzen("    duplicatesStrategy = DuplicatesStrategy.FAIL",
                          '    duplicatesStrategy = DuplicatesStrategy.FAIL\n    exclude "**/*.DO2"')

    def gradle_fehlt(f):
        f.nach_schreiben = lambda: os.remove(os.path.join(f.repo, *GRADLE_REL.split("/")))

    return (
        ("gute APK", nichts, 0, ["APK-ASSET-GATE-OK", "RE15DOOR:  Quelle 2, APK 2, sha256 gleich 2/2"]),
        ("Manifest mit CRLF (Geraet schneidet \\r ab)", crlf, 0, ["APK-ASSET-GATE-OK"]),
        ("Eintrag komprimiert (Inhalt gleich)", deflate, 0, ["APK-ASSET-GATE-OK", "WARNUNG"]),
        ("fehlende Datei: RE15DOOR-Eintrag entfernt", ohne_p07, 1, ["fehlt in der APK: " + P07]),
        ("gleiche Groesse, anderer Inhalt: RE2/DOOR 1 Byte", ein_byte, 1, ["Inhalt weicht ab (sha256", D7]),
        ("Zusatzeintrag unter assets/", zusatz, 1, ["zusaetzlich in der APK", "PSX/EXTRA.BIN"]),
        ("synchro/unused in der APK", unused, 1, ["zusaetzlich in der APK", "assets/synchro/unused/"]),
        ("Manifest: falsche Groesse", man_groesse, 1, ["Manifest-Groesse falsch: shared_assets/RE2/TORSE.VBS"]),
        ("Manifest: fehlende Zeile", man_zeile_fehlt, 1, ["fehlt im Manifest: shared_assets/RE15DOOR/P2DS.DO2"]),
        ("Datei nur im Quellbaum", nur_quelle, 1, ["fehlt in der APK: assets/shared_assets/RE15DOOR/NEU.DO2"]),
        ("APK-Eintrag kuerzer als die Quelle", kuerzer, 1, ["Groesse weicht ab: assets/shared_assets/PSX/DATA/TEX.TIM"]),
        ("Byte im APK-Datenstrom gekippt (CRC)", crc, 1, ["beschaedigt (CRC", "effect0_blood.tim"]),
        ("doppelter APK-Eintrag", doppelt, 1, ["doppelter Eintrag in der APK (2x): " + D7]),
        ("Manifest: Kopfzeile Anzahl falsch", kopf, 1, ["Manifest-Kopfzeile passt nicht"]),
        ("Manifest: BOM", bom, 1, ["BOM"]),
        ("Manifest fehlt in der APK", ohne_manifest, 1, ["Manifest fehlt in der APK"]),
        ("Manifest: Zeile ohne Tab", ohne_tab, 1, ["ohne Tab", "fehlt im Manifest: shared_assets/RE2/CDEMD0.EMS"]),
        ("Manifest: Pfad mit '..'", punktpunkt, 1, ["unzulaessiger Pfad 'shared_assets/../../boese.bin'"]),
        ("RE15DOOR leer (Quelle und APK)", re15door_leer, 1, ["Quellbaum leer: re15_port/shared_assets/RE15DOOR",
                                                               "Pflichtinhalt fehlt: re15_port/shared_assets/RE15DOOR"]),
        ("RE15DOOR-Ordner fehlt ganz", re15door_weg, 1, ["Quellbaum fehlt: re15_port/shared_assets/RE15DOOR"]),
        ("TORSE.VBS fehlt (Quelle und APK)", torse_weg, 1, ["Pflichtdatei fehlt/leer: re15_port/shared_assets/RE2/TORSE.VBS"]),
        ("APK ist kein ZIP", kein_zip, 2, ["APK nicht lesbar"]),
        ("APK fehlt", apk_fehlt, 2, ["APK fehlt"]),
        ("build.gradle: zusaetzlicher Baum", gradle_neu, 2, ["nur in build.gradle: portRoot/shared_assets/NEU"]),
        ("build.gradle: Baum RE15DOOR fehlt", gradle_ohne, 2, ["nur im Gate:         portRoot/shared_assets/RE15DOOR"]),
        ("build.gradle: include geaendert", gradle_include, 2, ["include ['**']", "include ['STAGE*/**']"]),
        ("build.gradle: unbekannte Anweisung", gradle_exclude, 2, ["unbekannte Anweisung", 'exclude "**/*.DO2"']),
        ("build.gradle fehlt", gradle_fehlt, 2, ["build.gradle fehlt"]),
    )


def selbsttest():
    t0 = time.monotonic()
    print("== APK-Asset-Gate: Selbsttest (Mini-Quellbaum + Mini-APK im Temp-Ordner) ==")
    tmp = tempfile.mkdtemp(prefix="apk_gate_selbsttest_")
    ok, schlecht = 0, []
    try:
        for nr, (titel, faelschen, soll_rc, soll_text) in enumerate(_faelle(), 1):
            wurzel = os.path.join(tmp, "f%02d" % nr)
            os.makedirs(wurzel)
            f = _Fall(wurzel)
            f.nach_schreiben = None
            faelschen(f)
            f.schreiben()
            if f.nach_schreiben:
                f.nach_schreiben()
            r = subprocess.run([sys.executable, os.path.abspath(__file__), "--repo", f.repo, f.apk],
                               stdout=subprocess.PIPE, stderr=subprocess.STDOUT, stdin=subprocess.DEVNULL,
                               timeout=120)
            aus = r.stdout.decode("utf-8", "replace")
            fehlt = [t for t in soll_text if t not in aus]
            gut = (r.returncode == soll_rc and not fehlt)
            print("   [%s] %02d %-50s rc=%d (soll %d)%s" % ("ok" if gut else "FEHLER", nr, titel, r.returncode, soll_rc,
                                                        "" if not fehlt else "  fehlende Meldung: %s" % fehlt))
            if gut:
                ok += 1
            else:
                schlecht.append((nr, titel, aus))
    finally:
        shutil.rmtree(tmp, ignore_errors=True)
    n = ok + len(schlecht)
    print("   Laufzeit: %.1f s" % (time.monotonic() - t0))
    if schlecht:
        for nr, titel, aus in schlecht:
            print("--- Ausgabe Fall %02d (%s) ---" % (nr, titel))
            print(aus.rstrip())
        print("== SELBSTTEST-FEHLER: %d von %d Faellen falsch - das Gate ist NICHT verlaesslich ==" % (len(schlecht), n))
        return RC_ABWEICHUNG
    print("== SELBSTTEST-OK: %d/%d Faelle (gute APK angenommen, jede Faelschung abgelehnt) ==" % (ok, n))
    return RC_GLEICH


# =============================================================================================
def main(argv=None):
    for s in (sys.stdout, sys.stderr):
        try:
            s.reconfigure(errors="backslashreplace", line_buffering=True)
        except Exception:
            pass
    try:
        if sys.version_info < (3, 8):
            raise Bedienfehler("Python >= 3.8 noetig, laeuft unter %s" % sys.version.split()[0])
        ap = argparse.ArgumentParser(prog="apk_asset_gate.py",
                                     description="Volle Asset-Pruefung der Android-APK gegen den Quellbaum.")
        ap.add_argument("apk", nargs="?", help="die zu pruefende APK")
        ap.add_argument("--repo", default=os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
                        help="Repo-Wurzel (Standard: Ordner ueber release/)")
        ap.add_argument("--selbsttest", action="store_true", help="Faelschungen im Temp-Ordner pruefen")
        ap.add_argument("--max-zeilen", type=int, default=25, help="Befunde je Art (Rest: 'und N weitere')")
        a = ap.parse_args(argv)
        if a.selbsttest == bool(a.apk):
            ap.error("genau eins von: <apk> oder --selbsttest")
        if a.max_zeilen < 1:
            ap.error("--max-zeilen muss >= 1 sein")
        rc = selbsttest() if a.selbsttest else pruefen(a.repo, a.apk, a.max_zeilen)
        return rc if rc in (RC_GLEICH, RC_ABWEICHUNG) else RC_FEHLER
    except Bedienfehler as e:
        print("ABBRUCH (Rueckgabe 2): %s" % e, file=sys.stderr)
        return RC_FEHLER
    except SystemExit as e:                       # argparse: --help = 0, Bedienfehler = 2
        if e.code is None or e.code == 0:
            return RC_GLEICH
        return e.code if e.code in (RC_ABWEICHUNG, RC_FEHLER) else RC_FEHLER
    except BaseException:                         # fail closed: alles Unerwartete = 2
        traceback.print_exc()
        print("ABBRUCH (Rueckgabe 2): unerwarteter Fehler im Gate - keine Aussage ueber die APK", file=sys.stderr)
        return RC_FEHLER


if __name__ == "__main__":
    sys.exit(main())
