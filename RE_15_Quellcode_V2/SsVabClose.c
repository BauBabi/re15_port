void SsVabClose(short param_1)

{
  int iVar1;
  
  if ((ushort)param_1 < 0x10) {
    iVar1 = (int)param_1;
    if ((&DAT_800b5334)[iVar1] == '\x01') {
      SpuFree(*(ulong *)(&DAT_800bbe08 + iVar1 * 4));
      (&DAT_800b5334)[iVar1] = 0;
      DAT_800bbe00 = DAT_800bbe00 + -1;
    }
  }
  return;
}
