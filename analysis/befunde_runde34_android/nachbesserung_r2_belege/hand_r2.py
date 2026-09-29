# -*- coding: utf-8 -*-
# Hand-Mutanten fuer mutanten_teil.py --hand (Nachbesserung R2). HAND = [(Name, [(alt, neu, Anzahl)])].
# Teil 1: die Hand-Mutanten der Runde 1 (mutanten_voll.py), soweit ihr Muster im Code noch vorkommt.
# Teil 2: die fuenf Teil-Mutanten der Gegenpruefung R2 in ihrer Form fuer den neuen Code + verwandte.
HAND = [
    ("D1_U3_nur_erster_block", [
        ("            h.update(b)\n            n += len(b)\n",
         "            h.update(b)\n            n += len(b)\n            return h.hexdigest(), n\n", 1),
        ("            if d is None:\n                nimm(b)\n                continue\n",
         "            if d is None:\n                nimm(b)\n                break\n", 1)]),
    ("D2_U4_crc32_statt_sha256", [
        ("class Bedienfehler(Exception):",
         "class _Crc(object):\n    def __init__(self):\n        self.c = 0\n    def update(self, b):\n"
         "        self.c = zlib.crc32(b, self.c)\n    def hexdigest(self):\n        return '%08x' % self.c\n\n\n"
         "class Bedienfehler(Exception):", 1),
        ("    h, st = hashlib.sha256(), [0, 0]", "    h, st = _Crc(), [0, 0]", 1),
        ("    h, n = hashlib.sha256(), 0\n    with open(pfad", "    h, n = _Crc(), 0\n    with open(pfad", 1)]),
    ("D3_offset_aus_zentralverzeichnis", [
        ("        e.daten_off = e.lho + LFH_LEN + l_nlen + l_xlen\n",
         "        e.daten_off = e.lho + LFH_LEN + l_nlen\n", 1)]),
    ("D4_namen_wie_zipfile", [
        ("        grund = _name_fehler(e.name_roh)\n",
         "        e.name_roh = e.name_roh.split(b'\\x00')[0].replace(b'\\\\', b'/')\n"
         "        grund = _name_fehler(e.name_roh)\n", 1)]),
    ("D5_lfh_nur_name", [
        ("        if (l_crc, l_csize, l_usize) != (e.crc, e.csize, e.usize):", "        if False:", 1)]),
    ("D6_manifest_cr_bleibt", [
        ("        z = z.rstrip(\"\\r\")", "        z = z", 1)]),
    ("D7_deflate_ende_egal", [
        ("            if not d.eof or d.unused_data:", "            if False:", 1)]),
    ("D8_nur_laenge_statt_crc", [
        ("    if crc != e.crc or n != e.usize:", "    if n != e.usize:", 1)]),
    ("D9_eocd_von_vorn", [
        ("        i = ende.rfind(EOCD_SIG)", "        i = ende.find(EOCD_SIG)", 1)]),
    ("D11_zentralverzeichnis_rest_egal", [
        ("    if p != len(cd):\n", "    if False:\n", 1)]),
    ("D12_lage_egal", [
        ("        if e.daten_off + e.csize > cd_off:", "        if False:", 1)]),
    ("D13_doppelte_namen_egal", [
        ("        if len(je_name[name]) > 1:", "        if False:", 1)]),
    ("D14_nur_stored_gelesen", [
        ("    d = zlib.decompressobj(-15) if e.methode == 8 else None", "    d = None", 1)]),
    ("D15_nicht_assets_nicht_gelesen", [
        ("        for e in sorted((x for x in eintraege if x.lesbar and x.nr not in gelesen), key=lambda x: x.lho):",
         "        for e in []:", 1)]),
    ("D16_sha_nur_quelle_ganz", [
        ("            if d is None:\n                nimm(b)\n                continue\n",
         "            if d is None:\n                nimm(b)\n                if st[1] >= BLOCK:\n                    break\n"
         "                continue\n", 1)]),
    ("D17_manifest_nur_kopf", [
        ("    for pfad in sorted(eintraege):\n        if pfad not in apk_dateien:",
         "    for pfad in []:\n        if pfad not in apk_dateien:", 1)]),
    ("D18_zusatz_nur_ausserhalb_assets", [
        ("            if name != man_name and name not in quellen:",
         "            if name != man_name and name not in quellen and not name.startswith('assets/'):", 1)]),
    # --- Teil 2: Gegenpruefung R2 (M14, M16) im neuen Code + verwandte Teilbedingungen
    ("DM14a_dd_nur_aus_cd", [
        ("        if (e.flags | l_flags) & FLAG_DATA_DESCRIPTOR:", "        if e.flags & FLAG_DATA_DESCRIPTOR:", 1)]),
    ("DM14b_dd_nur_aus_lfh", [
        ("        if (e.flags | l_flags) & FLAG_DATA_DESCRIPTOR:", "        if l_flags & FLAG_DATA_DESCRIPTOR:", 1)]),
    ("DM14c_dd_nur_wenn_beide", [
        ("        if (e.flags | l_flags) & FLAG_DATA_DESCRIPTOR:", "        if e.flags & l_flags & FLAG_DATA_DESCRIPTOR:", 1)]),
    ("DM16a_nicht_assets_nur_stored", [
        ("        for e in sorted((x for x in eintraege if x.lesbar and x.nr not in gelesen), key=lambda x: x.lho):",
         "        for e in sorted((x for x in eintraege if x.lesbar and x.nr not in gelesen and x.methode == 0), key=lambda x: x.lho):", 1)]),
    ("DM16b_nicht_assets_nur_deflate", [
        ("        for e in sorted((x for x in eintraege if x.lesbar and x.nr not in gelesen), key=lambda x: x.lho):",
         "        for e in sorted((x for x in eintraege if x.lesbar and x.nr not in gelesen and x.methode != 0), key=lambda x: x.lho):", 1)]),
    ("DV1_verschluesselt_nur_cd", [
        ("        if (e.flags | l_flags) & FLAG_VERSCHLUESSELT:", "        if e.flags & FLAG_VERSCHLUESSELT:", 1)]),
    ("DV2_verschluesselt_nur_lfh", [
        ("        if (e.flags | l_flags) & FLAG_VERSCHLUESSELT:", "        if l_flags & FLAG_VERSCHLUESSELT:", 1)]),
    ("DV3_verschluesselt_nur_bit0", [
        ("FLAG_VERSCHLUESSELT, FLAG_DATA_DESCRIPTOR = 0x0001 | 0x0040 | 0x2000, 0x0008",
         "FLAG_VERSCHLUESSELT, FLAG_DATA_DESCRIPTOR = 0x0001, 0x0008", 1)]),
    ("DP1_paket_nur_groesse", [
        ("        if q_sha != p_sha:", "        if q_n != p_n:", 1)]),
    ("DT1_tuer_ohne_fnv", [
        ("            if mit_fnv:\n                h = _fnv1a32(p)", "            if False:\n                h = _fnv1a32(p)", 1)]),
    ("DT2_tuer_fnv_ueber_erste_block", [
        ("        for b in iter(lambda: f.read(BLOCK), b\"\"):\n            for x in b:",
         "        for b in iter(lambda: f.read(BLOCK), b\"\"):\n            b = b[:1000]\n            for x in b:", 1)]),
    ("DT3_griff_ohne_rueckfall", [
        ("            kand = g31 + g33", "            kand = g31", 1)]),
    ("DT4_griff_ohne_fuer_eigen", [
        ("            kand = [g for g in g33 if g[\"fuer_eigen\"] == z[\"eigen\"]]", "            kand = g33", 1)]),
    ("DT5_basis_nicht_verlangt", [
        ("    for e in eigen:                                       # Quelle jedes Port-Archivs (tuer_archiv_bauen.py)\n"
         "        braucht(e[\"basis\"], \"Basis von %s\" % e[\"kennung\"])",
         "    for e in []:\n        braucht(e[\"basis\"], \"Basis von %s\" % e[\"kennung\"])", 1)]),
    ("DT6_zusatzdateien_egal", [
        ("        for name in sorted(da - set(soll)):", "        for name in []:", 1)]),
    ("DW1_wurzel_egal", [
        ("        if name not in SHARED_WURZEL:", "        if False:", 1)]),
    ("DK1_kopf_mit_unicode_ziffern", [
        ('KOPF_RE = re.compile(r"# re15 assets ([0-9]+) ([0-9]+)")', 'KOPF_RE = re.compile(r"# re15 assets (\\d+) (\\d+)")', 1)]),
    ("DZ1_praefix_egal", [
        ("        if f.read(4) != LFH_SIG:", "        if False:", 1)]),
]
