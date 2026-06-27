/* FUN_80045c10 @ 0x80045c10  (Ghidra headless, raw MIPS overlay) */

void FUN_80045c10(int param_1)

{
  bool bVar1;
  int iVar2;
  uint uVar3;
  uint *puVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  int iVar8;
  undefined4 *puVar9;
  int iVar10;
  int iVar11;
  int iVar12;
  uint local_38;
  undefined4 local_34;
  uint local_30;
  
  iVar7 = 2;
  uVar5 = (uint)DAT_800cfbf3;
  uVar3 = 0;
  iVar12 = 0;
  if (uVar5 != 0) {
    uVar6 = uVar5;
    if ((*DAT_800cfe1c & 1) == 0) goto LAB_80045c88;
    uVar3 = 1;
    puVar4 = DAT_800cfe1c;
    while (uVar6 = uVar6 - 1, *(uint **)(param_1 + 0x1b4) != puVar4) {
LAB_80045c88:
      do {
        iVar7 = iVar7 + 1;
        if (uVar6 == 0) goto LAB_80045ccc;
        puVar4 = (uint *)(&DAT_800cfe14)[iVar7];
      } while ((*puVar4 & 1) == 0);
      uVar3 = uVar3 + 1;
    }
LAB_80045ccc:
    iVar8 = iVar7 + 1;
    if (uVar5 == uVar3) {
      iVar8 = 2;
    }
    puVar9 = &DAT_800cfe14 + iVar8;
    iVar10 = 0;
    do {
      puVar4 = (uint *)*puVar9;
      iVar11 = iVar10;
      if ((*puVar4 & 1) != 0) {
        local_38 = puVar4[0x21];
        local_34 = *(undefined4 *)((uint)*(byte *)(param_1 + 0x1c1) * 0xac + puVar4[0x66] + 0x60);
        local_30 = puVar4[0x23];
        uVar5 = uVar5 - 1;
        if (-1 < *(short *)((int)puVar4 + 0x156)) {
          iVar2 = FUN_80015614(param_1,puVar4[0xe],puVar4[0x10],0x400);
          if (((iVar2 == 0) &&
              (iVar2 = FUN_80050858(*(int *)(param_1 + 0x198) +
                                    (uint)*(byte *)(param_1 + 0x1c1) * 0xac + 0x5c,&local_38,0x80,1)
              , iVar2 == 0)) && ((*(ushort *)((int)puVar4 + 0x10e) & 0xc000) == 0)) {
            iVar11 = iVar8;
            iVar12 = iVar10;
          }
        }
      }
      bVar1 = puVar9 == DAT_800ce334;
      puVar9 = puVar9 + 1;
      if (bVar1) {
        puVar9 = &DAT_800cfe1c;
        iVar8 = 2;
      }
      else {
        iVar8 = iVar8 + 1;
      }
      iVar10 = iVar11;
    } while (uVar5 != 0);
    if (iVar11 != 0) {
      iVar8 = iVar11 << 2;
      if ((iVar7 == iVar11) && (iVar12 != 0)) {
        iVar8 = iVar12 << 2;
      }
      *(undefined4 *)(param_1 + 0x1b4) = *(undefined4 *)((int)&DAT_800cfe14 + iVar8);
      return;
    }
  }
  *(int *)(param_1 + 0x1b4) = param_1;
  return;
}


