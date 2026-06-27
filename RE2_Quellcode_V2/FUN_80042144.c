/* FUN_80042144 @ 0x80042144  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_80042144(int param_1,uint param_2)

{
  int iVar1;
  int iVar2;
  uint uVar3;
  undefined4 uVar4;
  uint *puVar5;
  undefined4 *puVar6;
  uint uVar7;
  uint uVar8;
  uint *puVar9;
  uint *puVar10;
  uint local_38;
  undefined4 local_34;
  uint local_30;
  
  puVar10 = (uint *)0x0;
  uVar8 = 0x7fffffff;
  puVar9 = (uint *)0x0;
  if (DAT_800cfbf3 == '\0') {
LAB_80042320:
    *(int *)(param_1 + 0x1b4) = param_1;
    uVar4 = 0;
  }
  else {
    puVar6 = &DAT_800cfe1c;
    uVar7 = 0x7fffffff;
    if (param_2 != 0) {
      uVar7 = param_2;
    }
    do {
      puVar5 = (uint *)*puVar6;
      if (((*puVar5 & 1) != 0) && ((*(ushort *)((int)puVar5 + 0x10e) & 0xc000) == 0)) {
        iVar1 = *(int *)(param_1 + 0x38) - puVar5[0xe];
        iVar2 = *(int *)(param_1 + 0x40) - puVar5[0x10];
        uVar3 = SquareRoot0(iVar1 * iVar1 + iVar2 * iVar2);
        local_38 = puVar5[0x21];
        local_34 = *(undefined4 *)
                    ((uint)*(byte *)((int)puVar5 + 0x1c1) * 0xac + puVar5[0x66] + 0x60);
        local_30 = puVar5[0x23];
        if ((-1 < *(short *)((int)puVar5 + 0x156)) &&
           (iVar1 = FUN_80050858(*(int *)(param_1 + 0x198) + (uint)*(byte *)(param_1 + 0x1c1) * 0xac
                                 + 0x5c,&local_38,0x80,1), iVar1 == 0)) {
          if ((*(ushort *)((int)puVar5 + 0x10e) & 0x2000) == 0) {
            if (uVar3 < uVar7) {
              uVar7 = uVar3;
              puVar10 = puVar5;
            }
          }
          else if (uVar3 < uVar8) {
            uVar8 = uVar3;
            puVar9 = puVar5;
          }
        }
      }
      puVar6 = puVar6 + 1;
    } while (puVar6 != DAT_800ce334);
    if (puVar10 == (uint *)0x0) {
      if (puVar9 == (uint *)0x0) goto LAB_80042320;
      *(uint **)(param_1 + 0x1b4) = puVar9;
    }
    else {
      *(uint **)(param_1 + 0x1b4) = puVar10;
    }
    uVar4 = 1;
  }
  return uVar4;
}


