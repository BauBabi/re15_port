#!/usr/bin/env python3
"""discard_verlustwege.py - VERLUSTWEGE je Benutzungsstelle, aus den ausgelieferten Daten.

Die Frage, die dieses Werkzeug beantwortet (Runde 24, Auftragspunkt 4):

    Setzt ein "Ja" an dieser Benutzungsstelle ein Flag, das ein TOR schliesst,
    und war dieses Tor der einzige Weg zu der Stelle?

Gemessen wird am SCD-Bytecode, mit dem Laengen-Vorschub aus discard_zensus.op_size
(derselbe, den der Motor benutzt, scd_opcode_size_at), und mit dem BLOCK-MODELL des
Motors:

    op_ck (scd_vm.c:1992-2011) ist SELBST das Praedikat und gibt bei FALSCH
    SCD_R_IF_FALSE zurueck; der Dispatcher raeumt dann den Block-Stapel ab und springt
    auf das block_end des umgebenden If. Ein Ck ist damit ein TOR fuer ALLES, was in
    seinem Block dahinter steht - auch ohne eigenes Ifel_ck davor
    (ROOM4000 sub02 @0x01446 `Ck(12,31,0)` ist genau so ein Tor).

Je Stelle wird ausgegeben:
  TORE       die Ck-Bedingungen, die den `Message_on <msg>` ueberhaupt erreichbar machen
             (auf dem Pfad, im richtigen Zweig, Block noch offen)
  BESITZTOR  darunter ein Ck auf Flag-Zone 9 = das "genommen"-Bit des Gegenstands
             (Muster der Kartenleser, discard_nutzstellen.py Kopf). Fehlt es, fragt das
             ausgelieferte Skript nach einem Gegenstand, den der Spieler nicht haben muss.
  TOETEND    ein `Set(zone,idx,1)` NACH dem Message_on im selben Zweig, der genau ein
             Tor mit erwartetem Wert 0 auf 1 setzt -> der Zweig ist danach tot
  EINZIG     kommt `Message_on <msg>` im ganzen Raum genau EINMAL vor
  AUSGABE    in welchem Raum der Gegenstand per Item_aot_set (0x50) ausgegeben wird

Aufruf: python discard_verlustwege.py [ASSET_ROOT]
"""
import sys, os, glob, re, collections

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from discard_zensus import op_size, fwd_target, rdt_section_end, section_regions, u16, u32
from discard_nutzstellen import decode, messages

ROOT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")

# Die Stellen kommen aus DERSELBEN erzeugten Tabelle, die der Motor benutzt.
def sites_aus_inc():
    p = os.path.join(ROOT, "engine", "src", "gen", "discard_sites.inc")
    txt = open(p, "r", encoding="utf-8", errors="replace").read()
    out = []
    for m in re.finditer(r"\{\s*0x([0-9A-Fa-f]{4})\s*,\s*(\d+)\s*,\s*0x([0-9A-Fa-f]{2})\s*\}", txt):
        out.append((int(m.group(1), 16), int(m.group(2)), int(m.group(3), 16)))
    return out


def stage_dir(room):
    return "STAGE%d" % (room >> 12)


def region_ops(d, start, end):
    """(pc, op, size) in Skriptreihenfolge; STOP bei Desync (wie walk_ops)."""
    pc, maxf, res = start, start, []
    while pc < end:
        op = d[pc]
        sz = op_size(d, pc)
        if sz is None or pc + sz > end:
            break
        res.append((pc, op, sz))
        t = fwd_target(d, pc, op)
        if t is not None and t > maxf:
            maxf = t
        if op == 0x01 and pc + 2 > maxf:
            break
        pc += sz
    return res


