import io, json, re
J = r"C:\Users\mjoedicke\.claude\projects\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\subagents\workflows\wf_80b8f8c8-f4d\journal.jsonl"
W = r"C:\workspace\git\reAi_v2\.claude\worktrees"
OUT = r"C:\Users\MJOEDI~1\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\r36_rest3_args.json"

def find(d):
    if isinstance(d, dict):
        if 'maengel' in d and 'urteile' in d:
            return d
        for v in d.values():
            x = find(v)
            if x: return x
    if isinstance(d, list):
        for v in d:
            x = find(v)
            if x: return x
    return None

def kurz(a):
    return {
        'bestanden': False,
        'urteile': [{'punkt': u['punkt'][:160], 'urteil': u['urteil'], 'beleg': u['beleg'][:260]} for u in a['urteile']],
        'maengel': [m[:1100] for m in a['maengel']],
    }

abn = {}
for line in io.open(J, encoding='utf-8'):
    o = json.loads(line)
    if o.get('type') != 'result':
        continue
    aid = o.get('agentId', '')
    a = find(o)
    if not a:
        continue
    if aid == 'a41f081b8cae71c88': abn['A'] = kurz(a)
    if aid == 'a6628db8867a63267': abn['C'] = kurz(a)
assert 'A' in abn and 'C' in abn, abn.keys()

# F aus dem committeten Bericht
f = io.open(W + r"\r36_zombieki\analysis\befunde_runde36\F_abnahme_0.md", encoding='utf-8').read()
m5 = f[f.index('## 5. Maengel'):]
items = re.split(r'\n(?=\d+\. \*\*)', m5.split('\n', 1)[1])
fm = [re.sub(r'\s+', ' ', it).strip()[:1100] for it in items if re.match(r'\d+\. \*\*', it.strip())]
t3 = f[f.index('## 3. Urteile'):f.index('## 4. Gates')]
fu = []
for row in t3.splitlines():
    c = [x.strip() for x in row.strip().strip('|').split('|')]
    if len(c) >= 3 and c[1] in ('erfuellt', 'teilweise', 'nicht_erfuellt', 'nicht_pruefbar') or (len(c) >= 3 and re.search(r'erfuellt|teilweise', c[1] if len(c) > 1 else '')):
        fu.append({'punkt': c[0][:160], 'urteil': c[1][:40], 'beleg': c[2][:260]})
abn['F'] = {'bestanden': False, 'urteile': fu, 'maengel': fm}

abn['D'] = {'bestanden': False,
 'urteile': [
  {'punkt': '1 Kein Feuer-Effekt bei der normalen Granate (nur Zerplatzen), Feuer nur incendiary', 'urteil': 'erfuellt', 'beleg': 'D_abnahme_0.md §4 (selbst gemessen; Russ-Tinten @0x80106478-f8 bleiben als Hinweis)'},
  {'punkt': '2 Granate loest manchmal nicht aus / wird nicht geworfen', 'urteil': 'teilweise', 'beleg': 'D_abnahme_0.md §5: R1-Aussetzer behoben (ROOM1100 MITTE/HOCH/TIEF 27/27, Saeure/Brand 6/6, Elza 3/3; v0.8.23 6/27 bzw. 0/6); Griff waehrend der Wurfbewegung verbraucht weiter ohne Wurf'}],
 'maengel': [
  'M1 (Punkt 2): "Granate verbraucht, aber nicht geworfen" bleibt an der HEAD-exe reproduzierbar, sobald Leon in den 19-24 Bildern der Wurfbewegung gepackt/getroffen wird — Lauf c2 ROOM1140 P1, Skript MD0.6,MDA0.2,MD2.667,MDA0.2,MD2.0,W3: zweiter Abzug F105 (mg 4->3), Zombie 0x11 packt F113, Spawnbild waere F129 -> 2 Abzuege, 1 Wurf, Menge 5->3 (HEAD wie v0.8.23). STAGE1.BIN 80113b00 sb v1(=2),0x800aca58 + 80113b30 sh zero,0x800aca5a (Griff ersetzt das Kommando ohne Ziel-Gate).',
  'M2 (Regel "Original oder nicht", keine Wahlfragen): das Dossier legt den Griff-Fall dem Nutzer als Wahlfrage vor ("Soll sie im Inventar bleiben?"), obwohl die eigene Beta->Retail-Begruendung fuer Punkt 2 (Verbrauch im Bild 0 + Wurf im Bild 19-24 = unfertige Beta-Kombination; RE2-Retail-Werfer: Abgang im Rueckstossbild 1, info/re2leon/PSX.EXE 80044b98 lbu v1,333(s0) / 80044b9c addiu v0,zero,1 / 80044ba0 bne) genau diesen Fall mit abdeckt. ORCHESTRATOR-ENTSCHEIDUNG gemaess Nutzer-Wortlaut ("wird nicht geworfen"): eine Granate wird NUR verbraucht, wenn sie wirklich geworfen wird (Spawn) — bricht ein Griff/Treffer die Wurfbewegung vor dem Spawnbild ab, bleibt sie im Inventar. Umsetzen, belegen (Verbrauchsstelle, Spawn-Gate), messen (Lauf c2 nachstellen: 2 Abzuege, 1 Wurf -> Menge 5->4), Pin; Wahlfrage aus dem Dossier streichen.',
  'M3 (Hinweis): Abnahme 0 wurde beim Sitzungslimit 18:25 abgebrochen (Bericht D_abnahme_0.md Teilstand, committet 95d15ec7) — Gates/Vertrag/Testliste sind dort nicht abschliessend; die Abnahme 1 prueft vollstaendig.']}

