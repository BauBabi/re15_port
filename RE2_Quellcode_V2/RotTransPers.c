/* RotTransPers @ 0x8008e044  (Ghidra headless, raw MIPS overlay) */

long RotTransPers(SVECTOR *_2,long *sxy,long *p,long *flag)

{
  int iVar1;
  long lVar2;
  
  gte_ldv0(_2);
  gte_rtps_b();
  gte_stsxy(sxy);
  gte_stdp(p);
  lVar2 = gte_stFLAG();
  iVar1 = gte_stSZ3();
  *flag = lVar2;
  return iVar1 >> 2;
}


