# NB4 Messer (K): Lebenslauf des Szenen-Kanisters je Lauf (debug.log/gator_boss.log der Abnahme-6-Laeufe): Liegestelle, RUNDE (E1, PORT-WAHL NB1),
# Zielmodus +0x1C0&4 (gr2_zielmodus_takt), Stufe 1 (Aufnahme-Fenster), angehaengt; Treffer oben/unten zwischen RUNDE und Anhaengen aus state.log.
import re,sys,os,glob
for d in sorted(glob.glob(sys.argv[1]+'/*')):
    g=os.path.join(d,'debug.log')
    if not os.path.exists(g): continue
    txt=open(g,errors='ignore').read()+(open(os.path.join(d,'gator_boss.log'),errors='ignore').read() if os.path.exists(os.path.join(d,'gator_boss.log')) else '')
    m=re.search(r'Kanister Stufe 0 pos=\((-?\d+),(-?\d+),(-?\d+)\)',txt)
    if not m: continue
    runde=re.search(r'F(\d+) RUNDE zum Kanister an',txt); z1=re.findall(r'Zielmodus 1 \(Anschwimm-Sperre\) laengs=-?\d+ Stufe 0 F(\d+)',txt)
    z0=re.findall(r'Zielmodus 0 \(Anschwimm-Sperre\) laengs=-?\d+ Stufe 0 F(\d+)',txt); s1=re.search(r'KANISTER Stufe 1 laengs=(-?\d+) quer=(-?\d+) sub \d -> 1 F(\d+)',txt)
    print('%-34s Liegestelle (%s,%s) | RUNDE an %s | Zielmodus an %s aus %s | Stufe 1 %s'%(os.path.basename(d),m.group(1),m.group(3),
          'F'+runde.group(1) if runde else '-', ','.join('F'+x for x in z1) or '-', ','.join('F'+x for x in z0) or '-',
          ('F%s (laengs %s quer %s)'%(s1.group(3),s1.group(1),s1.group(2))) if s1 else '-'))