abn['B'] = {'bestanden': False,
 'urteile': [
  {'punkt': 'Leons Kleidung hat keine Blutflecken/Risse mehr bei Warning/Danger', 'urteil': 'teilweise', 'beleg': 'B_abnahme_0.md §5.1 (Teilstand): dokumentierter Nutzer-Weg Hunde ROOM1190 -> fuenf Bisse bis hp 0, 250 Wundpunkte auf Panel 4/5/7, kein Panel erreicht 0x78, KEIN Decal; Zombie-Weg (§5.2) noch nicht gemessen'}],
 'maengel': [
  'M1 (Nutzer-Weg Hunde, wesentlich): auf dem im befund.log des Nutzers dokumentierten Weg (ROOM1190, F2002 R1190 C11 hp=80, F2047 hp=80->60 in -20-Schritten; Memory-Card-Slots alle wunden 0/0 x8) geht Leon unter dem RE2-Standardgehirn ueber Caution (80/60/40) und Danger (20) bis in den Tod, ohne dass je ein Decal entsteht: RE15_DEBUG_JUMP=1190@240 RE15_DEBUG_SUB=13@450 RE15_DEBUG_DOGWAKE=500 RE15_KLEIDUNG_LOG=1 SDL_ASSERT=always_ignore -> [kleidung] F582/F585/F587/F673/F708 hundebiss typ=0x20, 250 Punkte auf Panel 4/5/7, gen bleibt 1. Ursache: der RE1.5-Hundebiss kostet 10 HP (STAGE1.BIN lhu player.hp @0x8010f2bc, addiu a0,v0,-10 @0x8010f2c8, sh @0x8010f2d0) und stempelt +0x32 (@0x8010f360/64) auf eines von zwei Panels je Blickrichtung -> von 100 bis 20 HP 8 Bisse = 400 Punkte auf 2 Panels -> Decal sicher vor Danger. Die Spur haengt dieselben +0x32 an den RE2-Biss, der im Port 20 HP kostet (Dossier §1.2 @0x80104EB8) -> das Verhaeltnis Wundpunkte:HP-Verlust halbiert sich, die Kopplung Warning/Danger -> Blut/Risse entsteht nicht. Abhilfe belegen (z.B. Stempel je Biss an den HP-Verlust des RE2-Bisses koppeln, so dass das RE1.5-Verhaeltnis 0x32 je 10 HP gilt) und am exe-Weg ROOM1190 messen (Decal vor Danger), Pin; unit_r36_kleidung_hund prueft bisher nur die FORM je Biss.',
  'M2 (Hinweis): Abnahme 0 wurde beim Sitzungslimit 18:25 abgebrochen (B_abnahme_0.md Teilstand, committet a096a1a5): §1-§4 (Diff, Bau, 7/7 Spur-Tests, RE-Gate, Verursacher e11962ec belegt) liegen, §5.2 Zombie-Weg und die Gates sind offen — die Abnahme 1 prueft vollstaendig. Messumgebung: die Maschine lief in einer RDP-Sitzung; jede exe blieb an der SDL-Debug-Assertion WIN_AddDisplay stehen -> fuer exe-Laeufe SDL_ASSERT=always_ignore setzen; ohne RE15_TITLE_SHOT(_AF) bleibt die exe im Titel.']}

