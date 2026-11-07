import subprocess, sys, re
S = r'C:\workspace\git\reAi_v2\.claude\skills\re15-psx-disasm\scripts\re2_disasm.py'
out = subprocess.run([sys.executable, r'C:\Users\MJOEDI~1\AppData\Local\Temp\claude\1ad3836299811990\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\ws\abn1\scan_cfb74.py'], capture_output=True, text=True).stdout
for line in out.splitlines():
    m = re.match(r'(\S+)\s+@(0x[0-9a-f]+)', line)
    if not m: continue
    name, a = m.group(1), int(m.group(2),16)
    args = [sys.executable, S, 'dis', hex(a-4*10), '12']
    if name != 'PSX.EXE': args += ['--bin', name]
    r = subprocess.run(args, capture_output=True, text=True, cwd=r'C:\workspace\git\reAi_v2').stdout
    print('=====', name, hex(a))
    print('\n'.join(l for l in r.splitlines() if not l.startswith(';')))
