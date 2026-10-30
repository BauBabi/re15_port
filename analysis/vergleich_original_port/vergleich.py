#!/usr/bin/env python3
"""Vergleich Original <-> Port, Stage fuer Stage (Nutzer-Auftrag 2026-10-30).

Was das Werkzeug misst (und was nicht):
  * Funktionen je STAGE*.BIN: jal-Ziele im Overlay, Code-Zeiger in Daten, die auf einen Prolog `addiu sp,sp,-N`
    zeigen, die Dispatch-Handler (Schreibzugriffe auf @0x80072bac, dispatch_scan.py) und die Decompile-Starts.
  * Byte-Signatur je Funktion: Overlay-Sprungziele und Overlay-Adressen (lui 0x8010..0x8013 + abhaengige
    Immediates) maskiert, EXE-Ziele bleiben exakt. Gleiche Signatur in zwei Stages = dieselbe Funktion.
  * Port-Zitate: jede 0x801xxxxx / FUN_801xxxxx-Stelle in re15_port/{engine,platform,include,tools}. Stage aus
    dem Kontext (STAGEn im Umfeld), sonst STAGE1. RE2-Zitate (RE2/EMOVL/re2_ im Umfeld oder Dateiname) zaehlen
    NICHT. Eine Funktion gilt als ZITIERT, wenn ihre Signatur-Klasse in irgendeiner Stage zitiert ist.
  * Zitiert heisst "jemand hat hier disassembliert und es im Port belegt" — nicht "vollstaendig byte-true".
"""
import os, re, struct, sys, json, hashlib, collections, bisect
sys.path.insert(0, os.path.dirname(__file__))
from dispatch_scan import scan as dispatch_scan

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
BIN = os.path.join(ROOT, 'info/Re1.5/PSX/BIN')
BASE = 0x80100000

def is_ov(a, size): return BASE <= a < BASE + size

def load(n):
    return open(os.path.join(BIN, 'STAGE%d.BIN' % n), 'rb').read()

