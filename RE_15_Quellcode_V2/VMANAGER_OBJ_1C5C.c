void VMANAGER_OBJ_1C5C(uint param_1,uint param_2,uint param_3)

{
  byte bVar1;
  int iVar2;
  short in_t0;
  int in_t1;
  uint uVar3;
  
  if (param_1 < 0x40) {
    uVar3 = (param_3 & 0xffff) * param_1;
    param_3 = uVar3 >> 6;
    if ((int)uVar3 < 0) {
      VMANAGER_OBJ_1CA8(param_1,param_2,uVar3 + 0x3f >> 6);
      return;
    }
  }
  else {
    uVar3 = (param_2 & 0xffff) * (0x7f - param_1);
    param_2 = uVar3 >> 6;
    if ((int)uVar3 < 0) {
      param_2 = uVar3 + 0x3f >> 6;
    }
  }
  uVar3 = param_3;
  if ((DAT_800b2678 == 1) && (uVar3 = param_2, (param_2 & 0xffff) < (param_3 & 0xffff))) {
    VMANAGER_OBJ_1CDC(in_t1 << 0x10,param_3);
    return;
  }
  iVar2 = (in_t1 << 0x10) >> 0xf;
  *(short *)((int)&DAT_8008f6ae + iVar2) = (short)uVar3;
  bVar1 = (&DAT_8008f82c)[in_t0];
  *(short *)((int)&DAT_8008f6ac + iVar2) = (short)param_2;
  (&DAT_8008f82c)[in_t0] = bVar1 | 3;
  return;
}
