long RotAverage4(SVECTOR *$2,SVECTOR *$3,SVECTOR *v2,SVECTOR *v3,long *sxy0,long *sxy1,long *sxy2,
                long *sxy3,long *p,long *flag)

{
  long lVar1;
  uint uVar2;
  uint uVar3;
  long *r0;
  uint *puVar4;
  undefined1 auStackX_0 [16];
  
  gte_ldv3($2,$3,v2);
  gte_rtpt_b();
  gte_stsxy3(*(long **)((int)register0x00000074 + 0x10),*(long **)((int)register0x00000074 + 0x14),
             *(long **)((int)register0x00000074 + 0x18));
  uVar2 = gte_stFLAG();
  gte_ldv0(v3);
  gte_rtps_b();
  r0 = *(long **)((int)register0x00000074 + 0x20);
  puVar4 = *(uint **)((int)register0x00000074 + 0x24);
  gte_stsxy(*(long **)((int)register0x00000074 + 0x1c));
  uVar3 = gte_stFLAG();
  gte_stdp(r0);
  *puVar4 = uVar3 | uVar2;
  gte_avsz4_b();
  lVar1 = gte_stOTZ();
  return lVar1;
}
