/* RotAverage3 @ 0x8008e0d4  (Ghidra headless, raw MIPS overlay) */

long RotAverage3(SVECTOR *_2,SVECTOR *_3,SVECTOR *v2,long *sxy0,long *sxy1,long *sxy2,long *p,
                long *flag)

{
  long lVar1;
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
  uVar2 = gte_stFLAG();
  gte_stdp(r0);
  *puVar3 = uVar2;
  gte_avsz3_b();
  lVar1 = gte_stOTZ();
  return lVar1;
}


