void VMANAGER_OBJ_1704(undefined4 param_1,uint param_2,uint param_3,short param_4)

{
  byte bVar1;
  uint uVar2;
  int iVar3;
  int in_t0;
  
  uVar2 = (uint)DAT_800b531f;
  if (uVar2 < 0x40) {
    param_3 = (param_3 & 0xffff) * uVar2 >> 6;
  }
  else {
    uVar2 = (param_2 & 0xffff) * (0x7f - uVar2);
    param_2 = uVar2 >> 6;
    if ((int)uVar2 < 0) {
      param_2 = uVar2 + 0x3f >> 6;
    }
  }
  uVar2 = (uint)DAT_800b5319;
  if (uVar2 < 0x40) {
    param_3 = (param_3 & 0xffff) * uVar2 >> 6;
  }
  else {
    uVar2 = (param_2 & 0xffff) * (0x7f - uVar2);
    param_2 = uVar2 >> 6;
    if ((int)uVar2 < 0) {
      param_2 = uVar2 + 0x3f >> 6;
    }
  }
  uVar2 = param_3;
  if ((DAT_800b2678 == 1) && (uVar2 = param_2, (param_2 & 0xffff) < (param_3 & 0xffff))) {
    VMANAGER_OBJ_17E8(in_t0 << 0x10,param_3);
    return;
  }
  iVar3 = (in_t0 << 0x10) >> 0xf;
  *(short *)((int)&DAT_8008f6ae + iVar3) = (short)uVar2;
  bVar1 = (&DAT_8008f82c)[param_4];
  *(short *)((int)&DAT_8008f6ac + iVar3) = (short)param_2;
  (&DAT_8008f82c)[param_4] = bVar1 | 3;
  return;
}
