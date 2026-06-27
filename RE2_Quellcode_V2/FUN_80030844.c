/* FUN_80030844 @ 0x80030844  (Ghidra headless, raw MIPS overlay) */

void FUN_80030844(int param_1,byte *param_2,uint param_3,int param_4)

{
  bool bVar1;
  ushort uVar2;
  ushort uVar3;
  byte bVar4;
  int iVar5;
  int iVar6;
  undefined4 uVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  uint uVar12;
  uint uVar13;
  
  uVar12 = (uint)*param_2;
  uVar13 = (uint)param_2[2];
  uVar8 = (uint)param_2[1];
  uVar2 = *(ushort *)(param_1 + 0x5ca0);
  uVar11 = *(byte *)(param_1 + 0x5c8c) & 0xf;
  uVar3 = *(ushort *)(param_1 + 0x5ca2);
  if (*(char *)(param_1 + 0x5caa) != '\0') {
    *(char *)(param_1 + 0x5caa) = *(char *)(param_1 + 0x5caa) + -1;
  }
  if ((DAT_800ce310 & 0x1000) == 0) {
    if ((DAT_800ce310 & 0x800) != 0) {
      FUN_8005ba28(0x4040000,0);
      bVar1 = uVar12 - 1 == uVar11;
      uVar11 = uVar11 + 1;
      if (bVar1) {
        uVar11 = 0;
      }
      *(undefined1 *)(param_1 + 0x5c91) = 0;
    }
    if ((DAT_800ce310 & 0x400) != 0) {
      FUN_8005ba28(0x4040000,0);
      if (uVar11 == 0) {
        uVar11 = uVar12;
      }
      uVar11 = uVar11 - 1;
      *(undefined1 *)(param_1 + 0x5c91) = 0;
    }
  }
  else if (*(char *)(param_1 + 0x5caa) == '\0') {
    DAT_800ce30c = 0xffff;
    DAT_800cfbdc = *(undefined4 *)(param_1 + 0x5cac);
    *(byte *)(param_1 + 0x5c8c) = *(byte *)(param_1 + 0x5c8c) & 0x7f;
    if (((*(ushort *)(param_1 + 0x5ca8) & 0x2000) == 0) && ((param_3 & 0x60) == 0)) {
      return;
    }
    if ((*(byte *)(param_1 + 0x5c8c) & 0xf) == 0) {
      uVar7 = 0x4060000;
      if ((param_3 & 0x20) != 0) {
        return;
      }
    }
    else {
      uVar7 = 0x4050000;
      if ((*(byte *)(param_1 + 0x5c8c) & 0xf) != 1) {
        uVar7 = 0x4060000;
      }
    }
    FUN_8005ba28(uVar7,0);
    return;
  }
  bVar4 = *(char *)(param_1 + 0x5c91) - 1;
  *(byte *)(param_1 + 0x5c91) = bVar4;
  if (DAT_800dfc1a == '\0') {
    bVar4 = bVar4 & 0x30;
  }
  else {
    bVar4 = bVar4 & 0x18;
  }
  if (bVar4 != 0) {
    uVar10 = (uint)uVar3;
    uVar9 = (uint)uVar2;
    if ((param_3 & 0x80) == 0) {
      uVar10 = *(ushort *)(param_1 + 0x5ca6) + 0x10;
      uVar9 = uVar8;
    }
    FUN_8002fa28(uVar9 + uVar13 * uVar11 + -0xe,uVar10,0,0,&DAT_80010a40,0);
  }
  iVar6 = (uint)*(ushort *)(param_2 + 4) << 1;
  if ((param_3 & 0x80) == 0) {
    uVar10 = 0;
    if (uVar12 != 0) {
      do {
        iVar5 = iVar6 + param_4;
        iVar6 = iVar6 + 2;
        uVar10 = uVar10 + 1;
        FUN_800313a4(uVar8,*(ushort *)(param_1 + 0x5ca6) + 0x10,
                     &UNK_800a1f64 + *(ushort *)(&DAT_800a2040 + iVar5 * 2),param_4 << 0xe | 3);
        uVar8 = uVar8 + uVar13;
      } while (uVar10 < uVar12);
    }
  }
  else {
    uVar8 = 0;
    if (uVar12 != 0) {
      uVar10 = (uint)uVar2;
      do {
        iVar5 = iVar6 + param_4;
        iVar6 = iVar6 + 2;
        uVar8 = uVar8 + 1;
        FUN_800313a4(uVar10,(uint)uVar3,&UNK_800a1f64 + *(ushort *)(&DAT_800a2040 + iVar5 * 2),
                     param_4 << 0xe | 3);
        uVar10 = uVar10 + uVar13;
      } while (uVar8 < uVar12);
    }
  }
  *(byte *)(param_1 + 0x5c8c) = *(byte *)(param_1 + 0x5c8c) & 0xf0 | (byte)uVar11;
  return;
}