bau = {
 'A': {'status': 'fertig', 'zusammenfassung': 'Kletter-Gate wie FUN_8002d2c0 @0x8002d358-35c (Flags != 0 statt Bit 0x100), Zensus-Pin 24/24, echter Weg 10F0/10F1 — Dossier A_klettern.md; Abnahme 0: Punkt erfuellt, nur Mangel 1 (widerlegte Null-Box-Aussage zu 3091 obj 1).', 'commits': ['1c926683', '64c66e78', 'd6cc4fd0', '42d5e7d0'], 'suite': 'gezielt laut Abnahme 0: 27/27 + integration_r36_klettern + integration_r35_cut10f0 gruen', 'offene_punkte': ['Herunterfall-Substate 0xd (FUN_8002dc48) nicht portiert — ausserhalb des Wortlauts']},
 'B': {'status': 'fertig', 'zusammenfassung': 'Phase-4-Wundstempel gehirn-unabhaengig (kleidung_r36.c), RE2-Abwurf/Arm/Hund/Kraehe stempeln, Elza-LUT Zeile 4 — Dossier B_kleidung.md.', 'commits': ['7b03f608', '4277116f', '2e3dfc7e', '866af8e3'], 'suite': 'laut Dossier 599/599 (vor der Abnahme)', 'offene_punkte': ['STAGE2-5-Zwillinge, Birkin-Stempel, RE2-Kraehe Dive/Strike']},
 'C': {'status': 'fertig', 'zusammenfassung': 'Sichtlinie + RE2-Fresser-Sperre fuer alle Schusswaffen, Blut-Anker Anker.R*a3+Anker.T, Kraehe/Magnum = RE2-GIB — Dossier C_schuss.md; Abnahme 0: Punkte 1 und 3 erfuellt, Punkt 2 teilweise (Maengel M1-M7).', 'commits': ['41de2772', 'df7ecdc5', '37d796f3', 'c2fe896c', '09ed2d02', 'caf7e609'], 'suite': 'gezielt laut Abnahme 0 gruen', 'offene_punkte': ['RE1.5-KI-Pfade (nicht Vorgabe)', 'Entscheidungsfrage liegender Spawn-Zombie']},
 'D': {'status': 'fertig', 'zusammenfassung': 'HE-Granate ohne Flammen an Zombies (NUTZER-VORGABE, RE2 FUN_8010640C), jeder Abzug ein Wurf (R1-Abbruch @0x80033600-50 behoben), voller Effektpool im Spawnbild (PORT-WAHL) — Dossier D_granate.md.', 'commits': ['e289dd0e', '2cb21596', 'dd88956b', '44a3eaf6', '95d15ec7'], 'suite': 'keine (Abnahme 0: Spur-Tests gruen)', 'offene_punkte': ['Griff waehrend der Wurfbewegung (jetzt Mangel M1/M2)']},
 'F': {'status': 'fertig', 'zusammenfassung': 'P1 Sicht-Bit aus FUN_8003dcc4 Nibble 3 (zombieki_r36.c), P2 Pad-Suche nur FERN (PORT-WAHL), P3 Gorilla-Blutlache FUN_80116f50/FUN_8011be54 — Dossier F_zombieki.md; Abnahme 0: P3 erfuellt, P1/P2 teilweise (8 Maengel).', 'commits': ['3df50132', 'fee4e956', '853cbfb3', '5e223660', '72050beb'], 'suite': 'Abnahme 0: 153/153 gezielt', 'offene_punkte': ['ROOM10B0 Terrasse Nav', 'GDB-Gegenmessung P2']},
}

stand = {
 'C': 'Baum r36_schuss: ein erster Nachbesserer (C#1) wurde nach dem Anlegen der Gliederung M1-M7 im Dossier vom Sitzungslimit beendet (Commit caf7e609, kein Code). Die Abnahme 0 (C_abnahme_0.md) ist vollstaendig.',
 'J': 'Baum r36_szenenleon, Zweig r36/szenenleon: Commits 31f6bd63 (Dossier), 803f6f1b (Messung vorher 11B0-Schieben + RE Plc_dest Modus 0x13 @0x80052864, Member 6 @0x800411c8, Objekt-Bit 0x200 @0x8002bec0; Irons-Kopf-RE @0x8011d278), 7f734852 (Punkt 3 10F0-Abgang ohne Cut_chg 0 — Kamera bleibt Cut 2 (RDT @0x0A0) auf Leon, Parken hinter der Kamera, Tuerton DOOR13 Door_exit zum Schluss; Programm 1342 B), 3d394d71 (UNGEPRUEFTER Zwischenstand des Orchestrators beim Sitzungslimit 18:25: szenenleon_r36.c + include/re15_szenenleon_r36.h neu, Haken in anim_select_common.c, aot_common.c, enemy_ai_common.c, game_step_common.c, scd_vm.c, main.c — NICHT gebaut/getestet; erst bauen, Diff lesen, jeden Haken pruefen). Dossier J_szenenleon.md: §1 Punkt 1 Messung + RE fertig, Umsetzung offen; §3 Punkt 3 umgesetzt + Messung nachher; §4 Punkt 4 RE fertig, Umsetzung offen; Punkt 2 (Werkstatt-Tuer, nur diese — AUFTRAG.md Nachtrag 14:00) offen.',
}

args = {
 'welle': ['A', 'C', 'F', 'D', 'B', 'J', 'E', 'L', 'Q', 'P', 'K', 'I', 'G', 'H', 'M', 'N', 'O'],
 'slots': 4, 'max_nach': 2, 'model': 'opus',
 'modus': {k: 'nachbesserung' for k in 'ACFDB'},
 'abn': abn, 'bau': bau, 'stand': stand,
}
io.open(OUT, 'w', encoding='utf-8').write(json.dumps(args, ensure_ascii=False))
print(len(json.dumps(args, ensure_ascii=False)), 'Zeichen')
print('F urteile', len(fu), [u['urteil'] for u in fu], 'F maengel', len(fm))
for k in 'ACFDB':
    print(k, 'maengel', len(abn[k]['maengel']), 'urteile', len(abn[k]['urteile']))
print(fm[0][:200])
