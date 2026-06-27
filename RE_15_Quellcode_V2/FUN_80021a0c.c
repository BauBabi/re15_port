void FUN_80021a0c(void)

{
  byte bVar1;
  int iVar2;
  
  if ((DAT_800aca3c & 0x10) == 0) {
    if (0xf < DAT_800b5568) {
      DAT_800b5568 = DAT_800b5568 - 0x10;
    }
  }
  else if (DAT_800b5568 < 0xf0) {
    DAT_800b5568 = DAT_800b5568 + 0x10;
  }
  bVar1 = DAT_800b5568;
  iVar2 = (uint)DAT_800aca34 * 0x18;
  (&DAT_80072ed0)[iVar2] = DAT_800b5568;
  (&DAT_80072ed1)[iVar2] = bVar1;
  (&DAT_80072ed2)[iVar2] = bVar1;
  bVar1 = DAT_800b5568;
  iVar2 = (uint)DAT_800aca34 * 0x18;
  (&DAT_80072f00)[iVar2] = DAT_800b5568;
  (&DAT_80072f01)[iVar2] = bVar1;
  (&DAT_80072f02)[iVar2] = bVar1;
  if (((DAT_800aca38 & 0x4000) == 0) && (DAT_800b5568 != 0)) {
    AddPrim(&UNK_800aa6a8 + (uint)DAT_800aca34 * 0x20,&DAT_80072ecc + (uint)DAT_800aca34 * 0x18);
    AddPrim(&UNK_800aa6a8 + (uint)DAT_800aca34 * 0x20,&DAT_80072efc + (uint)DAT_800aca34 * 0x18);
    AddPrim(&UNK_800aa6a8 + (uint)DAT_800aca34 * 0x20,&DAT_8008f644 + (uint)DAT_800aca34 * 0xc);
  }
  return;
}
