/* RotTransPers3 @ 0x8008e074  (Ghidra headless, raw MIPS overlay) */

long RotTransPers3(SVECTOR *_2,SVECTOR *_3,SVECTOR *v2,long *sxy0,long *sxy1,long *sxy2,long *p,
                  long *flag)

{
  int iVar1;
  undefined4 uVar2;
  long *r0;
  undefined4 *puVar3;
  undefined1 auStackX_0 [16];
  
  gte_ldv3(_2,_3,v2);
  gte_rtpt_b();
  r0 = *(long **)((int)register0x00000074 + 0x18);
  puVar3 = *(undefined4 **)((int)register0x00000074 + 0x1c);
  gte_stsxy3(sxy0,*(long **)((int)register0x00000074 + 0x10),
             *(long **)((int)register0x00000074 + 0x14));
  gte_stdp(r0);
  uVar2 = gte_stFLAG();
  iVar1 = gte_stSZ3();
  *puVar3 = uVar2;
  return iVar1 >> 2;
}