def tore_fuer(d, ops, ziel_pc):
    """Die Ck-Tore auf dem Pfad zum Opcode an ziel_pc, plus die Sets dahinter.

    Block-Modell: Ifel_ck (0x06) / Else_ck (0x07) tragen ihre Blocklaenge in pc[2..3]
    (u16, ab pc+4). Endif (0x08) schliesst. Ein Ck (0x21) gehoert zum INNERSTEN offenen
    Block; ist der bei ziel_pc noch offen und liegt der Ck davor, ist er ein Tor.
    """
    stack = []           # (block_end, kind)  kind: 'if' | 'else'
    cks = []             # (pc, zone, idx, val, block_end)
    ziel_stack = None
    for (pc, op, sz) in ops:
        while stack and pc >= stack[-1][0]:
            stack.pop()
        if pc == ziel_pc:
            ziel_stack = list(stack)
            break
        if op in (0x06, 0x07):
            ende = pc + 4 + u16(d, pc + 2)
            stack.append((ende, 'if' if op == 0x06 else 'else'))
        elif op == 0x08:
            if stack:
                stack.pop()
        elif op == 0x21:
            ende = stack[-1][0] if stack else 1 << 30
            cks.append((pc, d[pc + 1], d[pc + 2], d[pc + 3], ende))
    if ziel_stack is None:
        return None, None
    # Ein Ck ist Tor, wenn sein Block bei ziel_pc noch offen ist.
    offen = set(e for (e, k) in ziel_stack)
    tore = [c for c in cks if c[0] < ziel_pc and (c[4] in offen or c[4] > ziel_pc)]

    # ALLE Sets der Region - vor UND nach dem Ziel. Wer die Region betritt, fuehrt sie
    # aus; ein Set VOR dem Ziel (ROOM1090 sub03 @0x024CE `Set(3,132,0)`) macht die Stelle
    # damit genauso einmalig wie ein Set dahinter (ROOM4000 @0x01450 `Set(3,32,1)`).
    # Die Spalte "wo" haelt die Unterscheidung fest.
    sets = []
    for (pc, op, sz) in ops:
        if op == 0x22:
            sets.append((pc, d[pc + 1], d[pc + 2], d[pc + 3],
                         "vor" if pc < ziel_pc else "nach"))
    return tore, sets


def aufrufer_tore(d, ziel_sub):
    """Die Tore der AUFRUFER des Unterprogramms ziel_sub (eine Ebene hoch).

    Ein Tor kann im CALLER stehen statt in der Region der Nachricht selbst
    (ROOM1090 sub03 hat gar keins, sein Aufrufer schon). Gesammelt werden alle
    `Evt_exec <sub>` (0x04, sub = pc[3]) und `Gosub <sub>` (0x18, sub = pc[1])
    und je Aufrufstelle die Tore auf ihrem Pfad.
    """
    ms, ss = u32(d, 0x40), u32(d, 0x44)
    treffer = []
    for sec, tag in ((ms, "main"), (ss, "sub")):
        if sec == 0 or sec >= len(d):
            continue
        se = rdt_section_end(d, sec)
        for (o, e, idx) in section_regions(d, sec, se):
            ops = region_ops(d, sec + o, sec + e)
            for (pc, op, sz) in ops:
                ruft = (op == 0x04 and d[pc + 3] == ziel_sub) \
                    or (op == 0x18 and d[pc + 1] == ziel_sub)
                if not ruft:
                    continue
                tore, sets = tore_fuer(d, ops, pc)
                treffer.append((tag, idx, pc, tore or [], sets or []))
    return treffer


def item_ausgaben(root):
    """typ -> [(raum, anzahl, tk_bit)] aus allen Item_aot_set (0x50)."""
    out = collections.defaultdict(list)
    for p in sorted(glob.glob(os.path.join(root, "STAGE*", "ROOM*.RDT"))):
        d = open(p, "rb").read()
        if len(d) < 0x48:
            continue
        room = os.path.basename(p).split(".")[0]
        ms, ss = u32(d, 0x40), u32(d, 0x44)
        for sec in (ms, ss):
            if sec == 0 or sec >= len(d):
                continue
            se = rdt_section_end(d, sec)
            for (o, e, idx) in section_regions(d, sec, se):
                for (pc, op, sz) in region_ops(d, sec + o, sec + e):
                    if op == 0x50 and pc + 19 <= len(d):
                        out[d[pc + 14]].append((room, d[pc + 16], d[pc + 18]))
    return out


