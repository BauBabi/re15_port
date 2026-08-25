#!/usr/bin/env python3
"""RE1.5-SCD-Disassembler (minimal).

ACHTUNG Namen: die Tabelle unten traegt ueberwiegend RE2-Labels. RE1.5 weicht bei
0x50/0x51/0x53/0x54/0x56/0x58 ab — dort steht der RE1.5-Name mit "(RE15)", abgeleitet
aus den Handler-Registrierungen in re15_port/engine/src/scd_vm.c:290-365.
Laengen aus re15_port/engine/src/scd_vm.c s_opcode_sizes[] (dort je Eintrag
gegen den Handler-PC-Advance auditiert).
RDT: main_scd_start = u32 @0x40, sub_scd_start = u32 @0x44 (rdt_common.c:238-239).
"""
import re, struct, sys, os
REPO = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))

def lens():
    src = open(os.path.join(REPO,'re15_port/engine/src/scd_vm.c'),encoding='utf-8',errors='replace').read()
    i = src.index('static const uint8_t s_opcode_sizes[256]')
    j = src.index('\n};', i)
    body = src[i:j]
    body = re.sub(r'/\*.*?\*/','',body,flags=re.S)
    L=[0]*256
    for a,b in re.findall(r'\[(0x[0-9A-Fa-f]+)\]\s*=\s*(0x[0-9A-Fa-f]+|\d+)',body):
        L[int(a,16)]=int(b,0)
    return L

NAMES={0x00:'Nop',0x01:'Evt_end',0x02:'Evt_next',0x03:'Evt_chain',0x04:'Evt_exec',
 0x05:'Evt_kill',0x06:'Ifel_ck',0x07:'Else_ck',0x08:'Endif',0x09:'Sleep',0x0A:'Sleeping',
 0x0B:'Wsleep',0x0C:'Wsleeping',0x0D:'For',0x0E:'Next',0x0F:'While',0x10:'Ewhile',
 0x11:'Do',0x12:'Edwhile',0x13:'Switch',0x14:'Case',0x15:'Default',0x16:'Eswitch',
 0x17:'Goto',0x18:'Gosub',0x19:'Return',0x1A:'Break',0x1B:'For2',0x1C:'Break_point',
 0x1D:'Work_copy',0x1E:'Nop1E',0x20:'Nop20',0x21:'Ck',0x22:'Set',0x23:'Cmp',0x24:'Save',
 0x25:'Copy',0x26:'Calc',0x27:'Calc2',0x28:'Sce_rnd',0x29:'Cut_chg',0x2A:'Cut_old',
 0x2B:'Message_on',0x2C:'Aot_set',0x2D:'Obj_model_set',0x2E:'Work_set',0x2F:'Speed_set',
 0x30:'Add_speed',0x31:'Add_aspeed',0x32:'Pos_set',0x33:'Dir_set',0x34:'Member_set',
 0x35:'Member_set2',0x36:'Se_on',0x37:'Sca_id_set',0x38:'Flr_set',0x39:'Dir_ck',
 0x3A:'Sce_espr_on',0x3B:'Door_aot_set',0x3C:'Cut_auto',0x3D:'Member_copy',0x3E:'Member_cmp',
 0x3F:'Plc_motion',0x40:'Plc_dest',0x41:'Plc_neck',0x42:'Plc_ret',0x43:'Plc_flg',
 0x44:'Sce_em_set',0x45:'Col_chg_set',0x46:'Aot_reset',0x47:'Aot_on',0x48:'Super_set',
 0x49:'Super_reset',0x4A:'Plc_gun',0x4B:'Cut_replace',0x4C:'Sce_espr_kill',0x4D:'Door_model_set',
 0x4E:'Item_aot_set',0x4F:'Sce_key_ck',0x50:'Item_aot_set(RE15)',0x51:'Sce_key_ck(RE15)',
 0x52:'Sce_espr_control',0x53:'Work_set_indirekt(RE15)',0x54:'Sce_bgm_control(RE15)',0x55:'Member_calc',
 0x56:'Fade_config(RE15)',0x57:'Keep_Item_ck',0x58:'Flag_ck2(RE15)',0x59:'Weapon_chg',0x5A:'Member_calc2',
 0x5B:'Item_lost',0x5C:'Sce_scr_move',0x5D:'Sce_item_get',0x5E:'Sce_line_start'}

def dis(path, which='sub', idx=None, out=sys.stdout):
    d=open(path,'rb').read()
    L=lens()
    main_s=struct.unpack_from('<I',d,0x40)[0]
    sub_s =struct.unpack_from('<I',d,0x44)[0]
    blocks=[]
    if which in ('main','all'):
        n=struct.unpack_from('<H',d,main_s)[0]
        blocks.append(('MAIN',main_s,[struct.unpack_from('<H',d,main_s+2*i)[0] for i in range(n//2)]))
    if which in ('sub','all'):
        n=struct.unpack_from('<H',d,sub_s)[0]
        blocks.append(('SUB',sub_s,[struct.unpack_from('<H',d,sub_s+2*i)[0] for i in range(n//2)]))
    for name,base,subs in blocks:
        print("== %s block base=0x%X  %d subs: %s"%(name,base,len(subs),[hex(base+o) for o in subs]),file=out)
        for i,o in enumerate(subs):
            if idx is not None and i!=idx: continue
            s=base+o
            e=base+subs[i+1] if i+1<len(subs) else len(d)
            print("-- %s sub%02d  file 0x%X..0x%X"%(name,i,s,e),file=out)
            pc=s
            while pc<e:
                op=d[pc]; n=L[op] or 1
                if op==0x2D: n=34
                args=d[pc+1:pc+n]
                print("  0x%05X  %02X %-16s %s"%(pc,op,NAMES.get(op,'?'),' '.join('%02X'%b for b in args)),file=out)
                pc+=n
                if op==0x01 and pc>=e-3: break

if __name__=='__main__':
    p=sys.argv[1]
    which=sys.argv[2] if len(sys.argv)>2 else 'all'
    idx=int(sys.argv[3]) if len(sys.argv)>3 else None
    dis(p,which,idx)
