import os; exec(open(os.path.join(os.path.dirname(os.path.abspath(__file__)),'esp_zeilen_zensus.py')).read().split("IMPL =")[0])
raw=open(os.path.join(ROOT,"STAGE2/ROOM2000.RDT"),"rb").read()
idh,pe,tb,te=u32(raw,0x4c),u32(raw,0x50),u32(raw,0x54),u32(raw,0x58)
for e in parse(raw,idh,pe,tb,te):
    if e['id'] not in (0x0b,0x06): continue
    print("id %02x start %#x end %#x bound %#x hdr clut %04x tpage %04x"%(e['id'],e['start'],e['end'],e['bound'],u16(raw,e['start']+4),u16(raw,e['start']+6)))
    for sub,s,r,row in rows_of(raw,e):
        print("  sub%d st%d r%d: A=%d B=%d w=%04x h=%04x fl0e=%02x t16=%04x fl1e=%02x t26=%04x | %s"%(sub,s,r,u16(row,0),u16(row,2),u16(row,4),u16(row,6),row[0x0e],u16(row,0x16),row[0x1e],u16(row,0x26),row.hex()))
