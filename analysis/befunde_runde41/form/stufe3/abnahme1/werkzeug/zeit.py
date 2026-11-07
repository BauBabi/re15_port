import re,sys
# Zeitleiste: Zustandswechsel des Birkin (t=30) und G5 (t=36), Spieler-HP, Kamera
f=sys.argv[1]
last=None; lastpl=None; lastcam=None
rx=re.compile(r'\[(\d+) t=(30|36) st=(\d+) ss1=(\d+) ss2=(\d+) ss3=(\d+) g=([0-9a-f]+) mo=(\d+) af=(\d+)[^\]]*@\((-?\d+),(-?\d+),r(-?\d+)\)\] hp=(-?\d+)')
for line in open(f,errors='replace'):
    m0=re.match(r'F(\d+) pad=(\w+) PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\) pst=(\d+) ps1=(\d+)',line)
    if not m0: continue
    F=int(m0.group(1)); cam=re.search(r' cam=(\d+)',line).group(1)
    pl=(m0.group(6),m0.group(7),m0.group(8))
    for m in rx.finditer(line):
        key=(m.group(2),m.group(3),m.group(4),m.group(8))
        if key!=last:
            print(f"F{F} t={m.group(2)} st={m.group(3)} ss1={m.group(4)} ss2={m.group(5)} mo={m.group(8)} af={m.group(9)} @({m.group(10)},{m.group(11)},r{m.group(12)}) hp={m.group(13)} | PL hp={pl[0]} pst={pl[1]} ps1={pl[2]} cam={cam}")
            last=key
    if cam!=lastcam:
        print(f"F{F} CAM {lastcam}->{cam}"); lastcam=cam
    if pl!=lastpl and pl[0]!=(lastpl or ('',))[0]:
        print(f"F{F} PL hp {lastpl[0] if lastpl else None}->{pl[0]}")
    lastpl=pl