def words(d): return struct.unpack_from('<%dI' % (len(d) // 4), d)

def prolog(w): return (w >> 16) == 0x27bd and (w & 0x8000)

def funktionen(n, d):
    w = words(d); size = len(d)
    starts = set()
    for i, x in enumerate(w):
        if x >> 26 == 3:
            t = (BASE & 0xf0000000) | ((x & 0x3ffffff) << 2)
            if is_ov(t, size): starts.add(t)
    for i, x in enumerate(w):
        if is_ov(x, size) and x % 4 == 0 and prolog(w[(x - BASE) // 4]): starts.add(x)
    # Zeigertabellen im Datenteil (hinter dem Code): auch Blattfunktionen ohne Prolog (Zustandstabellen)
    code_end = max(starts) if starts else BASE
    for i in range((code_end - BASE) // 4, len(w)):
        x = w[i]
        if BASE <= x < code_end and x % 4 == 0 and (w[i - 1] == 0 or BASE <= w[i - 1] < code_end
                                                     or (i + 1 < len(w) and BASE <= w[i + 1] < code_end)):
            starts.add(x)
    # Dispatch-Tabellen: `lui r / addiu r,r,T / addu r,r,i / lw x,0(r) / ... jalr x` -> alle Eintraege sind Funktionen
    for i in range(len(w) - 8):
        x = w[i]
        if x >> 26 != 0x0f: continue
        r = (x >> 16) & 31; hi = (x & 0xffff) << 16
        y = w[i + 1]
        if y >> 26 != 0x09 or (y >> 21) & 31 != r or (y >> 16) & 31 != r: continue
        T = (hi + ((y & 0xffff) - 0x10000 if y & 0x8000 else y & 0xffff)) & 0xffffffff
        if not is_ov(T, size): continue
        jalr = any((w[j] >> 26) == 0 and (w[j] & 0x3f) == 9 for j in range(i + 2, min(i + 9, len(w))))
        if not jalr: continue
        k = (T - BASE) // 4
        for j in range(k, min(k + 96, len(w))):
            v = w[j]
            if v == 0: continue
            if is_ov(v, size) and v % 4 == 0: starts.add(v)
            else: break
    roots = dispatch_scan(os.path.join(BIN, 'STAGE%d.BIN' % n))
    for t, (h, _) in roots.items(): starts.add(h)
    for sub in ('STAGE%d' % n, 'STAGE%d_full' % n):
        p = os.path.join(ROOT, 'RE_15_Quellcode_Overlays', sub)
        if os.path.isdir(p):
            for f in os.listdir(p):
                m = re.match(r'FUN_(80[0-9a-fA-F]{6})\.c$', f)
                if m:
                    a = int(m.group(1), 16)
                    k = (a - BASE) // 4
                    # nur echte Starts: Prolog, oder direkt hinter `jr ra` + Delay-Slot (Blattfunktion)
                    if is_ov(a, size) and (prolog(w[k]) or (k >= 2 and w[k - 2] == 0x03e00008)): starts.add(a)
    starts = sorted(starts)
    fn = {}
    for k, s in enumerate(starts):
        e = starts[k + 1] if k + 1 < len(starts) else s + 4
        # Ende: letztes `jr ra` (+Delay-Slot) vor dem naechsten Start, sonst der naechste Start
        last = None
        for a in range(s, min(e, s + 0x4000), 4):
            if w[(a - BASE) // 4] == 0x03e00008: last = a + 8
        fn[s] = last if last else min(e, s + 0x4000)
    return fn, roots

def signatur(d, s, e):
    w = words(d); size = len(d)
    ovreg = set(); out = []
    for a in range(s, e, 4):
        x = w[(a - BASE) // 4]; op = x >> 26; rs = (x >> 21) & 31; rt = (x >> 16) & 31
        if op in (2, 3):
            t = (BASE & 0xf0000000) | ((x & 0x3ffffff) << 2)
            out.append((op << 26) if is_ov(t, 0x40000) else x)
            continue
        if op == 0x0f:
            if 0x8010 <= (x & 0xffff) <= 0x8013: ovreg.add(rt); out.append(x & 0xffff0000); continue
            ovreg.discard(rt)
        elif op >= 0x08 and rs in ovreg and op != 0x0f:
            out.append(x & 0xffff0000)
            if op in (0x09, 0x0d): ovreg.add(rt)
            elif op in (0x20, 0x21, 0x23, 0x24, 0x25): ovreg.discard(rt)
            continue
        out.append(x)
    return hashlib.sha1(struct.pack('<%dI' % len(out), *out)).hexdigest()[:16]

def kanten(d, fn):
    """jal-Ziele + Zeigertabellen, die die Funktion ueber lui/addiu adressiert."""
    w = words(d); size = len(d); starts = set(fn)
    g = {}
    for s, e in fn.items():
        ziele = set(); reg = {}
        for a in range(s, e, 4):
            x = w[(a - BASE) // 4]; op = x >> 26; rs = (x >> 21) & 31; rt = (x >> 16) & 31
            imm = x & 0xffff; simm = imm - 0x10000 if imm & 0x8000 else imm
            if op == 3:
                t = (BASE & 0xf0000000) | ((x & 0x3ffffff) << 2)
                if t in starts: ziele.add(t)
            elif op == 0x0f: reg[rt] = imm << 16
            elif op in (0x09, 0x23) and rs in reg:
                D = (reg[rs] + simm) & 0xffffffff
                if op == 0x09: reg[rt] = D
                else: reg.pop(rt, None)
                if is_ov(D, size):
                    k = (D - BASE) // 4
                    for j in range(k, min(k + 96, len(w))):
                        if w[j] in starts: ziele.add(w[j])
                        elif w[j] != 0 and j > k + 1: break
        # kein `jr ra` bis zum naechsten Start -> der Code laeuft dort weiter (falscher Start mitten im Koerper)
        vl = w[(e - 8 - BASE) // 4] if e - 8 >= s else 0
        endet = vl == 0x03e00008 or (vl >> 26) == 2 or ((vl >> 26) == 0 and (vl & 0x3f) == 8)
        if not endet and e in starts: ziele.add(e)
        g[s] = ziele
    return g

ZIT = re.compile(r'(?:0x|FUN_|LAB_)(801[0-3][0-9a-fA-F]{4})\b')
STG = re.compile(r'STAGE\s?([1-6])|Stage\s?([1-6])\b|stage([1-6])')
RE2 = re.compile(r'\bRE2\b|EMOVL|re2_disasm|RE2-|ghidra_re2')

def port_zitate():
    out = collections.defaultdict(set)   # stage -> {adresse}; Schluessel 0 = ohne Stage-Angabe (STAGE1-Annahme)
    for top in ('engine', 'platform', 'include', 'tools'):
        for dp, dn, fs in os.walk(os.path.join(ROOT, 're15_port', top)):
            if '/build' in dp or 'shared_assets' in dp: continue
            for f in fs:
                if not f.endswith(('.c', '.h')): continue
                lines = open(os.path.join(dp, f), errors='replace').read().split('\n')
                datei_re2 = 're2' in f.lower()
                zaehl = collections.Counter(int(next(g for g in mm.groups() if g)) for L in lines for mm in STG.finditer(L))
                datei_stage = zaehl.most_common(1)[0][0] if zaehl else 1
                for i, L in enumerate(lines):
                    for m in ZIT.finditer(L):
                        a = int(m.group(1), 16)
                        ctx_vor = lines[max(0, i - 15):i + 1]
                        st = None
                        for C in reversed(ctx_vor + lines[i + 1:i + 3]):
                            mm = STG.search(C)
                            if mm: st = int(next(g for g in mm.groups() if g)); break
                        if st is None:
                            umfeld = '\n'.join(lines[max(0, i - 6):i + 2])
                            if datei_re2 or RE2.search(umfeld): continue
                            out[0].add(a)
                            st = datei_stage
                        out[st].add(a)
    return out

def main():
    daten = {}
    for n in range(1, 7):
        d = load(n); fn, roots = funktionen(n, d)
        sig = {s: signatur(d, s, e) for s, e in fn.items()}
        daten[n] = dict(d=d, fn=fn, roots=roots, sig=sig, g=kanten(d, fn))
    zit = port_zitate()
    klasse_zit = set()
    for n in range(1, 7):
        fn = daten[n]['fn']; ss = sorted(fn)
        for a in zit.get(n, ()):
            k = bisect.bisect_right(ss, a) - 1
            if k >= 0 and ss[k] <= a < max(fn[ss[k]], ss[k] + 4):
                klasse_zit.add(daten[n]['sig'][ss[k]])
    # STRENG: die Startadresse selbst (oder die eines byte-gleichen Zwillings) steht im Port
    alle_zit = set().union(*zit.values()) if zit else set()
    klasse_start = set()
    for n in range(1, 7):
        for s0, h in daten[n]['sig'].items():
            if s0 in zit.get(n, ()) or (s0 in zit.get(0, ())): klasse_start.add(h)
    klasse_gross = set(klasse_zit)                  # obere Schranke: Zitat ohne Stage gilt fuer JEDE Stage
    for n in range(1, 7):
        fn = daten[n]['fn']; ss = sorted(fn)
        for a in zit.get(0, ()):
            k = bisect.bisect_right(ss, a) - 1
            if k >= 0 and ss[k] <= a < max(fn[ss[k]], ss[k] + 4):
                klasse_gross.add(daten[n]['sig'][ss[k]])
    klassen = collections.defaultdict(list)
    for n in range(1, 7):
        for s, h in daten[n]['sig'].items(): klassen[h].append((n, s))
    res = {}
    for n in range(1, 7):
        D = daten[n]; fn = D['fn']
        def erreichbar(r):
            seen = set(); st = [r]
            while st:
                x = st.pop()
                if x in seen or x not in fn: continue
                seen.add(x); st.extend(D['g'].get(x, ()))
            return seen
        typen = {}
        for t, (h, _) in sorted(D['roots'].items()):
            er = erreichbar(h)
            typen['0x%02x' % t] = dict(root='0x%08x' % h, n=len(er),
                zitiert=sum(1 for x in er if D['sig'][x] in klasse_zit),
                offen=sorted('0x%08x/%d' % (x, (fn[x] - x) // 4) for x in er if D['sig'][x] not in klasse_zit),
                sicher_offen=sorted('0x%08x/%d' % (x, (fn[x] - x) // 4) for x in er if D['sig'][x] not in klasse_gross))
        alle_typ = set()
        for t, (h, _) in D['roots'].items(): alle_typ |= erreichbar(h)
        rest = [x for x in fn if x not in alle_typ]
        res['STAGE%d' % n] = dict(
            funktionen=len(fn), zitiert=sum(1 for x in fn if D['sig'][x] in klasse_zit),
            befehle=sum((fn[x] - x) // 4 for x in fn),
            befehle_zitiert=sum((fn[x] - x) // 4 for x in fn if D['sig'][x] in klasse_zit),
            typen=typen,
            rest_offen=sorted('0x%08x/%d' % (x, (fn[x] - x) // 4) for x in rest if D['sig'][x] not in klasse_zit),
            rest_n=len(rest),
            start_zitiert=sum(1 for x in fn if D['sig'][x] in klasse_start),
            start_offen=sorted(('0x%08x/%d' % (x, (fn[x] - x) // 4) for x in fn if D['sig'][x] not in klasse_start),
                               key=lambda z: -int(z.split('/')[1])),
            sicher_offen=sorted('0x%08x/%d' % (x, (fn[x] - x) // 4) for x in fn if D['sig'][x] not in klasse_gross),
            befehle_sicher_offen=sum((fn[x] - x) // 4 for x in fn if D['sig'][x] not in klasse_gross),
            geteilt=sum(1 for x in fn if len({s for s, _ in klassen[D['sig'][x]]}) > 1))
    json.dump(res, open(os.path.join(os.path.dirname(__file__), 'vergleich.json'), 'w'), indent=1)
    for st, r in res.items():
        print('%s: %d Funktionen (%d mit Zwilling in anderer Stage), zitiert %d (%.0f%% der Befehle), ausserhalb Gegnerbaeumen %d'
              % (st, r['funktionen'], r['geteilt'], r['zitiert'], 100.0 * r['befehle_zitiert'] / max(1, r['befehle']), r['rest_n']))
        print('   Start zitiert (streng): %d/%d' % (r['start_zitiert'], r['funktionen']))
        print('   sicher offen (auch bei grosszuegiger Zuordnung): %d Funktionen, %d Befehle'
              % (len(r['sicher_offen']), r['befehle_sicher_offen']))
        for t, v in r['typen'].items():
            print('   Typ %s root %s: %d/%d zitiert, sicher offen %d' % (t, v['root'], v['zitiert'], v['n'], len(v['sicher_offen'])))

if __name__ == '__main__': main()
