#!/usr/bin/env python3
"""SCD-ZEITWALKER — wieviele Bilder gibt das Skript jeder Dialogzeile?

LAENGENTABELLE: uebernommen aus re15_port/engine/src/scd_vm.c:166 (s_opcode_sizes),
plus die drei DATENABHAENGIGEN Laengen 0x2C/0x3B/0x50 (Bit 0x80 in pc[3]).
Gegenprobe siehe --pruefe (ROOM1170 sub02).

ZEITQUELLEN IM PORT (die EINZIGEN drei `return 2` in scd_vm.c, Zeilen 767/1121/1123/1505):
  0x09 Sleep + 0x0A Sleeping  -> N Bilder   (op_sleep 1072 / op_sleeping 1098)
  0x02 Evt_next               -> 1 Bild     (scd_vm.c:767)
  0x2B Message_on             -> parkt, wenn Auswahl-Dialog ODER (neu) voice_wait
Dazu der GLOBALE SKRIPT-FREEZE: Message_on pc[2..3]<<16 wird in g_pauseflags
ge-ODERt (scd_vm.c:1533). Bit RE15_PAUSE_SCD = 0x02000000 (re15_scd.h:469), also
Bit 0x0200 im Halbwort. scd_vm_tick kehrt dann am Kopf zurueck (scd_vm.c:586) —
der ganze VM steht, bis der Dialog-FSM den Text schliesst. Diese Standzeit wird
hier aus dem .msg-Rumpf simuliert (FUN_80028134-Nachbau re15_dialog_step,
msg_common.c:404ff).
"""
import os, struct, sys, glob, heapq

# ---------------------------------------------------------------- Laengen
L = [0] * 256
_pairs = {
    0x00: 1, 0x01: 2, 0x02: 1, 0x03: 4, 0x04: 4, 0x05: 2, 0x06: 4, 0x07: 4,
    0x08: 2, 0x09: 4, 0x0A: 3, 0x0B: 1, 0x0C: 1, 0x0D: 6, 0x0E: 2, 0x0F: 4,
    0x10: 2, 0x11: 4, 0x12: 2, 0x13: 4, 0x14: 6, 0x15: 4, 0x16: 2, 0x17: 6,
    0x18: 2, 0x19: 2, 0x1A: 2, 0x1B: 6, 0x1C: 1, 0x1D: 1, 0x1E: 1,
    0x20: 1, 0x21: 4, 0x22: 4, 0x23: 6, 0x24: 4, 0x25: 3, 0x26: 6, 0x27: 4,
    0x28: 1, 0x29: 2, 0x2A: 1, 0x2B: 4, 0x2C: 20, 0x2D: 34, 0x2E: 3, 0x2F: 4,
    0x30: 1, 0x31: 1, 0x32: 8, 0x33: 8, 0x34: 4, 0x35: 3, 0x36: 12, 0x37: 4,
    0x38: 12, 0x39: 4, 0x3A: 16, 0x3B: 32, 0x3C: 2, 0x3D: 3, 0x3E: 6, 0x3F: 4,
    0x40: 8, 0x41: 10, 0x42: 1, 0x43: 4, 0x44: 20, 0x45: 3, 0x46: 10, 0x47: 2,
    0x48: 16, 0x49: 8, 0x4A: 2, 0x4B: 3, 0x4C: 18, 0x4D: 10, 0x4E: 5, 0x4F: 22,
    0x50: 22, 0x51: 4, 0x52: 4, 0x53: 3, 0x54: 6, 0x55: 6, 0x56: 6, 0x57: 4,
    0x58: 4, 0x59: 4, 0x5A: 6, 0x5B: 4, 0x5C: 4, 0x5D: 4, 0x5E: 4,
}
for k, v in _pairs.items():
    L[k] = v
for k in range(0x5F, 0x8F):
    L[k] = 1
L[0xFE] = 5

