# Emulation der Instruktionen FUN_8002da4c @0x8002da8c-0x8002dac0 (32-bit, vorzeichenlose sltu)
M = 0xFFFFFFFF
def hit(dx, dz, hx, hz, m):
    t1 = m & M
    v0 = (hx + t1) & M            # 8002da8c lhu v0,6(a2) ; 8002da94 addu v0,v0,t1
    a1 = (dx + v0) & M            # 8002da98 addu a1,a1,v0
    v0s = (v0 << 1) & M           # 8002da9c sll v0,v0,1
    if v0s < a1: return 0         # 8002daa0 sltu ; 8002daa4 beq -> sonst 8002daac j / addu v0,zero,zero
    a2 = (hz + t1) & M            # 8002daa8 addu a2,v1,t1 (Delay-Slot)
    v1 = (dz + a2) & M            # 8002dab4 addu v1,a0,a2
    v0s = (a2 << 1) & M           # 8002dab8 sll v0,a2,1
    return 0 if v0s < v1 else 1   # 8002dabc sltu ; 8002dac0 xori 1
for dx, dz in [(0,0),(0,300),(300,0),(111,111),(112,112),(300,300),(-500,800),(900,-900),(-111,500),(-112,500),(112,-111)]:
    print("h=0 m=-112 dx=%5d dz=%5d -> %d" % (dx, dz, hit(dx, dz, 0, 0, -112)))
# Grenzen per Brute force
xs = [d for d in range(-2000, 2001) if hit(d, 5000, 0, 0, -112)]
print("X-Bedingung erfuellt fuer dx in", [d for d in range(-300,301) if not hit(d, 5000, 0, 0, -112)][0], "..", [d for d in range(-300,301) if not hit(d, 5000, 0, 0, -112)][-1], "NICHT")
# Vergleich mit Box: h=675, m=-112 (normaler Tritt)
print("h=675 m=-112: dx 562 ->", hit(562,0,675,600,-112), " dx 563 ->", hit(563,0,675,600,-112), " dx -563 ->", hit(-563,0,675,600,-112), " dx -564 ->", hit(-564,0,675,600,-112))
print("h=0 m=18 (Standtest 0x12): dx 18 ->", hit(18,0,0,0,18), " dx 19 ->", hit(19,0,0,0,18), " dx -18 ->", hit(-18,0,0,0,18), " dx -19 ->", hit(-19,0,0,0,18))
print("h=0 m=200: dx 200 ->", hit(200,0,0,0,200), " dx 201 ->", hit(201,0,0,0,200))
print("(-112,-112) ->", hit(-112, -112, 0, 0, -112))
# 1D-Brute-force je Achse ueber [-30000, 30000] (Raumkoordinaten s16): geschlossene Form |d| >= 112
badx = [d for d in range(-30000, 30001) if hit(d, 5000, 0, 0, -112) != (1 if abs(d) >= 112 else 0)]
badz = [d for d in range(-30000, 30001) if hit(5000, d, 0, 0, -112) != (1 if abs(d) >= 112 else 0)]
print("Abweichungen geschlossene Form X:", len(badx), " Z:", len(badz))
