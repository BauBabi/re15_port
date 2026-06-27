void VMANAGER_OBJ_1C04(undefined4 param_1,uint param_2,uint param_3,uint param_4)

{
  byte bVar1;
  uint uVar2;
  int iVar3;
  short in_t0;
  int in_t1;
  uint uVar4;
  
  uVar2 = (uint)DAT_800b531f;
  if (uVar2 < 0x40) {
    VMANAGER_OBJ_1C5C(param_4,param_2,(param_3 & 0xffff) * uVar2 >> 6);
    return;
  }
  uVar2 = (param_2 & 0xffff) * (0x7f - uVar2);
  if ((int)uVar2 < 0) {
    uVar2 = uVar2 + 0x3f;
  }
  uVar2 = uVar2 >> 6;
  if (param_4 < 0x40) {
    uVar4 = (param_3 & 0xffff) * param_4;
    param_3 = uVar4 >> 6;
    if ((int)uVar4 < 0) {
      VMANAGER_OBJ_1CA8(param_4,uVar2,uVar4 + 0x3f >> 6);
      return;
    }
  }
  else {
    uVar4 = (uVar2 & 0xffff) * (0x7f - param_4);
    uVar2 = uVar4 >> 6;
    if ((int)uVar4 < 0) {
      uVar2 = uVar4 + 0x3f >> 6;
    }
  }
  uVar4 = param_3;
  if ((DAT_800b2678 == 1) && (uVar4 = uVar2, (uVar2 & 0xffff) < (param_3 & 0xffff))) {
    VMANAGER_OBJ_1CDC(in_t1 << 0x10,param_3);
    return;
  }
  iVar3 = (in_t1 << 0x10) >> 0xf;
  *(short *)((int)&DAT_8008f6ae + iVar3) = (short)uVar4;
  bVar1 = (&DAT_8008f82c)[in_t0];
  *(short *)((int)&DAT_8008f6ac + iVar3) = (short)uVar2;
  (&DAT_8008f82c)[in_t0] = bVar1 | 3;
  return;
}
