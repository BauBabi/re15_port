s = open('b_alles.py', encoding='utf-8').read()
s = s.replace("import b_lauf as L\n", "import b_lauf as L\nimport b_mal2 as M2\n")
s = s.replace("    durchgang(a.fit, a.tag, a.ab, a.snap)\n", "    durchgang(a.fit, a.tag, a.ab, a.snap, malen=M2.malen)\n")
open('b_alles.py', 'w', encoding='utf-8').write(s)
