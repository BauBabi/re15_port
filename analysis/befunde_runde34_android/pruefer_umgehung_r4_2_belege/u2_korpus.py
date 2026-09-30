# -*- coding: utf-8 -*-
"""Gegenpruefung R4-2 (Umgehung), Y4: Format v2 - Geraete-Leser (asset_abgleich.c HEAD ueber u1_liste_korpus.exe) und
Gate (manifest_lesen aus gate_echt.py = release/apk_asset_gate.py HEAD) auf DENSELBEN Bytes. Korpus = der 57-Listen-Korpus
der Runde 1 (u1_korpus.py, unveraendert importiert) + neue Faelle fuer die ASCII-/Segmentregel der Nachbesserung R4-1,
Kopf/Zeilenenden, die die Hand-Mutanten D13-D17 unterscheiden, und die Regel-Luecke Y4 (Ordner-Segment X.neu neben X).
Spalte "soll" = was das Format laut asset_abgleich.h verlangt (A annehmen / V verwerfen); "V?" = Runde-1-Unicode-Paare,
die seit R4-1 V sein muessen.
Aufruf: u2_korpus.py <korpus_exe> <ordner> <gate.py> [<gate2.py> ...]"""
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "u1"))
import u1_korpus as u1  # noqa: E402

S1, S2, S3 = u1.S1, u1.S2, b"3" * 64
P1, P2 = u1.P1, u1.P2
liste = u1.liste
G = u1.G


def z(p, s=S1, g=b"5"):
    return (g, s, p)


