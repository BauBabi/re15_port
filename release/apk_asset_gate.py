#!/bin/bash
''':' #
# Direktaufruf (./release/apk_asset_gate.py ...) laeuft zuerst als Bash-Skript (Nachbesserung R2,
# Gegenpruefung echtlauf B1): der alte Kopf '#!/usr/bin/env python3' startete unter Git-Bash den
# WindowsApps-Alias (v0.8.17: ungefragte Installation von Python 3.14). Der Interpreter kommt
# jetzt aus release/python_finden.sh, der den Alias nie startet. Fuer Python ist dieser Block eine
# Zeichenkette ohne Wirkung. Zeilenenden: release/.gitattributes haelt *.py auf LF (core.autocrlf).
. "$(dirname "$0")/python_finden.sh" || exit 2 #
exec "$PY" "$0" "$@" #
'''
__doc__ = """Volle Asset-Pruefung der Android-APK gegen den Quellbaum (Runde 34a, 2026-09-29).

WARUM (offener Punkt aus v0.8.19): Der Android-Bau pruefte nur Stichproben - von den 30 neuen
Port-Tuerarchiven (shared_assets/RE15DOOR) genau EINES (P07G.DO2, und das nur im Quellbaum).
Alle 30 wurden in v0.8.19 von Hand in der APK nachgemessen. Dieses Gate macht das fuer JEDE
Datei JEDES Asset-Baums, bei jedem Bau.

WIE DIE APK GELESEN WIRD (Nachbesserung R1, Befund B1 der Gegenpruefung)
  ROH, so wie Androids ZIP-Leser libziparchive (android_glue.c entpacken(): AAssetManager_open ->
  libziparchive) - NICHT ueber Pythons zipfile. zipfile schneidet Namen am
  NUL ab, ersetzt unter Windows '\\' durch '/' und vergleicht vom Local Header nur den Namen: drei
  Faelschungen an RE15DOOR/P07G.DO2 ('\\' im Namen, CRC bzw. Groesse nur im Local Header falsch)
  bestanden so die ganze Kette, obwohl libziparchive den Eintrag nicht oeffnet (aapt2 35.0.0:
  "failed to find file." / "size/crc32 mismatch ... Inconsistent information"). Jetzt gilt fuer
  JEDEN Eintrag der APK (auch lib/, classes.dex):
   - die Datei beginnt mit einem Local Header (libziparchive: "Entry at offset zero has invalid
     LFH signature" - Nachbesserung R2, B8), End-of-Central-Directory am Dateiende (Kommentar
     reicht genau bis zum Ende, weder zu kurz noch zu lang), Zentralverzeichnis vollstaendig und
     ohne Rest lesbar, kein ZIP64/mehrteilig -> sonst Rueckgabe 2;
   - Name roh: gueltiges UTF-8 und kein NUL (sonst verwirft libziparchive die GANZE APK), kein
     '\\', keine Steuerzeichen, kein absoluter Pfad/'.'/'..'/'//'; Namen roh eindeutig;
   - Local Header vorhanden, Name (Laenge + Bytes) und CRC/Groessen = Zentralverzeichnis; kein
     Data-Descriptor-Bit (Nachbesserung R2: AGP setzt es nie, Referenz v0.8.19 0 von 3616 - ein
     Leser, der die Groessen je nach Bit aus dem einen oder anderen Kopf nimmt, ist damit weg);
   - Daten liegen ganz vor dem Zentralverzeichnis; Methode 0 oder 8; nicht verschluesselt;
   - Daten ab dem Offset, den der LOCAL Header ergibt (wie das Geraet; AGP polstert dort zur
     Ausrichtung, das Zentralverzeichnis nicht), csize Bytes gelesen und entpackt: CRC32 und
     Laenge = Zentralverzeichnis, ein Deflate-Strom endet genau am Eintragsende.

WAS GEPRUEFT WIRD
  0. Die Baumliste unten (BAEUME) ist die EINZIGE Liste des Gates. Sie wird gegen den
     Gradle-Task stageAssets (re15_port/platform/android/app/build.gradle) geprueft: jede
     from(new File(<basis>, "<quelle>")) { into "<ziel>"; include ... }-Anweisung. Weicht die
     build.gradle ab (neuer Baum wie RE15DOOR in Runde 33, anderes include, unbekannte
     Anweisung), bricht das Gate ab, statt still etwas anderes zu pruefen als gebaut wird.
  a. Jede Quelldatei jedes Baums liegt in der APK unter assets/<ziel>/<pfad> (Name roh verglichen).
  b. Groesse UND sha256 sind gleich (Daten wie oben gelesen; CRC32 zusaetzlich gegen das
     Zentralverzeichnis).
  c. Unter assets/ liegt nichts ausser diesen Dateien und re15_assets.txt (keine Zusatz-,
     Doppel- oder Verzeichniseintraege).
  d. re15_assets.txt, FORMAT v2 (Runde 34a N1) - gelesen nach DENSELBEN Regeln wie auf dem Geraet
     (manifest_lesen = re15_port/platform/android/jni/asset_abgleich.c re15_abgleich_lesen, Regeln im
     Kopf von asset_abgleich.h; beide lesen BYTES): Zeilen an '\\n' getrennt, angehaengte '\\r'
     abgeschnitten, leere Zeilen uebersprungen, Zeile 1 = "# re15 assets v2 <anzahl> <bytes>",
     jede weitere = "<bytes>\\t<sha256>\\t<pfad>" (1-18 Ziffern 0-9, 64 Zeichen 0-9a-f klein, Pfad
     1-512 Bytes relativ mit '/', ohne '\\', Steuerzeichen, leere/'.'/'..'-Segmente, gueltiges UTF-8,
     nicht auf ".neu"); weitere '#'-Zeilen, doppelte (auch nur in Gross/klein verschiedene) Pfade,
     NUL, keine Datei, falsche Kopfzeile, > 64 MiB -> das Geraet verwirft die GANZE Liste. Die alte
     Liste v1 ("# re15 assets <n> <b>", "<bytes>\\t<pfad>", bis v0.8.19) wird ausdruecklich abgelehnt.
     Jede Zeile muss genau einen APK-Eintrag assets/<pfad> treffen, mit derselben Groesse UND demselben
     sha256 wie dessen Daten, und jede Asset-Datei der APK muss im Manifest stehen - sonst wird sie auf
     dem Geraet nie (oder nie richtig) entpackt. Schreiber: app/build.gradle writeAssetManifest.
  e. Ausgabe: Zaehlung je Baum und ausdruecklich RE2/DOOR, RE15DOOR, TORSE.VBS.
  Pflichtinhalt zusaetzlich: kein Baum leer, RE2/DOOR und RE15DOOR mit *.DO2, TORSE.VBS
  vorhanden (make_package.sh check_tree verlangt dasselbe fuer die PC-Pakete).
  f. SOLL-LISTE DER TUERARCHIVE (Nachbesserung R2, Befund B2): bis dahin verglich das Gate die
     APK nur mit dem Quellbaum - fehlte ein Archiv in BEIDEN (29 von 30) oder war es in beiden
     0 Byte, lief die ganze Kette gruen, und an den Tueren lief still der RE1.5-Uebergang. Jetzt
     gilt der Quellbaum gegen die Tabellen, nach denen die Engine die Archive laedt und prueft
     (door_scene_pc.c re2_archiv_lesen): jedes Port-Archiv aus gen/re15_tuer_eigen.inc liegt als
     RE15DOOR/<kennung>.DO2 mit Groesse, Aufbau (Sektor * 0x800 + Modellteil = Groesse, Tonteil
     davor) und FNV-1a der Tabelle; jedes RE2-Archiv, das eine Tuerzeile, ein Griff-Tausch oder
     ein Port-Archiv als Basis nennt, liegt als RE2/DOOR/DOORxx.DO2 mit Groesse und Aufbau aus
     gen/re2_tuer_tabelle.inc (@0x8009a520); keine Datei ausserhalb dieser Listen. Die Spalten
     liest das Gate aus den typedefs in include/re15_door_seq.h (C-Regeln: fehlende Felder am
     Zeilenende = 0) - unbekannte Form -> Rueckgabe 2.
  g. Unter re15_port/shared_assets/ liegt nichts ausser den Baeumen der Liste (Nachbesserung R2,
     Befund B6): ein neuer Ordner, den keine Liste kennt, fehlte sonst still in APK und Paketen.
  NICHT hier: Signatur, versionName, Paketname, ABIs, Ausrichtung - das pruefen apksigner, aapt
  und zipalign in release/apk_pruefen.sh (dieselbe Kette fuer build_android.sh und make_package.sh).

PC-PAKETE (Nachbesserung R2, Befund B6): make_package.sh kopiert die Asset-Baeume selbst
  (copy_common - eine dritte Liste neben build.gradle und BAEUME). Mit --paket <ordner> prueft das
  Gate den fertigen Paketordner gegen DIESELBE Liste: jede Datei unter <ordner>/<ziel>/ mit
  gleicher Groesse und sha256, unter <ordner>/shared_assets und <ordner>/synchro nichts sonst.
  --quellbaum prueft nur den Quellbaum (a-Teil, f, g) - make_package.sh vor den Kopierminuten.

RUECKGABE (fail closed)
  0 = APK und Quellbaum gleich
  1 = Abweichung (alle Befunde werden gelistet, je Art begrenzt mit "und N weitere")
  2 = Bedien-/Lesefehler, Konfigurationsabweichung zur build.gradle oder JEDER unerwartete
      Fehler - nie 0, wenn nicht wirklich alles verglichen wurde.

AUFRUF (reines Python >= 3.8, keine Fremdpakete; Windows + Linux). Interpreter IMMER ueber
release/python_finden.sh - so rufen es alle Skripte auf:
  source release/python_finden.sh
  "$PY" release/apk_asset_gate.py [--repo <repo>] <apk>       Pruefung (--repo Standard: Ordner ueber release/)
  "$PY" release/apk_asset_gate.py [--repo <repo>] --quellbaum Quellbaum allein (Tuer-Soll, Baumliste, ...)
  "$PY" release/apk_asset_gate.py [--repo <repo>] --paket <ordner>   PC-Paketordner gegen die Liste
  "$PY" release/apk_asset_gate.py --selbsttest    baut in einem Temp-Ordner Mini-Quellbaum,
                                             Mini-APK (zipfile als UNABHAENGIGER Schreiber, danach
                                             AGP-artig gepolstert) und Manifest und prueft, dass das
                                             Gate die guten APKs annimmt und JEDE Faelschung
                                             ablehnt (sonst bestaetigt sich das Gate nur selbst).
  Ein Direktaufruf ./release/apk_asset_gate.py ... geht ebenfalls: der Kopf oben laeuft dann als
  Bash-Skript und holt den Interpreter aus python_finden.sh (nie den WindowsApps-Alias "python3").
  Aufrufer: release/apk_pruefen.sh (aus build_android.sh und make_package.sh), make_package.sh.
  Pruefhaken (nur Selbsttest/Mutanten-Probe; keiner macht die Pruefung milder):
    RE15_GATE_MANIFEST_MAX=<n>        senkt die 64-MiB-Grenze des Manifests (nie hoeher)
    RE15_GATE_SELBSTTEST_SCHNELL=1    Selbsttest endet beim ersten falschen Fall (Ergebnis bleibt "FEHLER")

SELBSTTEST-ABDECKUNG (Nachbesserung R2, Gegenpruefung B1): die Faelle treffen jede Pruefung von BEIDEN
  Seiten und an ihren Grenzen (Manifest groesser/kleiner, Laenge ohne CRC-Fehler, Kommentar zu lang/zu kurz,
  1 B hinter dem EOCD, Digest kleiner/groesser, Tabellenwert +-1, ...). Belegt mit der Teil-Mutanten-Probe
  analysis/befunde_runde34_android/nachbesserung_r2_belege/mutanten_teil.py (jede Vergleichsrichtung,
  Grenze +-1, jeder Teil eines and/or, not, Tupelpaare; dazu Ganz-Abschaltungen und Hand-Mutanten).
  Runde 4 (Gegenpruefung R3, B2): dazu Zeichenklassen, strip-/split-Varianten, Konstante und Methodenmenge
  (Mutanten MU1-MU8 aus pruefer_umgehung_r3_belege/r3_mutanten.py): Leerraum/VT/FF/NBSP an Pfad und Kopfzeile,
  andere Zeilentrenner (U+2028, '\\r', FS, NEL), Nicht-ASCII-Ziffern, Methoden 1/7, Manifest 64 MiB + 1 B
  ohne Pruefhaken und die Geraete-Grenze als innere Probe.
"""
import argparse
import concurrent.futures
import hashlib
import os
import re
import shutil
import struct
import subprocess
import sys
import tempfile
import time
import traceback
import warnings
import zipfile          # NUR fuer den Selbsttest (unabhaengiger Schreiber) - die Pruefung liest roh
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
# abgeleitet: die Ordner direkt unter re15_port/shared_assets/, die ausgeliefert werden, und die
# obersten Ordner der Ziele (im PC-Paket: <paket>/shared_assets, <paket>/synchro)
SHARED_WURZEL = frozenset(q.split("/", 1)[1] for b, q, _z, _m in BAEUME if b == "portRoot" and q.startswith("shared_assets/"))
ZIEL_OBEN = tuple(sorted(set(z.split("/", 1)[0] for _b, _q, z, _m in BAEUME)))

MANIFEST = "re15_assets.txt"
MANIFEST_MAX = 64 << 20          # asset_abgleich.h RE15_ABGLEICH_LISTE_MAX: > 64 MiB -> Liste ungueltig
# Format v2 (Runde 34a N1, jni/asset_abgleich.h). Gelesen wird auf BYTES, NUR ASCII-Ziffern (Nachbesserung
# R2, B8: '\d' nahm auch arabisch-indische Ziffern; das Geraet liest nur 0-9) und nur Kleinbuchstaben a-f.
KOPF_RE = re.compile(rb"# re15 assets v2 ([0-9]{1,18}) ([0-9]{1,18})")
KOPF_V1_RE = re.compile(rb"# re15 assets ([0-9]+) ([0-9]+)")      # bis v0.8.19 - wird ABGELEHNT
GROESSE_RE = re.compile(rb"[0-9]{1,18}")
SHA_RE = re.compile(rb"[0-9a-f]{64}")
PFAD_MAX = 512                   # asset_abgleich.h RE15_ABGLEICH_PFAD_MAX (Bytes)
NEU_ENDUNG = b".neu"             # asset_abgleich.h RE15_ABGLEICH_NEU_ENDUNG (Zwischendatei, ASCII gross/klein egal)
_ASCII_KLEIN = bytes.maketrans(b"ABCDEFGHIJKLMNOPQRSTUVWXYZ", b"abcdefghijklmnopqrstuvwxyz")

# Pflichtinhalt (Pfade relativ zu assets/ bzw. zu re15_port/)
PFLICHT_ORDNER = (("shared_assets/RE2/DOOR", ".DO2"), ("shared_assets/RE15DOOR", ".DO2"))
PFLICHT_DATEI = ("shared_assets/RE2/TORSE.VBS",)
# ausdrueckliche Zaehlung in der Ausgabe (Praefix unter assets/)
ZAEHLEN = (("RE2/DOOR", "shared_assets/RE2/DOOR/"), ("RE15DOOR", "shared_assets/RE15DOOR/"),
           ("TORSE.VBS", "shared_assets/RE2/TORSE.VBS"))

BLOCK = 1 << 20

# ZIP-Aufbau (APPNOTE 4.3.7 / 4.3.12 / 4.3.16)
EOCD_SIG, CD_SIG, LFH_SIG, Z64_LOC_SIG = b"PK\x05\x06", b"PK\x01\x02", b"PK\x03\x04", b"PK\x06\x07"
EOCD_LEN, CD_LEN, LFH_LEN = 22, 46, 30
# Bit 0 verschluesselt, Bit 6 starke Verschluesselung, Bit 13 maskierter Kopf (APPNOTE 4.4.4)
FLAG_VERSCHLUESSELT, FLAG_DATA_DESCRIPTOR = 0x0001 | 0x0040 | 0x2000, 0x0008

# Tuerarchive: die Tabellen, nach denen die Engine sie laedt (Nachbesserung R2, Befund B2)
TUER_KOPF = "re15_port/include/re15_door_seq.h"                 # typedefs = Spaltenreihenfolge
TUER_EIGEN_INC = "re15_port/engine/src/gen/re15_tuer_eigen.inc"  # Port-Archive, Zeilen, Griff-Tausch
TUER_RE2_INC = "re15_port/engine/src/gen/re2_tuer_tabelle.inc"   # RE2-Archivtabelle @0x8009a520
TUER_ZUORDNUNG_INC = "re15_port/engine/src/gen/tuer_zuordnung.inc"  # Tuerzeilen + Griff-Tausch Runde 31
RE15DOOR_REL = "re15_port/shared_assets/RE15DOOR"
RE2DOOR_REL = "re15_port/shared_assets/RE2/DOOR"
SEKTOR = 0x800                   # door_scene_pc.c:242 sektor * 0x800 + modell == n


class Bedienfehler(Exception):
    """Gate kann keine gueltige Aussage treffen -> Rueckgabe 2."""


class _Lesefehler(Exception):
    """Ein Eintrag laesst sich nicht so lesen, wie sein Zentralverzeichnis-Eintrag ihn beschreibt."""


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
            try:
                i = text.index("\n", i)
            except ValueError:
                i = n
        elif text.startswith("/*", i):
            # ohne '*/' unerreichbar: der Block kommt aus _klammer_ende, das jedes offene /* schon meldet
            # (ValueError -> Rueckgabe 2 ueber main)
            j = text.index("*/", i + 2)
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
            try:
                j = code.index("\n", j)
            except ValueError:
                j = len(code)
            continue
        if code.startswith("/*", j):
            try:
                j = code.index("*/", j + 2) + 2
            except ValueError:
                raise Bedienfehler("build.gradle: Kommentar /* ohne Ende (ab Zeichen %d)" % j)
            continue
        if c in paar:
            stapel.append(paar[c])
        elif c in ")}]":
            if stapel.pop() != c:          # nie leer: der Stapel leert sich nur beim return unten
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
            tiefe -= 1        # nie < 0: _klammer_ende hat den Block schon als ausgeglichen erkannt
        elif not tiefe and c in "\n;":
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


def _walk_fehler(e):
    """os.walk-Fehler (unlesbarer Ordner) -> Rueckgabe 2; ein still uebersprungener Ordner fehlte sonst im
    Vergleich. Im Fall-Selbsttest nicht nachstellbar - die inneren Proben rufen es direkt."""
    raise Bedienfehler("Quellbaum nicht lesbar: %s" % e)


def quelldateien(repo, befund):
    """{apk_name: (abs_pfad, rel_zum_repo)} ueber alle Baeume + Zaehlung je Baum."""
    dateien, zahl = {}, {}

    for basis, quelle, ziel, muster in BAEUME:
        rel_baum = "/".join(x for x in (BASIS_ORDNER[basis], quelle) if x)
        wurzel = os.path.join(repo, *rel_baum.split("/"))
        zahl[ziel] = 0
        if not os.path.isdir(wurzel):
            befund("Quellbaum", "Quellbaum fehlt: %s (Gradle kopiert aus einem fehlenden Ordner "
                                "still NICHTS)" % rel_baum)
            continue
        for dp, dns, fns in os.walk(wurzel, onerror=_walk_fehler, followlinks=True):
            dns.sort()
            for fn in sorted(fns):
                p = os.path.join(dp, fn)
                rel = os.path.relpath(p, wurzel).replace(os.sep, "/")
                if muster and not any(_ant_passt(m, rel) for m in muster):
                    continue
                dateien["assets/%s/%s" % (ziel, rel)] = (p, "%s/%s" % (rel_baum, rel))
                zahl[ziel] += 1
        if not zahl[ziel]:
            befund("Quellbaum", "Quellbaum leer: %s" % rel_baum)
    for ordner, endung in PFLICHT_ORDNER:
        n = sum(1 for a in dateien if a.startswith("assets/%s/" % ordner) and a.endswith(endung)
                and "/" not in a[len("assets/%s/" % ordner):])
        if not n:
            befund("Quellbaum", "Pflichtinhalt fehlt: re15_port/%s/*%s (0 Dateien)" % (ordner, endung))
    for datei in PFLICHT_DATEI:
        q = dateien.get("assets/" + datei)
        if not q or not os.path.getsize(q[0]):
            befund("Quellbaum", "Pflichtdatei fehlt/leer: re15_port/%s" % datei)
    return dateien, zahl


def wurzel_pruefen(repo, befund):
    """Unter re15_port/shared_assets/ nur die Baeume der Liste (Nachbesserung R2, Befund B6: ein neuer
    Ordner, den weder build.gradle noch BAEUME noch copy_common kennt, fehlte still in APK und Paketen)."""
    wurzel = os.path.join(repo, "re15_port", "shared_assets")
    for name in sorted(os.listdir(wurzel)) if os.path.isdir(wurzel) else ():
        if name not in SHARED_WURZEL:
            befund("Quellbaum: in keiner Liste", "re15_port/shared_assets/%s liegt in keinem Asset-Baum (build.gradle "
                   "stageAssets = BAEUME des Gates = make_package.sh copy_common) - der Port liest nur aus shared_assets, "
                   "APK und Pakete haetten es nicht" % name)


# --- Engine-Tabellen der Tuerarchive lesen (C-Initialisierer, Spalten aus den typedefs) ---------
def _c_lesen(repo, rel):
    p = os.path.join(repo, *rel.split("/"))
    if not os.path.isfile(p):
        raise Bedienfehler("Engine-Tabelle fehlt: %s (die Soll-Liste der Tuerarchive steht dort)" % rel)
    with open(p, "r", encoding="utf-8") as f:
        return _c_ohne_kommentare(f.read(), rel)


def _c_ohne_kommentare(text, datei):
    """C-Quelltext ohne /* */ und //; Zeichenketten bleiben."""
    aus, i, n = [], 0, len(text)
    while i < n:
        c = text[i]
        if c == '"':
            m = _C_STR_RE.match(text, i)
            if not m:
                raise Bedienfehler("%s: Zeichenkette ohne Ende (Zeichen %d)" % (datei, i))
            aus.append(m.group(0))
            i = m.end()
        elif text.startswith("/*", i):
            try:
                i = text.index("*/", i + 2) + 2
            except ValueError:
                raise Bedienfehler("%s: Kommentar /* ohne Ende (Zeichen %d)" % (datei, i))
            aus.append(" ")
        elif text.startswith("//", i):
            try:
                i = text.index("\n", i)
            except ValueError:
                i = n
        else:
            aus.append(c)
            i += 1
    return "".join(aus)


_C_STR_RE = re.compile(r'"(?:[^"\\\n]|\\.)*"')


_C_TYP_RE = re.compile(r"(?:const\s+)?(?:unsigned\s+|signed\s+)?(char|u?int(?:8|16|32|64)_t|int|short|long)\s+(.+)$")
_C_DEKL_RE = re.compile(r"([A-Za-z_]\w*)\s*(?:\[\s*([0-9]+)\s*\])?$")


def c_struktur(code, typname, datei):
    """typedef struct { ... } <typname>; -> [(feld, typ, anzahl|None)] in Speicherreihenfolge."""
    t = re.findall(r"typedef\s+struct\s*\{([^{}]*)\}\s*" + re.escape(typname) + r"\s*;", code)
    if len(t) != 1:
        raise Bedienfehler("%s: 'typedef struct { ... } %s;' %s" % (datei, typname, "mehrfach" if t else "nicht gefunden"))
    felder = []
    for dekl in t[0].split(";"):
        dekl = " ".join(dekl.split())
        if not dekl:
            continue
        m = _C_TYP_RE.match(dekl)
        if not m:
            raise Bedienfehler("%s: %s - Feld nicht lesbar: '%s'" % (datei, typname, dekl))
        for d in m.group(2).split(","):
            md = _C_DEKL_RE.match(d.strip())
            if not md:
                raise Bedienfehler("%s: %s - Deklarator nicht lesbar: '%s'" % (datei, typname, d.strip()))
            felder.append((md.group(1), m.group(1), int(md.group(2)) if md.group(2) else None))
    return felder


_C_ZEICHENTYPEN = ("char",)      # char[N] wird mit einer Zeichenkette initialisiert
_C_TOK_RE = re.compile(r'(\{)|(\})|(,)|("(?:[^"\\\n]|\\.)*")|(-?(?:0[xX][0-9a-fA-F]+|[0-9]+))[uUlL]*(?![\w.])')


def _c_werte(code, i, datei, name):
    """Initialisierer ab code[i] (hinter der oeffnenden '{') bis zur passenden '}' ->
    (verschachtelte Listen aus int/str, Index hinter der '}')."""
    stapel = [[]]
    while i < len(code):
        if code[i].isspace():
            i += 1
            continue
        m = _C_TOK_RE.match(code, i)
        if not m:
            raise Bedienfehler("%s: %s - unerwartetes Zeichen im Initialisierer: '%s'"
                               % (datei, name, code[i:i + 30].replace("\n", " ")))
        auf, zu, _komma, s, zahl = m.groups()
        i = m.end()
        if auf:
            stapel[-1].append([])
            stapel.append(stapel[-1][-1])
        elif zu:
            fertig = stapel.pop()
            if not stapel:
                return fertig, i
        elif s is not None:
            stapel[-1].append(s[1:-1])
        elif zahl is not None:
            stapel[-1].append(int(zahl, 0))
    raise Bedienfehler("%s: Tabelle %s ohne Ende (schliessende '}' fehlt)" % (datei, name))