def alle_sets(root):
    """(zone, idx, op) -> [(raum, tag, idx_region, pc)] ueber ALLE RDTs, plus die
    Umkehr-Tabelle je Region.

    Damit laesst sich fragen, ob ein geschlossenes Tor irgendwo im Spiel WIEDER geoeffnet
    wird - der Unterschied zwischen "einmalig" und "endgueltig tot" - UND von WEM
    (Raum + Unterprogramm + Datei-Offset), und was derselbe Wiederoeffner NEBENBEI
    schreibt.

    ⛔ WARUM DIE REGION MITGEZAEHLT WIRD (Runde 25, ROOM1090):
    Der Wiederoeffner ist selbst ein Glied einer KETTE, und ohne das Unterprogramm laesst
    sich die Kette nicht aufschreiben. ROOM1090 ist das Beispiel, an dem das aufgefallen
    ist - ein DREIGLIED:
        sub00 @0x022A6  Ck(3,132,1)   das Tor
        sub00 @0x022E0  Evt_exec 3    und der Aufruf dahinter
        sub03 @0x024CE  Set(3,132,0)  loescht das Tor als ERSTE Anweisung
        sub06 @0x0271E  Set(3,129,1)  ... und sub06 setzt es wieder
        sub06 @0x02722  Set(3,132,1)
    sub06 hat KEINEN Evt_exec/Gosub-Aufrufer; es haengt an einem AOT, den sub00 nur im
    Zweig `Ck(3,129,0)` @0x02332 installiert:
        0x02336  Aot_set 2c 02 03 b1 01 00 ... ff 00 18 06 00 00   (LANGE Form)
    pc[25] = 0x06 ist die eventId, also sub06 (scd_vm.c:2823; gleiche Nutzlast-Form
    {0xFF,0x18,sub} wie ROOM1150 @0x0D7E mit `... ff 00 18 04`). sub06 nimmt sich mit
    seinem eigenen `Set(3,129,1)` @0x0271E also SELBST vom Netz. Ob die Stelle damit
    wiederholbar ist, haengt an der UMKEHRBARKEIT von flag(3,129), und die steht in einem
    anderen Raum: ROOM10B1 sub02 @0x01832 `Set(3,129,1)` setzt, ROOM10B1 sub03 @0x01954
    `Set(3,129,0)` LOESCHT. Genau diese zwei Zeilen gibt die Spalte NEBENWIRK jetzt aus.
    Rueckgabe: (sets, regions) - regions[(raum, tag, idx)] = alle Sets dieser Region.
    """
    out = collections.defaultdict(list)
    regions = collections.defaultdict(list)
    for p in sorted(glob.glob(os.path.join(root, "STAGE*", "ROOM*.RDT"))):
        d = open(p, "rb").read()
        if len(d) < 0x48:
            continue
        room = os.path.basename(p).split(".")[0]
        ms, ss = u32(d, 0x40), u32(d, 0x44)
        for sec, tag in ((ms, "main"), (ss, "sub")):
            if sec == 0 or sec >= len(d):
                continue
            se = rdt_section_end(d, sec)
            for (o, e, idx) in section_regions(d, sec, se):
                for (pc, op, sz) in region_ops(d, sec + o, sec + e):
                    if op == 0x22:
                        out[(d[pc + 1], d[pc + 2], d[pc + 3])].append((room, tag, idx, pc))
                        regions[(room, tag, idx)].append((d[pc + 1], d[pc + 2], d[pc + 3], pc))
    return out, regions


def umkehr(sets_global, z, i, v):
    """Wer schreibt flag(z,i) auf den GEGENWERT von v? Liste (raum, tag, idx, pc)."""
    return list(sets_global.get((z, i, 0 if v == 1 else 1), []))


def rufkette(d, ziel_sub, tiefe=3):
    """Die Aufrufkette zu ziel_sub, bis `tiefe` Ebenen hoch - als Liste von Gliedern
    (tag, region_idx, pc, gerufenes_sub). `aufrufer_tore` geht nur EINE Ebene; fuer ein
    Dreiglied wie ROOM1090 (sub00 -> sub03, sub06 setzt das Tor) reicht das nicht."""
    ms, ss = u32(d, 0x40), u32(d, 0x44)
    kette, offen, gesehen = [], [ziel_sub], set([ziel_sub])
    while offen and tiefe > 0:
        tiefe -= 1
        neu = []
        for ziel in offen:
            for sec, tag in ((ms, "main"), (ss, "sub")):
                if sec == 0 or sec >= len(d):
                    continue
                se = rdt_section_end(d, sec)
                for (o, e, idx) in section_regions(d, sec, se):
                    for (pc, op, sz) in region_ops(d, sec + o, sec + e):
                        ruft = (op == 0x04 and d[pc + 3] == ziel) \
                            or (op == 0x18 and d[pc + 1] == ziel)
                        if ruft:
                            kette.append((tag, idx, pc, ziel))
                            if tag == "sub" and idx not in gesehen:
                                gesehen.add(idx)
                                neu.append(idx)
        offen = neu
    return kette


