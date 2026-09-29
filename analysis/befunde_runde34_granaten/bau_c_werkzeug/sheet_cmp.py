import struct
def rgb(v):
    if v == 0: return None
    return ((v&31)<<3, ((v>>5)&31)<<3, ((v>>10)&31)<<3)
tex = open('PSX/DATA/TEX.TIM','rb').read()
ln,cx,cy,cw,ch = struct.unpack_from('<IHHHH',tex,8)
clut_off = 8+12
ib = 8+ln
bl,ix,iy,iw,ih = struct.unpack_from('<IHHHH',tex,ib)
img_off = ib+12
def tex_hw(hx, hy): return struct.unpack_from('<H',tex,img_off+(hy*iw+hx)*2)[0]
def clut_row(y): return [struct.unpack_from('<H',tex,clut_off+((y-cy)*cw+16+i)*2)[0] for i in range(16)]
def page_idx(tpage):
    tx = (tpage & 0xF)*64 ; col0 = tx - 704  # VRAM x -> TEX hw col (blit dx 704)
    out = [[0]*256 for _ in range(256)]; hwb = [[0]*256 for _ in range(256)]
    for y in range(256):
        for x in range(256):
            hw = tex_hw(col0 + x//4, y)
            out[y][x] = (hw >> (4*(x%4))) & 0xF
            hwb[y][x] = hw
    return out, hwb
def load_tim(p):
    d = open(p,'rb').read()
    m,fl = struct.unpack_from('<II',d,0); off=8; clut=None
    if fl & 8:
        b,ccx,ccy,ccw,cch = struct.unpack_from('<IHHHH',d,off); clut=[struct.unpack_from('<H',d,off+12+i*2)[0] for i in range(ccw*cch)]; off+=b
    b,x,y,w,h = struct.unpack_from('<IHHHH',d,off)
    px = d[off+12:off+12+w*h*2]
    return fl&3, clut, w, h, px
for name, tpage, clutw in [('effect0_blood',0x1f,0x7951),('effect2_muzzle',0x1f,0x7a51),('effect3_smoke',0x1e,0x7811),('effect4_shell',0x1f,0x7ad1),('effect8_fire',0x1e,0x7911)]:
    idx, hwb = page_idx(tpage)
    row = clut_row(clutw>>6)
    bpp, clut, w, h, px = load_tim('extracted_fx/%s.tim'%name)
    diff=0; diff_bit15=0; diff_other=0; samples=[]
    for y in range(256):
        for x in range(256):
            new = rgb(row[idx[y][x]])
            if bpp == 2:
                v = struct.unpack_from('<H',px,(y*w+x)*2)[0]; old = rgb(v)
            else:
                hw = struct.unpack_from('<H',px,(y*w + x//4)*2)[0]; oi=(hw>>(4*(x%4)))&0xF; old = rgb(clut[oi])
            if old != new:
                diff += 1
                if (hwb[y][x] & 0x8000) and (x%4)==3: diff_bit15 += 1
                else:
                    diff_other += 1
                    if len(samples)<5: samples.append((x,y,old,new,hex(hwb[y][x])))
    print('%-15s tpage=0x%02x clut=0x%04x(row %d): Texel-Abweichungen %d, davon Bit15/Nibble3 %d, andere %d %s' % (name,tpage,clutw,clutw>>6,diff,diff_bit15,diff_other,samples))