def c_tabelle(code, typname, name, felder, datei):
    """static const <typname> <name>[N] = { {..}, ... }; -> Liste von dict. C-Regeln: fehlende Felder am
    Zeilenende = 0 bzw. ''. Mehr Werte als Felder, andere Form oder Zeilenzahl != N -> Bedienfehler."""
    kopf = re.compile(r"static\s+const\s+" + re.escape(typname) + r"\s+" + re.escape(name) + r"\s*\[\s*([0-9]+)\s*\]\s*=\s*\{")
    treffer = list(kopf.finditer(code))
    if len(treffer) != 1:
        raise Bedienfehler("%s: 'static const %s %s[N] = {' %s" % (datei, typname, name,
                                                                   "mehrfach" if treffer else "nicht gefunden"))
    m = treffer[0]
    zeilen, j = _c_werte(code, m.end(), datei, name)
    if not re.match(r"\s*;", code[j:]):
        raise Bedienfehler("%s: Tabelle %s: nach der schliessenden '}' fehlt ';'" % (datei, name))
    n_soll = int(m.group(1))
    if len(zeilen) != n_soll:
        raise Bedienfehler("%s: %s[%d] hat %d Zeilen (der Generator schreibt alle; fehlende fuellte C mit Nullen)"
                           % (datei, name, n_soll, len(zeilen)))
    aus = []
    for k, z in enumerate(zeilen, 1):
        if not isinstance(z, list):
            raise Bedienfehler("%s: %s Zeile %d ist kein { ... }" % (datei, name, k))
        if len(z) > len(felder):
            raise Bedienfehler("%s: %s Zeile %d: %d Werte, %s hat %d Felder" % (datei, name, k, len(z), typname, len(felder)))
        d = {}
        for idx, (feld, typ, anz) in enumerate(felder):
            w = z[idx] if idx < len(z) else None
            if typ in _C_ZEICHENTYPEN and anz:
                w = "" if w is None else w
                if not isinstance(w, str) or len(w.encode("utf-8")) >= anz:
                    raise Bedienfehler("%s: %s Zeile %d Feld %s: Zeichenkette mit < %d Bytes erwartet" % (datei, name, k, feld, anz))
            elif anz:
                w = [] if w is None else w
                if not isinstance(w, list) or len(w) > anz or not all(isinstance(x, int) for x in w):
                    raise Bedienfehler("%s: %s Zeile %d Feld %s: { bis zu %d Zahlen } erwartet" % (datei, name, k, feld, anz))
                w = w + [0] * (anz - len(w))
            else:
                w = 0 if w is None else w
                if not isinstance(w, int):
                    raise Bedienfehler("%s: %s Zeile %d Feld %s: Zahl erwartet" % (datei, name, k, feld))
            d[feld] = w
        aus.append(d)
    return aus


def _fnv1a32(pfad):
    """FNV-1a 32 ueber die Datei - der Pruefwert der Port-Archive (door_scene_pc.c:209-214)."""
    h = 2166136261
    with open(pfad, "rb") as f:
        for b in iter(lambda: f.read(BLOCK), b""):
            for x in b:
                h = ((h ^ x) * 16777619) & 0xFFFFFFFF
    return h


def tuer_soll(repo, befund):
    """Soll-Liste der Tuerarchive aus den Engine-Tabellen, so wie door_scene_pc.c sie laedt:
    -> (eigen {kennung: zeile}, re2 {nr: (tabellenzeile, grund)})."""
    kopf = _c_lesen(repo, TUER_KOPF)
    eigen_code = _c_lesen(repo, TUER_EIGEN_INC)
    re2_code = _c_lesen(repo, TUER_RE2_INC)
    zu_code = _c_lesen(repo, TUER_ZUORDNUNG_INC)
    m = re.search(r"#define\s+RE15_DOOR_KEIN_SPENDER\s+(0[xX][0-9a-fA-F]+|[0-9]+)[uU]?\s", kopf)
    if not m:
        raise Bedienfehler("%s: #define RE15_DOOR_KEIN_SPENDER nicht gefunden (Tuerzeilen ohne Griff-Tausch)" % TUER_KOPF)
    kein_spender = int(m.group(1), 0)
    f_eigen = c_struktur(kopf, "re15_tuer_eigen_t", TUER_KOPF)
    f_zeile = c_struktur(kopf, "re15_tuer_zeile_t", TUER_KOPF)
    f_griff = c_struktur(kopf, "re15_griff_tausch_t", TUER_KOPF)
    f_re2 = c_struktur(re2_code, "re2_tuer_arch_t", TUER_RE2_INC)
    for felder, noetig, typ in ((f_eigen, ("kennung", "basis", "ton", "modell", "sektor", "datei", "fnv"), "re15_tuer_eigen_t"),
                                (f_zeile, ("re2_nr", "spender", "eigen"), "re15_tuer_zeile_t"),
                                (f_griff, ("archiv", "spender", "spender_eigen", "fuer_eigen"), "re15_griff_tausch_t"),
                                (f_re2, ("ton", "modell", "sektor", "datei"), "re2_tuer_arch_t")):
        fehlt = [x for x in noetig if x not in [f[0] for f in felder]]
        if fehlt:
            raise Bedienfehler("Engine-Tabellen: %s ohne Feld(er) %s - Gate (tuer_soll) nachziehen" % (typ, fehlt))
    eigen = c_tabelle(eigen_code, "re15_tuer_eigen_t", "re15_tuer_eigen", f_eigen, TUER_EIGEN_INC)
    arch = c_tabelle(re2_code, "re2_tuer_arch_t", "re2_tuer_arch", f_re2, TUER_RE2_INC)
    z31 = c_tabelle(zu_code, "re15_tuer_zeile_t", "re15_tuer_zeilen", f_zeile, TUER_ZUORDNUNG_INC)
    z33 = c_tabelle(eigen_code, "re15_tuer_zeile_t", "re15_tuer_zeilen_eigen", f_zeile, TUER_EIGEN_INC)
    g31 = c_tabelle(zu_code, "re15_griff_tausch_t", "re15_griff_tausche", f_griff, TUER_ZUORDNUNG_INC)
    g33 = c_tabelle(eigen_code, "re15_griff_tausch_t", "re15_griff_tausche_eigen", f_griff, TUER_EIGEN_INC)

    kenn = {}
    for i, e in enumerate(eigen, 1):
        if not re.fullmatch(r"[A-Za-z0-9_]+", e["kennung"]) or e["kennung"] in kenn:
            raise Bedienfehler("%s: re15_tuer_eigen Zeile %d: Kennung '%s' leer, doppelt oder kein Dateiname"
                               % (TUER_EIGEN_INC, i, e["kennung"]))
        kenn[e["kennung"]] = e
    re2 = {}

    def braucht(nr, grund):
        if 0 <= nr < len(arch):
            re2.setdefault(nr, (arch[nr], grund))
        else:
            befund("Engine-Tabelle", "%s nennt RE2-Tuerarchiv %d, re2_tuer_arch hat %d Zeilen - die Engine findet es nie"
                   % (grund, nr, len(arch)))

    def griff(z):                                         # door_seq_zuordnung.c:149-175, erster Treffer zaehlt
        if z["eigen"]:
            kand = [g for g in g33 if g["fuer_eigen"] == z["eigen"]]
        else:
            kand = g31 + g33
        for g in kand:
            if g["archiv"] == z["re2_nr"] and g["spender"] == z["spender"]:
                return g
        return None

    for z in z31 + z33:
        if not z["eigen"]:                                # door_scene_pc.c:322 RE2-Datei DOORxx.DO2
            braucht(z["re2_nr"], "Tuerzeile")
        elif z["eigen"] > len(eigen):
            befund("Engine-Tabelle", "Tuerzeile nennt Port-Archiv %d, re15_tuer_eigen hat %d" % (z["eigen"], len(eigen)))
        elif eigen[z["eigen"] - 1]["basis"] != z["re2_nr"]:  # :227 'if (!e || e->basis != nr) return -1'
            e = eigen[z["eigen"] - 1]
            befund("Engine-Tabelle", "Tuerzeile nennt DOOR%02X mit Port-Archiv %s (Basis DOOR%02X) - die Engine "
                   "verwirft das Archiv" % (z["re2_nr"], e["kennung"], e["basis"]))
        if z["spender"] == kein_spender:                  # door_scene_pc.c:347 RE15_DOOR_KEIN_SPENDER
            continue
        g = griff(z)                                      # :347-350 Spender des Griff-Tauschs
        if g is None:
            continue
        if not g["spender_eigen"]:
            braucht(z["spender"], "Griff-Spender")
        elif g["spender_eigen"] > len(eigen):
            befund("Engine-Tabelle", "Griff-Tausch nennt Port-Archiv %d, re15_tuer_eigen hat %d"
                   % (g["spender_eigen"], len(eigen)))
    for e in eigen:                                       # Quelle jedes Port-Archivs (tuer_archiv_bauen.py)
        braucht(e["basis"], "Basis von %s" % e["kennung"])
    return kenn, re2


def tueren_pruefen(repo, befund):
    """RE15DOOR und RE2/DOOR des Quellbaums gegen die Soll-Liste (Befund B2). Dieselben Bedingungen wie
    door_scene_pc.c:242-243 beim Laden: Groesse = Tabelle, Sektor * 0x800 + Modellteil = Groesse, Tonteil
    <= Sektor * 0x800, bei Port-Archiven FNV-1a = Tabelle. -> [(art, ordner, soll, gut)]"""
    kenn, re2 = tuer_soll(repo, befund)
    ergebnis = []
    for art, rel, soll, mit_fnv, quelle in (
            ("Port-Tuerarchiv", RE15DOOR_REL, {"%s.DO2" % k: (z, TUER_EIGEN_INC) for k, z in kenn.items()}, True,
             TUER_EIGEN_INC),
            ("RE2-Tuerarchiv", RE2DOOR_REL, {"DOOR%02X.DO2" % nr: v for nr, v in re2.items()}, False,
             "Tuerzeilen, Griff-Tausch, Basis der Port-Archive")):
        ordner = os.path.join(repo, *rel.split("/"))
        da = set(os.listdir(ordner)) if os.path.isdir(ordner) else set()
        gut = 0
        for name in sorted(soll):
            z, grund = soll[name]
            p = os.path.join(ordner, name)
            if not os.path.isfile(p):
                befund("Quellbaum: Tuerarchiv", "%s fehlt: %s/%s (verlangt von: %s; ohne die Datei laeuft an diesen "
                       "Tueren still der RE1.5-Uebergang)" % (art, rel, name, grund))
                continue
            n = os.path.getsize(p)
            if n != z["datei"] or z["sektor"] * SEKTOR + z["modell"] != n or z["ton"] > z["sektor"] * SEKTOR:
                befund("Quellbaum: Tuerarchiv", "%s passt nicht zur Engine-Tabelle: %s/%s hat %d B, Tabelle %d B (Tonteil "
                       "%d, Modellteil %d ab Sektor %d) - die Engine verwirft es" % (art, rel, name, n, z["datei"], z["ton"],
                                                                                   z["modell"], z["sektor"]))
                continue
            if mit_fnv:
                h = _fnv1a32(p)
                if h != z["fnv"]:
                    befund("Quellbaum: Tuerarchiv", "%s passt nicht zur Engine-Tabelle: %s/%s FNV-1a %08x, Tabelle %08x - "
                           "die Engine verwirft es" % (art, rel, name, h, z["fnv"]))
                    continue
            gut += 1
        for name in sorted(da - set(soll)):
            befund("Quellbaum: Tuerarchiv", "%s/%s steht in keiner Engine-Tabelle (%s) - Ordner und Tabellen passen nicht "
                   "zusammen" % (rel, name, quelle))
        ergebnis.append((art, rel, len(soll), gut, mit_fnv))
    return ergebnis


def quellbaum_pruefen(repo, befund):
    """Alles am Quellbaum, was nicht die APK braucht: Baeume, Pflichtinhalt, Wurzel, Tuer-Soll.
    -> (quellen, zahl_quelle, tuer_ergebnis)"""
    quellen, zahl = quelldateien(repo, befund)
    wurzel_pruefen(repo, befund)
    return quellen, zahl, tueren_pruefen(repo, befund)


def _tuer_zeilen_drucken(tuer):
    for art, rel, soll, gut, mit_fnv in tuer:
        print("   Tuer-Soll: %-15s %-34s %d/%d wie die Engine-Tabelle (Groesse, Aufbau%s)"
              % (art, rel, gut, soll, ", FNV-1a" if mit_fnv else ""))


def paket_pruefen(repo, paket, max_zeilen):
    """PC-Paketordner gegen dieselbe Liste wie die APK (Befund B6: make_package.sh copy_common ist eine dritte
    Liste). Jede Datei der Baeume unter <paket>/<ziel>/ mit gleicher Groesse + sha256, unter <paket>/shared_assets
    und <paket>/synchro nichts sonst."""
    t0 = time.monotonic()
    befunde, reihenfolge, befund = _sammler()

    repo, paket = os.path.abspath(repo), os.path.abspath(paket)
    print("== APK-Asset-Gate: PC-Paket gegen die Asset-Liste (release/apk_asset_gate.py --paket) ==")
    print("   Paket:     %s" % paket)
    print("   Quellbaum: %s" % repo)
    n_baeume = gradle_baeume_pruefen(repo)
    print("   Baumliste: %d Baeume = build.gradle stageAssets (from/into/include geprueft)" % n_baeume)
    if not os.path.isdir(paket):
        raise Bedienfehler("Paketordner fehlt: %s" % paket)
    quellen, zahl_quelle, tuer = quellbaum_pruefen(repo, befund)
    _tuer_zeilen_drucken(tuer)
    gleich = {}
    for name in sorted(quellen):
        pfad, rel = quellen[name]
        ziel = os.path.join(paket, *name[len("assets/"):].split("/"))
        if not os.path.isfile(ziel):
            befund("fehlt im Paket", "fehlt im Paket: %s  (Quelle %s)" % (name[len("assets/"):], rel))
            continue
        q_sha, q_n = _sha_datei(pfad)
        p_sha, p_n = _sha_datei(ziel)
        if q_sha != p_sha:
            befund("Inhalt weicht ab (sha256)", "Inhalt weicht ab: %s  Quelle %d B %s.., Paket %d B %s.."
                   % (name[len("assets/"):], q_n, q_sha[:16], p_n, p_sha[:16]))
            continue
        gleich[name] = q_n
    for oben in ZIEL_OBEN:
        wurzel = os.path.join(paket, oben)
        for dp, dns, fns in os.walk(wurzel):
            dns.sort()
            for fn in sorted(fns):
                rel = os.path.relpath(os.path.join(dp, fn), paket).replace(os.sep, "/")
                if "assets/" + rel not in quellen:
                    befund("zusaetzlich im Paket", "zusaetzlich im Paket (kein Asset-Baum liefert es; make_package.sh "
                           "copy_common weicht von build.gradle/BAEUME ab?): %s" % rel)
    print("   %-28s %7s %7s" % ("Baum", "Quelle", "gleich"))
    for _b, _q, ziel, _m in BAEUME:
        pre = "assets/%s/" % ziel
        print("   %-28s %7d %7d" % (ziel, zahl_quelle.get(ziel, 0), sum(1 for n in gleich if n.startswith(pre))))
    print("   Laufzeit:  %.1f s" % (time.monotonic() - t0))
    n_befunde = sum(len(v) for v in befunde.values())
    _widerspruch_pruefen(n_befunde, len(gleich), len(quellen))
    return _befunde_ausgeben(befunde, reihenfolge, max_zeilen, "APK-ASSET-GATE-PAKET",
                             "%d Dateien in %d Baeumen bytegleich, nichts zusaetzlich" % (len(gleich), len(BAEUME)))


def nur_quellbaum(repo, max_zeilen):
    """--quellbaum: Baumliste, Baeume, Pflichtinhalt, Wurzel, Tuer-Soll - ohne APK und ohne Paket."""
    befunde, reihenfolge, befund = _sammler()

    repo = os.path.abspath(repo)
    print("== APK-Asset-Gate: Quellbaum (release/apk_asset_gate.py --quellbaum) ==")
    print("   Quellbaum: %s" % repo)
    n_baeume = gradle_baeume_pruefen(repo)
    print("   Baumliste: %d Baeume = build.gradle stageAssets (from/into/include geprueft)" % n_baeume)
    quellen, zahl_quelle, tuer = quellbaum_pruefen(repo, befund)
    for _b, _q, ziel, _m in BAEUME:
        print("   %-28s %7d Dateien" % (ziel, zahl_quelle.get(ziel, 0)))
    _tuer_zeilen_drucken(tuer)
    return _befunde_ausgeben(befunde, reihenfolge, max_zeilen, "APK-ASSET-GATE-QUELLBAUM",
                             "%d Dateien in %d Baeumen, Tuer-Soll erfuellt" % (len(quellen), len(BAEUME)))


def _widerspruch_pruefen(n_befunde, n_gleich, n_quellen):
    """Sicherheitsnetz: keine Befunde, aber nicht jede Quelldatei als gleich gezaehlt -> eine Pruefung hat still
    uebersprungen (zweiter Fehler) -> Rueckgabe 2 statt 0. Im Lauf unerreichbar; die inneren Proben pruefen es."""
    if not n_befunde and n_gleich != n_quellen:
        raise Bedienfehler("interner Widerspruch: %d Quelldateien, %d gleich, aber keine Befunde" % (n_quellen, n_gleich))


def _sammler():
    """-> (befunde {art: [text]}, reihenfolge [art], befund(art, text))"""
    befunde, reihenfolge = {}, []

    def befund(art, text):
        if art not in befunde:
            befunde[art] = []
            reihenfolge.append(art)
        befunde[art].append(text)
    return befunde, reihenfolge, befund


def _befunde_ausgeben(befunde, reihenfolge, max_zeilen, marke, ok_text):
    """Befunde je Art (begrenzt) ausgeben; '== <marke>-OK/-ABWEICHUNG ==' -> Rueckgabe 0/1."""
    n_befunde = sum(len(v) for v in befunde.values())
    if n_befunde:
        print("--- Abweichungen: %d ---" % n_befunde)
        for art in reihenfolge:
            liste = befunde[art]
            print("   [%s] %d" % (art, len(liste)))
            for t in liste[:max_zeilen]:
                print("      " + t)
            if len(liste) > max_zeilen:
                print("      ... und %d weitere" % (len(liste) - max_zeilen))
        print("== %s-ABWEICHUNG: %d Befunde ==" % (marke, n_befunde))
        return RC_ABWEICHUNG
    print("== %s-OK: %s ==" % (marke, ok_text))
    return RC_GLEICH


# =============================================================================================
# APK roh lesen (wie libziparchive; Befund B1 der Gegenpruefung R1)
# =============================================================================================
class _Eintrag(object):
    """Ein Zentralverzeichnis-Eintrag; name/daten_off/lesbar setzt struktur_pruefen."""
    __slots__ = ("nr", "name_roh", "flags", "methode", "crc", "csize", "usize", "lho",
                 "name", "daten_off", "lesbar")

    def __init__(self, nr, name_roh, flags, methode, crc, csize, usize, lho):
        self.nr, self.name_roh, self.flags, self.methode = nr, name_roh, flags, methode
        self.crc, self.csize, self.usize, self.lho = crc, csize, usize, lho
        self.name, self.daten_off, self.lesbar = None, None, False


def zip_verzeichnis(pfad):
    """End-of-Central-Directory + Zentralverzeichnis roh -> (cd_off, [ _Eintrag ]).
    Wie libziparchive (MapCentralDirectory0/ParseZipArchive), teils strenger. Ist schon die
    Grundstruktur unlesbar, gibt es keine Aussage ueber Eintraege -> Bedienfehler (Rueckgabe 2)."""
    groesse = os.path.getsize(pfad)
    with open(pfad, "rb") as f:
        n_ende = min(groesse, EOCD_LEN + 0xFFFF)
        f.seek(groesse - n_ende)
        ende = f.read(n_ende)
        i = ende.rfind(EOCD_SIG)          # libziparchive: die LETZTE Signatur (Suche von hinten)
        if i < 0 or n_ende - i < EOCD_LEN:
            raise Bedienfehler("APK nicht lesbar: %s (kein End-of-Central-Directory - kein ZIP oder "
                               "abgeschnitten)" % pfad)
        (_sig, disk, cd_disk, n_hier, n_ges, cd_groesse, cd_off, kom) = struct.unpack(
            "<4sHHHHIIH", ende[i:i + EOCD_LEN])
        eocd_pos = groesse - n_ende + i
        rest = groesse - eocd_pos - EOCD_LEN - kom
        if rest > 0:
            raise Bedienfehler("APK nicht lesbar: %s (%d Bytes hinter dem End-of-Central-Directory - "
                               "libziparchive lehnt die Datei ab)" % (pfad, rest))
        if rest < 0:
            raise Bedienfehler("APK nicht lesbar: %s (Kommentarlaenge %d des End-of-Central-Directory reicht %d B "
                               "ueber das Dateiende - libziparchive lehnt die Datei ab)" % (pfad, kom, -rest))
        f.seek(0)
        if f.read(4) != LFH_SIG:
            raise Bedienfehler("APK nicht lesbar: %s (beginnt nicht mit einem Local Header - Bytes vor dem ersten "
                               "Eintrag? libziparchive: 'Entry at offset zero has invalid LFH signature')" % pfad)
        f.seek(max(0, eocd_pos - 20))
        z64 = f.read(4) == Z64_LOC_SIG
        if (z64 or 0xFFFF in (n_hier, n_ges) or 0xFFFFFFFF in (cd_groesse, cd_off)
                or disk or cd_disk or n_hier != n_ges):
            raise Bedienfehler("APK nicht lesbar: %s (ZIP64/mehrteiliges Archiv - so baut AGP die APK nicht, "
                               "das Gate liest es nicht)" % pfad)
        if cd_off + cd_groesse > eocd_pos:
            raise Bedienfehler("APK nicht lesbar: %s (Zentralverzeichnis ausserhalb der Datei: Offset %d + "
                               "%d B > End-of-Central-Directory bei %d)" % (pfad, cd_off, cd_groesse, eocd_pos))
        f.seek(cd_off)
        cd = f.read(cd_groesse)
    eintraege, p = [], 0
    for nr in range(n_ges):
        if p + CD_LEN > len(cd) or cd[p:p + 4] != CD_SIG:
            raise Bedienfehler("APK nicht lesbar: %s (Zentralverzeichnis kaputt bei Eintrag %d von %d, "
                               "Datei-Offset %d)" % (pfad, nr + 1, n_ges, cd_off + p))
        (flags, methode, crc, csize, usize, nlen, xlen, klen, lho) = struct.unpack(
            "<8xHH4xIIIHHH8xI", cd[p:p + CD_LEN])
        eintraege.append(_Eintrag(nr, cd[p + CD_LEN:p + CD_LEN + nlen], flags, methode, crc, csize, usize, lho))
        p += CD_LEN + nlen + xlen + klen
    if p != len(cd):
        raise Bedienfehler("APK nicht lesbar: %s (Zentralverzeichnis: %d B laut End-of-Central-Directory, "
                           "die %d Eintraege belegen %d B - Rest oder Ueberlauf; ein anderer Leser saehe "
                           "andere Eintraege)" % (pfad, len(cd), n_ges, p))
    return cd_off, eintraege


def _name_fehler(roh):
    """'' = Name so, wie Android ihn findet; sonst der Grund (libziparchive + strenger)."""
    if b"\x00" in roh:
        return "NUL-Byte im Namen (libziparchive: 'Invalid entry name' - die GANZE APK ist unlesbar)"
    try:
        s = roh.decode("utf-8")
    except UnicodeDecodeError:
        return "Eintragsname kein gueltiges UTF-8 (libziparchive: 'Invalid entry name' - die GANZE APK ist unlesbar)"
    if "\\" in s:
        return ("'\\' im Namen (Android liest ihn woertlich - unter dem '/'-Pfad findet kein Leser den "
                "Eintrag; zipfile ersetzte ihn unter Windows still durch '/')")
    if any(ord(c) < 0x20 or ord(c) == 0x7F for c in s):
        return "Steuerzeichen im Namen"
    teile = s.split("/")
    if any(t in (".", "..") for t in teile) or "" in teile[:-1]:     # absolut: der erste Teil ist ""
        return "absoluter Pfad, '.'/'..' oder '//' im Namen"
    return ""


def struktur_pruefen(fa, cd_off, eintraege, befund):
    """Je Eintrag: Name, Local Header, Lage, Flags/Methode. Setzt name/daten_off/lesbar.
    -> {name: [_Eintrag, ...]} aller Eintraege mit gueltigem Namen."""
    je_name = {}
    for e in eintraege:
        grund = _name_fehler(e.name_roh)
        if grund:
            befund("APK-Struktur: Eintragsname", "unzulaessiger Eintragsname %r: %s" % (e.name_roh, grund))
            continue
        e.name = e.name_roh.decode("utf-8")
        je_name.setdefault(e.name, []).append(e)
        fa.seek(e.lho)
        kopf = fa.read(LFH_LEN)
        if len(kopf) < LFH_LEN or kopf[:4] != LFH_SIG:
            befund("APK-Struktur: Local Header", "Local Header fehlt bei Offset %d: %s" % (e.lho, e.name))
            continue
        (l_flags, _l_meth, l_crc, l_csize, l_usize, l_nlen, l_xlen) = struct.unpack("<6xHH4xIIIHH", kopf)
        l_name = fa.read(l_nlen)
        if l_name != e.name_roh:
            befund("APK-Struktur: Local Header", "Name im Local Header weicht vom Zentralverzeichnis ab: %s "
                   "(Local Header %r) - libziparchive: 'Inconsistent information'" % (e.name, l_name))
            continue
        if (e.flags | l_flags) & FLAG_DATA_DESCRIPTOR:
            # Nachbesserung R2 (Gegenpruefung B1/M14): mit dem Bit nimmt libziparchive CRC/Groessen aus einem
            # Data Descriptor HINTER den Daten statt aus dem Local Header; AGP setzt es nie (v0.8.19: 0 von 3616)
            befund("APK-Struktur: Methode/Flags", "Data-Descriptor-Bit gesetzt (%s): %s - AGP setzt es nie; das "
                   "Gate liest nur Eintraege mit CRC/Groessen im Local Header" % (
                       "Local Header und Zentralverzeichnis" if (e.flags & l_flags & FLAG_DATA_DESCRIPTOR) else
                       ("Local Header" if l_flags & FLAG_DATA_DESCRIPTOR else "nur Zentralverzeichnis"), e.name))
            continue
        if (l_crc, l_csize, l_usize) != (e.crc, e.csize, e.usize):
            befund("APK-Struktur: Local Header", "CRC/Groessen im Local Header weichen vom Zentralverzeichnis "
                   "ab: %s (Local Header %08x/%d/%d, Zentralverzeichnis %08x/%d/%d) - libziparchive: "
                   "'size/crc32 mismatch ... Inconsistent information'"
                   % (e.name, l_crc, l_csize, l_usize, e.crc, e.csize, e.usize))
            continue
        e.daten_off = e.lho + LFH_LEN + l_nlen + l_xlen
        if e.daten_off + e.csize > cd_off:
            befund("APK-Struktur: Lage", "Daten ragen ins Zentralverzeichnis: %s (Daten %d + %d B > "
                   "Zentralverzeichnis bei %d) - libziparchive: 'bad ... length'" % (e.name, e.daten_off, e.csize, cd_off))
            continue
        if (e.flags | l_flags) & FLAG_VERSCHLUESSELT:
            befund("APK-Struktur: Methode/Flags", "verschluesselter Eintrag: %s" % e.name)
            continue
        if e.methode not in (0, 8):
            befund("APK-Struktur: Methode/Flags", "Methode %d (lesbar sind 0 = Stored, 8 = Deflate): %s"
                   % (e.methode, e.name))
            continue
        e.lesbar = True
    for name in sorted(je_name):
        if len(je_name[name]) > 1:
            befund("APK: doppelt/Verzeichnis", "doppelter Eintrag in der APK (%dx): %s - libziparchive: "
                   "'Duplicate entry'" % (len(je_name[name]), name))
    return je_name


