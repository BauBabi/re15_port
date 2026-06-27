void SeAutoPan(short param_1,ushort param_2,short param_3,short param_4)

{
  int iVar1;
  undefined2 uVar2;
  int iVar3;
  int iVar4;
  
  iVar3 = (int)(short)param_2;
  iVar4 = (int)param_3;
  if (iVar3 == iVar4) {
    return;
  }
  iVar1 = (int)param_1;
  (&DAT_8008f86c)[iVar1 * 0x1a] = 1;
  (&DAT_8008f874)[iVar1 * 0x1a] = param_2;
  *(short *)(&DAT_8008f876 + iVar1 * 0x34) = param_3;
  if (iVar3 - iVar4 < 0) {
    if ((int)param_4 <= iVar4 - iVar3) {
      iVar3 = (int)param_4;
      if (iVar3 == 0) {
        trap(0x1c00);
      }
      if ((iVar3 == -1) && ((int)(short)param_2 - (int)param_3 == -0x80000000)) {
        trap(0x1800);
      }
      (&DAT_8008f870)[param_1 * 0x1a] = 0;
      (&DAT_8008f86e)[param_1 * 0x1a] = (short)(((int)(short)param_2 - (int)param_3) / iVar3);
      return;
    }
  }
  else if ((int)param_4 <= iVar3 - iVar4) {
    VMANAGER_OBJ_1964((uint)param_2 << 0x10);
    return;
  }
  iVar3 = (int)(short)param_2 - (int)param_3;
  if (iVar3 == 0) {
    trap(0x1c00);
  }
  if ((iVar3 == -1) && (param_4 == -0x80000000)) {
    trap(0x1800);
  }
  iVar4 = (int)param_1;
  (&DAT_8008f86e)[iVar4 * 0x1a] = 1;
  uVar2 = (undefined2)((int)param_4 / iVar3);
  (&DAT_8008f870)[iVar4 * 0x1a] = uVar2;
  (&DAT_8008f872)[iVar4 * 0x1a] = uVar2;
  VMANAGER_OBJ_19DC();
  return;
}