NAMES = {
    0x00: "Nop", 0x01: "Evt_end", 0x02: "Evt_next", 0x03: "Evt_chain", 0x04: "Evt_exec",
    0x05: "Evt_kill", 0x06: "Ifel_ck", 0x07: "Else_ck", 0x08: "Endif", 0x09: "Sleep",
    0x0A: "Sleeping", 0x0B: "Wsleep", 0x0C: "Wsleeping", 0x0D: "For", 0x0E: "Next",
    0x0F: "While", 0x10: "Ewhile", 0x11: "Do", 0x12: "Edwhile", 0x13: "Switch",
    0x14: "Case", 0x15: "Default", 0x16: "Eswitch", 0x17: "Goto", 0x18: "Gosub",
    0x19: "Return", 0x1A: "Break", 0x1B: "For2", 0x1C: "Break_point", 0x1D: "Work_copy",
    0x21: "Ck", 0x22: "Set", 0x23: "Cmp", 0x24: "Save", 0x25: "Copy", 0x26: "Calc",
    0x27: "Calc2", 0x28: "Sce_rnd", 0x29: "Cut_chg", 0x2A: "Cut_old", 0x2B: "Message_on",
    0x2C: "Aot_set", 0x2D: "Obj_model_set", 0x2E: "Work_set", 0x2F: "Speed_set",
    0x30: "Add_speed", 0x31: "Add_aspeed", 0x32: "Pos_set", 0x33: "Dir_set",
    0x34: "Member_set", 0x35: "Member_set2", 0x36: "Se_on", 0x37: "Sca_id_set",
    0x38: "Flr_set", 0x39: "Sca_floor_set", 0x3A: "Sce_espr_on", 0x3B: "Door_aot_set",
    0x3C: "Cut_auto", 0x3D: "Member_get", 0x3E: "Member_cmp", 0x3F: "Plc_motion",
    0x40: "Plc_dest", 0x41: "Plc_neck", 0x42: "Plc_ret", 0x43: "Plc_flg",
    0x44: "Sce_em_set", 0x45: "Col_chg_set", 0x46: "Aot_reset", 0x47: "Aot_on",
    0x48: "Super_set", 0x49: "Super_reset", 0x4A: "Plc_gun", 0x4B: "Cut_replace",
    0x4C: "Sce_espr_kill", 0x4D: "Op4D", 0x4E: "Op4E", 0x4F: "Op4F",
    0x50: "Item_aot_set", 0x51: "Sce_key_ck", 0x52: "Sce_espr_control",
    0x53: "Sce_fade_set", 0x54: "Sce_bgm_control", 0x55: "Member_calc",
    0x56: "Member_calc2", 0x57: "Sce_fade_adjust", 0x58: "Plc_rot", 0x59: "Flag_set2",
    0x5A: "Weapon_chg", 0x5B: "Plc_cnt", 0x5C: "Sce_shake_on", 0x5D: "Mizu_div_set",
    0x5E: "Keep_Item_ck",
}


def u16(b, o):
    return struct.unpack_from('<H', b, o)[0]


def s16(b, o):
    return struct.unpack_from('<h', b, o)[0]


def u32(b, o):
    return struct.unpack_from('<I', b, o)[0]


def oplen(d, pc):
    op = d[pc]
    if op in (0x2C, 0x3B, 0x50):
        big = {0x2C: 28, 0x3B: 40, 0x50: 30}[op]
        sml = {0x2C: 20, 0x3B: 32, 0x50: 22}[op]
        return big if (d[pc + 3] & 0x80) else sml
    n = L[op]
    return n if n else 1


# ---------------------------------------------------------------- RDT
class Rdt(object):
    def __init__(self, path):
        self.path = path
        self.d = open(path, 'rb').read()
        d = self.d
        self.main = self._block(0x40)
        self.sub = self._block(0x44)
        self.extra = self._block(0x48)
        self.msgs = self._msgs()

    def _block(self, hdr_off):
        d = self.d
        base = u32(d, hdr_off)
        if base == 0 or base + 2 >= len(d):
            return []
        n = u16(d, base)
        if n == 0 or n % 2:
            return []
        cnt = n // 2
        if base + 2 * cnt > len(d):
            return []
        return [base + u16(d, base + 2 * i) for i in range(cnt)]

    def _msgs(self):
        """(id -> raw body bytes), byte-true nach rdt_common.c:290ff + msg_common.c:310ff."""
        d = self.d
        ms = u32(d, 0x3c)
        out = {}
        if ms == 0 or ms + 4 > len(d):
            return out
        b = d[ms:]
        block_end = len(d)
        for h in range(0x08, 0x60, 4):
            p = u32(d, h)
            if ms < p < block_end and p <= len(d):
                block_end = p
        block_size = block_end - ms
        off0 = b[0] | (b[1] << 8)
        n_off = off0 // 2
        if n_off < 1:
            return out
        for i in range(min(n_off, 64)):
            o0 = b[i * 2] | (b[i * 2 + 1] << 8)
            o1 = block_size
            if i + 1 < n_off:
                nxt = b[(i + 1) * 2] | (b[(i + 1) * 2 + 1] << 8)
                if o0 < nxt <= block_size:
                    o1 = nxt
            if o1 <= o0 or o0 >= block_size:
                continue
            out[i] = bytes(b[o0:o1])
        return out