def _eintrag_lesen(fa, e, sammeln=False):
    """Liest e wie das Geraet: ab dem Datenoffset aus dem LOCAL Header csize Bytes, entpackt.
    -> (sha256_hex, crc32, n_bytes, daten|None); _Lesefehler bei kaputtem Strom."""
    h, st = hashlib.sha256(), [0, 0]            # st = [crc32, n]
    teile = [] if sammeln else None

    def nimm(b):
        if not b:
            return
        st[1] += len(b)
        if st[1] > e.usize:
            raise _Lesefehler("entpackt mehr als die %d B des Zentralverzeichnisses" % e.usize)
        h.update(b)
        st[0] = zlib.crc32(b, st[0])
        if teile is not None:
            teile.append(b)

    d = zlib.decompressobj(-15) if e.methode else None      # nur 0/8 kommen hierher (struktur_pruefen)
    fa.seek(e.daten_off)
    rest = e.csize
    try:
        while rest:
            b = fa.read(min(rest, BLOCK))
            if not b:
                # defensiv, unerreichbar: struktur_pruefen verlangt daten_off + csize <= Zentralverzeichnis
                raise _Lesefehler("Datei endet mitten im Eintrag")
            rest -= len(b)
            if d is None:
                nimm(b)
                continue
            nimm(d.decompress(b, BLOCK))
            while d.unconsumed_tail:
                nimm(d.decompress(d.unconsumed_tail, BLOCK))
        if d is not None:
            nimm(d.flush())
            if not d.eof or d.unused_data:
                raise _Lesefehler("Deflate-Strom endet nicht genau am Eintragsende (%s)" % (
                    "unvollstaendig" if not d.eof else "%d B dahinter" % len(d.unused_data)))
    except zlib.error as ex:
        raise _Lesefehler("Deflate-Strom kaputt: %s" % ex)
    return h.hexdigest(), st[0] & 0xFFFFFFFF, st[1], (b"".join(teile) if teile is not None else None)


def _eintrag_pruefen(fa, e, befund):
    """Eintrag lesen, CRC32 + Laenge gegen das Zentralverzeichnis. -> (sha256, n) oder None."""
    try:
        sha, crc, n, _ = _eintrag_lesen(fa, e)
    except _Lesefehler as ex:
        befund("APK: beschaedigt (CRC)", "APK-Eintrag beschaedigt (CRC/Entpacken): %s: %s" % (e.name, ex))
        return None
    if crc != e.crc or n != e.usize:
        befund("APK: beschaedigt (CRC)", "APK-Eintrag beschaedigt (CRC/Entpacken): %s: gelesen %d B, CRC %08x - "
               "Zentralverzeichnis %d B, CRC %08x" % (e.name, n, crc, e.usize, e.crc))
        return None
    return sha, n


def _sha_datei(pfad):
    h, n = hashlib.sha256(), 0
    with open(pfad, "rb") as f:
        while True:
            b = f.read(BLOCK)
            if not b:
                return h.hexdigest(), n
            h.update(b)
            n += len(b)


def _manifest_grenze():
    """64 MiB wie das Geraet. RE15_GATE_MANIFEST_MAX (Pruefhaken des Selbsttests, Nachbesserung R2) kann die
    Grenze nur SENKEN - so laesst sich die Grenze mit einem kleinen Manifest beidseitig pruefen."""
    w = os.environ.get("RE15_GATE_MANIFEST_MAX", "")
    return min(MANIFEST_MAX, int(w)) if w.isdigit() else MANIFEST_MAX


def _t(b, n=120):
    """Bytes fuer eine Meldung (gekuerzt, nicht darstellbares ersetzt)."""
    return b[:n].decode("utf-8", "replace")


def _pfad_fehler(p):
    """Pfadregel der Liste v2 auf BYTES - dieselben Regeln wie asset_abgleich.c re15_abgleich_pfad_ok.
    -> None (zulaessig) oder der Grund."""
    if not p:
        return "leer"
    if len(p) > PFAD_MAX:
        return "%d Bytes > %d" % (len(p), PFAD_MAX)
    if any(c < 0x20 or c == 0x7f for c in p):
        return "Steuerzeichen"
    if b"\\" in p:
        return "'\\'"
    teile = p.split(b"/")
    if len(teile) < 2:
        return "ohne '/' (Assets liegen nie direkt im Speicherordner)"
    if any(t in (b"", b".", b"..") for t in teile):
        return "absolut, '//', '/' am Ende, '.' oder '..' - landet ausserhalb des Ankers"
    if p[-len(NEU_ENDUNG):].translate(_ASCII_KLEIN) == NEU_ENDUNG:
        return "endet auf .neu (Endung der Zwischendatei beim Entpacken)"
    try:
        p.decode("utf-8")
    except UnicodeDecodeError:
        return "kein gueltiges UTF-8"
    return None


def manifest_lesen(roh, grenze=MANIFEST_MAX):
    """re15_assets.txt (Format v2) nach DENSELBEN Regeln lesen wie das Geraet
    (re15_port/platform/android/jni/asset_abgleich.c re15_abgleich_lesen; Kopf von asset_abgleich.h).
    Das Geraet verwirft bei der ERSTEN Abweichung die ganze Liste und entpackt nichts; hier werden alle
    Abweichungen gesammelt. Es gilt: das Geraet nimmt die Liste an <=> fehler ist leer.
    -> (eintraege {pfad: (groesse, sha256)}, zeile_von {pfad: nr}, v1, fehler [text])"""
    fehler = []
    if len(roh) > grenze:
        fehler.append("Manifest %d B > %d B (64 MiB): das Geraet liest es nicht (asset_abgleich.h "
                      "RE15_ABGLEICH_LISTE_MAX)" % (len(roh), grenze))
    if b"\0" in roh:
        fehler.append("Manifest enthaelt ein NUL-Byte (Stelle %d)" % roh.index(b"\0"))
    if roh.startswith(b"\xef\xbb\xbf"):
        fehler.append("Manifest beginnt mit BOM: Kopfzeile auf dem Geraet unlesbar")
    try:
        roh.decode("utf-8")
    except UnicodeDecodeError as e:
        fehler.append("Manifest ist kein gueltiges UTF-8 (%s) - Pfade treffen die APK-Namen nicht" % e)

    zeilen = roh.split(b"\n")                      # nur '\n' trennt (asset_abgleich.c: strchr '\n')
    kopf = zeilen[0].rstrip(b"\r")                 # angehaengte '\r' weg
    m = KOPF_RE.fullmatch(kopf)
    if not m and KOPF_V1_RE.fullmatch(kopf):
        fehler.append("Manifest im alten Format v1 ('%s', bis v0.8.19: ohne sha256) - das Geraet lehnt es ab "
                      "und entpackt NICHTS (RE15_ABGLEICH_ALTES_FORMAT); erwartet Format v2 "
                      "'# re15 assets v2 <anzahl> <bytes>' + '<bytes>\\t<sha256>\\t<pfad>'" % _t(kopf, 80))
        return {}, {}, True, fehler
    if not m:
        fehler.append("Manifest-Kopfzeile fehlt/unlesbar: '%s' (erwartet '# re15 assets v2 <anzahl> <bytes>', "
                      "build.gradle writeAssetManifest)" % _t(kopf, 80))
    # Geraet: Zeile 1 ist IMMER die Kopfzeile. Beginnt sie nicht mit '#', wird sie hier zusaetzlich als
    # Datenzeile gelesen - nur fuer die Meldungen (das Urteil "ungueltig" steht dann schon fest).
    daten = zeilen[1:] if kopf.startswith(b"#") else zeilen
    eintraege, zeile_von, summe = {}, {}, 0
    for nr, z in enumerate(daten, len(zeilen) - len(daten) + 1):
        z = z.rstrip(b"\r")
        if not z:
            continue                                # leere Zeile: uebersprungen
        if z.startswith(b"#"):
            fehler.append("Manifest-Zeile %d: unerwartete Kommentarzeile '%s' (der Schreiber schreibt nur die "
                          "Kopfzeile)" % (nr, _t(z, 80)))
            continue
        teile = z.split(b"\t", 2)
        if len(teile) < 3:
            fehler.append("Manifest-Zeile %d: erwartet '<bytes>\\t<sha256>\\t<pfad>', %d Tab(s): '%s'"
                          % (nr, len(teile) - 1, _t(z)))
            continue
        groesse, sha, pfad = teile
        if not GROESSE_RE.fullmatch(groesse):
            fehler.append("Manifest-Zeile %d: Groessenfeld '%s' ist keine Zahl (1-18 Ziffern 0-9)" % (nr, _t(groesse, 40)))
            continue
        if not SHA_RE.fullmatch(sha):
            fehler.append("Manifest-Zeile %d: Pruefsumme '%s' ist kein sha256 (genau 64 Zeichen 0-9a-f, "
                          "Kleinbuchstaben)" % (nr, _t(sha, 70)))
            continue
        grund = _pfad_fehler(pfad)
        if grund:
            fehler.append("Manifest-Zeile %d: unzulaessiger Pfad '%s' (%s)" % (nr, _t(pfad), grund))
            continue
        p = pfad.decode("utf-8")
        if p in eintraege:
            fehler.append("Manifest nennt %s mehrfach (Zeilen %d und %d)" % (p, zeile_von[p], nr))
            continue
        eintraege[p] = (int(groesse), sha.decode("ascii"))
        zeile_von[p] = nr
        summe += int(groesse)
        if summe > (1 << 63) - 1:
            fehler.append("Manifest-Zeile %d: Summe der Groessen > 2^63-1 (asset_abgleich.c: laeuft ueber)" % nr)
            summe = -(1 << 70)                      # nur einmal melden; Kopfzeile passt dann sicher nicht
    if not eintraege and m:
        fehler.append("Manifest ohne Dateien (das Geraet entpackt dann nichts)")
    if m:
        n_kopf, b_kopf = int(m.group(1)), int(m.group(2))
        n_ist, b_ist = len(eintraege), sum(g for g, _s in eintraege.values())
        if (n_kopf, b_kopf) != (n_ist, b_ist):
            fehler.append("Manifest-Kopfzeile passt nicht: nennt %d Dateien / %d Bytes, die Zeilen ergeben %d / %d"
                          % (n_kopf, b_kopf, n_ist, b_ist))
    klein = {}
    for p in eintraege:                             # der App-Speicher ist case-insensitiv (Dossier N1, 1.3)
        klein.setdefault(p.encode("utf-8").translate(_ASCII_KLEIN), []).append(p)
    for gruppe in sorted(v for v in klein.values() if len(v) > 1):
        fehler.append("Manifest: Pfade nur in Gross/klein verschieden: %s (auf dem Geraet EINE Datei)"
                      % " / ".join(sorted(gruppe)))
    return eintraege, zeile_von, False, fehler


def manifest_pruefen(roh, apk_dateien, befund):
    """roh = Bytes von assets/re15_assets.txt; apk_dateien = {rel: (groesse, sha256|None)} (ohne Manifest;
    sha256 None = Eintrag nicht lesbar, der Grund steht schon unter "APK: beschaedigt")."""
    eintraege, _zeile_von, v1, fehler = manifest_lesen(roh, _manifest_grenze())
    for text in fehler:
        befund("Manifest", text)
    if v1:
        return 0, 0
    for pfad in sorted(eintraege):
        m_groesse, m_sha = eintraege[pfad]
        if pfad not in apk_dateien:
            befund("Manifest: Zeile ohne APK-Eintrag",
                   "Manifest nennt %s (%d B), die APK hat keinen Eintrag assets/%s" % (pfad, m_groesse, pfad))
            continue
        a_groesse, a_sha = apk_dateien[pfad]
        if a_groesse != m_groesse:
            befund("Manifest: falsche Groesse",
                   "Manifest-Groesse falsch: %s Manifest %d B, APK %d B (das Geraet verwirft die entpackte Datei, "
                   "android_glue.c entpacken)" % (pfad, m_groesse, a_groesse))
        elif a_sha is not None and a_sha != m_sha:
            befund("Manifest: falsche Pruefsumme",
                   "Manifest-Pruefsumme falsch: %s Manifest %s.., APK %s.. (das Geraet verwirft die entpackte "
                   "Datei, android_glue.c entpacken)" % (pfad, m_sha[:16], a_sha[:16]))
    for pfad in sorted(apk_dateien):
        if pfad not in eintraege:
            befund("Manifest: Datei fehlt im Manifest",
                   "fehlt im Manifest: %s (wird auf dem Geraet nie entpackt)" % pfad)
    return len(eintraege), sum(g for g, _s in eintraege.values())


def pruefen(repo, apk, max_zeilen):
    t0 = time.monotonic()
    befunde, reihenfolge, befund = _sammler()

    repo = os.path.abspath(repo)
    print("== APK-Asset-Gate (release/apk_asset_gate.py) ==")
    print("   APK:       %s" % os.path.abspath(apk))
    print("   Quellbaum: %s" % repo)

    n_baeume = gradle_baeume_pruefen(repo)
    print("   Baumliste: %d Baeume = build.gradle stageAssets (from/into/include geprueft)" % n_baeume)

    if not os.path.isfile(apk):
        raise Bedienfehler("APK fehlt: %s" % apk)
    cd_off, eintraege = zip_verzeichnis(apk)

    quellen, zahl_quelle, tuer = quellbaum_pruefen(repo, befund)
    _tuer_zeilen_drucken(tuer)
    man_name = "assets/" + MANIFEST

    with open(apk, "rb") as fa:
        je_name = struktur_pruefen(fa, cd_off, eintraege, befund)
        print("   ZIP-Struktur: %d Eintraege roh gelesen, %d fuer Android lesbar (Name, Local Header = "
              "Zentralverzeichnis, Lage, Methode)" % (len(eintraege), sum(1 for e in eintraege if e.lesbar)))
        asset_infos = {}
        for name, liste in je_name.items():
            if not name.startswith("assets/"):
                continue
            if name.endswith("/"):
                befund("APK: doppelt/Verzeichnis", "Verzeichniseintrag in der APK: %s" % name)
                continue
            asset_infos[name] = liste[-1]

        # (a)+(b) jede Quelldatei in der APK, Groesse + CRC32 + sha256 (in APK-Reihenfolge lesen)
        gleich = {}
        bytes_gleich = 0
        n_komprimiert = {}
        gelesen, kaputt = set(), set()
        apk_sha = {}                                  # Name -> sha256 der Daten (fuer die Manifest-Pruefsummen)

        def lage(name):
            e = asset_infos.get(name)
            return (e.lho if e is not None else -1, name)

        for name in sorted(quellen, key=lage):
            pfad, rel = quellen[name]
            e = asset_infos.get(name)
            if e is None:
                hinweis = _auslass_hinweis(name[len("assets/"):])
                befund("fehlt in der APK", "fehlt in der APK: %s  (Quelle %s)%s"
                       % (name, rel, ("  - " + hinweis) if hinweis else ""))
                continue
            if not e.lesbar:
                continue                              # Grund steht unter "APK-Struktur"
            q_groesse = os.path.getsize(pfad)
            if e.usize != q_groesse:
                befund("Groesse weicht ab", "Groesse weicht ab: %s  Quelle %d B, APK %d B"
                       % (name, q_groesse, e.usize))
                continue
            gelesen.add(e.nr)
            r = _eintrag_pruefen(fa, e, befund)
            if r is None:
                kaputt.add(e.nr)
                continue
            a_sha, a_n = r
            apk_sha[name] = a_sha
            q_sha, q_n = _sha_datei(pfad)
            if a_sha != q_sha:
                befund("Inhalt weicht ab (sha256)", "Inhalt weicht ab (sha256, %s): %s  Quelle %s.., APK %s.."
                       % ("gleiche Groesse %d B" % q_n, name, q_sha[:16], a_sha[:16]))
            else:
                gleich[name] = q_n
                bytes_gleich += q_n
            if e.methode:
                n_komprimiert[name] = e.methode

        # (b2) alle uebrigen lesbaren Eintraege (lib/, classes.dex, Manifest, ...): CRC32 + Laenge
        for e in sorted((x for x in eintraege if x.lesbar and x.nr not in gelesen), key=lambda x: x.lho):
            r = _eintrag_pruefen(fa, e, befund)
            if r is None:
                kaputt.add(e.nr)
            elif asset_infos.get(e.name) is e:
                apk_sha[e.name] = r[0]

        # (c) nichts Zusaetzliches unter assets/
        for name in sorted(asset_infos):
            if name != man_name and name not in quellen:
                befund("zusaetzlich in der APK", "zusaetzlich in der APK (kein Asset-Baum liefert ihn): %s" % name)

        # (d) Manifest
        n_man = b_man = None
        e_man = asset_infos.get(man_name)
        if e_man is None:
            befund("Manifest", "Manifest fehlt in der APK: %s (das Geraet entpackt dann NICHTS, "
                               "android_glue.c apk_datei_lesen)" % man_name)
        elif e_man.lesbar and e_man.nr not in kaputt:
            roh = _eintrag_lesen(fa, e_man, sammeln=True)[3]
            apk_dateien = {n[len("assets/"):]: (x.usize, apk_sha.get(n)) for n, x in asset_infos.items() if n != man_name}
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
            a = asset_infos[name].usize if name in asset_infos else None
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
    _widerspruch_pruefen(n_befunde, len(gleich), len(quellen))
    return _befunde_ausgeben(befunde, reihenfolge, max_zeilen, "APK-ASSET-GATE",
                             "%d Dateien in %d Baeumen bytegleich, Tuer-Soll erfuellt, Manifest stimmt, ZIP-Struktur "
                             "wie Android sie liest" % (len(gleich), len(BAEUME)))


# =============================================================================================
# Selbsttest: gute APKs annehmen, JEDE Faelschung ablehnen
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
    ("re15_port/shared_assets/extracted_fx/A~B C.tim", 300),         # Leerzeichen und '~' sind erlaubt
    ("re15_port/shared_assets/extracted_fx/gr\u00fcn.tim", 310),     # UTF-8-Name ist erlaubt
    ("re15_port/shared_assets/RE2/CDEMD0.EMS", 1200),
    ("re15_port/shared_assets/RE2/TORSE.VBS", 900),
    # Tuerarchive: Groessen = Aufbau der Mini-Engine-Tabellen unten (Sektor * 0x800 + Modellteil)
    ("re15_port/shared_assets/RE2/DOOR/DOOR07.DO2", 2548),      # Tuerzeile, Basis P07G
    ("re15_port/shared_assets/RE2/DOOR/DOOR09.DO2", 2148),      # NUR Griff-Spender (Runde-31-Tausch)
    ("re15_port/shared_assets/RE2/DOOR/DOOR0C.DO2", 400),       # NUR Spender ueber den Rueckfall in die Port-Tabelle
    ("re15_port/shared_assets/RE2/DOOR/DOOR13.DO2", 2648),      # Tuerzeile, G12-Zeile, Basis P2DS
    ("re15_port/shared_assets/RE15DOOR/P07G.DO2", 2748),
    ("re15_port/shared_assets/RE15DOOR/P2DS.DO2", 4396),
    ("synchro/STAGE1/room1170/main00.wav", 2000),
    ("synchro/STAGE2/room2000/main00.wav", 2100),
    ("synchro/unused/STAGE1/room1170/alt.mp3", 800),     # ausserhalb include "STAGE*/**"
    ("synchro/README.md", 100),                          # ausserhalb include "STAGE*/**"
)
# Datei > BLOCK: nur die "gross"-Faelle (sonst kaeme ein Vergleich nur ueber den 1. Block durch)
_GROSS = ("re15_port/shared_assets/PSX/MOVIE/GROSS.STR", BLOCK + 4096 + 37)
# Manifest-Grenze DES GERAETS, als eigene Zahl der Fixture (Runde 4, Gegenpruefung R3 MU4; seit Runde 34a N1:
# asset_abgleich.h RE15_ABGLEICH_LISTE_MAX (64u << 20), android_glue.c apk_datei_lesen verwirft groessere). Bewusst NICHT die Konstante des Gates - sonst
# wanderte ein Fall mit einer verschobenen Konstante (64 << 30) einfach mit und bestaetigte sich selbst.
_FX_GERAET_MANIFEST = 64 * 1024 * 1024
# wie AGP: Manifest/dex komprimiert (Deflate), .so und resources.arsc Stored (Nachbesserung R2, M16:
# vorher waren ALLE Nicht-Assets Stored - ein Gate, das Deflate-Eintraege ausserhalb assets/ nie las,
# bestand den Selbsttest)
_NICHT_ASSETS = (("AndroidManifest.xml", b"<manifest package='de.re15.port'/>" * 20, zipfile.ZIP_DEFLATED),
                 ("classes.dex", b"dex\n035\0" + b"".join(hashlib.sha256(b"dex%d" % k).digest() for k in range(94)),
                  zipfile.ZIP_DEFLATED),
                 ("lib/arm64-v8a/libmain.so", b"\x7fELF" + bytes(range(256)) * 8, zipfile.ZIP_STORED),
                 ("resources.arsc", b"\x02\x00" + bytes(300), zipfile.ZIP_STORED))

