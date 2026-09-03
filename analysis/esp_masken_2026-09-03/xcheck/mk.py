import io,os
txt = open('append.txt','r',encoding='utf-8').read()
p = os.path.abspath('../H_runtime_sld_plan.md')
with open(p,'a',encoding='utf-8') as f:
    f.write(txt)
print('appended', os.path.getsize(p))
