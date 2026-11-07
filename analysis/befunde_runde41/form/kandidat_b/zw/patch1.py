import json
s = open('b_fit.py', encoding='utf-8').read()
s = s.replace("""        l_form = (dp ** 2).sum(1).mean() / (900.0 ** 2)
        # Boden in beiden Posen""", """        l_form = (dp ** 2).sum(1).mean() / (900.0 ** 2)
        l_kup = (dp[i_d] ** 2).sum(1).mean() / (170.0 ** 2) + ((mod.logr[i_d] - torch.log(mod.rad0[i_d])) ** 2).mean() / (0.12 ** 2)
        # Boden in beiden Posen""")
s = s.replace("""    r_i = {k: i for k, i in mod.ni.items()}
    for it in range(iters):""", """    r_i = {k: i for k, i in mod.ni.items()}
    i_d = [i for k, i in mod.ni.items() if k.startswith('d')]
    for it in range(iters):""")
s = s.replace("los = l1 + l2 + 0.03 * l_tief + 0.015 * l_form", "los = l1 + l2 + 0.05 * l_tief + 0.015 * l_form + 0.03 * l_kup")
open('b_fit.py', 'w', encoding='utf-8').write(s)
s = open('b_teile.py', encoding='utf-8').read()
s = s.replace("n('nl_s', (620, -1520, -640), 420)", "n('nl_s', (560, -1560, -760), 480)")
s = s.replace("n('nr_s', (620, -1520, 440), 400)", "n('nr_s', (560, -1560, 520), 460)")
open('b_teile.py', 'w', encoding='utf-8').write(s)
B = {'kuppel': 690, 'k01': 62, 'k02': 60, 'k03': 34, 'k04': 26, 'k05': 120, 'k06': 64, 'k07': 38, 'k08': 30, 'k11': 74, 'k12': 62, 'k13': 88, 'k14': 52, 'k16': 70, 'k17': 62, 'k18': 88, 'k19': 40}
print(sum(B.values()))
json.dump(B, open('zw/budget1.json', 'w'))