# --- Mini-Engine-Tabellen der Tuerarchive (Nachbesserung R2, B2). Aufbau wie die echten Dateien
# (include/re15_door_seq.h, engine/src/gen/*.inc); die Werte fuellt _Fall.schreiben ein - FNV-1a mit
# eigener Umsetzung (_fx_fnv1a32), nicht mit der des Gates.
_TUER_KOPF_MUSTER = r'''/* Mini-Kopf fuer den Selbsttest: dieselben typedefs wie include/re15_door_seq.h */
#include <stdint.h>   // Zeilenkommentar mit Klammer { und "Anfuehrungszeichen
#define RE15_DOOR_KEIN_SPENDER  0xFFu
typedef struct {
    uint16_t raum;        /* volle Raum-Id {Kommentar mit Klammern} */
    uint8_t  form;
    uint8_t  band;
    int32_t  x, z, hw, hh;
    int16_t  qx[4], qz[4];
    uint8_t  re2_nr;
    uint8_t  variante;
    uint8_t  bit7;
    uint8_t  spender;
    uint16_t seite, tuer;
    uint32_t off;
    uint8_t  eigen;
} re15_tuer_zeile_t;
typedef struct {
    char     kennung[8];
    uint8_t  basis;
    uint16_t ton, modell;
    uint32_t sektor, datei;
    uint32_t fnv;
    uint8_t  md1_eigen;
    char     reserve;     /* nur im Selbsttest: skalares char */
} re15_tuer_eigen_t;
typedef struct {
    uint8_t  archiv, spender;
    uint8_t  mesh_archiv, mesh_spender;
    uint16_t rot_vorn[3], rot_hinten[3];
    int16_t  aus_archiv, aus_spender;
    uint8_t  spender_eigen;
    int16_t  versatz_vorn[3], versatz_hinten[3];
    uint8_t  fuer_eigen;
} re15_griff_tausch_t;
'''
# Tuerzeilen Runde 31: A = DOOR07; B = DOOR13 mit Spender DOOR09 (Tausch in re15_griff_tausche);
# F = DOOR07 mit Spender DOOR0C - der Tausch steht NUR in der Port-Tabelle (Rueckfall :168-171)
_TUER_ZUORDNUNG_MUSTER = r'''/* Mini: gen/tuer_zuordnung.inc - Kommentar mit } und "{" */
static const re15_tuer_zeile_t re15_tuer_zeilen[3] = {
    /* A */ { 0x1000, 0, 0, 100, 200, 500, 1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 0, 2, 0x00BBE },
    /* B */ { 0x1020, 0, 0, -2750, -7250, 750, 1150, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x13, 0, 0, 0x09, 5, 3, 0x01C82 },
    /* F */ { 0x1030, 1, 0, 0, 0, 0, 0, {1, 2, 3, 4}, {5, 6, 7, 8}, 0x07, 0, 0, 0x0C, 9, 7, 0x01C8A },
};
static const re15_griff_tausch_t re15_griff_tausche[1] = {
    { 0x13, 0x09, 1, 1, {0, 0, 0}, {2048, 2048, 0}, 1500, -702 },
};
'''
# Port-Archive (FNV/Groessen per %-Platzhalter), Port-Zeilen C (P07G), D (P2DS, Spender DOOR0A ueber den
# Selbst-Tausch: DOOR0A liegt NICHT im Baum und darf nicht verlangt werden), G (G12 objektlos DOOR13);
# Griff-Tausch E1 (fuer_eigen 1, spender_eigen 0 -> wuerde DOOR0A verlangen, gilt aber nicht fuer D),
# E2 (fuer_eigen 2, Selbst-Tausch), E3 (fuer_eigen 1, DOOR07 <- DOOR0C, Rueckfall fuer Zeile F); Zeile H (P07G,
# Spender DOOR0A) findet KEINEN Tausch: E4 passt, gilt aber fuer_eigen 2; E5 hat Spender 0xFF (= keiner) -
# wer fuer_eigen nicht genau vergleicht oder 0xFF-Zeilen nachschlaegt, verlangt DOOR0A bzw. DOOR FF
_TUER_EIGEN_MUSTER = r'''/* Mini: gen/re15_tuer_eigen.inc */
static const re15_tuer_eigen_t re15_tuer_eigen[2] = {
    { "P07G", 0x07, %(P07G_ton)d, %(P07G_modell)d, %(P07G_sektor)d, %(P07G_datei)d, 0x%(P07G_fnv)08Xu, 0 },
    { "P2DS", 0x13, %(P2DS_ton)d, %(P2DS_modell)d, %(P2DS_sektor)d, %(P2DS_datei)d, 0x%(P2DS_fnv)08Xu, 1 },
};
static const re15_tuer_zeile_t re15_tuer_zeilen_eigen[6] = {
    /* C */ { 0x1000, 0, 0, 22850, -13400, 500, 1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 0, 2, 0x00BBE, 1 },
    /* D */ { 0x5060, 0, 0, -26300, -11600, 800, 1800, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x13, 0, 0, 0x0A, 273, 142, 0x02AAE, 2 },
    /* G */ { 0x4080, 0, 0, 1, 2, 3, 4, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x13, 0, 0, 0xFF, 7, 8, 0x0100, 0 },
    /* H */ { 0x1070, 0, 0, 5, 6, 7, 8, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0x0A, 11, 12, 0x0200, 1 },
    /* I */ { 0x1071, 0, 0, 5, 6, 7, 8, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0x0B, 13, 14, 0x0210, 1 },
    /* J */ { 0x1072, 0, 0, 5, 6, 7, 8, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 0, 0, 0x0D, 15, 16, 0x0220, 1 },
};
static const re15_griff_tausch_t re15_griff_tausche_eigen[5] = {
    /* E1 */ { 0x13, 0x0A, 1, 1, {0, 0, 0}, {2048, 2048, 0}, 0, -450, 0, {0, -584, 458}, {0, -1064, 418}, 1 },
    /* E2 */ { 0x13, 0x0A, 1, 1, {0, 0, 0}, {2048, 2048, 0}, 0, -450, 2, {0, 0, 0}, {0, 0, 0}, 2 },
    /* E3 */ { 0x07, 0x0C, 1, 1, {0, 0, 0}, {2048, 2048, 0}, -245, -702, 0, {0, 0, 0}, {0, 0, 0}, 1 },
    /* E4 */ { 0x07, 0x0A, 1, 1, {0, 0, 0}, {0, 0, 0}, 0, 0, 0, {0, 0, 0}, {0, 0, 0}, 2 },
    /* E5 */ { 0x07, 0xFF, 1, 1, {0, 0, 0}, {0, 0, 0}, 0, 0, 0, {0, 0, 0}, {0, 0, 0}, 0 },
};
static const uint16_t re15_tuer_geplant[2] = { 0, 1 };
'''
# RE2-Tabelle: 20 Zeilen (DOOR00..DOOR13), belegt 07, 09, 0A, 0C, 13 - die uebrigen {0} wie ein leerer Platz
_TUER_RE2_MUSTER = r'''/* Mini: gen/re2_tuer_tabelle.inc */
typedef struct { uint16_t ton, modell; uint32_t sektor; uint8_t ck_ton; uint32_t datei; } re2_tuer_arch_t;
static const re2_tuer_arch_t re2_tuer_arch[20] = {
%(zeilen)s
};
'''
# (ton, modell, sektor) je RE2-Archiv der Mini-Tabelle; Datei = Sektor * 0x800 + Modellteil
_TUER_RE2_AUFBAU = {0x07: (1000, 500, 1), 0x09: (50, 100, 1), 0x0A: (10, 20, 1), 0x0C: (0, 400, 0),
                    0x13: (900, 600, 1)}
_TUER_EIGEN_AUFBAU = {"P07G": (1000, 700, 1), "P2DS": (3000, 300, 2)}


def _fx_fnv1a32(b):
    """FNV-1a 32 - eigene Umsetzung der Fixture (NICHT _fnv1a32 des Gates)."""
    h = 0x811C9DC5
    for x in bytearray(b):
        h ^= x
        h = (h * 0x01000193) % (1 << 32)
    return h


def _inhalt(name, groesse):
    teile, n, k = [], 0, 0
    while n < groesse:
        d = hashlib.sha256(("%s#%d" % (name, k)).encode()).digest()
        teile.append(d)
        n += len(d)
        k += 1
    return b"".join(teile)[:groesse]


