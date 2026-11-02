def rng(a0):
    v1=(a0>>7)&0xff; o=(a0+v1)&0xff; a0n=o|(v1<<8); return o, a0n
tab=[0x80101058,0x80101008,0x801010a8,0x801010a8,0x80101008,0x8010118c,0x8010113c,0x8010118c]
for i in range(16):
    E=0x800acc2c+i*0x1f4
    o,a=rng(E)
    if o&3: print("slot%2d E=%08x r=%02x -> keine Variante"%(i,E,o)); continue
    o2,a2=rng(a)
    print("slot%2d E=%08x r=%02x -> Variante idx %d = %08x"%(i,E,o,o2&7,tab[o2&7]))
