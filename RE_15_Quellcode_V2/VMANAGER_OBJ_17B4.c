void VMANAGER_OBJ_17B4(undefined4 param_1,uint param_2,uint param_3,short param_4)

{
  byte bVar1;
  int iVar2;
  uint uVar3;
  int in_t0;
  
  uVar3 = param_3;
  if ((DAT_800b2678 == 1) && (uVar3 = param_2, (param_2 & 0xffff) < (param_3 & 0xffff))) {
    VMANAGER_OBJ_17E8(in_t0 << 0x10,param_3);
    return;
  }
  iVar2 = (in_t0 << 0x10) >> 0xf;
  *(short *)((int)&DAT_8008f6ae + iVar2) = (short)uVar3;
  bVar1 = (&DAT_8008f82c)[param_4];
  *(short *)((int)&DAT_8008f6ac + iVar2) = (short)param_2;
  (&DAT_8008f82c)[param_4] = bVar1 | 3;
  return;
}
