/* FUN_8005c040 @ 0x8005c040  (Ghidra headless, raw MIPS overlay) */

void FUN_8005c040(short param_1,short param_2,int param_3)

{
  byte bVar1;
  char cVar2;
  byte bVar3;
  undefined4 uVar4;
  uint uVar5;
  byte bVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int *piVar10;
  ushort uVar11;
  uint uVar12;
  int iVar13;
  undefined2 local_48;
  short local_46;
  undefined2 local_44;
  ushort local_40;
  int local_38;
  
  if (*(char *)(param_3 + 8) == '\r') {
    iVar13 = (int)DAT_800d4c4c;
    local_40 = 0xb;
    uVar12 = (uint)(param_2 != 4);
    local_38 = DAT_800d75b0;
    iVar8 = DAT_800dbb88;
  }
  else {
    local_40 = FUN_800528e8(*(undefined4 *)(param_3 + 0x38),*(undefined4 *)(param_3 + 0x40),
                            *(undefined1 *)(param_3 + 0x106));
    iVar13 = (int)DAT_800d4c4a;
    local_38 = DAT_800d75a8;
    iVar8 = DAT_800dbb80;
    if (*(byte *)(param_3 + 8) < 0x10) {
      uVar12 = (uint)(param_2 != 4);
    }
    else {
      uVar12 = 3;
      if (param_2 == 4) {
        uVar12 = 2;
      }
    }
  }
  iVar7 = DAT_800dbb80;
  if (*(short *)(param_3 + 0x10c) != 0) {
    local_40 = 0x1a;
    iVar9 = *(int *)(param_3 + 0x198);
    iVar13 = (int)DAT_800d4c4a;
    local_38 = DAT_800d75a8;
    local_46 = *(short *)(param_3 + 0x10c) - (short)*(undefined4 *)(iVar9 + 0x10c);
    local_44 = 0xa0;
    if (param_2 == 4) {
      local_44 = 0xff60;
    }
    iVar8 = iVar7;
    if (*(byte *)(param_3 + 8) == 0xf) {
      if (local_46 < 500) {
        if (param_1 == 0) {
          FUN_800395b8(1,0x82,100,0);
          FUN_800395b8(9,0x55,10,1);
        }
        if (param_1 == 1) {
          FUN_800395b8(3,0x32,100,0);
          FUN_800395b8(5,0x55,0,3);
        }
      }
      local_46 = local_46 + 100;
      if (param_1 == 0) {
        iVar7 = iVar9 + 0xf4;
        local_48 = 100;
        FUN_8001bf10(0x1a000e00,(int)DAT_800cfc6e,iVar7,&local_48);
        local_48 = 0;
        FUN_8001bf10(0x1a000c00,DAT_800cfc6e + 0x400,iVar7,&local_48);
        FUN_8001bf10(0x1a000c00,DAT_800cfc6e + 0xc00,iVar7,&local_48);
      }
      if (param_1 != 1) goto LAB_8005c43c;
      local_48 = 300;
      FUN_8001bf10(0x1a001200,(int)DAT_800cfc6e,iVar9 + 0xf4,&local_48);
      local_48 = 0;
      FUN_8001bf10(0x1a000e00,DAT_800cfc6e + 0x400,iVar9 + 0xf4,&local_48);
      uVar4 = 0x1a000e00;
    }
    else {
      if ((local_46 < 0x4b0) && (*(byte *)(param_3 + 8) < 0x10)) {
        if (param_1 == 0) {
          FUN_800395b8(1,0x82,100,0);
          FUN_800395b8(9,0x55,10,1);
        }
        if (param_1 == 1) {
          FUN_800395b8(3,0x32,100,0);
          FUN_800395b8(5,0x55,10,3);
        }
      }
      if (param_1 == 0) {
        iVar7 = iVar9 + 0xf4;
        local_48 = 200;
        FUN_8001bf10(0x1a000e00,(int)DAT_800cfc6e,iVar7,&local_48);
        local_48 = 0;
        FUN_8001bf10(0x1a000c00,DAT_800cfc6e + 0x400,iVar7,&local_48);
        FUN_8001bf10(0x1a000c00,DAT_800cfc6e + 0xc00,iVar7,&local_48);
      }
      if (param_1 != 1) goto LAB_8005c43c;
      local_48 = 600;
      FUN_8001bf10(0x1a001600,(int)DAT_800cfc6e,iVar9 + 0xf4,&local_48);
      local_48 = 0;
      FUN_8001bf10(0x1a001100,DAT_800cfc6e + 0x400,iVar9 + 0xf4,&local_48);
      uVar4 = 0x1a001100;
    }
    FUN_8001bf10(uVar4,DAT_800cfc6e + 0xc00,iVar9 + 0xf4,&local_48);
  }
LAB_8005c43c:
  if (*(int *)(param_3 + 0x1e4) == 0) {
    uVar11 = param_1 + (local_40 & 0x7f);
  }
  else {
    uVar11 = 6;
  }
  piVar10 = (int *)(iVar8 + (uint)uVar11 * 4);
  if ((*piVar10 != -1) && (iVar13 != -1)) {
    bVar6 = (*(byte *)((int)piVar10 + 2) & 0xf) - 1;
    cVar2 = FUN_8005c92c(uVar12,bVar6);
    if (cVar2 != '\0') {
      uVar12 = 0x17;
    }
    uVar5 = *(byte *)((int)piVar10 + 1) & 0x7f;
    bVar1 = *(byte *)((int)piVar10 + 2) >> 4;
    iVar8 = local_38 + uVar5 * 0x200 + (uint)bVar1 * 0x20 + 0x820;
    if (DAT_800c4410 == 0) {
      bVar3 = *(byte *)(iVar8 + 1) & 0xfb;
    }
    else {
      bVar3 = *(byte *)(iVar8 + 1) | DAT_800c4410;
    }
    *(byte *)(iVar8 + 1) = bVar3;
    iVar7 = uVar12 * 0x20;
    (&DAT_800d4ca0)[uVar12 * 2] = bVar6 & 7;
    (&DAT_800d4ca1)[uVar12 * 2] = (char)uVar12;
    (&DAT_800d4f19)[iVar7] = (char)uVar11;
    (&DAT_800d4f18)[iVar7] = 1;
    *(short *)(&DAT_800d4f1c + iVar7) = (short)iVar13;
    *(short *)(&DAT_800d4f1e + iVar7) = (short)uVar5;
    *(ushort *)(&DAT_800d4f20 + iVar7) = (ushort)bVar1;
    iVar13 = FUN_80015fe8();
    *(ushort *)(&DAT_800d4f22 + iVar7) =
         (ushort)*(byte *)(iVar8 + 6) + (short)iVar13 + (short)(iVar13 / 2) * -2;
    *(ushort *)(&DAT_800d4f24 + iVar7) = (ushort)*(byte *)(iVar8 + 5);
    *(ushort *)(&DAT_800d4f26 + iVar7) = (ushort)*(byte *)(iVar8 + 2);
    *(ushort *)(&DAT_800d4f28 + iVar7) = (ushort)*(byte *)(iVar8 + 2);
    *(undefined4 *)(&DAT_800d4f2c + iVar7) = *(undefined4 *)(param_3 + 0x38);
    *(undefined4 *)(&DAT_800d4f30 + iVar7) = *(undefined4 *)(param_3 + 0x3c);
    uVar4 = *(undefined4 *)(param_3 + 0x40);
    *(undefined2 *)(&DAT_800d4f2a + iVar7) = 1;
    *(undefined4 *)(&DAT_800d4f34 + iVar7) = uVar4;
  }
  return;
}


