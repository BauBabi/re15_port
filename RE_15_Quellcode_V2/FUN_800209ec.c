void FUN_800209ec(int param_1,int param_2)

{
  undefined *puVar1;
  int iVar2;
  uint *in_t1;
  
  if (param_1 == 1) {
    puVar1 = &DAT_800aa6d8 + param_2 * 4;
    iVar2 = (uint)DAT_800aca34 << 0xc;
  }
  else if (param_1 == 0) {
    puVar1 = &UNK_800aa698 + param_2 * 4;
    iVar2 = (uint)DAT_800aca34 << 5;
  }
  else {
    if (param_1 != 2) goto LAB_80020a6c;
    puVar1 = &DAT_800ac6d8 + param_2 * 4;
    iVar2 = (uint)DAT_800aca34 << 6;
  }
  in_t1 = (uint *)(puVar1 + iVar2);
LAB_80020a6c:
  (&DAT_800b8284)[DAT_800aca34] = (&DAT_800b8284)[DAT_800aca34] & 0xff000000 | *in_t1 & 0xffffff;
  *in_t1 = *in_t1 & 0xff000000 | (uint)(&DAT_800b8284 + DAT_800aca34) & 0xffffff;
  DAT_800b557c = &DAT_800b8284 + DAT_800aca34;
  DAT_800b5580 = in_t1;
  DAT_800b5578 = (&DAT_800b8284)[DAT_800aca34];
  return;
}
