short SsUtChangeADSR(short param_1,short param_2,short param_3,short param_4,ushort param_5,
                    ushort param_6)

{
  short sVar1;
  int iVar2;
  
  if ((ushort)param_1 < 0x18) {
    iVar2 = (int)param_1;
    if ((((&DAT_8008f85a)[iVar2 * 0x1a] == param_2) && ((&DAT_8008f856)[iVar2 * 0x1a] == param_3))
       && ((&DAT_8008f850)[iVar2 * 0x1a] == param_4)) {
      (&DAT_8008f6b4)[iVar2 * 8] = param_5;
      (&DAT_8008f6b6)[iVar2 * 8] = param_6;
      (&DAT_8008f82c)[iVar2] = (&DAT_8008f82c)[iVar2] | 0x30;
      sVar1 = VMANAGER_OBJ_49E4();
      return sVar1;
    }
  }
  return -1;
}
