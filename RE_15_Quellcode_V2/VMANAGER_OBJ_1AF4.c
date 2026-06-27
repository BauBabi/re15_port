void VMANAGER_OBJ_1AF4(undefined2 param_1,int param_2)

{
  byte bVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  uint uVar6;
  short in_t0;
  int in_t1;
  
  *(undefined2 *)((int)&DAT_8008f874 + param_2) = param_1;
  *(undefined2 *)((int)&DAT_8008f86c + param_2) = 0;
  DAT_800b5319 = *(byte *)(&DAT_8008f874 + in_t0 * 0x1a);
  uVar6 = (uint)DAT_800b5319;
  uVar3 = (uint)DAT_800b5322;
  uVar2 = ((((uint)DAT_800b5318 * (uint)*(byte *)(DAT_800b2b34 + 0x18) * 0x3fff) / 0x3f01) *
           (uint)DAT_800b531e * (uint)DAT_800b5321) / 0x3f01;
  if (uVar3 < 0x40) {
    VMANAGER_OBJ_1C04(uVar3,uVar2,uVar2 * uVar3 >> 6);
    return;
  }
  uVar5 = uVar2 * (0x7f - uVar3) >> 6;
  uVar3 = (uint)DAT_800b531f;
  if (0x3f < uVar3) {
    uVar3 = (uVar5 & 0xffff) * (0x7f - uVar3);
    if ((int)uVar3 < 0) {
      uVar3 = uVar3 + 0x3f;
    }
    uVar3 = uVar3 >> 6;
    if (uVar6 < 0x40) {
      uVar2 = (uVar2 & 0xffff) * uVar6 >> 6;
    }
    else {
      uVar6 = (uVar3 & 0xffff) * (0x7f - uVar6);
      uVar3 = uVar6 >> 6;
      if ((int)uVar6 < 0) {
        uVar3 = uVar6 + 0x3f >> 6;
      }
    }
    uVar6 = uVar2;
    if ((DAT_800b2678 == 1) && (uVar6 = uVar3, (uVar3 & 0xffff) < (uVar2 & 0xffff))) {
      VMANAGER_OBJ_1CDC(in_t1 << 0x10,uVar2);
      return;
    }
    iVar4 = (in_t1 << 0x10) >> 0xf;
    *(short *)((int)&DAT_8008f6ae + iVar4) = (short)uVar6;
    bVar1 = (&DAT_8008f82c)[in_t0];
    *(short *)((int)&DAT_8008f6ac + iVar4) = (short)uVar3;
    (&DAT_8008f82c)[in_t0] = bVar1 | 3;
    return;
  }
  VMANAGER_OBJ_1C5C(uVar6,uVar5,(uVar2 & 0xffff) * uVar3 >> 6);
  return;
}
