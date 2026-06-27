void VMANAGER_OBJ_12CC(undefined2 param_1)

{
  undefined2 in_v0;
  undefined2 uVar1;
  uint unaff_s0;
  int unaff_s1;
  
  uVar1 = note2pitch2(param_1,in_v0);
  (&DAT_8008f6b0)[unaff_s1] = uVar1;
  (&DAT_8008f82c)[unaff_s0 & 0xff] = (&DAT_8008f82c)[unaff_s0 & 0xff] | 4;
  return;
}