NEU = [
    # --- ASCII-Grenzen im Pfad (R4-1: nur 0x20-0x7e)
    ("n_pfad_0x20_innen", liste([z(b"shared_assets/PSX/a b.bin"), G[1]]), "A"),
    ("n_pfad_0x7e", liste([z(b"shared_assets/PSX/a~.bin"), G[1]]), "A"),
    ("n_pfad_0x7f", liste([z(b"shared_assets/PSX/a\x7f.bin"), G[1]]), "V"),
    ("n_pfad_0x80", liste([z(b"shared_assets/PSX/a\x80.bin"), G[1]]), "V"),
    ("n_pfad_0xff", liste([z(b"shared_assets/PSX/a\xff.bin"), G[1]]), "V"),
    ("n_pfad_utf8_ae", liste([z("shared_assets/PSX/ä.bin".encode()), G[1]]), "V"),
    ("n_pfad_kelvin_allein", liste([z("shared_assets/PSX/K.bin".encode()), G[1]]), "V"),
    ("n_pfad_tab_am_ende", liste([z(P1 + b"\t"), G[1]]), "V"),
    ("n_pfad_leerzeichen_am_ende", liste([z(P1 + b" "), G[1]]), "A"),
    ("n_pfad_nur_leerzeichen_segment", liste([z(b"shared_assets/ /x.bin"), G[1]]), "A"),
    ("n_pfad_drei_punkte", liste([z(b"shared_assets/.../x.bin"), G[1]]), "A"),
    ("n_pfad_punkt_leer", liste([z(b"shared_assets/. /x.bin"), G[1]]), "A"),
    ("n_pfad_dot_slash_vorn", liste([z(b"./shared_assets/x.bin"), G[1]]), "V"),
    # --- Segment-/Pfadlaengen (R4-1: Segment <= 251, Pfad <= 512)
    ("n_seg_251_datei", liste([z(b"shared_assets/" + b"x" * 251), G[1]]), "A"),
    ("n_seg_252_datei", liste([z(b"shared_assets/" + b"x" * 252), G[1]]), "V"),
    ("n_seg_251_ordner", liste([z(b"s/" + b"x" * 251 + b"/y"), G[1]]), "A"),
    ("n_seg_252_ordner", liste([z(b"s/" + b"x" * 252 + b"/y"), G[1]]), "V"),
    ("n_seg_252_erstes", liste([z(b"x" * 252 + b"/y"), G[1]]), "V"),
    ("n_pfad_512", liste([z(b"a" * 9 + b"/" + b"x" * 251 + b"/" + b"y" * 250), G[1]]), "A"),
    ("n_pfad_513", liste([z(b"a" * 10 + b"/" + b"x" * 251 + b"/" + b"y" * 250), G[1]]), "V"),
    # --- .neu-Regel (Endung der Zwischendatei)
    ("n_neu_klein", liste([z(b"shared_assets/PSX/x.neu"), G[1]]), "V"),
    ("n_neu_gross", liste([z(b"shared_assets/PSX/x.NEU"), G[1]]), "V"),
    ("n_neu_gemischt", liste([z(b"shared_assets/PSX/x.nEu"), G[1]]), "V"),
    ("n_neu_leerzeichen_hinten", liste([z(b"shared_assets/PSX/x.neu "), G[1]]), "A"),
    ("n_neu_punkt_hinten", liste([z(b"shared_assets/PSX/x.neu."), G[1]]), "A"),
    ("n_neu_als_ordner", liste([z(b"shared_assets/PSX/X.neu/B.bin"), G[1]]), "A"),
    # Y4: Datei X UND Ordner X.neu/ - nach der Regel gueltig; auf dem Geraet ist X.neu/ danach ein ORDNER, die
    # Zwischendatei fuer X (s_root/.../X.neu) laesst sich nie mehr anlegen (open O_CREAT auf Ordner -> EISDIR)
    ("n_Y4_datei_und_neu_ordner", liste([z(b"shared_assets/PSX/X"), z(b"shared_assets/PSX/X.neu/B", S2)]), "A"),
    ("n_Y4_datei_und_NEU_ordner_gk", liste([z(b"shared_assets/PSX/x"), z(b"shared_assets/PSX/X.NEU/B", S2)]), "A"),
    # --- Gross/klein-Dubletten (nur ASCII-Buchstaben falten)
    ("n_dub_Z_z", liste([z(b"shared_assets/PSX/Z.bin"), z(b"shared_assets/PSX/z.bin", S2)]), "V"),
    ("n_dub_ordner_gk", liste([z(b"shared_assets/PSX/DATA/a.bin"), z(b"shared_assets/psx/data/b.bin", S2)]), "A"),
    ("n_kein_dub_klammer", liste([z(b"shared_assets/PSX/[.bin"), z(b"shared_assets/PSX/{.bin", S2)]), "A"),
    ("n_kein_dub_at_backtick", liste([z(b"shared_assets/PSX/@.bin"), z(b"shared_assets/PSX/`.bin", S2)]), "A"),
    ("n_dub_exakt_kopf_zaehlt_einmal", b"# re15 assets v2 1 5\n5\t" + S1 + b"\t" + P1 + b"\n5\t" + S1 + b"\t" + P1 + b"\n", "V"),
    ("n_dub_exakt_kopf_zaehlt_zweimal", b"# re15 assets v2 2 10\n5\t" + S1 + b"\t" + P1 + b"\n5\t" + S1 + b"\t" + P1 + b"\n", "V"),
    # --- Kopf/Zeilenenden, die D13-D17 unterscheiden
    ("n_kopf_cr_vorn", b"\r" + liste(G), "V"),
    ("n_kopf_leerzeichen_hinten", liste(G, kopf=b"# re15 assets v2 2 10 "), "V"),
    ("n_kopf_tab_hinten", liste(G, kopf=b"# re15 assets v2 2 10\t"), "V"),
    ("n_kopf_anzahl_19_ziffern", liste(G, kopf=b"# re15 assets v2 0000000000000000002 10"), "V"),
    ("n_kopf_bytes_19_ziffern", liste(G, kopf=b"# re15 assets v2 2 0000000000000000010"), "V"),
    ("n_zeile_cr_vorn", b"# re15 assets v2 2 10\n\r5\t" + S1 + b"\t" + P1 + b"\n5\t" + S2 + b"\t" + P2 + b"\n", "V"),
    ("n_zeile_lf_cr", liste(G, nl=b"\n\r"), "V"),
    ("n_zeile_cr_innen_kopf", b"# re15 assets v2\r2 10\n5\t" + S1 + b"\t" + P1 + b"\n5\t" + S2 + b"\t" + P2 + b"\n", "V"),
    ("n_groesse_null", liste([z(P1, g=b"0"), G[1]]), "A"),
    ("n_groesse_19_ziffern", liste([z(P1, g=b"0000000000000000005"), G[1]], b=10), "V"),
    ("n_nur_kopf_leerzeilen", b"# re15 assets v2 0 0\n\n\n", "V"),
    ("n_pfad_ohne_toplevel_baum", liste([z(b"foo/bar.bin"), G[1]]), "A"),
]

u1.KORPUS = u1.KORPUS + NEU

if __name__ == "__main__":
    u1.main()