def _crc_erhaltend(daten, q, p):
    """Byte q kippen (^0x01) und 4 Ausgleichsbytes ab p so waehlen, dass zlib.crc32 GLEICH bleibt
    (CRC32 ist linear ueber GF(2); 32 aufeinanderfolgende Bits gleichen jede Aenderung aus)."""
    n = len(daten)
    null = zlib.crc32(bytes(n))

    def wirkung(pos, maske):
        v = bytearray(n)
        v[pos] = maske
        return zlib.crc32(bytes(v)) ^ null

    ziel = wirkung(q, 0x01)
    basis = []                                   # (wert, auswahl), absteigend, verschiedene Spitzenbits
    for k in range(32):
        w, a = wirkung(p + k // 8, 1 << (k % 8)), 1 << k
        for bw, ba in basis:
            if w ^ bw < w:
                w, a = w ^ bw, a ^ ba
        if w:
            basis.append((w, a))
            basis.sort(reverse=True)
    w, a = ziel, 0
    for bw, ba in basis:
        if w ^ bw < w:
            w, a = w ^ bw, a ^ ba
    if w:
        raise AssertionError("Selbsttest-Fixture: kein CRC-Ausgleich gefunden")
    neu = bytearray(daten)
    neu[q] ^= 0x01
    for k in range(32):
        if a >> k & 1:
            neu[p + k // 8] ^= 1 << (k % 8)
    neu = bytes(neu)
    if neu == daten or zlib.crc32(neu) != zlib.crc32(daten):
        raise AssertionError("Selbsttest-Fixture: CRC-Ausgleich falsch")
    return neu


# --- Selbsttest-Helfer auf ROHEN Bytes, bewusst UNABHAENGIG vom Leser oben (eigener Mini-Parser),
#     damit eine Faelschung nicht mit demselben Fehler gebaut wird, den sie aufdecken soll.
def _fx_cd(d):
    """-> ([(cd_pos, lho, nlen, xlen, klen, csize, name)], cd_off, eocd_pos) des Selbsttest-APKs."""
    eocd = d.rfind(b"PK\x05\x06")
    n, _cd_groesse, cd_off = struct.unpack("<HII", d[eocd + 10:eocd + 20])
    aus, p = [], cd_off
    for _ in range(n):
        csize, = struct.unpack("<I", d[p + 20:p + 24])
        nlen, xlen, klen = struct.unpack("<HHH", d[p + 28:p + 34])
        lho, = struct.unpack("<I", d[p + 42:p + 46])
        aus.append((p, lho, nlen, xlen, klen, csize, d[p + 46:p + 46 + nlen]))
        p += 46 + nlen + xlen + klen
    return aus, cd_off, eocd


def _fx_lesen(apk):
    with open(apk, "rb") as f:
        return f.read()


def _fx_schreiben(apk, d):
    with open(apk, "wb") as f:
        f.write(d)


def _fx_polstern(apk, anhang=None):
    """Neu schreiben wie AGP/zipflinger: jeder Local Header bekommt ein Ausrichtungsfeld 0xD935
    (6-9 B), Stored-Daten liegen 4-Byte-ausgerichtet; das Zentralverzeichnis behaelt Extra 0.
    So ist Local-Header-Extra != Zentralverzeichnis-Extra (Referenz-APK: 1584 von 3616 Eintraegen) -
    ein Leser, der den Datenoffset aus dem Zentralverzeichnis rechnet, liest daneben.
    anhang = (name_bytes, muell): Muell hinter die Daten des Eintrags, csize in LFH + CD mitziehen."""
    alt = _fx_lesen(apk)
    cd, _cd_off, eocd = _fx_cd(alt)
    neu, neue_cd = bytearray(), bytearray()
    for (p, lho, nlen, xlen, klen, csize, name) in cd:
        l_nlen, l_xlen = struct.unpack("<HH", alt[lho + 26:lho + 30])
        start = lho + 30 + l_nlen + l_xlen
        daten = alt[start:start + csize]
        lfh = bytearray(alt[lho:lho + 30])
        cdr = bytearray(alt[p:p + 46 + nlen + xlen + klen])
        if anhang is not None and name == anhang[0]:
            daten += anhang[1]
            struct.pack_into("<I", lfh, 18, csize + len(anhang[1]))
            struct.pack_into("<I", cdr, 20, csize + len(anhang[1]))
        off = len(neu)
        pad = 6 + (-(off + 30 + l_nlen + 6)) % 4
        struct.pack_into("<H", lfh, 28, pad)
        neu += lfh + alt[lho + 30:lho + 30 + l_nlen] + struct.pack("<HHH", 0xD935, pad - 4, 4) + bytes(pad - 6) + daten
        struct.pack_into("<I", cdr, 42, off)
        neue_cd += cdr
    ende = bytearray(alt[eocd:eocd + 22])
    struct.pack_into("<I", ende, 12, len(neue_cd))
    struct.pack_into("<I", ende, 16, len(neu))
    _fx_schreiben(apk, bytes(neu + neue_cd + ende))


def _fx_stelle(apk, name):
    """(lho, cd_pos, datenoffset) des Eintrags name (str oder bytes) im fertigen Selbsttest-APK."""
    nb = name.encode("utf-8") if isinstance(name, str) else name
    d = _fx_lesen(apk)
    cd, _cd_off, _eocd = _fx_cd(d)
    t = [(lho, p) for (p, lho, _n, _x, _k, _s, nm) in cd if nm == nb]
    if len(t) != 1:
        raise AssertionError("Selbsttest-Fixture: Eintrag %r %d-mal" % (nb, len(t)))
    lho, p = t[0]
    l_nlen, l_xlen = struct.unpack("<HH", d[lho + 26:lho + 30])
    return lho, p, lho + 30 + l_nlen + l_xlen


def _fx_patch(apk, stellen):
    """stellen = [(offset, bytes)] in place schreiben."""
    with open(apk, "r+b") as f:
        for off, b in stellen:
            f.seek(off)
            f.write(b)


def _fx_name_ersetzen(apk, name, neu, nur_lfh=False):
    """Namen (gleiche Laenge) in Local Header und Zentralverzeichnis (oder nur im Local Header) ersetzen."""
    nb = name.encode("utf-8")
    if len(neu) != len(nb):
        raise AssertionError("Selbsttest-Fixture: Name muss gleich lang bleiben")
    lho, p, _d = _fx_stelle(apk, nb)
    _fx_patch(apk, [(lho + 30, neu)] + ([] if nur_lfh else [(p + 46, neu)]))


def _fx_eocd(apk, feld, wert=None, plus=0):
    """Feld des End-of-Central-Directory aendern: feld = (offset, fmt)."""
    d = bytearray(_fx_lesen(apk))
    eocd = d.rfind(b"PK\x05\x06")
    off, fmt = feld
    alt, = struct.unpack_from(fmt, d, eocd + off)
    struct.pack_into(fmt, d, eocd + off, (wert if wert is not None else alt + plus))
    _fx_schreiben(apk, bytes(d))


class _Fall:
    """Ein Selbsttest-Fall: Mini-Repo + Mini-APK aufbauen, dann faelschen."""

    def __init__(self, wurzel, gross=False):
        self.repo = os.path.join(wurzel, "repo")
        self.apk = os.path.join(wurzel, "mini.apk")
        self.gradle = _GRADLE_MUSTER
        fixture = _FIXTURE + ((_GROSS,) if gross else ())
        self.quelle = {rel: _inhalt(rel, n) for rel, n in fixture}
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
        self.verzeichnisse = []              # zusaetzliche Verzeichniseintraege (Name endet auf '/')
        self.schreibname = {}                # Eintrag unter anderem Namen schreiben (danach roh patchen)
        self.anhang = None                   # (name_bytes, muell) beim Polstern hinter die Daten
        self.roh = []                        # Eingriffe auf rohen Bytes NACH dem Polstern: f(apk)
        self.leere_ordner = []
        self.nach_schreiben = None           # Eingriff nach allem (APK/build.gradle zerstoeren)
        # Mini-Engine-Tabellen (Nachbesserung R2, B2): Texte (Faelle aendern sie per Ersetzen) und Werte
        # der Port-Archive - aus dem URSPRUENGLICHEN Inhalt, damit eine geaenderte Datei auffaellt
        self.tuer_kopf, self.tuer_zuordnung = _TUER_KOPF_MUSTER, _TUER_ZUORDNUNG_MUSTER
        self.tuer_eigen, self.tuer_re2 = _TUER_EIGEN_MUSTER, _TUER_RE2_MUSTER
        self.re2_aufbau = dict(_TUER_RE2_AUFBAU)
        self.tuer_werte = {}
        for kennung, (ton, modell, sektor) in _TUER_EIGEN_AUFBAU.items():
            b = self.quelle["re15_port/shared_assets/RE15DOOR/%s.DO2" % kennung]
            if len(b) != sektor * 0x800 + modell:
                raise AssertionError("Selbsttest-Fixture: %s passt nicht zum Aufbau" % kennung)
            self.tuer_werte.update({kennung + "_ton": ton, kennung + "_modell": modell, kennung + "_sektor": sektor,
                                    kennung + "_datei": len(b), kennung + "_fnv": _fx_fnv1a32(b)})
        self.tuer_dateien_weg = []           # Tabellendateien, die nicht geschrieben werden
        # Modus: "apk" (Standard), "quellbaum" (--quellbaum), "paket" (--paket <wurzel>/paket)
        self.modus = "apk"
        self.paket = os.path.join(wurzel, "paket")
        self.paket_eingriffe = []            # f(paketordner) nach dem Kopieren
        self.umgebung = {}                   # zusaetzliche Umgebung des Gate-Laufs (Pruefhaken)
        self.leer_am_ende = False            # Eintrag mit leerem Namen als LETZTER Eintrag (0 B Name im CD)
        self.argumente = []                  # zusaetzliche Aufrufargumente (z.B. --max-zeilen)

    def manifest_aus_eintraegen(self):
        """[(pfad, groesse, sha256)] wie writeAssetManifest sie aus den gestagten Dateien schreibt."""
        return [(n[len("assets/"):], len(b), hashlib.sha256(b).hexdigest()) for n, b, _m in self.eintraege]

    @staticmethod
    def manifest_text(zeilen):
        """Format v2 wie writeAssetManifest (nach Pfad sortiert). Zeilen (pfad, groesse[, sha256]); ohne sha256
        (Geisterzeilen) steht eine gueltige, aber zu keiner Datei passende Summe da."""
        z = []
        for t in zeilen:
            s = t[2] if len(t) > 2 else hashlib.sha256(("geist:" + t[0]).encode("utf-8", "surrogatepass")).hexdigest()
            z.append((t[0], "%d\t%s\t%s" % (t[1], s, t[0])))
        z.sort()
        return ("# re15 assets v2 %d %d\n" % (len(z), sum(t[1] for t in zeilen)) + "\n".join(l for _p, l in z)
                + "\n")

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
        re2_zeilen = []
        for nr in range(20):
            if nr in self.re2_aufbau:
                w = self.re2_aufbau[nr]              # (ton, modell, sektor[, datei abweichend])
                ton, modell, sektor = w[:3]
                re2_zeilen.append("    { 0x%04X, 0x%04X, %2d, 0x9A, %6d },  /* DOOR%02X */"
                                  % (ton, modell, sektor, w[3] if len(w) > 3 else sektor * 0x800 + modell, nr))
            else:
                re2_zeilen.append("    { 0 },")
        for rel, text in ((TUER_KOPF, self.tuer_kopf), (TUER_ZUORDNUNG_INC, self.tuer_zuordnung),
                          (TUER_EIGEN_INC, self.tuer_eigen % self.tuer_werte),
                          (TUER_RE2_INC, self.tuer_re2 % {"zeilen": "\n".join(re2_zeilen)})):
            if rel in self.tuer_dateien_weg:
                continue
            p = os.path.join(self.repo, *rel.split("/"))
            os.makedirs(os.path.dirname(p), exist_ok=True)
            with open(p, "w", encoding="utf-8", newline="\n") as f:
                f.write(text)
        if self.modus == "paket":            # wie make_package.sh copy_common: Baeume unter <paket>/<ziel>
            for rel, b in self.quelle.items():
                if rel.startswith("re15_port/shared_assets/") or rel.startswith("synchro/STAGE"):
                    ziel = rel[len("re15_port/"):] if rel.startswith("re15_port/") else rel
                    p = os.path.join(self.paket, *ziel.split("/"))
                    os.makedirs(os.path.dirname(p), exist_ok=True)
                    with open(p, "wb") as f:
                        f.write(b)
            with open(os.path.join(self.paket, "re15_pc.exe"), "wb") as f:   # liegt ausserhalb der Baeume
                f.write(b"MZ")
            for eingriff in self.paket_eingriffe:
                eingriff(self.paket)
        if self.manifest_roh is not None:
            man = self.manifest_roh
        else:
            zeilen = self.manifest_zeilen if self.manifest_zeilen is not None else self.manifest_aus_eintraegen()
            man = self.manifest_text(zeilen).encode("utf-8")
        with warnings.catch_warnings():
            warnings.simplefilter("ignore")          # "Duplicate name" beim Doppel-Fall
            with zipfile.ZipFile(self.apk, "w") as zf:
                for name, b, methode in _NICHT_ASSETS:
                    zf.writestr(zipfile.ZipInfo(name), b, methode)
                if not self.ohne_manifest:
                    zf.writestr(zipfile.ZipInfo("assets/" + MANIFEST), man, zipfile.ZIP_STORED)
                for name, b, methode in self.eintraege + self.doppelt:
                    zf.writestr(zipfile.ZipInfo(self.schreibname.get(name, name)), b, methode)
                for name in self.verzeichnisse:
                    zf.writestr(zipfile.ZipInfo(name), b"", zipfile.ZIP_STORED)
                if self.leer_am_ende:
                    zf.writestr(zipfile.ZipInfo("x"), b"", zipfile.ZIP_STORED)   # Name danach roh auf Laenge 0
        _fx_polstern(self.apk, self.anhang)
        for eingriff in self.roh:
            eingriff(self.apk)

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
    """(Titel, Faelschung(fall), erwartete Rueckgabe, Pflicht-Teilstrings der Ausgabe[, gross])"""
    D7 = "assets/shared_assets/RE2/DOOR/DOOR07.DO2"
    P07 = "assets/shared_assets/RE15DOOR/P07G.DO2"
    P2DS = "shared_assets/RE15DOOR/P2DS.DO2"
    TEX = "assets/shared_assets/PSX/DATA/TEX.TIM"
    GROSS = "assets/" + _GROSS[0][len("re15_port/"):]
    # Zaehlzeile PSX mit der grossen Datei: 4 Dateien, alle gleich (ROOM1240 3000 + wincfg 0 + TEX 5000)
    PSX_GROSS_ZEILE = "%-28s %7d %7d %8d %12d" % ("shared_assets/PSX", 4, 4, 4, 3000 + 0 + 5000 + _GROSS[1])

    def nichts(f):
        pass

    def crlf(f):
        f.manifest_roh = _Fall.manifest_text(f.manifest_aus_eintraegen()).replace("\n", "\r\n").encode()

    def deflate(f):
        f.eintrag(TEX)[2] = zipfile.ZIP_DEFLATED

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
        f.manifest_zeilen = [(p, g + 1 if p == "shared_assets/RE2/TORSE.VBS" else g, s)
                             for p, g, s in f.manifest_aus_eintraegen()]

    def man_zeile_fehlt(f):
        f.manifest_zeilen = [z for z in f.manifest_aus_eintraegen() if z[0] != P2DS]

    def nur_quelle(f):
        f.quelle["re15_port/shared_assets/RE15DOOR/NEU.DO2"] = b"nur im Quellbaum"

    def kuerzer(f):
        e = f.eintrag(TEX)
        e[1] = e[1][:-10]

    def crc(f):
        def kippen(apk):
            _lho, _p, d = _fx_stelle(apk, "assets/shared_assets/extracted_fx/effect0_blood.tim")
            b = _fx_lesen(apk)[d + 100]
            _fx_patch(apk, [(d + 100, bytes([b ^ 0x5A]))])
        f.roh.append(kippen)

    def doppelt(f):
        e = f.eintrag(D7)
        f.doppelt.append([e[0], e[1], zipfile.ZIP_STORED])

    def kopf(f):
        t = _Fall.manifest_text(f.manifest_aus_eintraegen())
        n = len(f.eintraege)
        f.manifest_roh = t.replace("# re15 assets v2 %d " % n, "# re15 assets v2 %d " % (n + 1), 1).encode()

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
                h.write(b"kein ZIP, nur Text - " * 8)
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

    # --- Nachbesserung R1, Befund B3: Luecken des Selbsttests (Mutanten U1-U4, U7 der Gegenpruefung)
    def gross_spaet(f):                  # U3: Vergleich nur ueber den ersten Block
        e = f.eintrag(GROSS)
        k = BLOCK + 100
        e[1] = e[1][:k] + bytes([e[1][k] ^ 1]) + e[1][k + 1:]

    def crc_gleich(f):                   # U4: CRC32 statt sha256 - CRC bleibt, Inhalt nicht
        e = f.eintrag("assets/" + P2DS)
        e[1] = _crc_erhaltend(e[1], 900, 1000)

    def geisterzeile(f):                 # U1: Zeile fuer eine Datei, die es nirgends gibt
        z = f.manifest_aus_eintraegen() + [("shared_assets/RE15DOOR/GEIST.DO2", 5)]
        f.manifest_roh = _Fall.manifest_text(z).encode()

    def doppelzeile(f):                  # U2: dieselbe Datei zweimal, erste Zeile mit falscher Groesse
        t = _Fall.manifest_text(f.manifest_aus_eintraegen())
        kopf_, rest = t.split("\n", 1)
        f.manifest_roh = (kopf_ + "\n9999\t" + "9" * 64 + "\t" + P2DS + "\n" + rest).encode()

    def unterstrich(f):                  # U7: '4_396' - int() nimmt es, das Geraet liest nur 0-9
        t = _Fall.manifest_text(f.manifest_aus_eintraegen())
        if t.count("\n4396\t") != 1:
            raise AssertionError("Selbsttest-Fixture: '4396' nicht genau einmal als Groesse im Manifest")
        f.manifest_roh = t.replace("\n4396\t", "\n4_396\t", 1).encode()

    def man_kein_utf8(f):
        t = _Fall.manifest_text(f.manifest_aus_eintraegen()).encode()
        f.manifest_roh = t.replace(b"P2DS.DO2", b"P2DS.D\xff2", 1)

    def man_notiz(f):
        f.manifest_roh = (_Fall.manifest_text(f.manifest_aus_eintraegen()) + "# Notiz\n").encode()

    def man_selbst(f):
        f.manifest_roh = (_Fall.manifest_text(f.manifest_aus_eintraegen()) + "12\t" + "0" * 64 + "\tre15_assets.txt\n").encode()

    def man_ohne_kopf(f):
        f.manifest_roh = _Fall.manifest_text(f.manifest_aus_eintraegen()).split("\n", 1)[1].encode()

    # --- Nachbesserung R1, Befund B1: ZIP-Struktur so pruefen, wie Android sie liest
    def name_backslash(f):               # umgehung F2
        f.roh.append(lambda apk: _fx_name_ersetzen(apk, P07, P07.replace("/", "\\").encode()))

    def name_nul(f):                     # umgehung F3: Name + NUL + 'abc' (zipfile schnitt am NUL ab)
        f.schreibname[P07] = P07 + "Xabc"

        def nul(apk):
            lho, p, _d = _fx_stelle(apk, P07 + "Xabc")
            k = len(P07)                     # das 'X' hinter dem echten Namen
            _fx_patch(apk, [(lho + 30 + k, b"\0"), (p + 46 + k, b"\0")])
        f.roh.append(nul)

    def name_utf8(f):
        f.roh.append(lambda apk: _fx_name_ersetzen(apk, P07, P07.encode().replace(b"P07G", b"P07\xff")))

    def name_steuer(f):
        f.roh.append(lambda apk: _fx_name_ersetzen(apk, P07, P07.encode().replace(b"P07G", b"P07\x01")))

    def name_punkte(f):
        f.roh.append(lambda apk: _fx_name_ersetzen(apk, P07, P07.encode().replace(b"P07G.DO2", b"../G.DO2")))

    def lfh_crc(f):                      # umgehung F4
        def e(apk):
            lho, _p, _d = _fx_stelle(apk, P07)
            alt, = struct.unpack("<I", _fx_lesen(apk)[lho + 14:lho + 18])
            _fx_patch(apk, [(lho + 14, struct.pack("<I", alt ^ 1))])
        f.roh.append(e)

    def lfh_groesse(f):                  # umgehung F5
        def e(apk):
            lho, _p, _d = _fx_stelle(apk, P07)
            alt, = struct.unpack("<I", _fx_lesen(apk)[lho + 22:lho + 26])
            _fx_patch(apk, [(lho + 22, struct.pack("<I", alt + 1))])
        f.roh.append(e)

    def lfh_name(f):
        f.roh.append(lambda apk: _fx_name_ersetzen(apk, P07, P07.replace("DO2", "DO3").encode(), nur_lfh=True))

    def lfh_signatur(f):
        def e(apk):
            lho, _p, _d = _fx_stelle(apk, P07)
            _fx_patch(apk, [(lho, b"PK\x09\x09")])
        f.roh.append(e)

    def verschluesselt(f):
        def e(apk):
            lho, p, _d = _fx_stelle(apk, P07)
            d = _fx_lesen(apk)
            lf, = struct.unpack("<H", d[lho + 6:lho + 8])
            cf, = struct.unpack("<H", d[p + 8:p + 10])
            _fx_patch(apk, [(lho + 6, struct.pack("<H", lf | 1)), (p + 8, struct.pack("<H", cf | 1))])
        f.roh.append(e)

    def methode99(f):
        def e(apk):
            lho, p, _d = _fx_stelle(apk, P07)
            _fx_patch(apk, [(lho + 8, struct.pack("<H", 99)), (p + 10, struct.pack("<H", 99))])
        f.roh.append(e)

    def ragt(f):
        def e(apk):
            lho, p, _d = _fx_stelle(apk, P07)
            g = struct.pack("<II", 10 ** 7, 10 ** 7)
            _fx_patch(apk, [(lho + 18, g), (p + 20, g)])
        f.roh.append(e)

    def dex_kippen(f):                   # kein Asset: nur CRC32 gegen das Zentralverzeichnis schuetzt es
        def e(apk):
            _lho, _p, d = _fx_stelle(apk, "classes.dex")
            b = _fx_lesen(apk)[d + 2]
            _fx_patch(apk, [(d + 2, bytes([b ^ 0x5A]))])
        f.roh.append(e)

    def deflate_muell(f):
        f.eintrag(TEX)[2] = zipfile.ZIP_DEFLATED
        f.anhang = (TEX.encode(), b"MUELL!!")

    def verzeichnis(f):
        f.verzeichnisse.append("assets/shared_assets/RE15DOOR/")

    def abgeschnitten(f):                # umgehung F7
        f.roh.append(lambda apk: _fx_schreiben(apk, _fx_lesen(apk)[:-10]))

    def muell_hinten(f):
        f.roh.append(lambda apk: _fx_schreiben(apk, _fx_lesen(apk) + b"M"))      # genau 1 B (Grenze, R2)

    def zip64(f):
        f.roh.append(lambda apk: (_fx_eocd(apk, (8, "<H"), 0xFFFF), _fx_eocd(apk, (10, "<H"), 0xFFFF)))

    def cd_signatur(f):                  # umgehung F8
        def e(apk):
            _cd, cd_off, _eocd = _fx_cd(_fx_lesen(apk))
            _fx_patch(apk, [(cd_off, b"PK\x09\x09")])
        f.roh.append(e)

    def cd_offset(f):
        f.roh.append(lambda apk: _fx_eocd(apk, (16, "<I"), plus=1))

    def eintrag_weniger(f):              # libziparchive saehe den letzten Eintrag nicht
        f.roh.append(lambda apk: (_fx_eocd(apk, (8, "<H"), plus=-1), _fx_eocd(apk, (10, "<H"), plus=-1)))

    def deflate_kaputt(f):               # Blocktyp 3 (reserviert) im ersten Byte des Deflate-Stroms
        f.eintrag(TEX)[2] = zipfile.ZIP_DEFLATED

        def e(apk):
            _lho, _p, d = _fx_stelle(apk, TEX)
            _fx_patch(apk, [(d, b"\x07")])
        f.roh.append(e)

    def eocd_in_daten(f):                # gut: die LETZTE Signatur zaehlt (libziparchive sucht von hinten)
        rel = "synchro/STAGE2/room2000/main00.wav"
        b = f.quelle[rel]
        b = b[:100] + EOCD_SIG + bytes(18) + b[122:]
        f.quelle[rel] = b
        f.eintrag("assets/" + rel)[1] = b

    # --- build.gradle nicht lesbar: jede Abbruchstelle des Gradle-Lesers (Nachbesserung R1, Mutanten-Probe)
    def g_ohne_task(f):
        f.gradle_ersetzen('tasks.register("stageAssets", Sync) {', 'tasks.register("stageAssetsX", Sync) {')

    def g_zweimal(f):
        f.gradle += '\ntasks.register("stageAssets", Sync) {\n    into(assetStage)\n}\n'

    def g_from_format(f):
        f.gradle_ersetzen('from(new File(portRoot, "shared_assets/RE2"))', 'from(file("shared_assets/RE2"))')

    def g_from_anweisung(f):
        f.gradle_ersetzen('{ into "shared_assets/RE2" }', '{ into "shared_assets/RE2"; exclude "**/*.VBS" }')

    def g_ohne_into(f):
        f.gradle_ersetzen('{ into "shared_assets/RE2" }', '{ include "**" }')

    def g_ohne_preserve(f):
        f.gradle_ersetzen('    preserve { include "re15_assets.txt" }\n', '')

    def g_klammer_falsch(f):
        f.gradle_ersetzen('    into(assetStage)\n', '    into(assetStage]\n')

    def g_klammer_offen(f):
        f.gradle = f.gradle[:f.gradle.index('    into(assetStage)')]

    def g_zeichenkette_offen(f):
        f.gradle = f.gradle[:f.gradle.index('Asset-Baeume nach app/build')]

    def g_kommentar_offen(f):
        f.gradle = f.gradle[:f.gradle.index('/* Block-Kommentar') + len('/* Block')]

    def stored_zu_lang(f):               # csize 5 B groesser als usize (LFH + CD gleich): Leseschutz greift
        def e(apk):
            lho, p, _d = _fx_stelle(apk, P07)
            d = _fx_lesen(apk)
            cs, = struct.unpack("<I", d[lho + 18:lho + 22])
            _fx_patch(apk, [(lho + 18, struct.pack("<I", cs + 5)), (p + 20, struct.pack("<I", cs + 5))])
        f.roh.append(e)

    # --- Nachbesserung R2, Befund B1: die fuenf Teil-Mutanten der Gegenpruefung (M1, M9, M5, M14, M16)
    SO = "lib/arm64-v8a/libmain.so"
    D13 = "re15_port/shared_assets/RE2/DOOR/DOOR13.DO2"
    Q07 = "re15_port/shared_assets/RE15DOOR/P07G.DO2"

    def man_kleiner(f):                  # M1: Manifest nennt 1 B WENIGER als die APK hat
        f.manifest_zeilen = [(p, g - 1 if p == P2DS else g, s) for p, g, s in f.manifest_aus_eintraegen()]

    def usize_plus(name):                # M9: entpackte Laenge (LFH + CD) +100, Daten und CRC bleiben
        def faelschen(f):
            def e(apk):
                lho, p, _d = _fx_stelle(apk, name)
                us, = struct.unpack("<I", _fx_lesen(apk)[lho + 22:lho + 26])
                _fx_patch(apk, [(lho + 22, struct.pack("<I", us + 100)), (p + 24, struct.pack("<I", us + 100))])
            f.roh.append(e)
        return faelschen

    def kommentar_lang(f):               # M5: EOCD-Kommentarlaenge 1 B groesser als die Datei hergibt (Grenze)
        f.roh.append(lambda apk: _fx_eocd(apk, (20, "<H"), plus=1))

    def dd_bit(im_lfh, im_cd, crc_kippen):   # M14: Data-Descriptor-Bit (Bit 3)
        def faelschen(f):
            def e(apk):
                lho, p, _d = _fx_stelle(apk, P07)
                d = _fx_lesen(apk)
                lf, = struct.unpack("<H", d[lho + 6:lho + 8])
                cf, = struct.unpack("<H", d[p + 8:p + 10])
                stellen = []
                if im_lfh:
                    stellen.append((lho + 6, struct.pack("<H", lf | 8)))
                if im_cd:
                    stellen.append((p + 8, struct.pack("<H", cf | 8)))
                if crc_kippen:
                    c, = struct.unpack("<I", d[lho + 14:lho + 18])
                    stellen.append((lho + 14, struct.pack("<I", c ^ 1)))
                _fx_patch(apk, stellen)
            f.roh.append(e)
        return faelschen

    def so_kippen(f):                    # M16 (Stored-Seite): Byte in lib/arm64-v8a/libmain.so
        def e(apk):
            _lho, _p, d = _fx_stelle(apk, SO)
            b = _fx_lesen(apk)[d + 100]
            _fx_patch(apk, [(d + 100, bytes([b ^ 0x5A]))])
        f.roh.append(e)

    # --- Nachbesserung R2, Befund B8: Praefix vor dem ersten Local Header, arabisch-indische Kopfziffern
    def praefix(f):
        def e(apk):
            d = bytearray(_fx_lesen(apk))
            cd, cd_off, eocd = _fx_cd(bytes(d))
            for (p, lho, _n, _x, _k, _s, _nm) in cd:
                struct.pack_into("<I", d, p + 42, lho + 16)
            struct.pack_into("<I", d, eocd + 16, cd_off + 16)
            _fx_schreiben(apk, bytes(16) + bytes(d))
        f.roh.append(e)

    def kopf_arabisch(f):
        t = _Fall.manifest_text(f.manifest_aus_eintraegen())
        kopf_, rest = t.split("\n", 1)
        kopf_ = "# re15 assets v2 " + "".join(chr(0x0660 + int(c)) if c.isdigit() else c
                                               for c in kopf_[len("# re15 assets v2 "):])
        f.manifest_roh = (kopf_ + "\n" + rest).encode("utf-8")

    # --- Nachbesserung R2, Befund B2: Tuerarchive gegen die Engine-Tabellen (Quelle UND APK gleich falsch)
    def beide(rel, neu):                 # Quelle und APK-Eintrag gleich aendern (neu = None: entfernen)
        def faelschen(f):
            name = "assets/" + rel[len("re15_port/"):]
            if neu is None:
                del f.quelle[rel]
                f.eintraege.remove(f.eintrag(name))
            elif rel in f.quelle:
                b = neu(f.quelle[rel])
                f.quelle[rel] = b
                f.eintrag(name)[1] = b
            else:
                b = neu(b"")
                f.quelle[rel] = b
                f.eintraege.append([name, b, zipfile.ZIP_STORED])
        return faelschen

    def tabelle(attr, alt, neu, n=1):    # Text einer Mini-Tabelle ersetzen
        def faelschen(f):
            t = getattr(f, attr)
            if t.count(alt) != n:
                raise AssertionError("Selbsttest-Fixture: %r %d-mal in %s" % (alt, t.count(alt), attr))
            setattr(f, attr, t.replace(alt, neu))
        return faelschen

    def re2_aufbau(nr, wert):
        def faelschen(f):
            f.re2_aufbau[nr] = wert
        return faelschen

    def tuer_wert(schluessel, wert):
        def faelschen(f):
            f.tuer_werte[schluessel] = wert
        return faelschen

    def tabellendatei_weg(rel):
        def faelschen(f):
            f.tuer_dateien_weg.append(rel)
        return faelschen

    def byte_kippen(b):
        return b[:100] + bytes([b[100] ^ 1]) + b[101:]

    def tabelle_name(name, alt, neu):
        def faelschen(f):
            f.roh.append(lambda apk: _fx_name_ersetzen(apk, name, name.encode().replace(alt, neu, 1)))
        return faelschen

    def max_zeilen(n):
        def faelschen(f):
            f.argumente += ["--max-zeilen", str(n)]
        return faelschen

    def ohne_d13_apk(f):                 # zweiter Befund derselben Art ("fehlt in der APK")
        f.eintraege.remove(f.eintrag("assets/" + D13[len("re15_port/"):]))

    def alle(*faelschungen):
        def faelschen(f):
            for x in faelschungen:
                x(f)
        return faelschen

    def anhaengen(attr, text):
        def faelschen(f):
            setattr(f, attr, getattr(f, attr) + text)
        return faelschen

    ZE = "{ 0x1000, 0, 0, 22850, -13400, 500, 1000, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 0, 2, 0x00BBE, 1 }"
    ZD = "{ 0x5060, 0, 0, -26300, -11600, 800, 1800, {0, 0, 0, 0}, {0, 0, 0, 0}, 0x13, 0, 0, 0x0A, 273, 142, 0x02AAE, 2 }"
    E2 = "{ 0x13, 0x0A, 1, 1, {0, 0, 0}, {2048, 2048, 0}, 0, -450, 2, {0, 0, 0}, {0, 0, 0}, 2 }"

    # --- Nachbesserung R2, Befund B6: shared_assets-Wurzel, PC-Paket gegen die Liste
    def wurzel_neu(f):
        f.quelle["re15_port/shared_assets/RE15NEU/NEU.DAT"] = b"in keiner Liste"

    def modus(m, *eingriffe):
        def faelschen(f):
            f.modus = m
            for e in eingriffe:
                e(f)
        return faelschen

    def paket(eingriff):
        def faelschen(f):
            f.paket_eingriffe.append(eingriff)
        return faelschen

    def paket_datei_weg(rel):
        return paket(lambda d: os.remove(os.path.join(d, *rel.split("/"))))

    def paket_datei_neu(rel):
        def e(d):
            p = os.path.join(d, *rel.split("/"))
            os.makedirs(os.path.dirname(p), exist_ok=True)
            with open(p, "wb") as h:
                h.write(b"nur im Paket")
        return paket(e)

    def paket_byte(rel):
        def e(d):
            p = os.path.join(d, *rel.split("/"))
            with open(p, "r+b") as h:
                h.seek(10)
                b = h.read(1)
                h.seek(10)
                h.write(bytes([b[0] ^ 1]))
        return paket(e)

    def paket_ordner_weg(f):
        f.paket_eingriffe.append(lambda d: shutil.rmtree(d))

    # --- Nachbesserung R2, Mutanten-Probe: Grenzen und Teilbedingungen, die bisher kein Fall traf
    def man_grenze(ueber):               # Manifest genau an bzw. 1 B ueber der (per Pruefhaken gesenkten) Grenze
        def faelschen(f):
            roh = _Fall.manifest_text(f.manifest_aus_eintraegen()).encode() + b"\n" * 7   # Leerzeilen: uebersprungen
            f.manifest_roh = roh
            f.umgebung["RE15_GATE_MANIFEST_MAX"] = str(len(roh) - (1 if ueber else 0))
        return faelschen

    def eocd_feld(off, wert=None, plus=0):
        return lambda f: f.roh.append(lambda apk: _fx_eocd(apk, (off, "<H"), wert, plus))

    def z64_locator(f):                  # ZIP64-Locator vor dem EOCD, Zaehler normal
        def e(apk):
            d = _fx_lesen(apk)
            i = d.rfind(EOCD_SIG)
            _fx_schreiben(apk, d[:i] + Z64_LOC_SIG + bytes(16) + d[i:])
        f.roh.append(e)

    # --- Nachbesserung R2, Mutanten-Probe Runde 2: Richtungen, Grenzen, Teilbedingungen
    def _richtung(daten, kleiner, wert):
        """erste Byte-Position, deren Bit-0-Kippen wert(daten) KLEINER bzw. GROESSER macht"""
        alt = wert(daten)
        for i in range(len(daten)):
            neu = daten[:i] + bytes([daten[i] ^ 1]) + daten[i + 1:]
            if (wert(neu) < alt) == kleiner:
                return neu
        raise AssertionError("Selbsttest-Fixture: keine Richtung gefunden")

    def sha(b):
        return hashlib.sha256(b).hexdigest()

    def apk_inhalt_richtung(name, kleiner):      # APK-Eintrag: sha256 kleiner/groesser als die Quelle
        def faelschen(f):
            e = f.eintrag(name)
            e[1] = _richtung(e[1], kleiner, sha)
        return faelschen

    def tuer_fnv_richtung(kleiner):               # Quelle UND APK: FNV-1a kleiner/groesser als die Tabelle
        def faelschen(f):
            b = _richtung(f.quelle[Q07], kleiner, _fx_fnv1a32)
            f.quelle[Q07] = b
            f.eintrag(P07)[1] = b
        return faelschen

    def paket_richtung(rel, kleiner):             # Paketdatei: sha256 kleiner/groesser als die Quelle
        def e(d):
            pfad = os.path.join(d, *rel.split("/"))
            with open(pfad, "rb") as h:
                b = h.read()
            with open(pfad, "wb") as h:
                h.write(_richtung(b, kleiner, sha))
        return paket(e)

    def laenger(f):                               # APK-Eintrag laenger als die Quelle
        e = f.eintrag(TEX)
        e[1] = e[1] + b"0123456789"

    def zusatz_vorn(f):                           # Zusatzeintrag, der VOR dem Manifest sortiert
        f.eintraege.append(["assets/AAA.BIN", b"vorn", zipfile.ZIP_STORED])

    def man_zeilen(neu_zeilen, kopf_anzahl=0, kopf_bytes=0, nach_kopf=None):
        def faelschen(f):
            z = f.manifest_aus_eintraegen() + list(neu_zeilen)
            t = _Fall.manifest_text(z)
            kopf_, rest = t.split("\n", 1)
            n, b = [int(x) for x in kopf_.split()[-2:]]
            kopf_ = "# re15 assets v2 %d %d" % (n + kopf_anzahl, b + kopf_bytes)
            f.manifest_roh = (kopf_ + "\n" + (nach_kopf + "\n" if nach_kopf else "") + rest).encode("utf-8")
        return faelschen

    def lfh_feld(off, delta, fmt="<I"):           # Feld NUR im Local Header um delta aendern
        def faelschen(f):
            def e(apk):
                lho, _p, _d = _fx_stelle(apk, P07)
                w, = struct.unpack(fmt, _fx_lesen(apk)[lho + off:lho + off + struct.calcsize(fmt)])
                _fx_patch(apk, [(lho + off, struct.pack(fmt, (w + delta) % (1 << 32)))])
            f.roh.append(e)
        return faelschen

    def flag_bit(maske, im_lfh, im_cd):
        def faelschen(f):
            def e(apk):
                lho, p, _d = _fx_stelle(apk, P07)
                d = _fx_lesen(apk)
                lf, = struct.unpack("<H", d[lho + 6:lho + 8])
                cf, = struct.unpack("<H", d[p + 8:p + 10])
                stellen = []
                if im_lfh:
                    stellen.append((lho + 6, struct.pack("<H", lf | maske)))
                if im_cd:
                    stellen.append((p + 8, struct.pack("<H", cf | maske)))
                _fx_patch(apk, stellen)
            f.roh.append(e)
        return faelschen

    def lfh_hinten(f):                            # Local-Header-Offset zeigt auf 'PK\3\4' in den letzten 8 Bytes
        def e(apk):
            d = _fx_lesen(apk)
            i = d.rfind(EOCD_SIG)
            _cd, _cd_off, _eocd = _fx_cd(d)
            ende = bytearray(d[i:i + 22])
            struct.pack_into("<H", ende, 20, 8)
            neu = d[:i] + bytes(ende) + LFH_SIG + b"xxxx"
            _fx_schreiben(apk, neu)
            _lho, p, _d = _fx_stelle(apk, P07)
            _fx_patch(apk, [(p + 42, struct.pack("<I", i + 22))])
        f.roh.append(e)

    def deflate_kurz(f):                          # Deflate-Strom um 10 B gekuerzt (csize in LFH + CD)
        f.eintrag(TEX)[2] = zipfile.ZIP_DEFLATED

        def e(apk):
            lho, p, _d = _fx_stelle(apk, TEX)
            cs, = struct.unpack("<I", _fx_lesen(apk)[lho + 18:lho + 22])
            _fx_patch(apk, [(lho + 18, struct.pack("<I", cs - 10)), (p + 20, struct.pack("<I", cs - 10))])
        f.roh.append(e)

    def eocd_cd_off_ffff(f):
        f.roh.append(lambda apk: _fx_eocd(apk, (16, "<I"), 0xFFFFFFFF))

    def praefix_zz(f):                   # Praefix aus 'Z' (Bytes GROESSER als 'PK\3\4')
        def e(apk):
            d = bytearray(_fx_lesen(apk))
            cd, cd_off, eocd = _fx_cd(bytes(d))
            for (p, lho, _n, _x, _k, _s, _nm) in cd:
                struct.pack_into("<I", d, p + 42, lho + 16)
            struct.pack_into("<I", d, eocd + 16, cd_off + 16)
            _fx_schreiben(apk, b"Z" * 16 + bytes(d))
        f.roh.append(e)

    def kommentar_max(f):                # gut: EOCD mit 65535 B Kommentar - die Signatur steht dann am Anfang des Suchfensters
        def e(apk):
            d = bytearray(_fx_lesen(apk))
            i = d.rfind(EOCD_SIG)
            struct.pack_into("<H", d, i + 20, 0xFFFF)
            _fx_schreiben(apk, bytes(d[:i + 22]) + b"k" * 0xFFFF)
        f.roh.append(e)

    def torse_leer(f):
        rel = "re15_port/shared_assets/RE2/TORSE.VBS"
        f.quelle[rel] = b""
        f.eintrag("assets/" + rel[len("re15_port/"):])[1] = b""

    def re15door_ohne_do2(f):            # RE15DOOR: nur eine Nicht-DO2-Datei und ein DO2 in einem Unterordner
        for rel in [r for r in f.quelle if r.startswith("re15_port/shared_assets/RE15DOOR/")]:
            del f.quelle[rel]
        f.eintraege = [e for e in f.eintraege if not e[0].startswith("assets/shared_assets/RE15DOOR/")]
        for rel, b in (("re15_port/shared_assets/RE15DOOR/liesmich.txt", b"text"),
                       ("re15_port/shared_assets/RE15DOOR/alt/P99X.DO2", b"alt")):
            f.quelle[rel] = b
            f.eintraege.append(["assets/" + rel[len("re15_port/"):], b, zipfile.ZIP_STORED])

    def cd_groesse_kleiner(f):           # EOCD nennt ein 3 B kleineres Zentralverzeichnis (letzter Name ragt hinaus)
        f.roh.append(lambda apk: _fx_eocd(apk, (12, "<I"), plus=-3))

    def leer_am_ende(f):                 # gut: letzter CD-Eintrag ohne Name/Extra/Kommentar (Kopf endet genau am CD-Ende)
        f.leer_am_ende = True

        def e(apk):
            d = bytearray(_fx_lesen(apk))
            cd, cd_off, eocd = _fx_cd(bytes(d))
            p, lho, nlen, xlen, klen, _cs, name = cd[-1]
            if name != b"x" or (xlen, klen) != (0, 0):
                raise AssertionError("Selbsttest-Fixture: letzter Eintrag nicht 'x'")
            l_nlen, l_xlen = struct.unpack("<HH", bytes(d[lho + 26:lho + 30]))
            # Local Header: Name 'x' raus (Laenge 0), Daten/Polster bleiben; CD: Name raus
            neu = d[:lho + 26] + struct.pack("<HH", 0, l_xlen) + d[lho + 30 + 1:p] + d[p:p + 28] + struct.pack("<H", 0) + d[p + 30:p + 46]
            neu = bytearray(neu)
            neu_cd_off = cd_off - 1
            ende = bytearray(d[eocd:eocd + 22])
            struct.pack_into("<I", ende, 12, (eocd - cd_off) - 1)
            struct.pack_into("<I", ende, 16, neu_cd_off)
            # CD-Offset des letzten Eintrags im Zentralverzeichnis bleibt lho (davor nichts verschoben)
            _fx_schreiben(apk, bytes(neu) + bytes(ende))
        f.roh.append(e)

    def signatur_klein(wo):              # Signatur durch KLEINERE Bytes ersetzt ('PK\0\0' < 'PK\1\2'/'PK\3\4')
        def faelschen(f):
            def e(apk):
                if wo == "cd":
                    _cd, off, _eocd = _fx_cd(_fx_lesen(apk))
                else:
                    off, _p, _d = _fx_stelle(apk, P07)
                _fx_patch(apk, [(off, b"PK\x00\x00")])
            f.roh.append(e)
        return faelschen

    def crc_richtung(kleiner):           # Stored-Nicht-Asset: berechnete CRC32 kleiner/groesser als die im CD
        def faelschen(f):
            def e(apk):
                _lho, _p, d = _fx_stelle(apk, SO)
                n = len(_NICHT_ASSETS[2][1])
                alt = _fx_lesen(apk)[d:d + n]
                neu = _richtung(alt, kleiner, zlib.crc32)
                _fx_patch(apk, [(d, neu)])
            f.roh.append(e)
        return faelschen

    def manifest_daten_kippen(f):        # Manifest-Daten: '1200' -> '1201' (CRC bleibt alt) - Eintrag beschaedigt
        def e(apk):
            _lho, _p, d = _fx_stelle(apk, "assets/" + MANIFEST)
            daten = _fx_lesen(apk)
            i = daten.index(b"1200\t", d)
            _fx_patch(apk, [(i + 3, b"1")])
        f.roh.append(e)

    def manifest_lfh_crc(f):             # Manifest: CRC nur im Local Header +1 - Eintrag nicht lesbar
        def e(apk):
            lho, _p, _d = _fx_stelle(apk, "assets/" + MANIFEST)
            w, = struct.unpack("<I", _fx_lesen(apk)[lho + 14:lho + 18])
            _fx_patch(apk, [(lho + 14, struct.pack("<I", (w + 1) % (1 << 32)))])
        f.roh.append(e)

    def cd_kopf_halb(f):                 # Zentralverzeichnis endet mitten im Kopf eines weiteren Eintrags
        def e(apk):
            d = _fx_lesen(apk)
            i = d.rfind(EOCD_SIG)
            _fx_schreiben(apk, d[:i] + CD_SIG + bytes(10) + d[i:])
            _fx_eocd(apk, (8, "<H"), plus=1)
            _fx_eocd(apk, (10, "<H"), plus=1)
            _fx_eocd(apk, (12, "<I"), plus=14)
        f.roh.append(e)

    # --- Runde 4 (Gegenpruefung R3, B2): natuerliche Ein-Zeilen-Abschwaechungen des Manifestlesers und der
    # Methodenpruefung, die der Selbsttest bis dahin NICHT fing (Mutanten MU1-MU8 der Gegenpruefung:
    # rstrip() statt rstrip('\r'), '\d' bzw. isdigit() statt [0-9], splitlines(), 64 << 30, Methode <= 8,
    # strip() an Kopfzeile bzw. Pfad). Das echte Gate lehnt jede dieser Faelschungen ab (wie das Geraet sie
    # liest, asset_abgleich.c re15_abgleich_lesen: Trennung NUR an '\n', nur angehaengte '\r' weg, nur 0-9);
    # je Mutant trifft mindestens ein Fall GENAU seine Abschwaechung - der Mutant nimmt die Faelschung an.
    P07M = P07[len("assets/"):]                  # Manifestpfad der Fixture: shared_assets/RE15DOOR/P07G.DO2, 2748 B

    def man_ersetzen(alt, neu):
        """Manifest wie writeAssetManifest, dann GENAU EINE Textstelle ersetzen (Kopfzeile bleibt).
        '{S}' in alt/neu = die echte sha256 von P07G.DO2 (Format v2: '<bytes>\\t<sha256>\\t<pfad>')."""
        def faelschen(f):
            s = hashlib.sha256(f.eintrag(P07)[1]).hexdigest()
            platz = (("{SU}", s.upper()), ("{S63}", s[:63]), ("{S}", s))
            a, n = alt, neu
            for k, v in platz:
                a, n = a.replace(k, v), n.replace(k, v)
            t = _Fall.manifest_text(f.manifest_aus_eintraegen())
            if t.count(a) != 1:
                raise AssertionError("Selbsttest-Fixture: %r %d-mal im Manifest" % (a, t.count(a)))
            f.manifest_roh = t.replace(a, n).encode("utf-8")
        return faelschen

    def pfad_ende(zeichen):              # MU1 rstrip() / MU8 strip(): Zeichen am Ende von Pfad bzw. Zeile
        return man_ersetzen("\t%s\n" % P07M, "\t%s%s\n" % (P07M, zeichen))

    def trenner(zeichen):                # MU3 splitlines(): anderer Zeilentrenner statt '\n' VOR der P07G-Zeile
        return man_ersetzen("\n2748\t{S}\t%s\n" % P07M, "%s2748\t{S}\t%s\n" % (zeichen, P07M))

    def groesse_ziffern(ziffern):        # MU2 '\d' / MU6 isdigit(): Groesse 2748 in anderen Ziffern (ziffern[0..9])
        return man_ersetzen("\n2748\t{S}\t%s\n" % P07M,
                            "\n%s\t{S}\t%s\n" % ("".join(ziffern[int(c)] for c in "2748"), P07M))

    def kopf_ziffern(ziffern):           # KOPF_RE nur [0-9]: Kopfzeile in anderen Ziffern
        def faelschen(f):
            t = _Fall.manifest_text(f.manifest_aus_eintraegen())
            kopf_, rest = t.split("\n", 1)
            zahlen = kopf_[len("# re15 assets v2 "):]
            f.manifest_roh = ("# re15 assets v2 " + "".join(ziffern[int(c)] if "0" <= c <= "9" else c for c in zahlen)
                              + "\n" + rest).encode("utf-8")
        return faelschen

    # --- Runde 34a N1 (Format v2): die neue Spalte sha256 und die neuen Pfad-/Listenregeln
    #     (jni/asset_abgleich.h). Das Geraet verwirft die GANZE Liste bei jeder Abweichung; das Gate meldet sie.
    def man_v1(f):                       # die Liste von v0.8.19 (Format v1) in einer sonst guten APK
        z = sorted("%d\t%s" % (g, p) for p, g, _s in f.manifest_aus_eintraegen())
        f.manifest_roh = ("# re15 assets %d %d\n" % (len(z), sum(g for _p, g, _s in f.manifest_aus_eintraegen()))
                          + "\n".join(z) + "\n").encode()

    def man_geister(zeilen):             # zusaetzliche (Geister-)Zeilen, Kopfzeile passend mitgerechnet
        def faelschen(f):
            f.manifest_zeilen = f.manifest_aus_eintraegen() + list(zeilen)
        return faelschen

    def kopf_rand(vorn, hinten):         # MU7 strip(): Leerraum vor bzw. hinter der Kopfzeile
        def faelschen(f):
            t = _Fall.manifest_text(f.manifest_aus_eintraegen())
            kopf_, rest = t.split("\n", 1)
            f.manifest_roh = (vorn + kopf_ + hinten + "\n" + rest).encode("utf-8")
        return faelschen

    def methode_nr(nr, deflate):         # MU5 'methode > 8': Methode 1..7 in Local Header UND Zentralverzeichnis
        def faelschen(f):
            if deflate:                  # Deflate-Daten: ein Leser, der jede Methode != 0 entpackt, liest sie fehlerfrei
                f.eintrag(TEX)[2] = zipfile.ZIP_DEFLATED

            def e(apk):
                lho, p, _d = _fx_stelle(apk, TEX)
                _fx_patch(apk, [(lho + 8, struct.pack("<H", nr)), (p + 10, struct.pack("<H", nr))])
            f.roh.append(e)
        return faelschen

    def man_geraet_grenze(f):            # MU4 '64 << 30': Manifest 1 B ueber der GERAETE-Grenze, OHNE Pruefhaken
        roh = _Fall.manifest_text(f.manifest_aus_eintraegen()).encode("utf-8")
        # Polster = EINE Zeile aus '\r' vor dem letzten '\n' (Geraet und Gate: leer -> uebersprungen); 64 Mi
        # Leerzeilen waeren 64 Mi Listenelemente
        f.manifest_roh = roh + b"\r" * (_FX_GERAET_MANIFEST - len(roh)) + b"\n"

    return (
        ("gute APK", nichts, 0, ["APK-ASSET-GATE-OK", "RE15DOOR:  Quelle 2, APK 2, sha256 gleich 2/2",
                                 "RE2/DOOR:  Quelle 4, APK 4, sha256 gleich 4/4",
                                 "TORSE.VBS: Quelle 900 B, APK 900 B, sha256 gleich",
                                 "2/2 wie die Engine-Tabelle (Groesse, Aufbau, FNV-1a)",
                                 "4/4 wie die Engine-Tabelle (Groesse, Aufbau)"]),
        ("Manifest mit CRLF (Geraet schneidet \\r ab)", crlf, 0, ["APK-ASSET-GATE-OK"]),
        ("Eintrag komprimiert (Inhalt gleich)", deflate, 0, ["APK-ASSET-GATE-OK", "WARNUNG"]),
        ("fehlende Datei: RE15DOOR-Eintrag entfernt", ohne_p07, 1, ["fehlt in der APK: " + P07]),
        ("gleiche Groesse, anderer Inhalt: RE2/DOOR 1 Byte", ein_byte, 1, ["Inhalt weicht ab (sha256", D7]),
        ("Zusatzeintrag unter assets/", zusatz, 1, ["zusaetzlich in der APK", "PSX/EXTRA.BIN"]),
        ("synchro/unused in der APK", unused, 1, ["zusaetzlich in der APK", "assets/synchro/unused/"]),
        ("Manifest: falsche Groesse", man_groesse, 1, ["Manifest-Groesse falsch: shared_assets/RE2/TORSE.VBS"]),
        ("Manifest: fehlende Zeile", man_zeile_fehlt, 1, ["fehlt im Manifest: " + P2DS]),
        ("Datei nur im Quellbaum", nur_quelle, 1, ["fehlt in der APK: assets/shared_assets/RE15DOOR/NEU.DO2"]),
        ("APK-Eintrag kuerzer als die Quelle", kuerzer, 1, ["Groesse weicht ab: " + TEX]),
        ("Byte im APK-Datenstrom gekippt (CRC)", crc, 1, ["beschaedigt (CRC", "effect0_blood.tim"], False,
         ["[APK: beschaedigt (CRC)] 2"]),
        ("doppelter APK-Eintrag", doppelt, 1, ["doppelter Eintrag in der APK (2x): " + D7]),
        ("Manifest: Kopfzeile Anzahl falsch", kopf, 1, ["Manifest-Kopfzeile passt nicht"]),
        ("Manifest: BOM", bom, 1, ["BOM"]),
        ("Manifest fehlt in der APK", ohne_manifest, 1, ["Manifest fehlt in der APK"]),
        ("Manifest: Zeile mit nur einem Tab (Summe + Pfad verschmolzen)", ohne_tab, 1,
         ["1 Tab(s)", "fehlt im Manifest: shared_assets/RE2/CDEMD0.EMS"]),
        ("Manifest: Pfad mit '..'", punktpunkt, 1, ["unzulaessiger Pfad 'shared_assets/../../boese.bin'"]),
        ("RE15DOOR leer (Quelle und APK)", re15door_leer, 1, ["Quellbaum leer: re15_port/shared_assets/RE15DOOR",
                                                               "Pflichtinhalt fehlt: re15_port/shared_assets/RE15DOOR"]),
        ("RE15DOOR-Ordner fehlt ganz", re15door_weg, 1, ["Quellbaum fehlt: re15_port/shared_assets/RE15DOOR"]),
        ("TORSE.VBS fehlt (Quelle und APK)", torse_weg, 1, ["Pflichtdatei fehlt/leer: re15_port/shared_assets/RE2/TORSE.VBS"]),
        ("APK ist kein ZIP", kein_zip, 2, ["APK nicht lesbar", "kein End-of-Central-Directory"]),
        ("APK fehlt", apk_fehlt, 2, ["APK fehlt"]),
        ("build.gradle: zusaetzlicher Baum", gradle_neu, 2, ["nur in build.gradle: portRoot/shared_assets/NEU"]),
        ("build.gradle: Baum RE15DOOR fehlt", gradle_ohne, 2, ["nur im Gate:         portRoot/shared_assets/RE15DOOR"]),
        ("build.gradle: include geaendert", gradle_include, 2, ["include ['**']", "include ['STAGE*/**']"]),
        ("build.gradle: unbekannte Anweisung", gradle_exclude, 2, ["unbekannte Anweisung", 'exclude "**/*.DO2"']),
        ("build.gradle fehlt", gradle_fehlt, 2, ["build.gradle fehlt"]),
        # --- ab hier Nachbesserung R1 (B3: Selbsttest-Luecken, B1: ZIP-Struktur wie Android)
        ("gute APK mit Datei > 1 MiB", nichts, 0, ["APK-ASSET-GATE-OK", PSX_GROSS_ZEILE], True),
        ("Datei > 1 MiB: 1 Byte hinter dem 1. MiB", gross_spaet, 1, ["Inhalt weicht ab (sha256", GROSS], True),
        ("CRC32-erhaltende Aenderung (gleiche Groesse)", crc_gleich, 1, ["Inhalt weicht ab (sha256", "assets/" + P2DS]),
        ("Manifest: Geisterzeile (weder APK noch Quelle)", geisterzeile, 1,
         ["Manifest nennt shared_assets/RE15DOOR/GEIST.DO2 (5 B), die APK hat keinen Eintrag"]),
        ("Manifest: Doppelzeile, erste mit falscher Groesse", doppelzeile, 1, ["Manifest nennt " + P2DS + " mehrfach"]),
        ("Manifest: Groessenfeld '4_396'", unterstrich, 1, ["Groessenfeld '4_396' ist keine Zahl"]),
        ("Manifest: kein UTF-8", man_kein_utf8, 1, ["Manifest ist kein gueltiges UTF-8"]),
        ("Manifest: Kommentarzeile '# Notiz'", man_notiz, 1, ["unerwartete Kommentarzeile '# Notiz'"]),
        ("Manifest: nennt sich selbst", man_selbst, 1, ["unzulaessiger Pfad 're15_assets.txt' (ohne '/'"]),
        ("Manifest: Kopfzeile fehlt", man_ohne_kopf, 1, ["Manifest-Kopfzeile fehlt/unlesbar"]),
        ("Eintragsname mit '\\' (F2)", name_backslash, 1, ["'\\' im Namen", "fehlt in der APK: " + P07]),
        ("Eintragsname mit NUL-Anhang (F3)", name_nul, 1, ["NUL-Byte im Namen", "fehlt in der APK: " + P07]),
        ("Eintragsname kein UTF-8", name_utf8, 1, ["Eintragsname kein gueltiges UTF-8"]),
        ("Eintragsname mit Steuerzeichen", name_steuer, 1, ["Steuerzeichen im Namen"]),
        ("Eintragsname mit '..'", name_punkte, 1, ["'.'/'..' oder '//' im Namen"]),
        ("Local Header: CRC falsch, Zentralverzeichnis richtig (F4)", lfh_crc, 1,
         ["CRC/Groessen im Local Header weichen", P07]),
        ("Local Header: Groesse falsch, Zentralverzeichnis richtig (F5)", lfh_groesse, 1,
         ["CRC/Groessen im Local Header weichen", P07]),
        ("Local Header: anderer Name", lfh_name, 1, ["Name im Local Header weicht", P07]),
        ("Local Header: Signatur zerstoert", lfh_signatur, 1, ["Local Header fehlt", P07]),
        ("Eintrag verschluesselt", verschluesselt, 1, ["verschluesselter Eintrag: " + P07]),
        ("Eintrag mit Methode 99", methode99, 1, ["Methode 99", P07]),
        ("Daten ragen ins Zentralverzeichnis", ragt, 1, ["Daten ragen ins Zentralverzeichnis: " + P07]),
        ("M16: classes.dex (Deflate, kein Asset) Byte im Strom gekippt", dex_kippen, 1, ["beschaedigt (CRC", "classes.dex"]),
        ("Deflate-Strom mit Muell dahinter", deflate_muell, 1, ["beschaedigt (CRC", "Deflate-Strom endet nicht", TEX]),
        ("Verzeichniseintrag unter assets/", verzeichnis, 1, ["Verzeichniseintrag in der APK: assets/shared_assets/RE15DOOR/"]),
        ("APK abgeschnitten (F7)", abgeschnitten, 2, ["APK nicht lesbar", "kein End-of-Central-Directory"]),
        ("Bytes hinter dem End-of-Central-Directory", muell_hinten, 2, ["(1 Bytes hinter dem End-of-Central-Directory"]),
        ("ZIP64-Markierung im End-of-Central-Directory", zip64, 2, ["ZIP64/mehrteiliges Archiv"]),
        ("Zentralverzeichnis-Signatur zerstoert (F8)", cd_signatur, 2, ["Zentralverzeichnis kaputt bei Eintrag 1"]),
        ("Zentralverzeichnis-Offset zu gross", cd_offset, 2, ["Zentralverzeichnis ausserhalb der Datei"]),
        ("End-of-Central-Directory nennt einen Eintrag zu wenig", eintrag_weniger, 2, ["Rest oder Ueberlauf"]),
        ("Deflate-Strom kaputt (Blocktyp 3)", deflate_kaputt, 1, ["Deflate-Strom kaputt", TEX]),
        ("gute APK: End-of-Central-Directory-Signatur in Asset-Daten", eocd_in_daten, 0, ["APK-ASSET-GATE-OK"]),
        ("build.gradle: stageAssets fehlt", g_ohne_task, 2, ['stageAssets", Sync) { ... } nicht gefunden']),
        ("build.gradle: stageAssets zweimal", g_zweimal, 2, ["mehrfach vorhanden"]),
        ("build.gradle: from in unbekanntem Format", g_from_format, 2, ["from-Anweisung nicht im bekannten Format"]),
        ("build.gradle: unbekannte Anweisung im from-Block", g_from_anweisung, 2, ["unbekannte Anweisung im from-Block"]),
        ("build.gradle: from ohne into", g_ohne_into, 2, ["from 'shared_assets/RE2' ohne into"]),
        ("build.gradle: from ohne Block", lambda f: f.gradle_ersetzen('from(new File(portRoot, "shared_assets/RE2"))          { into "shared_assets/RE2" }',
                                                                    'from(new File(portRoot, "shared_assets/RE2"))'),
         2, ["from 'shared_assets/RE2' ohne into"]),
        ("build.gradle: into(\"...\") mit Klammern (gut)", lambda f: f.gradle_ersetzen('{ into "shared_assets/RE2" }',
                                                                                    '{ into("shared_assets/RE2") }'),
         0, ["APK-ASSET-GATE-OK"]),
        ("build.gradle: preserve fehlt", g_ohne_preserve, 2, ["erwartet genau ein 'into(assetStage)'"]),
        ("build.gradle: Klammern passen nicht", g_klammer_falsch, 2,
         ["Klammern passen nicht (Zeichen %d)" % (_GRADLE_MUSTER.index("    into(assetStage)") + len("    into(assetStage"))]),
        ("build.gradle: '{' mit ')' geschlossen", lambda f: f.gradle_ersetzen('{ into "shared_assets/PSX" }', '{ into "shared_assets/PSX" )'),
         2, ["Klammern passen nicht (Zeichen %d)" % (_GRADLE_MUSTER.index('{ into "shared_assets/PSX" }') + len('{ into "shared_assets/PSX" '))]),
        ("build.gradle: stageAssets-Block ohne Ende", g_klammer_offen, 2, ["Klammer ohne Ende"]),
        ("build.gradle: Zeichenkette ohne Ende", g_zeichenkette_offen, 2, ["Zeichenkette ohne Ende"]),
        ("build.gradle: Kommentar /* ohne Ende", g_kommentar_offen, 2, ["Kommentar /* ohne Ende (ab Zeichen"]),
        ("Stored-Eintrag: csize 5 B groesser als usize", stored_zu_lang, 1,
         ["entpackt mehr als die 2748 B des Zentralverzeichnisses", P07]),
        # --- ab hier Nachbesserung R2 (B1 Teil-Mutanten, B8, B2 Tuer-Soll, B6 Wurzel + PC-Paket)
        ("M1: Manifest-Groesse 1 B KLEINER als die APK", man_kleiner, 1,
         ["Manifest-Groesse falsch: " + P2DS + " Manifest 4395 B, APK 4396 B"]),
        ("M9: classes.dex (Deflate) Laenge +100, CRC richtig", usize_plus("classes.dex"), 1,
         ["beschaedigt (CRC", "classes.dex: gelesen 3016 B", "Zentralverzeichnis 3116 B"]),
        ("M9: libmain.so (Stored) Laenge +100, CRC richtig", usize_plus(SO), 1,
         ["beschaedigt (CRC", "libmain.so: gelesen 2052 B", "Zentralverzeichnis 2152 B"]),
        ("M5: EOCD-Kommentarlaenge reicht ueber das Dateiende", kommentar_lang, 2, ["reicht 1 B ueber das Dateiende"]),
        ("M14: Data-Descriptor-Bit nur im Zentralverzeichnis, LFH-CRC falsch", dd_bit(False, True, True), 1,
         ["Data-Descriptor-Bit gesetzt (nur Zentralverzeichnis): " + P07]),
        ("M14: Data-Descriptor-Bit nur im Local Header", dd_bit(True, False, False), 1,
         ["Data-Descriptor-Bit gesetzt (Local Header): " + P07]),
        ("M14: Data-Descriptor-Bit in beiden Koepfen", dd_bit(True, True, False), 1,
         ["Data-Descriptor-Bit gesetzt (Local Header und Zentralverzeichnis): " + P07]),
        ("M16: libmain.so (Stored, kein Asset) Byte gekippt", so_kippen, 1, ["beschaedigt (CRC", SO]),
        ("B8: Praefix vor dem ersten Local Header, Offsets verschoben", praefix, 2,
         ["beginnt nicht mit einem Local Header"]),
        ("B8: Manifest-Kopfzeile mit arabisch-indischen Ziffern", kopf_arabisch, 1, ["Manifest-Kopfzeile fehlt/unlesbar"]),
        ("B2: Port-Tuerarchiv fehlt in Quelle UND APK", beide(Q07, None), 1,
         ["Port-Tuerarchiv fehlt: re15_port/shared_assets/RE15DOOR/P07G.DO2 (verlangt von: " + TUER_EIGEN_INC]),
        ("B2: Port-Tuerarchiv 0 Byte in Quelle UND APK", beide(Q07, lambda b: b""), 1,
         ["Port-Tuerarchiv passt nicht zur Engine-Tabelle: re15_port/shared_assets/RE15DOOR/P07G.DO2 hat 0 B, "
          "Tabelle 2748 B"]),
        ("B2: Port-Tuerarchiv gleiche Groesse, anderer Inhalt (FNV-1a)", beide(Q07, byte_kippen), 1,
         ["re15_port/shared_assets/RE15DOOR/P07G.DO2 FNV-1a"]),
        ("B2: Port-Tuerarchiv ohne Tabellenzeile (Quelle + APK)",
         beide("re15_port/shared_assets/RE15DOOR/P99X.DO2", lambda b: b"x" * 100), 1,
         ["re15_port/shared_assets/RE15DOOR/P99X.DO2 steht in keiner Engine-Tabelle"]),
        ("B2: RE2-Tuerarchiv fehlt in Quelle UND APK", beide(D13, None), 1,
         ["RE2-Tuerarchiv fehlt: re15_port/shared_assets/RE2/DOOR/DOOR13.DO2 (verlangt von: Tuerzeile"]),
        ("B2: RE2-Tuerarchiv 1 B kuerzer (Quelle + APK)",
         beide("re15_port/shared_assets/RE2/DOOR/DOOR07.DO2", lambda b: b[:-1]), 1,
         ["RE2-Tuerarchiv passt nicht zur Engine-Tabelle: re15_port/shared_assets/RE2/DOOR/DOOR07.DO2 hat 2547 B, "
          "Tabelle 2548 B"]),
        ("B2: RE2-Tabelle Tonteil > Sektor * 0x800", re2_aufbau(0x07, (3000, 500, 1)), 1,
         ["DOOR07.DO2 hat 2548 B, Tabelle 2548 B (Tonteil 3000"]),
        ("B2: RE2-Tabelle Sektor * 0x800 + Modellteil != Groesse", re2_aufbau(0x07, (1000, 499, 1, 2548)), 1,
         ["DOOR07.DO2 hat 2548 B, Tabelle 2548 B (Tonteil 1000, Modellteil 499"]),
        ("B2: Port-Tabelle Modellteil passt nicht", tuer_wert("P07G_modell", 699), 1,
         ["P07G.DO2 hat 2748 B, Tabelle 2748 B (Tonteil 1000, Modellteil 699"]),
        ("B2: Port-Tabelle Tonteil 1 B ueber Sektor * 0x800", tuer_wert("P07G_ton", 2049), 1,
         ["P07G.DO2 hat 2748 B, Tabelle 2748 B (Tonteil 2049"]),
        ("B2: RE2-Archiv nur als Griff-Spender verlangt, fehlt",
         beide("re15_port/shared_assets/RE2/DOOR/DOOR09.DO2", None), 1,
         ["RE2-Tuerarchiv fehlt: re15_port/shared_assets/RE2/DOOR/DOOR09.DO2 (verlangt von: Griff-Spender"]),
        ("B2: RE2-Spender nur ueber den Rueckfall in die Port-Tabelle, fehlt",
         beide("re15_port/shared_assets/RE2/DOOR/DOOR0C.DO2", None), 1,
         ["RE2-Tuerarchiv fehlt: re15_port/shared_assets/RE2/DOOR/DOOR0C.DO2 (verlangt von: Griff-Spender"]),
        ("B2: RE2-Archiv nur als Basis eines Port-Archivs verlangt, fehlt",
         alle(tabelle("tuer_eigen", '{ "P2DS", 0x13,', '{ "P2DS", 0x0A,'),
              tabelle("tuer_eigen", ZD, ZD.replace("0x13, 0, 0, 0x0A", "0x0A, 0, 0, 0x0A"))), 1,
         ["RE2-Tuerarchiv fehlt: re15_port/shared_assets/RE2/DOOR/DOOR0A.DO2 (verlangt von: Basis von P2DS"]),
        ("B2: Tuerzeile mit Port-Archiv anderer Basis", tabelle("tuer_eigen", ZD, ZD.replace("0x13, 0, 0, 0x0A", "0x07, 0, 0, 0x0A")),
         1, ["Tuerzeile nennt DOOR07 mit Port-Archiv P2DS (Basis DOOR13)"]),
        ("B2: Tuerzeile nennt Port-Archiv ausserhalb der Tabelle", tabelle("tuer_eigen", ZD, ZD.replace("0x02AAE, 2 }", "0x02AAE, 5 }")),
         1, ["Tuerzeile nennt Port-Archiv 5, re15_tuer_eigen hat 2"]),
        ("B2: Griff-Tausch nennt Port-Archiv ausserhalb der Tabelle", tabelle("tuer_eigen", E2, E2.replace("-450, 2,", "-450, 7,")),
         1, ["Griff-Tausch nennt Port-Archiv 7, re15_tuer_eigen hat 2"]),
        ("B2: Tuerzeile nennt RE2-Archiv ausserhalb der RE2-Tabelle",
         tabelle("tuer_zuordnung", "{0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 0, 2, 0x00BBE }", "{0, 0, 0, 0}, 0x14, 1, 0, 0xFF, 0, 2, 0x00BBE }"),
         1, ["Tuerzeile nennt RE2-Tuerarchiv 20, re2_tuer_arch hat 20 Zeilen"]),
        ("B2: Tabelle re15_tuer_eigen[3] mit 2 Zeilen", tabelle("tuer_eigen", "re15_tuer_eigen[2]", "re15_tuer_eigen[3]"), 2,
         ["re15_tuer_eigen[3] hat 2 Zeilen"]),
        ("B2: typedef ohne Feld fnv", tabelle("tuer_kopf", "uint32_t fnv;", "uint32_t fnw;"), 2,
         ["re15_tuer_eigen_t ohne Feld(er) ['fnv']"]),
        ("B2: Tabellenzeile mit mehr Werten als Feldern", tabelle("tuer_eigen", "08Xu, 0 },", "08Xu, 0, 0, 0 },"), 2,
         ["re15_tuer_eigen Zeile 1: 10 Werte, re15_tuer_eigen_t hat 9 Felder"]),
        ("B2: Tabellenzeile fuellt auch das skalare char-Feld (gut)", tabelle("tuer_eigen", "08Xu, 0 },", "08Xu, 0, 0 },"), 0,
         ["APK-ASSET-GATE-OK"]),
        ("B2: Engine-Tabelle re2_tuer_tabelle.inc fehlt", tabellendatei_weg(TUER_RE2_INC), 2,
         ["Engine-Tabelle fehlt: " + TUER_RE2_INC]),
        ("B2: Kennung doppelt", tabelle("tuer_eigen", '{ "P2DS",', '{ "P07G",'), 2,
         ["Kennung 'P07G' leer, doppelt oder kein Dateiname"]),
        ("B2: Kennung leer", tabelle("tuer_eigen", '{ "P2DS",', '{ "",'), 2, ["Kennung '' leer, doppelt"]),
        ("C-Leser: typedef fehlt", tabelle("tuer_kopf", "} re15_tuer_zeile_t;", "} re15_tuer_zeile_x;"), 2,
         ["'typedef struct { ... } re15_tuer_zeile_t;' nicht gefunden"]),
        ("C-Leser: typedef doppelt", anhaengen("tuer_kopf", "typedef struct { uint8_t a; } re15_griff_tausch_t;\n"), 2,
         ["'typedef struct { ... } re15_griff_tausch_t;' mehrfach"]),
        ("C-Leser: Feldtyp unbekannt", tabelle("tuer_kopf", "uint8_t  band;", "float    band;"), 2,
         ["re15_tuer_zeile_t - Feld nicht lesbar: 'float band'"]),
        ("C-Leser: Deklarator unlesbar", tabelle("tuer_kopf", "int16_t  qx[4], qz[4];", "int16_t  qx[4], *qz;"), 2,
         ["Deklarator nicht lesbar: '*qz'"]),
        ("C-Leser: Makroname statt Zahl", tabelle("tuer_zuordnung", "0x07, 1, 0, 0xFF, 0, 2, 0x00BBE }",
                                                  "TUER_07, 1, 0, 0xFF, 0, 2, 0x00BBE }"), 2,
         ["unerwartetes Zeichen im Initialisierer: 'TUER_07"]),
        ("C-Leser: Tabelle ohne schliessende Klammer", tabelle("tuer_re2", "%(zeilen)s\n};", "%(zeilen)s\n"), 2,
         ["re2_tuer_arch ohne Ende (schliessende '}' fehlt)"]),
        ("C-Leser: ';' nach der Tabelle fehlt", tabelle("tuer_eigen", "08Xu, 1 },\n};", "08Xu, 1 },\n}"), 2,
         ["re15_tuer_eigen: nach der schliessenden '}' fehlt ';'"]),
        ("C-Leser: Zeile ist kein { ... }", tabelle("tuer_zuordnung", "{ 0x13, 0x09, 1, 1, {0, 0, 0}, {2048, 2048, 0}, 1500, -702 },",
                                                    "0x13,"), 2, ["re15_griff_tausche Zeile 1 ist kein { ... }"]),
        ("C-Leser: Kennung ist keine Zeichenkette", tabelle("tuer_eigen", '{ "P07G", 0x07,', '{ 0x50, 0x07,'), 2,
         ["Feld kennung: Zeichenkette mit < 8 Bytes erwartet"]),
        ("C-Leser: Kennung 8 Bytes (char[8] braucht das NUL)", tabelle("tuer_eigen", '{ "P07G", 0x07,', '{ "P07GABCD", 0x07,'),
         2, ["Feld kennung: Zeichenkette mit < 8 Bytes erwartet"]),
        ("C-Leser: Feld qx ist keine Liste", tabelle("tuer_eigen", ZE, ZE.replace("1000, {0, 0, 0, 0}, {0, 0, 0, 0}", "1000, 0, {0, 0, 0, 0}")),
         2, ["Feld qx: { bis zu 4 Zahlen } erwartet"]),
        ("C-Leser: Feld qx mit 5 Zahlen", tabelle("tuer_eigen", ZE, ZE.replace("1000, {0, 0, 0, 0},", "1000, {0, 0, 0, 0, 0},")),
         2, ["Feld qx: { bis zu 4 Zahlen } erwartet"]),
        ("C-Leser: Feld qx mit Zeichenkette", tabelle("tuer_eigen", ZE, ZE.replace("1000, {0, 0, 0, 0},", '1000, {0, "a", 0, 0},')),
         2, ["Feld qx: { bis zu 4 Zahlen } erwartet"]),
        ("C-Leser: Feld re2_nr ist keine Zahl", tabelle("tuer_eigen", ZE, ZE.replace("0x07, 1, 0, 0xFF", '"x", 1, 0, 0xFF')),
         2, ["Feld re2_nr: Zahl erwartet"]),
        ("C-Leser: Zeichenkette ohne Ende", tabelle("tuer_eigen", '{ "P07G", 0x07,', '{ "P07G, 0x07,'), 2,
         ["Zeichenkette ohne Ende"]),
        ("C-Leser: Kommentar /* ohne Ende", anhaengen("tuer_zuordnung", "/* offen"), 2, ["Kommentar /* ohne Ende"]),
        ("C-Leser: Tabelle doppelt", anhaengen("tuer_zuordnung", "static const re15_griff_tausch_t re15_griff_tausche[1] = { {0} };\n"),
         2, ["re15_griff_tausche[N] = {' mehrfach"]),
        ("C-Leser: Tabelle fehlt", tabelle("tuer_zuordnung", "re15_griff_tausche[1]", "re15_griff_tauschx[1]"), 2,
         ["re15_griff_tausche[N] = {' nicht gefunden"]),
        ("B6: Ordner unter shared_assets in keiner Liste", wurzel_neu, 1,
         ["re15_port/shared_assets/RE15NEU liegt in keinem Asset-Baum"]),
        ("--paket: gutes PC-Paket", modus("paket"), 0, ["APK-ASSET-GATE-PAKET-OK"]),
        ("--paket: Datei fehlt im Paket", modus("paket", paket_datei_weg("shared_assets/RE2/DOOR/DOOR07.DO2")), 1,
         ["fehlt im Paket: shared_assets/RE2/DOOR/DOOR07.DO2"]),
        ("--paket: Zusatzbaum im Paket (copy_common weicht ab)", modus("paket", paket_datei_neu("shared_assets/RE15NEU/NEU.DAT")),
         1, ["zusaetzlich im Paket", "shared_assets/RE15NEU/NEU.DAT"]),
        ("--paket: gleiche Groesse, anderer Inhalt", modus("paket", paket_byte("synchro/STAGE2/room2000/main00.wav")), 1,
         ["Inhalt weicht ab: synchro/STAGE2/room2000/main00.wav"]),
        ("--paket: synchro/unused im Paket", modus("paket", paket_datei_neu("synchro/unused/STAGE1/room1170/alt.mp3")), 1,
         ["zusaetzlich im Paket", "synchro/unused/STAGE1/room1170/alt.mp3"]),
        ("--paket: Tuer-Soll greift auch hier", modus("paket", beide(Q07, None)), 1,
         ["Port-Tuerarchiv fehlt: re15_port/shared_assets/RE15DOOR/P07G.DO2"]),
        ("--paket: Paketordner fehlt", modus("paket", paket_ordner_weg), 2, ["Paketordner fehlt"]),
        ("--quellbaum: gut", modus("quellbaum"), 0, ["APK-ASSET-GATE-QUELLBAUM-OK",
                                                      "2/2 wie die Engine-Tabelle (Groesse, Aufbau, FNV-1a)"]),
        ("--quellbaum: RE2-Tuerarchiv fehlt", modus("quellbaum", beide(D13, None)), 1,
         ["RE2-Tuerarchiv fehlt: re15_port/shared_assets/RE2/DOOR/DOOR13.DO2"]),
        ("Manifest genau an der Grenze (Pruefhaken senkt 64 MiB)", man_grenze(False), 0, ["APK-ASSET-GATE-OK"]),
        ("Manifest 1 B ueber der Grenze", man_grenze(True), 1, ["das Geraet liest es nicht (asset_abgleich.h"]),
        ("EOCD: Datentraeger-Nummer 1", eocd_feld(4, 1), 2, ["ZIP64/mehrteiliges Archiv"]),
        ("EOCD: Zentralverzeichnis auf Datentraeger 1", eocd_feld(6, 1), 2, ["ZIP64/mehrteiliges Archiv"]),
        ("EOCD: Eintraege hier != Eintraege gesamt", eocd_feld(8, plus=-1), 2, ["ZIP64/mehrteiliges Archiv"]),
        ("ZIP64-Locator vor dem EOCD (Zaehler normal)", z64_locator, 2, ["ZIP64/mehrteiliges Archiv"]),
        ("Zentralverzeichnis endet mitten im Kopf", cd_kopf_halb, 2, ["Zentralverzeichnis kaputt bei Eintrag"]),
        ("RE2-Tabelle: Groesse 1 B ueber der Datei", re2_aufbau(0x07, (1000, 500, 1, 2549)), 1,
         ["DOOR07.DO2 hat 2548 B, Tabelle 2549 B"]),
        ("RE2-Tabelle: Groesse 1 B unter der Datei", re2_aufbau(0x07, (1000, 500, 1, 2547)), 1,
         ["DOOR07.DO2 hat 2548 B, Tabelle 2547 B"]),
        ("RE2-Tabelle: Modellteil 1 B zu gross", re2_aufbau(0x07, (1000, 501, 1, 2548)), 1,
         ["DOOR07.DO2 hat 2548 B, Tabelle 2548 B (Tonteil 1000, Modellteil 501"]),
        ("Port-Archiv: FNV-1a kleiner als die Tabelle", tuer_fnv_richtung(True), 1, ["P07G.DO2 FNV-1a"]),
        ("Port-Archiv: FNV-1a groesser als die Tabelle", tuer_fnv_richtung(False), 1, ["P07G.DO2 FNV-1a"]),
        ("Tuerzeile nennt DOOR00 (erste Tabellenzeile)",
         tabelle("tuer_zuordnung", "{0, 0, 0, 0}, 0x07, 1, 0, 0xFF, 0, 2, 0x00BBE }", "{0, 0, 0, 0}, 0x00, 1, 0, 0xFF, 0, 2, 0x00BBE }"),
         1, ["RE2-Tuerarchiv fehlt: re15_port/shared_assets/RE2/DOOR/DOOR00.DO2"]),
        ("Tuerzeile mit Port-Archiv kleinerer Basis", tabelle("tuer_eigen", ZE, ZE.replace("0x07, 1, 0, 0xFF", "0x13, 1, 0, 0xFF")),
         1, ["Tuerzeile nennt DOOR13 mit Port-Archiv P07G (Basis DOOR07)"]),
        ("Tabelle re15_tuer_eigen[1] mit 2 Zeilen", tabelle("tuer_eigen", "re15_tuer_eigen[2]", "re15_tuer_eigen[1]"), 2,
         ["re15_tuer_eigen[1] hat 2 Zeilen"]),
        ("C-Leser: Zeichenkette am Dateiende ohne Ende", anhaengen("tuer_zuordnung", '"offen'), 2, ["Zeichenkette ohne Ende"]),
        ("C-Leser: //-Kommentar am Dateiende ohne Zeilenumbruch (gut)", anhaengen("tuer_zuordnung", "// Kommentar {"), 0,
         ["APK-ASSET-GATE-OK"]),
        ("Eintragsname mit Steuerzeichen 0x1F", tabelle_name(P07, b"P07G", b"P07\x1f"), 1, ["Steuerzeichen im Namen"]),
        ("Eintragsname mit DEL (0x7F)", tabelle_name(P07, b"P07G", b"P07\x7f"), 1, ["Steuerzeichen im Namen"]),
        ("Eintragsname beginnt mit '/'", tabelle_name(P07, b"assets/", b"/ssets/"), 1, ["absoluter Pfad"]),
        ("Eintragsname mit '//'", tabelle_name(P07, b"/RE15DOOR/", b"//E15DOOR/"), 1, ["'//' im Namen"]),
        ("Eintragsname mit '.'-Teil", tabelle_name(P07, b"/P07G.DO2", b"/./7G.DO2"), 1, ["'.'/'..' oder '//' im Namen"]),
        ("Manifest: Geisterzeile, die vor dem Manifest sortiert", man_zeilen([("a/geist.bin", 5)]), 1,
         ["Manifest nennt a/geist.bin (5 B), die APK hat keinen Eintrag"]),
        ("Manifest: Pfad mit Backslash", man_zeilen([("shared_assets\\RE2\\X.BIN", 5)]), 1, ["unzulaessiger Pfad"]),
        ("Manifest: Kopfzeile Bytes 1 zu gross", man_zeilen([], kopf_bytes=1), 1, ["Manifest-Kopfzeile passt nicht"]),
        ("Manifest: Kopfzeile Anzahl 1 zu klein", man_zeilen([], kopf_anzahl=-1), 1, ["Manifest-Kopfzeile passt nicht"]),
        ("Manifest: Kommentarzeile direkt nach dem Kopf", man_zeilen([], nach_kopf="# Notiz"), 1,
         ["Manifest-Zeile 2: unerwartete Kommentarzeile"]),
        ("APK-Eintrag laenger als die Quelle", laenger, 1, ["Groesse weicht ab: " + TEX + "  Quelle 5000 B, APK 5010 B"]),
        ("Zusatzeintrag assets/AAA.BIN (sortiert vor dem Manifest)", zusatz_vorn, 1,
         ["zusaetzlich in der APK (kein Asset-Baum liefert ihn): assets/AAA.BIN"], False, ["Manifest nennt AAA.BIN"]),
        ("APK-Inhalt: sha256 kleiner als die Quelle", apk_inhalt_richtung(D7, True), 1, ["Inhalt weicht ab (sha256", D7]),
        ("APK-Inhalt: sha256 groesser als die Quelle", apk_inhalt_richtung(D7, False), 1, ["Inhalt weicht ab (sha256", D7]),
        ("Local Header zeigt auf die letzten 8 Bytes ('PK\\3\\4' im EOCD-Kommentar)", lfh_hinten, 1,
         ["Local Header fehlt bei Offset"]),
        ("Local Header: Name kleiner (DO1)", lambda f: f.roh.append(
            lambda apk: _fx_name_ersetzen(apk, P07, P07.replace("DO2", "DO1").encode(), nur_lfh=True)), 1,
         ["Name im Local Header weicht"]),
        ("Local Header: CRC - 1", lfh_feld(14, -1), 1, ["CRC/Groessen im Local Header weichen"]),
        ("Local Header: CRC + 1", lfh_feld(14, 1), 1, ["CRC/Groessen im Local Header weichen"]),
        ("Local Header: csize + 1", lfh_feld(18, 1), 1, ["CRC/Groessen im Local Header weichen"]),
        ("Local Header: usize - 1", lfh_feld(22, -1), 1, ["CRC/Groessen im Local Header weichen"]),
        ("verschluesselt: Bit 0 nur im Local Header", flag_bit(0x0001, True, False), 1, ["verschluesselter Eintrag: " + P07]),
        ("verschluesselt: Bit 0 nur im Zentralverzeichnis", flag_bit(0x0001, False, True), 1, ["verschluesselter Eintrag: " + P07]),
        ("verschluesselt: Bit 6 (starke Verschluesselung)", flag_bit(0x0040, True, True), 1, ["verschluesselter Eintrag: " + P07]),
        ("verschluesselt: Bit 13 (maskierter Kopf)", flag_bit(0x2000, True, True), 1, ["verschluesselter Eintrag: " + P07]),
        ("EOCD: Zentralverzeichnis-Offset 0xFFFFFFFF", eocd_cd_off_ffff, 2, ["ZIP64/mehrteiliges Archiv"]),
        ("Deflate-Strom um 10 B gekuerzt", deflate_kurz, 1, ["Deflate-Strom endet nicht genau am Eintragsende (unvollstaendig)"]),
        ("build.gradle: zwei into im from-Block", lambda f: f.gradle_ersetzen('{ into "shared_assets/RE2" }',
                                                                            '{ into "shared_assets/RE2"; into "shared_assets/X" }'),
         2, ["unbekannte Anweisung im from-Block"]),
        ("build.gradle: into(assetStage) fehlt", lambda f: f.gradle_ersetzen("    into(assetStage)\n", ""), 2,
         ["erwartet genau ein 'into(assetStage)'"]),
        ("build.gradle: into(assetStage) zweimal", lambda f: f.gradle_ersetzen("    into(assetStage)\n", "    into(assetStage)\n    into(assetStage)\n"),
         2, ["erwartet genau ein 'into(assetStage)'"]),
        ("build.gradle: preserve zweimal", lambda f: f.gradle_ersetzen('    preserve { include "re15_assets.txt" }\n',
                                                                     '    preserve { include "re15_assets.txt" }\n    preserve { include "re15_assets.txt" }\n'),
         2, ["erwartet genau ein 'into(assetStage)'"]),
        ("--paket: sha256 kleiner als die Quelle", modus("paket", paket_richtung("shared_assets/RE2/CDEMD0.EMS", True)), 1,
         ["Inhalt weicht ab: shared_assets/RE2/CDEMD0.EMS"]),
        ("--paket: sha256 groesser als die Quelle", modus("paket", paket_richtung("shared_assets/RE2/CDEMD0.EMS", False)), 1,
         ["Inhalt weicht ab: shared_assets/RE2/CDEMD0.EMS"]),
        ("Befundliste gekuerzt (--max-zeilen 1, 2 Befunde einer Art)", alle(max_zeilen(1), ohne_p07, ohne_d13_apk), 1,
         ["... und 1 weitere"]),
        ("--max-zeilen 1 bei genau 1 Befund: nicht gekuerzt", alle(max_zeilen(1), ohne_p07), 1,
         ["fehlt in der APK: " + P07], False, ["weitere"]),
        ("Kopf ohne #define RE15_DOOR_KEIN_SPENDER", tabelle("tuer_kopf", "#define RE15_DOOR_KEIN_SPENDER  0xFFu\n", ""), 2,
         ["#define RE15_DOOR_KEIN_SPENDER nicht gefunden"]),
        ("Praefix aus 'Z' vor dem ersten Local Header", praefix_zz, 2, ["beginnt nicht mit einem Local Header"]),
        ("EOCD mit 65535 B Kommentar (gut)", kommentar_max, 0, ["APK-ASSET-GATE-OK"]),
        ("TORSE.VBS 0 B in Quelle UND APK", torse_leer, 1, ["Pflichtdatei fehlt/leer: re15_port/shared_assets/RE2/TORSE.VBS"]),
        ("RE15DOOR nur mit liesmich.txt und alt/P99X.DO2", re15door_ohne_do2, 1,
         ["Pflichtinhalt fehlt: re15_port/shared_assets/RE15DOOR/*.DO2 (0 Dateien)"]),
        ("EOCD: Zentralverzeichnis 3 B kleiner als belegt", cd_groesse_kleiner, 2, ["Rest oder Ueberlauf"]),
        ("EOCD: Eintraege hier > Eintraege gesamt", eocd_feld(8, plus=1), 2, ["ZIP64/mehrteiliges Archiv"]),
        ("letzter Eintrag ohne Name (Kopf endet genau am Ende des Zentralverzeichnisses, gut)", leer_am_ende, 0,
         ["APK-ASSET-GATE-OK"]),
        ("Zentralverzeichnis-Signatur durch kleinere Bytes ersetzt", signatur_klein("cd"), 2, ["Zentralverzeichnis kaputt bei Eintrag 1"]),
        ("Local-Header-Signatur durch kleinere Bytes ersetzt", signatur_klein("lfh"), 1, ["Local Header fehlt", P07]),
        ("libmain.so: CRC32 der Daten kleiner als im Zentralverzeichnis", crc_richtung(True), 1, ["beschaedigt (CRC", SO]),
        ("libmain.so: CRC32 der Daten groesser als im Zentralverzeichnis", crc_richtung(False), 1, ["beschaedigt (CRC", SO]),
        ("Manifest-Eintrag beschaedigt: kein Manifest-Befund aus kaputten Daten", manifest_daten_kippen, 1,
         ["beschaedigt (CRC", "assets/re15_assets.txt"], False, ["Manifest-Groesse falsch", "Manifest-Kopfzeile passt nicht"]),
        ("Manifest-Eintrag mit falscher LFH-CRC: nicht gelesen", manifest_lfh_crc, 1,
         ["CRC/Groessen im Local Header weichen", "assets/re15_assets.txt"]),
        # --- ab hier Runde 4 (Gegenpruefung R3, B2: Mutanten MU1-MU8); erwartete Texte nur ASCII (die Ausgabe
        # des Gates ist unter Windows in der ANSI-Codepage kodiert)
        ("R4 MU1/MU8: Manifestpfad mit Leerzeichen am Ende", pfad_ende(" "), 1,
         ["Manifest nennt %s  (2748 B), die APK hat keinen Eintrag" % P07M, "fehlt im Manifest: %s (wird" % P07M]),
        ("R4 MU1/MU8: Manifestpfad mit Tab am Ende", pfad_ende("\t"), 1,
         ["unzulaessiger Pfad '%s\t' (Steuerzeichen)" % P07M, "fehlt im Manifest: %s (wird" % P07M]),
        ("R4 MU1/MU3: Manifestzeile endet mit VT (\\x0b)", pfad_ende("\x0b"), 1,
         ["unzulaessiger Pfad '%s\x0b' (Steuerzeichen)" % P07M, "fehlt im Manifest: %s (wird" % P07M]),
        ("R4 MU1/MU3: Manifestzeile endet mit FF (\\x0c)", pfad_ende("\x0c"), 1,
         ["unzulaessiger Pfad '%s\x0c' (Steuerzeichen)" % P07M, "fehlt im Manifest: %s (wird" % P07M]),
        ("R4 MU1/MU8: Manifestpfad mit NBSP (U+00A0) am Ende", pfad_ende(" "), 1,
         ["Manifest nennt %s" % P07M, "fehlt im Manifest: %s (wird" % P07M]),
        ("R4 MU8: Manifestpfad mit Leerzeichen am Anfang", man_ersetzen("\t%s\n" % P07M, "\t %s\n" % P07M), 1,
         ["Manifest nennt  %s (2748 B), die APK hat keinen Eintrag" % P07M, "fehlt im Manifest: %s (wird" % P07M]),
        ("R4 MU3: U+2028 statt '\\n' vor einer Zeile", trenner(" "), 1,
         ["fehlt im Manifest: %s (wird" % P07M, "Manifest-Kopfzeile passt nicht"]),
        ("R4 MU3: '\\r' allein als Zeilentrenner", trenner("\r"), 1,
         ["fehlt im Manifest: %s (wird" % P07M, "Manifest-Kopfzeile passt nicht"]),
        ("R4 MU3: FS (\\x1c) als Zeilentrenner", trenner("\x1c"), 1,
         ["fehlt im Manifest: %s (wird" % P07M, "Manifest-Kopfzeile passt nicht"]),
        ("R4 MU3: NEL (U+0085) als Zeilentrenner", trenner("\u0085"), 1,
         ["fehlt im Manifest: %s (wird" % P07M, "Manifest-Kopfzeile passt nicht"]),
        ("R4 MU3: VT (\\x0b) als Zeilentrenner", trenner("\x0b"), 1,
         ["fehlt im Manifest: %s (wird" % P07M, "Manifest-Kopfzeile passt nicht"]),
        ("R4 MU3: FF (\\x0c) als Zeilentrenner", trenner("\x0c"), 1,
         ["fehlt im Manifest: %s (wird" % P07M, "Manifest-Kopfzeile passt nicht"]),
        # Groesse >= 64 GiB (atoll liest 64 Bit, das Geraet vergleicht mit der echten Dateigroesse): Kopfzeile
        # passend mitgerechnet, damit NUR die Groesse abweicht - ein Leser mit 32-Bit-Groessen saehe 2748 B
        ("R4: Groessenfeld 64 GiB + 2748 (32-Bit-Ueberlauf waere 2748)",
         lambda f: setattr(f, "manifest_zeilen", [(p, g + (64 << 30) if p == P07M else g, s)
                                                  for p, g, s in f.manifest_aus_eintraegen()]), 1,
         ["Manifest-Groesse falsch: %s Manifest %d B, APK 2748 B" % (P07M, (64 << 30) + 2748)], False,
         ["Manifest-Kopfzeile passt nicht"]),
        ("R4 MU2/MU6: Groessenfeld in Vollbreit-Ziffern", groesse_ziffern("０１２３４５６７８９"),
         1, ["ist keine Zahl (1-18 Ziffern 0-9)", "fehlt im Manifest: %s (wird" % P07M]),
        ("R4 MU2/MU6: Groessenfeld in arabisch-indischen Ziffern",
         groesse_ziffern("٠١٢٣٤٥٦٧٨٩"), 1,
         ["ist keine Zahl (1-18 Ziffern 0-9)", "fehlt im Manifest: %s (wird" % P07M]),
        ("R4 MU6: Groessenfeld hochgestellt (isdigit, nicht Nd)",
         groesse_ziffern("⁰¹²³⁴⁵⁶⁷⁸⁹"), 1,
         ["ist keine Zahl (1-18 Ziffern 0-9)", "fehlt im Manifest: %s (wird" % P07M]),
        ("R4: Manifest-Kopfzeile in Vollbreit-Ziffern",
         kopf_ziffern("０１２３４５６７８９"), 1, ["Manifest-Kopfzeile fehlt/unlesbar"]),
        ("R4 MU7: Kopfzeile mit Leerzeichen davor", kopf_rand(" ", ""), 1,
         ["Manifest-Kopfzeile fehlt/unlesbar", "Manifest-Zeile 1: erwartet"]),
        ("R4 MU7: Kopfzeile mit Leerzeichen dahinter", kopf_rand("", " "), 1, ["Manifest-Kopfzeile fehlt/unlesbar"]),
        ("R4 MU7: Kopfzeile mit Tab dahinter", kopf_rand("", "\t"), 1, ["Manifest-Kopfzeile fehlt/unlesbar"]),
        ("R4 MU5: Methode 1 auf Deflate-Daten", methode_nr(1, True), 1,
         ["Methode 1 (lesbar sind 0 = Stored, 8 = Deflate): " + TEX]),
        ("R4 MU5: Methode 7 auf Deflate-Daten", methode_nr(7, True), 1,
         ["Methode 7 (lesbar sind 0 = Stored, 8 = Deflate): " + TEX]),
        ("R4 MU5: Methode 1 auf Stored-Daten", methode_nr(1, False), 1,
         ["Methode 1 (lesbar sind 0 = Stored, 8 = Deflate): " + TEX]),
        ("R4 MU4: Manifest 1 B ueber 64 MiB, OHNE Pruefhaken", man_geraet_grenze, 1,
         ["Manifest %d B > %d B (64 MiB): das Geraet liest es nicht" % (_FX_GERAET_MANIFEST + 1, _FX_GERAET_MANIFEST)]),
        # --- ab hier Runde 34a N1: Format v2 (Spalte sha256, Pfad-/Listenregeln aus jni/asset_abgleich.h, v1 abgelehnt)
        ("N1: Manifest im alten Format v1 (wie v0.8.19)", man_v1, 1,
         ["Manifest im alten Format v1", "entpackt NICHTS"], False, ["fehlt im Manifest"]),
        ("N1: Pruefsumme falsch (andere gueltige sha256, gleiche Groesse)",
         man_ersetzen("\t{S}\t%s\n" % P07M, "\t%s\t%s\n" % ("0123456789abcdef" * 4, P07M)), 1,
         ["Manifest-Pruefsumme falsch: %s Manifest 0123456789abcdef.., APK " % P07M], False,
         ["Manifest-Groesse falsch", "fehlt im Manifest"]),
        ("N1: Pruefsumme fehlt (v1-Zeile in einer v2-Liste)", man_ersetzen("\t{S}\t%s\n" % P07M, "\t%s\n" % P07M), 1,
         ["1 Tab(s): '2748\t%s'" % P07M, "fehlt im Manifest: %s (wird" % P07M]),
        ("N1: Pruefsumme in Grossbuchstaben (Wert sonst gleich)", man_ersetzen("\t{S}\t%s\n" % P07M, "\t{SU}\t%s\n" % P07M), 1,
         ["ist kein sha256 (genau 64 Zeichen 0-9a-f, Kleinbuchstaben)", "fehlt im Manifest: %s (wird" % P07M]),
        ("N1: Pruefsumme mit Leerzeichen dahinter", man_ersetzen("\t{S}\t%s\n" % P07M, "\t{S} \t%s\n" % P07M), 1,
         ["ist kein sha256", "fehlt im Manifest: %s (wird" % P07M]),
        ("N1: Pruefsumme mit Leerzeichen davor", man_ersetzen("\t{S}\t%s\n" % P07M, "\t {S}\t%s\n" % P07M), 1,
         ["ist kein sha256", "fehlt im Manifest: %s (wird" % P07M]),
        ("N1: Pruefsumme 63 Zeichen", man_ersetzen("\t{S}\t%s\n" % P07M, "\t{S63}\t%s\n" % P07M), 1,
         ["ist kein sha256", "fehlt im Manifest: %s (wird" % P07M]),
        ("N1: Pruefsumme 65 Zeichen", man_ersetzen("\t{S}\t%s\n" % P07M, "\t{S}0\t%s\n" % P07M), 1,
         ["ist kein sha256", "fehlt im Manifest: %s (wird" % P07M]),
        ("N1: Pruefsumme leer", man_ersetzen("\t{S}\t%s\n" % P07M, "\t\t%s\n" % P07M), 1,
         ["Pruefsumme '' ist kein sha256", "fehlt im Manifest: %s (wird" % P07M]),
        ("N1: Groessenfeld mit 19 Ziffern", man_ersetzen("\n2748\t{S}\t%s\n" % P07M, "\n0000000000000002748\t{S}\t%s\n" % P07M),
         1, ["Groessenfeld '0000000000000002748' ist keine Zahl", "fehlt im Manifest: %s (wird" % P07M]),
        ("N1: leere Zeilen zwischen den Zeilen (gut)", man_ersetzen("\n2748\t{S}\t", "\n\n\r\n\r\r\n2748\t{S}\t"), 0,
         ["APK-ASSET-GATE-OK"]),
        ("N1: Pfad ohne '/'", man_geister([("GEIST.BIN", 5)]), 1, ["unzulaessiger Pfad 'GEIST.BIN' (ohne '/'"]),
        ("N1: Pfad endet auf .neu", man_geister([("shared_assets/PSX/X.neu", 5)]), 1,
         ["unzulaessiger Pfad 'shared_assets/PSX/X.neu' (endet auf .neu"]),
        ("N1: Pfad endet auf .NEU (Gross/klein egal)", man_geister([("shared_assets/PSX/X.NEU", 5)]), 1,
         ["unzulaessiger Pfad 'shared_assets/PSX/X.NEU' (endet auf .neu"]),
        ("N1: Pfad 513 Bytes", man_geister([("shared_assets/" + "x" * 499, 5)]), 1, ["(513 Bytes > 512)"]),
        ("N1: Pfad genau 512 Bytes (Regel gut, Datei fehlt in der APK)", man_geister([("shared_assets/" + "x" * 498, 5)]), 1,
         ["Manifest nennt shared_assets/xxxx"], False, ["unzulaessiger Pfad"]),
        ("N1: Pfade nur in Gross/klein verschieden", man_geister([("shared_assets/re15door/P07G.DO2", 2748)]), 1,
         ["Pfade nur in Gross/klein verschieden: shared_assets/RE15DOOR/P07G.DO2 / shared_assets/re15door/P07G.DO2"]),
        ("N1: NUL-Byte im Pfad", man_ersetzen("\t%s\n" % P07M, "\t%s\0x\n" % P07M), 1,
         ["Manifest enthaelt ein NUL-Byte", "fehlt im Manifest: %s (wird" % P07M]),
        ("N1: Manifest ohne Dateien", lambda f: setattr(f, "manifest_roh", b"# re15 assets v2 0 0\n"), 1,
         ["Manifest ohne Dateien", "fehlt im Manifest: %s (wird" % P07M]),
        ("N1: Kopfzeile v3", lambda f: setattr(f, "manifest_roh", _Fall.manifest_text(f.manifest_aus_eintraegen()).replace(
            "# re15 assets v2 ", "# re15 assets v3 ", 1).encode()), 1, ["Manifest-Kopfzeile fehlt/unlesbar"]),
        ("N1: Summe der Groessen > 2^63-1 (10 x 18 Ziffern)",
         man_geister([("shared_assets/PSX/G%d.BIN" % i, 999999999999999999) for i in range(10)]), 1,
         ["Summe der Groessen > 2^63-1"]),
    )


def _fall_vorbereiten(tmp, nr, fall):
    titel, faelschen, soll_rc, soll_text = fall[:4]
    gross = len(fall) > 4 and fall[4]
    wurzel = os.path.join(tmp, "f%02d" % nr)
    os.makedirs(wurzel)
    f = _Fall(wurzel, gross=gross)
    faelschen(f)
    f.schreiben()
    if f.nach_schreiben:
        f.nach_schreiben()
    return f


def _fall_laufen(tmp, nr, fall):
    """Fall aufbauen (im Arbeiter-Thread, damit das parallel laeuft) und das Gate darauf starten."""
    try:
        f = _fall_vorbereiten(tmp, nr, fall)
    except Exception:
        return -1, "Fixture-Fehler:\n" + traceback.format_exc()
    ziel = {"apk": [f.apk], "quellbaum": ["--quellbaum"], "paket": ["--paket", f.paket]}[f.modus]
    umgebung = dict(os.environ)
    umgebung.pop("RE15_GATE_MANIFEST_MAX", None)
    umgebung.update(f.umgebung)
    r = subprocess.run([sys.executable, os.path.abspath(__file__), "--repo", f.repo] + f.argumente + ziel,
                       stdout=subprocess.PIPE, stderr=subprocess.STDOUT, stdin=subprocess.DEVNULL,
                       timeout=120, env=umgebung)
    return r.returncode, r.stdout.decode("utf-8", "replace")


# Innere Proben (Nachbesserung R2, Mutanten-Probe): Hilfsfunktionen, die die Fall-Fixture nur mit EINER
# Eingabe erreicht - die Baumliste hat genau ein include-Muster ("STAGE*/**"), und die muss build.gradle
# gleichen. Teil-Abschwaechungen von _ant_passt (z.B. '*' wie '**', Grenze j <= len) blieben dort
# unsichtbar; hier laeuft sie gegen feste Ant/Gradle-Antworten.
_ANT_PROBEN = (
    ("STAGE*/**", "STAGE1/room1170/main00.wav", True), ("STAGE*/**", "STAGE1", True),
    ("STAGE*/**", "unused/STAGE1/x.mp3", False), ("STAGE*/**", "README.md", False), ("STAGE*/**", "stage1/x", False),
    ("*/x", "a/x", True), ("*/x", "a/b/x", False), ("*", "a/b", False), ("*", "", True), ("x", "", False),
    ("a/?", "a/b", True), ("a/?", "a/bc", False), ("a/?", "a/", False), ("?b", "!b", True),
    ("**/x.wav", "x.wav", True), ("**/x.wav", "a/b/x.wav", True), ("**/x.wav", "a/b/y.wav", False),
    ("a/**/b", "a/b", True), ("a/**/b", "a/x/y/b", True), ("a/**/b", "a/x/y/c", False), ("a/**", "a", True),
    ("a/**", "b", False), ("a/b", "a", False), ("a", "a/b", False), ("A*", "a1", False), ("#*", "#1", True),
    ("(*)", "(1)", True), ("x*y", "x/y", False),
    # Zeichen UNTER '*' bzw. '?' sind Literale (kein Platzhalter): '#', '(', '1'
    ("#x", "ax", False), ("#x", "#x", True), ("(x)", "axb", False), ("1x", "2x", False), ("1x", "1x", True),
)


# Hinweis in "fehlt in der APK" (warum Gradle/AAPT die Datei weglassen): Teiltext oder "" = kein Hinweis
_AUSLASS_PROBEN = (
    ("s/PSX/.versteckt.bin", "Name beginnt mit '.'"), ("s/.cache/x.bin", "Ordner '.cache' beginnt mit '.'"),
    ("s/_intern/x.bin", "Ordner '_intern' beginnt mit '_'"), ("s/x.bin~", "Gradle-Standardausschluss"),
    ("s/#x#", "Gradle-Standardausschluss"), ("s/%x%", "Gradle-Standardausschluss"), ("s/x#", ""), ("s/#x", ""),
    ("s/%x", ""), ("s/x%", ""), ("s/Thumbs.db", "AAPT-Ignoriermuster"), ("s/picasa.ini", "AAPT-Ignoriermuster"),
    ("s/a.SCC", "AAPT-Ignoriermuster"), ("s/x.bin", ""), ("s/a_b/x.bin", ""), ("s/a.b/x.bin", ""),
    ("s/x~y", ""), ("s/thumbs.dbx", ""),
)


# manifest_lesen gegen die Regeln des Geraets (Runde 34a N1): dieselben Listen wie der PC-Unit-Test
# re15_port/tests/unit/test_r34a_asset_abgleich.c (asset_abgleich.c) - (Titel, Bytes, Geraet nimmt an?)
_MS = b"0123456789abcdef" * 4
_MK = b"# re15 assets v2 1 5\n5\t" + _MS + b"\t"
_MANIFEST_PROBEN = (
    ("gut", _MK + b"a/b\n", True),
    ("CRLF", b"# re15 assets v2 1 5\r\n5\t" + _MS + b"\ta/b\r\n", True),
    ("mehrere \\r", b"# re15 assets v2 1 5\r\r\n5\t" + _MS + b"\ta/b\r\r\r\n", True),
    ("Leerzeilen", b"# re15 assets v2 1 5\n\n\r\n5\t" + _MS + b"\ta/b\n\n\n", True),
    ("ohne \\n am Ende", _MK + b"a/b", True),
    ("Nullen vorn", b"# re15 assets v2 01 005\n005\t" + _MS + b"\ta/b\n", True),
    ("18 Ziffern", b"# re15 assets v2 1 999999999999999999\n999999999999999999\t" + _MS + b"\ta/b\n", True),
    ("UTF-8 2 Byte", b"# re15 assets v2 1 1\n1\t" + _MS + b"\ta/\xc3\x84.bin\n", True),
    ("UTF-8 4 Byte", b"# re15 assets v2 1 1\n1\t" + _MS + b"\ta/\xf0\x9f\x98\x80\n", True),
    ("Leerzeichen im Pfad", b"# re15 assets v2 1 1\n1\t" + _MS + b"\ta/b c.bin\n", True),
    ("Punkt-Segmente als Name", b"# re15 assets v2 1 1\n1\t" + _MS + b"\ta/.b/..c/x.neux\n", True),
    ("Pfad 512 Bytes", _MK + b"a/" + b"x" * 510 + b"\n", True),
    ("v1-Liste", b"# re15 assets 2 10\n5\ta/b\n5\ta/c\n", False),
    ("Kopf fehlt", b"5\t" + _MS + b"\ta/b\n", False),
    ("Kopf v3", b"# re15 assets v3 1 5\n5\t" + _MS + b"\ta/b\n", False),
    ("Kopf + Leerzeichen", b"# re15 assets v2 1 5 \n5\t" + _MS + b"\ta/b\n", False),
    ("Kopf nach Leerzeile", b"\n" + _MK + b"a/b\n", False),
    ("Kopf mit BOM", b"\xef\xbb\xbf" + _MK + b"a/b\n", False),
    ("Kopf 19 Ziffern", b"# re15 assets v2 1 0000000000000000005\n5\t" + _MS + b"\ta/b\n", False),
    ("Kopf Anzahl falsch", b"# re15 assets v2 2 5\n5\t" + _MS + b"\ta/b\n", False),
    ("Kopf Bytes falsch", b"# re15 assets v2 1 6\n5\t" + _MS + b"\ta/b\n", False),
    ("keine Datei", b"# re15 assets v2 0 0\n", False),
    ("leer", b"", False),
    ("Kommentarzeile", b"# re15 assets v2 1 5\n# x\n5\t" + _MS + b"\ta/b\n", False),
    ("Groesse 19 Ziffern", b"# re15 assets v2 1 5\n0000000000000000005\t" + _MS + b"\ta/b\n", False),
    ("Groesse +5", b"# re15 assets v2 1 5\n+5\t" + _MS + b"\ta/b\n", False),
    ("Summe fehlt", b"# re15 assets v2 1 5\n5\ta/b\n", False),
    ("Summe gross", b"# re15 assets v2 1 5\n5\t" + _MS.upper() + b"\ta/b\n", False),
    ("Summe 63", b"# re15 assets v2 1 5\n5\t" + _MS[:63] + b"\ta/b\n", False),
    ("Summe 65", b"# re15 assets v2 1 5\n5\t" + _MS + b"0\ta/b\n", False),
    ("Summe + Leerzeichen", b"# re15 assets v2 1 5\n5\t" + _MS + b" \ta/b\n", False),
    ("Pfad leer", _MK + b"\n", False),
    ("Pfad mit Tab", _MK + b"a/b\tc\n", False),
    ("Pfad '..'", _MK + b"shared_assets/../../boese.bin\n", False),
    ("Pfad '.'", _MK + b"a/./b\n", False),
    ("Pfad absolut", _MK + b"/data/b\n", False),
    ("Pfad Backslash", _MK + b"a\\b\n", False),
    ("Pfad ohne '/'", _MK + b"re15_card.mcr\n", False),
    ("Pfad '//'", _MK + b"a//b\n", False),
    ("Pfad '/' am Ende", _MK + b"a/b/\n", False),
    ("Pfad 0x01", _MK + b"a/b\x01\n", False),
    ("Pfad DEL", _MK + b"a/b\x7f\n", False),
    ("Pfad \\r innen", _MK + b"a/\rb\n", False),
    ("Pfad .neu", _MK + b"a/b.neu\n", False),
    ("Pfad .Neu", _MK + b"a/b.Neu\n", False),
    ("Pfad 513 Bytes", _MK + b"a/" + b"x" * 511 + b"\n", False),
    ("UTF-8 0xff", _MK + b"a/b\xff\n", False),
    ("UTF-8 ueberlang", _MK + b"a/\xc0\xaf\n", False),
    ("UTF-8 Surrogat", _MK + b"a/\xed\xa0\x80\n", False),
    ("UTF-8 > U+10FFFF", _MK + b"a/\xf4\x90\x80\x80\n", False),
    ("UTF-8 abgeschnitten", _MK + b"a/\xe2\x82\n", False),
    ("Pfad doppelt", b"# re15 assets v2 2 10\n5\t" + _MS + b"\ta/b\n5\t" + _MS + b"\ta/b\n", False),
    ("nur Gross/klein", b"# re15 assets v2 2 10\n5\t" + _MS + b"\ta/B\n5\t" + _MS + b"\ta/b\n", False),
    ("NUL", _MK + b"a/b\0c\n", False),
    ("Summe > 2^63-1", b"# re15 assets v2 10 999999999999999999\n"
     + b"".join(b"999999999999999999\t" + _MS + b"\ta/%d\n" % i for i in range(10)), False),
)


def _innere_proben():
    """-> Liste der falschen Antworten (leer = gut)."""
    falsch = []
    for titel, roh, soll in _MANIFEST_PROBEN:
        try:
            _e, _z, _v1, fehler = manifest_lesen(roh)
            ist = not fehler
        except Exception as ex:
            ist, fehler = "Ausnahme %r" % (ex,), []
        if ist is not soll:
            falsch.append("manifest_lesen(%s) nimmt an = %r, soll %r (%s)" % (titel, ist, soll, "; ".join(fehler)[:200]))
    for muster, pfad, soll in _ANT_PROBEN:
        try:
            ist = _ant_passt(muster, pfad)
        except Exception as ex:
            ist = "Ausnahme %r" % (ex,)
        if ist is not soll:
            falsch.append("_ant_passt(%r, %r) = %r, soll %r" % (muster, pfad, ist, soll))
    # Sicherheitsnetz und walk-Fehler (im Fall-Selbsttest unerreichbar)
    for args, soll_ausnahme in (((0, 5, 5), False), ((0, 4, 5), True), ((0, 6, 5), True), ((1, 4, 5), False),
                                ((2, 5, 5), False)):
        try:
            _widerspruch_pruefen(*args)
            ist = False
        except Bedienfehler:
            ist = True
        if ist != soll_ausnahme:
            falsch.append("_widerspruch_pruefen%r: Ausnahme %r, soll %r" % (args, ist, soll_ausnahme))
    try:
        _walk_fehler(OSError("Probe"))
        falsch.append("_walk_fehler: keine Ausnahme")
    except Bedienfehler as ex:
        if "Quellbaum nicht lesbar: Probe" not in str(ex):
            falsch.append("_walk_fehler: falsche Meldung %r" % str(ex))
    # Datei endet mitten im Eintrag (struktur_pruefen laesst es im Lauf nie dazu kommen)
    import io
    e = _Eintrag(0, b"x", 0, 0, 0, 10, 10, 0)
    e.daten_off = 2
    try:
        _eintrag_lesen(io.BytesIO(b"0123456"), e)
        falsch.append("_eintrag_lesen: 5 von 10 B ohne Fehler")
    except _Lesefehler as ex:
        if "Datei endet mitten im Eintrag" not in str(ex):
            falsch.append("_eintrag_lesen: falsche Meldung %r" % str(ex))
    for rel, soll in _AUSLASS_PROBEN:
        try:
            ist = _auslass_hinweis(rel)
        except Exception as ex:
            ist = "Ausnahme %r" % (ex,)
        if not isinstance(ist, str) or (soll not in ist if soll else ist != ""):
            falsch.append("_auslass_hinweis(%r) = %r, soll %s" % (rel, ist, repr(soll) if soll else "''"))
    # Manifest-Grenze = die des Geraets (Runde 4, Gegenpruefung R3 MU4; asset_abgleich.h RE15_ABGLEICH_LISTE_MAX);
    # die Fall-Laeufe pruefen die Grenze sonst nur ueber den Pruefhaken, der sie senkt. Der Haken darf nie anheben.
    haken_vorher = os.environ.pop("RE15_GATE_MANIFEST_MAX", None)
    try:
        for haken, soll in ((None, _FX_GERAET_MANIFEST), (str(_FX_GERAET_MANIFEST + 1), _FX_GERAET_MANIFEST),
                            (str(_FX_GERAET_MANIFEST - 1), _FX_GERAET_MANIFEST - 1)):
            if haken is None:
                os.environ.pop("RE15_GATE_MANIFEST_MAX", None)
            else:
                os.environ["RE15_GATE_MANIFEST_MAX"] = haken
            try:
                ist = _manifest_grenze()
            except Exception as ex:
                ist = "Ausnahme %r" % (ex,)
            if ist != soll:
                falsch.append("_manifest_grenze() mit Pruefhaken %r = %r, soll %r (Geraet: 64 MiB)" % (haken, ist, soll))
    finally:
        if haken_vorher is None:
            os.environ.pop("RE15_GATE_MANIFEST_MAX", None)
        else:
            os.environ["RE15_GATE_MANIFEST_MAX"] = haken_vorher
    return falsch


def _fall_bewerten(fall, ergebnis):
    """-> (gut, fehlende Meldungen)"""
    _titel, _f, soll_rc, soll_text = fall[:4]
    verboten = fall[5] if len(fall) > 5 else ()
    rc, aus = ergebnis
    fehlt = [t for t in soll_text if t not in aus] + ["VERBOTEN: " + t for t in verboten if t in aus]
    return rc == soll_rc and not fehlt, fehlt


def selbsttest():
    t0 = time.monotonic()
    print("== APK-Asset-Gate: Selbsttest (Mini-Quellbaum + Mini-APK im Temp-Ordner) ==")
    # RE15_GATE_SELBSTTEST_SCHNELL=1 (nur fuer die Mutanten-Probe): beim ersten falschen Fall aufhoeren - am
    # Ergebnis aendert das nichts (ein falscher Fall = SELBSTTEST-FEHLER), es spart nur die Laufzeit
    schnell = os.environ.get("RE15_GATE_SELBSTTEST_SCHNELL") == "1"
    falsch = _innere_proben()
    n_innen = len(_ANT_PROBEN) + len(_AUSLASS_PROBEN) + 7 + 3 + len(_MANIFEST_PROBEN)
    print("   Innere Proben: %d/%d (_ant_passt, _auslass_hinweis, Sicherheitsnetz, walk-Fehler, Dateiende, "
          "Manifest-Grenze, manifest_lesen v2)" % (n_innen - len(falsch), n_innen))
    if falsch:
        for z in falsch:
            print("   [FEHLER] " + z)
        print("== SELBSTTEST-FEHLER: %d innere Probe(n) falsch - das Gate ist NICHT verlaesslich ==" % len(falsch))
        return RC_ABWEICHUNG
    tmp = tempfile.mkdtemp(prefix="apk_gate_selbsttest_")
    faelle = _faelle()
    ergebnis = {}
    try:
        arbeiter = max(1, min(8, os.cpu_count() or 1))
        with concurrent.futures.ThreadPoolExecutor(max_workers=arbeiter) as pool:
            laeufe = {pool.submit(_fall_laufen, tmp, nr, fall): nr for nr, fall in enumerate(faelle, 1)}
            for lauf in concurrent.futures.as_completed(laeufe):
                nr = laeufe[lauf]
                try:
                    ergebnis[nr] = lauf.result()
                except Exception:
                    ergebnis[nr] = (-1, "Lauf-Fehler:\n" + traceback.format_exc())
                if schnell and not _fall_bewerten(faelle[nr - 1], ergebnis[nr])[0]:
                    for rest in laeufe:
                        rest.cancel()
                    break
    finally:
        shutil.rmtree(tmp, ignore_errors=True)
    ok, schlecht, uebersprungen = 0, [], 0
    for nr, fall in enumerate(faelle, 1):
        titel, _f, soll_rc, _soll_text = fall[:4]
        if nr not in ergebnis:
            uebersprungen += 1                      # nur im Schnellmodus nach dem ersten falschen Fall
            continue
        gut, fehlt = _fall_bewerten(fall, ergebnis[nr])
        print("   [%s] %02d %-58s rc=%d (soll %d)%s" % ("ok" if gut else "FEHLER", nr, titel, ergebnis[nr][0], soll_rc,
                                                    "" if not fehlt else "  fehlende Meldung: %s" % fehlt))
        if gut:
            ok += 1
        else:
            schlecht.append((nr, titel, ergebnis[nr][1]))
    n = ok + len(schlecht)
    print("   Laufzeit: %.1f s" % (time.monotonic() - t0))
    if schlecht or uebersprungen:
        for nr, titel, aus in schlecht:
            print("--- Ausgabe Fall %02d (%s) ---" % (nr, titel))
            print(aus.rstrip())
        print("== SELBSTTEST-FEHLER: %d von %d Faellen falsch%s - das Gate ist NICHT verlaesslich =="
              % (len(schlecht), n, (", %d nicht gelaufen (Schnellmodus)" % uebersprungen) if uebersprungen else ""))
        return RC_ABWEICHUNG
    print("== SELBSTTEST-OK: %d/%d Faelle (gute APKs angenommen, jede Faelschung abgelehnt) ==" % (ok, n))
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
        ap.add_argument("--quellbaum", action="store_true",
                        help="nur den Quellbaum pruefen (Baumliste, Pflichtinhalt, Tuer-Soll, shared_assets-Wurzel)")
        ap.add_argument("--paket", metavar="ORDNER", help="PC-Paketordner gegen dieselbe Asset-Liste pruefen")
        ap.add_argument("--max-zeilen", type=int, default=25, help="Befunde je Art (Rest: 'und N weitere')")
        a = ap.parse_args(argv)
        if [bool(a.apk), a.selbsttest, a.quellbaum, a.paket is not None].count(True) != 1:
            ap.error("genau eins von: <apk>, --selbsttest, --quellbaum, --paket <ordner>")
        if a.max_zeilen < 1:
            ap.error("--max-zeilen muss >= 1 sein")
        if a.paket is not None and not a.paket:
            ap.error("--paket braucht einen Ordner")
        if a.selbsttest:
            rc = selbsttest()
        elif a.quellbaum:
            rc = nur_quellbaum(a.repo, a.max_zeilen)
        elif a.paket is not None:
            rc = paket_pruefen(a.repo, a.paket, a.max_zeilen)
        else:
            rc = pruefen(a.repo, a.apk, a.max_zeilen)
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
