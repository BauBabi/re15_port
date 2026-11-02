import json,sys
R=json.load(open(sys.argv[1],encoding='utf-8'))
K=R['kennzahlen']
print('Flaechen %d (%d D, %d V), Rahmen: %s, Boden %s'%(K['flaechen'],K['dreiecke'],K['vierecke'],K['rahmen'] or 'ok',K['boden_y']))
for n in (1,2):
    B=R['bild%d'%n]
    u=B['umriss']
    def z(l):
        q=[x for x in l if x['zaehlt']]
        return sum(1 for x in q if x['ok']),len(q)
    print('Bild %d: IoU %.3f Weiss %.3f Schwarz %d | Punkte %d/%d Flaechen %d/%d Zeichnung %d/%d Verbote %d/%d | MAE %.1f NCC %.3f'%(n,u['iou'],u['weiss_gedeckt'],u['schwarz_getroffen'],*z(B['punkte']),*z(B['flaechen']),*z(B['zeichnung']),*z(B['verbote']),B['mae_weiss'],B['ncc_weiss']))
    f=[]
    for x in B['punkte']+B['flaechen']+B['zeichnung']+B['verbote']:
        if x['zaehlt'] and not x['ok']:
            d=x.get('abstand')
            f.append('%s%s'%(x['id'],'' if d is None else '(%+.1f/%.1f)'%(d,x['grenze'])))
    print('   offen:',' '.join(f))
    print('   Schwarz:',B['schwarz_treffer'],'Weiss fehlt:',B['weiss_fehlt'][:2])