# ------------------------------------------------- Dialog-Standzeit (FSM)
def dialog_lifetime(raw):
    """Bilder, die der Text ohne Tastendruck stehen bleibt.
    Nachbau von re15_dialog_step (msg_common.c:404ff), Startwerte aus
    re15_dialog_open_mask (msg_common.c:381ff): fsm=0, timer=1, scroll=2.
    Rueckgabe (bilder, 'ende') mit ende in {'hold','taste','seite'}."""
    rlen = len(raw)
    fsm, parse, timer, scroll = 0, 0, 1, 2
    frames = 0
    while frames < 100000:
        frames += 1
        if fsm == 0:
            if timer > 1:
                timer -= 1
                continue
            timer = 0
            budget = 1
            while True:
                if parse >= rlen:
                    fsm = 6
                    break
                b = raw[parse]
                if b == 0x01:
                    parse += 1
                    a = raw[parse] if parse < rlen else 0
                    parse += 1
                    if a == 0:
                        fsm = 4
                    else:
                        fsm = 5
                        timer = a
                    break
                if b == 0x02:
                    a = raw[parse + 1] if parse + 1 < rlen else 0
                    if a == 0:
                        fsm = 1
                    else:
                        fsm = 2
                        timer = a
                    break
                if b == 0x03:
                    fsm = 3
                    break
                if b == 0x05:
                    parse += 2
                    continue
                if b == 0x04:
                    a = raw[parse + 1] if parse + 1 < rlen else 2
                    if a == 0:
                        parse += 2
                        while parse < rlen and raw[parse] != 0x04:
                            c = raw[parse]
                            if c in (0x05, 0x06, 0x09, 0x0a, 0x0b):
                                parse += 1
                            parse += 1
                        nn = raw[parse + 1] if parse + 1 < rlen else 0
                        parse += 2
                        if nn:
                            scroll = nn
                        timer = scroll
                        break
                    scroll = a
                    parse += 2
                    continue
                if b == 0x08:
                    parse += 1
                    continue
                parse += 1
                budget -= 1
                if budget > 0:
                    continue
                timer = scroll
                break
            continue
        if fsm == 1:
            return frames, 'seite'            # wartet auf Tastendruck (Seitenumbruch)
        if fsm == 2:
            if timer > 1:
                timer -= 1
            else:
                parse += 2
                fsm = 0
                timer = scroll
            continue
        if fsm == 3:
            return frames, 'auswahl'
        if fsm == 4:
            return frames, 'taste'            # wartet auf Tastendruck (Ende)
        if fsm == 5:
            if timer > 1:
                timer -= 1
            else:
                fsm = 6
            continue
        if fsm == 6:
            return frames, 'hold'
    return frames, 'endlos'


PAUSE_SCD_BIT = 0x0200   # RE15_PAUSE_SCD 0x02000000 >> 16 (re15_scd.h:469)


