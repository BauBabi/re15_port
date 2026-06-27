long RotAverageNclip3(SVECTOR *$2,SVECTOR *$3,SVECTOR *v2,long *sxy0,long *sxy1,long *sxy2,long *p,
                     long *otz,long *flag)

{
  long lVar1;
  undefined4 uVar2;
  long *r0;
  undefined1 auStackX_0 [16];
  
  gte_ldv3($2,$3,v2);
  gte_rtpt_b();
  uVar2 = gte_stFLAG();
  **(undefined4 **)((int)register0x00000074 + 0x20) = uVar2;
  gte_nclip_b();
  r0 = *(long **)((int)register0x00000074 + 0x18);
  lVar1 = gte_stMAC0();
  if (0 < lVar1) {
    gte_stsxy3(sxy0,*(long **)((int)register0x00000074 + 0x10),
               *(long **)((int)register0x00000074 + 0x14));
    gte_stdp(r0);
    gte_avsz3_b();
    uVar2 = gte_stOTZ();
    **(undefined4 **)((int)register0x00000074 + 0x1c) = uVar2;
  }
  return lVar1;
}
