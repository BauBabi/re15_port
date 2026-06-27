short SsUtSetVVol(short param_1,short param_2,short param_3)

{
  byte bVar1;
  short sVar2;
  int iVar3;
  
  if (0x17 < (ushort)param_1) {
    sVar2 = VMANAGER_OBJ_4BC0();
    return sVar2;
  }
  iVar3 = (int)param_1;
  (&DAT_8008f6ae)[iVar3 * 8] = param_3 * 0x81;
  bVar1 = (&DAT_8008f82c)[iVar3];
  (&DAT_8008f6ac)[iVar3 * 8] = param_2 * 0x81;
  (&DAT_8008f82c)[iVar3] = bVar1 | 3;
  return 0;
}