def main():
    root = sys.argv[1] if len(sys.argv) > 1 else os.path.join(ROOT, "shared_assets", "PSX")
    sites = sites_aus_inc()
    ausg = item_ausgaben(root)
    sets_global, regionen = alle_sets(root)

    print("=== VERLUSTWEGE je Benutzungsstelle (ausgelieferte Daten) ===")
    n_ohne_besitztor = 0
    n_toetend = 0
    n_toetend_und_einzig = 0
    n_toetend_ohne_besitztor = 0
    n_neben_fest = 0
    zeilen = []
    for (room, msg, item) in sites:
        p = os.path.join(root, stage_dir(room), "ROOM%04X.RDT" % room)
        if not os.path.exists(p):
            print("  ROOM%04X: RDT fehlt" % room)
            continue
        d = open(p, "rb").read()
        ms, ss = u32(d, 0x40), u32(d, 0x44)
        vorkommen = []
        for sec, tag in ((ms, "main"), (ss, "sub")):
            if sec == 0 or sec >= len(d):
                continue
            se = rdt_section_end(d, sec)
            for (o, e, idx) in section_regions(d, sec, se):
                ops = region_ops(d, sec + o, sec + e)
                for (pc, op, sz) in ops:
                    if op == 0x2B and d[pc + 1] == msg:
                        vorkommen.append((tag, idx, pc, ops))
        if not vorkommen:
            print("  ROOM%04X msg %2d: kein Message_on" % (room, msg))
            continue
        tag, idx, pc, ops = vorkommen[0]
        tore, sets = tore_fuer(d, ops, pc)
        if tore is None:
            print("  ROOM%04X msg %2d: Ziel nicht im Blockmodell erreichbar" % (room, msg))
            continue
        # Eine Ebene hoch: die Tore der AUFRUFER dieses Unterprogramms.
        ruf = aufrufer_tore(d, idx) if tag == "sub" else []
        for (rtag, ridx, rpc, rtore, rsets) in ruf:
            tore = tore + rtore
            sets = sets + rsets
        besitztor = [c for c in tore if c[1] == 9]
        # TOETEND: ein Set, das genau ein Tor dieser Stelle unwahr macht.
        #   Set-op 1 (Bit setzen)   toetet ein Tor mit erwartetem Wert 0
        #   Set-op 0 (Bit loeschen) toetet ein Tor mit erwartetem Wert 1
        toetend = []
        for (spc, sz_, si, sv, wo) in sets:
            for c in tore:
                if c[1] != sz_ or c[2] != si:
                    continue
                if (sv == 1 and c[3] == 0) or (sv == 0 and c[3] == 1):
                    toetend.append((spc, sz_, si, c[0], wo, sv))
        # ENDGUELTIG tot ist ein geschlossenes Tor nur, wenn es nirgends im Spiel WIEDER
        # geoeffnet wird. Der Wiederoeffner zu Ck(z,i,v) ist Set(z,i,1) fuer v==1 bzw.
        # Set(z,i,0) fuer v==0 - an einer ANDEREN Stelle als der toetenden.
        endgueltig = []
        wiederoeffner = []       # (gz, gi, raum, tag, ridx, rpc)
        for t in toetend:
            gz, gi = t[1], t[2]
            gv = [c[3] for c in tore if c[1] == gz and c[2] == gi]
            gv = gv[0] if gv else 0
            auf = 1 if gv == 1 else 0
            andere = [s for s in sets_global.get((gz, gi, auf), []) if s[3] != t[0]]
            if not andere:
                endgueltig.append(t)
            for (rraum, rtag, ridx, rpc) in andere:
                wiederoeffner.append((gz, gi, rraum, rtag, ridx, rpc))
        einzig = (len(vorkommen) == 1)
        if not besitztor:
            n_ohne_besitztor += 1
        if endgueltig:
            n_toetend += 1
            if einzig:
                n_toetend_und_einzig += 1
            if not besitztor:
                n_toetend_ohne_besitztor += 1
        zeilen.append((room, msg, item, tag, idx, pc, tore, besitztor, toetend, einzig))

        print("\n  ROOM%04X %s%02d  Message_on %d  (Gegenstand 0x%02X)"
              % (room, tag, idx, msg, item))
        print("    TORE      : " + (", ".join(
            "Ck(%d,%d,%d)@0x%05X" % (c[1], c[2], c[3], c[0]) for c in tore) or "KEINE"))
        print("    BESITZTOR : " + (", ".join(
            "Ck(9,%d,%d)@0x%05X" % (c[2], c[3], c[0]) for c in besitztor)
            or "FEHLT - das Skript fragt ohne Besitzpruefung"))
        print("    TOETEND   : " + (", ".join(
            "Set(%d,%d,%d)@0x%05X (%s der Nachricht) schliesst Ck@0x%05X"
            % (t[1], t[2], t[5], t[0], t[4], t[3])
            for t in toetend) or "keins"))
        print("    ENDGUELTIG: " + (", ".join(
            "Ck(%d,%d)@0x%05X wird nirgends wieder geoeffnet" % (t[1], t[2], t[3])
            for t in endgueltig) or "keins - jedes geschlossene Tor hat einen Wiederoeffner"))
        # ⛔ WER oeffnet wieder - mit Raum UND Unterprogramm, nicht nur "es gibt einen".
        print("    WIEDEROEFF: " + (", ".join(
            "flag(%d,%d) von %s %s%02d@0x%05X" % (w[0], w[1], w[2], w[3], w[4], w[5])
            for w in wiederoeffner) or "keiner"))
        # ⛔ UND WAS DERSELBE WIEDEROEFFNER NEBENBEI SCHREIBT - samt Umkehrbarkeit.
        # Das ist der ROOM1090-Fall: sub06 setzt neben flag(3,132) auch flag(3,129), und
        # flag(3,129) entscheidet, ob sub06 ueberhaupt noch aufgerufen wird.
        neben = []
        for w in wiederoeffner:
            for (nz, ni, nv, npc) in regionen.get((w[2], w[3], w[4]), []):
                if npc == w[5]:
                    continue
                um = umkehr(sets_global, nz, ni, nv)
                # Die Umkehr-Liste wird GEKAPPT: Zonen wie flag(2,7) (das Szenen-Bit)
                # haben dreistellig viele Schreiber, und dann ist nicht mehr die Liste
                # die Auskunft, sondern ihre LAENGE. Drei Beispiele + Anzahl.
                if um:
                    wie = "umkehrbar (%d Stellen): %s" % (
                        len(um), ", ".join("%s %s%02d@0x%05X" % (u[0], u[1], u[2], u[3])
                                           for u in um[:3]))
                    if len(um) > 3:
                        wie += ", ..."
                else:
                    wie = "NICHT umkehrbar"
                neben.append("%s %s%02d@0x%05X Set(%d,%d,%d) -> %s"
                             % (w[2], w[3], w[4], npc, nz, ni, nv, wie))
        print("    NEBENWIRK : " + (",\n                ".join(neben) or "keine"))
        if [x for x in neben if "NICHT umkehrbar" in x]:
            n_neben_fest += 1
        print("    EINZIG    : %s (%d Message_on %d im Raum)"
              % ("ja" if einzig else "NEIN", len(vorkommen), msg))
        print("    AUFRUFER  : " + (", ".join(
            "%s%02d@0x%05X [%s]" % (r[0], r[1], r[2], ";".join(
                "Ck(%d,%d,%d)" % (c[1], c[2], c[3]) for c in r[3]) or "-")
            for r in ruf) or "keiner (nur AOT/Ereignis)"))
        if tag == "sub":
            kette = rufkette(d, idx)
            print("    RUFKETTE  : " + (" | ".join(
                "%s%02d@0x%05X ruft sub%02d" % (k[0], k[1], k[2], k[3])
                for k in kette) or "keine (nur AOT/Ereignis)"))
        print("    AUSGABE   : " + (", ".join(
            "%s n=%d tk=%d" % a for a in ausg.get(item, [])) or "keine"))

    print("\n=== SUMME ueber %d Benutzungsstellen ===" % len(zeilen))
    print("  ohne Besitztor im Skript (kein Ck auf Zone 9)      : %d" % n_ohne_besitztor)
    print("  mit ENDGUELTIG geschlossenem Tor (kein Wiederoeffner)  : %d" % n_toetend)
    print("  davon Tor = EINZIGER Weg zu dieser Stelle          : %d" % n_toetend_und_einzig)
    print("  davon OHNE Besitztor (fragt + toetet ohne Besitz)  : %d"
          % n_toetend_ohne_besitztor)
    print("  Stellen, deren WIEDEROEFFNER eine NICHT umkehrbare Nebenwirkung hat: %d"
          % n_neben_fest)


if __name__ == "__main__":
    main()
