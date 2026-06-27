void FUN_800160c4(void)

{
  undefined *p;
  int iVar1;
  
  iVar1 = 0;
  p = &DAT_800b898c;
  do {
    iVar1 = iVar1 + 1;
    AddPrim(&DAT_800ac6d8 + (uint)DAT_800aca34 * 0x40,p);
    p = p + 0x48;
  } while (iVar1 < 4);
  return;
}
