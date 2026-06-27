void FUN_80045630(short param_1,ushort param_2)

{
  byte bVar1;
  undefined1 uVar2;
  byte bVar3;
  char cVar4;
  ushort uVar5;
  undefined4 uVar6;
  int iVar7;
  int iVar8;
  uint uVar9;
  uint uVar10;
  short sVar11;
  undefined4 local_30;
  undefined4 local_2c;
  undefined4 local_28;
  
  uVar5 = FUN_800437d4(*(undefined4 *)(DAT_800ac784 + 0x34),*(undefined4 *)(DAT_800ac784 + 0x3c));
  iVar8 = *(int *)(DAT_800ac784 + 0x188) + (uint)param_2 * 0xac;
  if ((uVar5 & 0x80) == 0) {
    iVar7 = 0x800;
    if (*(short *)(DAT_800ac784 + 0x88) == 0) goto LAB_80045718;
    uVar5 = uVar5 - 3;
    uVar6 = 0x6032000;
    iVar8 = iVar8 + -0x6c;
  }
  else {
    uVar6 = 0x6050d00;
    iVar8 = iVar8 + 0x40;
    iVar7 = (int)*(short *)(DAT_800ac784 + 0x6a);
  }
  local_28 = 0;
  local_2c = 100;
  local_30 = 0;
  FUN_80019700(uVar6,iVar7,iVar8,&local_30);
LAB_80045718:
  uVar5 = param_1 + (uVar5 & 0x7f);
  if (*(byte *)(DAT_800ac784 + 8) < 0x40) {
    iVar8 = *(int *)(DAT_800ac778 + 8);
    if ((DAT_800aca3c & 0x4000) != 0) {
      uVar5 = 9;
    }
  }
  else {
    iVar8 = *(int *)(DAT_800ac778 + 8);
    uVar5 = uVar5 - 8;
  }
  sVar11 = (short)DAT_800b21ee;
  iVar8 = iVar8 + (uint)uVar5 * 4;
  uVar9 = ((*(byte *)(iVar8 + 3) & 0x1f) - 0x10) + (uint)(param_2 != 4);
  bVar1 = *(byte *)(iVar8 + 2);
  uVar10 = *(byte *)(iVar8 + 1) & 0x7f;
  iVar7 = DAT_800b25a0 + uVar10 * 0x200 + 0x820;
  bVar3 = *(byte *)(iVar8 + 2) >> 4;
  uVar2 = *(undefined1 *)(DAT_800ac778 + 6);
  iVar8 = iVar7 + (uint)bVar3 * 0x20;
  *(undefined1 *)(iVar7 + 3) = uVar2;
  *(undefined1 *)(iVar8 + 1) = uVar2;
  cVar4 = FUN_80045a18(uVar9 & 0xff,bVar1 & 0xf);
  if (cVar4 == '\0') {
    (&DAT_800b22cc)[uVar9] = bVar1 & 7;
    FUN_80045a64(DAT_800ac784 + 0x34);
    iVar7 = uVar9 * 0x12;
    *(ushort *)(&DAT_800b2422 + iVar7) = uVar5;
    *(undefined2 *)(&DAT_800b2420 + iVar7) = 1;
    *(short *)(&DAT_800b2424 + iVar7) = sVar11;
    *(short *)(&DAT_800b2426 + iVar7) = (short)uVar10;
    *(ushort *)(&DAT_800b2428 + iVar7) = (ushort)bVar3;
    uVar5 = FUN_8001af20();
    *(ushort *)(&DAT_800b242a + iVar7) = (ushort)*(byte *)(iVar8 + 6) + (uVar5 & 2);
    *(ushort *)(&DAT_800b242c + iVar7) = (ushort)*(byte *)(iVar8 + 5);
    *(undefined2 *)(&DAT_800b242e + iVar7) = DAT_800b2824;
    *(undefined2 *)(&DAT_800b2430 + iVar7) = DAT_800b2826;
  }
  return;
}
