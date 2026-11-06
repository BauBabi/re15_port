import struct
from PIL import Image
R = open(r"C:\workspace\git\reAi_v2\info\re2leon\PL0\RDT\ROOM7040.RDT", "rb").read()
t = R[0x33844:0x33844+34336]
cl = struct.unpack_from("<IHHHH", t, 8)
cw, ch = cl[3], cl[4]
clut = [struct.unpack_from("<H", t, 20 + 2*i)[0] for i in range(cw*ch)]
io = 8 + cl[0]
im = struct.unpack_from("<IHHHH", t, io)
w, h = im[3]*2, im[4]
px = t[io+12: io+12+w*h]
def c15(v):
    return ((v & 31) << 3, ((v >> 5) & 31) << 3, ((v >> 10) & 31) << 3, v >> 15)
out = Image.new("RGB", (w*3*2, h*2))
for row in range(ch):
    img = Image.new("RGB", (w, h))
    for y in range(h):
        for x in range(w):
            r,g,b,s = c15(clut[row*256 + px[y*w+x]])
            img.putpixel((x,y),(r,g,b))
    out.paste(img.resize((w*2,h*2), Image.NEAREST), (row*w*2, 0))
out.save("lampe_tim.png")
# STP / black stats for used area
used = set(px[y*w+x] for y in range(126) for x in range(126))
print("w,h", w, h, "clut rows", ch, "colors used", len(used))
print("clut row0 sample:", [hex(clut[i]) for i in sorted(used)[:20]])
