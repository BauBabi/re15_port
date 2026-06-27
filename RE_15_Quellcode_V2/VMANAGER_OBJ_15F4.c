void VMANAGER_OBJ_15F4(undefined2 param_1,int param_2,undefined4 param_3,short param_4)

{
  byte bVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  int in_t0;
  
  *(undefined2 *)((int)&DAT_8008f868 + param_2) = param_1;
  *(undefined2 *)((int)&DAT_8008f860 + param_2) = 0;
  DAT_800b5318 = (undefined1)(&DAT_8008f868)[param_4 * 0x1a];
  uVar3 = (uint)DAT_800b5322;
  uVar2 = (((int)((int)(short)(&DAT_8008f868)[param_4 * 0x1a] *
                 (uint)*(byte *)(DAT_800b2b34 + 0x18) * 0x3fff) / 0x3f01) * (uint)DAT_800b531e *
          (uint)DAT_800b5321) / 0x3f01;
  if (uVar3 < 0x40) {
    VMANAGER_OBJ_1704(uVar3,uVar2,uVar2 * uVar3 >> 6);
    return;
  }
  uVar5 = uVar2 * (0x7f - uVar3) >> 6;
  uVar3 = (uint)DAT_800b531f;
  if (uVar3 < 0x40) {
    uVar2 = (uVar2 & 0xffff) * uVar3 >> 6;
  }
  else {
    uVar3 = (uVar5 & 0xffff) * (0x7f - uVar3);
    uVar5 = uVar3 >> 6;
    if ((int)uVar3 < 0) {
      uVar5 = uVar3 + 0x3f >> 6;
    }
  }
  uVar3 = (uint)DAT_800b5319;
  if (uVar3 < 0x40) {
    uVar2 = (uVar2 & 0xffff) * uVar3 >> 6;
  }
  else {
    uVar3 = (uVar5 & 0xffff) * (0x7f - uVar3);
    uVar5 = uVar3 >> 6;
    if ((int)uVar3 < 0) {
      uVar5 = uVar3 + 0x3f >> 6;
    }
  }
  uVar3 = uVar2;
  if ((DAT_800b2678 == 1) && (uVar3 = uVar5, (uVar5 & 0xffff) < (uVar2 & 0xffff))) {
    VMANAGER_OBJ_17E8(in_t0 << 0x10,uVar2);
    return;
  }
  iVar4 = (in_t0 << 0x10) >> 0xf;
  *(short *)((int)&DAT_8008f6ae + iVar4) = (short)uVar3;
  bVar1 = (&DAT_8008f82c)[param_4];
  *(short *)((int)&DAT_8008f6ac + iVar4) = (short)uVar5;
  (&DAT_8008f82c)[param_4] = bVar1 | 3;
  return;
}