# ------------------------------------------------- CFG / Zeitkosten
class Walker(object):
    """Dijkstra ueber (pc, callstack). Kosten = Bilder."""

    def __init__(self, rdt, voiced, freeze_lifetimes):
        self.r = rdt
        self.d = rdt.d
        self.voiced = voiced            # set von message-ids mit wav
        self.life = freeze_lifetimes    # id -> (bilder, ende) oder None

    def succ(self, pc, stack):
        """-> Liste (kosten, pc', stack', note). Leere Liste = Thread-Ende."""
        d = self.d
        if pc + 1 > len(d):
            return []
        op = d[pc]
        n = oplen(d, pc)
        if op == 0x01:                                   # Evt_end
            # op_evt_end gibt SCD_R_FRAME_RET (0) zurueck (scd_vm.c:753); der
            # Dispatcher (scd_vm.c:652ff) POPPT damit den Gosub-Rahmen und setzt
            # pc = call_stack[depth-1]. Nur bei depth==0 stirbt der Thread.
            if stack:
                return [(0, stack[-1], stack[:-1], 'evt_end->return')]
            return []
        if op == 0x02:                                   # Evt_next: 1 Bild
            return [(1, pc + 1, stack, 'Evt_next 1')]
        if op == 0x09:                                   # Sleep + Sleeping
            nn = u16(d, pc + 2)
            return [(nn, pc + 4, stack, 'Sleep %d' % nn)]
        if op == 0x0A:                                   # nacktes Sleeping
            nn = u16(d, pc + 1)
            return [(nn, pc + 3, stack, 'Sleeping %d' % nn)]
        if op == 0x06:                                   # Ifel_ck
            bl = u16(d, pc + 2)
            return [(0, pc + 4, stack, 'if-wahr'),
                    (0, pc + 4 + bl, stack, 'if-falsch')]
        if op == 0x07:                                   # Else_ck
            sk = s16(d, pc + 2)
            return [(0, pc + sk, stack, 'else-skip')]
        if op == 0x17:                                   # Goto
            off = s16(d, pc + 4)
            return [(0, pc + off, stack, 'goto')]
        if op == 0x18:                                   # Gosub
            sid = d[pc + 1]
            if sid < len(self.r.sub) and len(stack) < 8:
                return [(0, self.r.sub[sid], stack + (pc + 2,), 'gosub %d' % sid)]
            return [(0, pc + 2, stack, 'gosub(leer)')]
        if op == 0x19:                                   # Return
            if stack:
                return [(0, stack[-1], stack[:-1], 'return')]
            return []
        if op == 0x13:                                   # Switch: alle Zweige
            bl = u16(d, pc + 2)
            tbl = pc + 4
            outs = []
            a3 = tbl
            guard = 0
            while guard < 64:
                guard += 1
                o = d[a3]
                if o == 0x15:                            # Default
                    outs.append((0, a3 + 2, stack, 'default'))
                    break
                if o == 0x16:                            # Eswitch
                    outs.append((0, a3 + 2, stack, 'eswitch'))
                    break
                if o == 0x14:                            # Case
                    cbl = u16(d, a3 + 2)
                    val = s16(d, a3 + 4)
                    outs.append((0, a3 + 6, stack, 'case %d' % val))
                    a3 = a3 + 6 + cbl
                    continue
                break
            outs.append((0, tbl + bl, stack, 'switch-ende'))
            return outs
        if op == 0x14:                                   # Case (fallthrough)
            cbl = u16(d, pc + 2)
            return [(0, pc + 6, stack, 'case-fall')]
        if op == 0x15:
            return [(0, pc + 2, stack, 'default-fall')]
        if op == 0x16:
            return [(0, pc + 2, stack, 'eswitch-fall')]
        if op == 0x2B:                                   # Message_on
            mid = d[pc + 1]
            mask = u16(d, pc + 2)
            cost = 0
            note = 'msg %d mask=%04x' % (mid, mask)
            if mask & PAUSE_SCD_BIT:
                lf = self.life.get(mid)
                if lf is None:
                    cost = 0
                    note += ' FREEZE(unbekannt)'
                elif lf[1] in ('taste', 'seite', 'auswahl'):
                    cost = 100000            # wartet auf den Spieler
                    note += ' FREEZE(Taste)'
                else:
                    cost = lf[0]
                    note += ' FREEZE(%d)' % lf[0]
            return [(cost, pc + 4, stack, note)]
        # alles andere: keine Zeit
        return [(0, pc + n, stack, NAMES.get(op, 'OP%02X' % op))]

    def kill_here(self, pc):
        """Toetet ein Message_on an dieser Stelle eine laufende Aufnahme?
        re15_voice_play kehrt VOR re15_xa_read_s zurueck, wenn fuer (Raum,Id)
        keine Datei existiert (audio_pc.c:1711) -> nur vertonte Ids toeten."""
        return self.d[pc] == 0x2B and self.d[pc + 1] in self.voiced

    def min_gap(self, start_pc, stack=()):
        """Kuerzeste Bilderzahl vom Message_on bei start_pc bis zum naechsten
        toetenden Message_on. -> (bilder, killer_id) oder (None, None).
        Der Freeze des STARTENDEN Message_on zaehlt mit (succ() gibt ihn aus)."""
        INF = 100000
        pq = [(0, start_pc, stack)]
        seen = set()
        best = (None, None)
        while pq:
            cost, pc, st = heapq.heappop(pq)
            if cost >= INF:
                continue
            key = (pc, st)
            if key in seen:
                continue
            seen.add(key)
            if len(seen) > 300000:
                break
            for c, npc, nst, note in self.succ(pc, st):
                if npc < 0 or npc + 4 > len(self.d):
                    continue
                nc = cost + c
                if nc >= INF:
                    continue
                if self.kill_here(npc):
                    return nc, self.d[npc + 1]
                heapq.heappush(pq, (nc, npc, nst))
        return best

    def trace(self, start_pc, stack=(), grenze=400):
        """Geradeaus-Spur (if-wahr-Zweig) ab start_pc bis zum toetenden
        Message_on — fuer die Begruendung im Bericht."""
        out = []
        pc, st = start_pc, stack
        total = 0
        for _ in range(grenze):
            ss = self.succ(pc, st)
            if not ss:
                out.append(("ENDE (Evt_end/Return)", 0))
                break
            c, npc, nst, note = ss[0]
            if c:
                out.append((note, c))
                total += c
            elif note not in ('Nop',):
                out.append((note, 0))
            if npc + 4 > len(self.d) or npc < 0:
                out.append(("ADRESSE AUSSERHALB", 0))
                break
            if self.kill_here(npc):
                out.append(("-> TOETET: Message_on %d @0x%04X" % (self.d[npc+1], npc), 0))
                break
            pc, st = npc, nst
        return out, total
