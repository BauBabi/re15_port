/* FUN_80065890 @ 0x80065890  (Ghidra headless, raw MIPS overlay) */

int FUN_80065890(undefined4 param_1,uint param_2)

{
  int iVar1;
  uint uVar2;
  int *piVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  ushort *puVar8;
  uint uVar9;
  
  uVar4 = 0;
  uVar7 = 0;
  iVar5 = 3;
  piVar3 = &DAT_800d7650;
  do {
    if (*piVar3 != 0) {
      uVar7 = uVar7 + 1;
    }
    iVar5 = iVar5 + -1;
    piVar3 = piVar3 + -1;
  } while (iVar5 != 0);
  uVar9 = 0;
  if (uVar7 != 0) {
    puVar8 = &DAT_800dfad8;
    piVar3 = &DAT_800d7648;
    uVar6 = uVar7;
    do {
      uVar6 = uVar6 - 1;
      iVar5 = *piVar3;
      if (*puVar8 < param_2) {
        iVar1 = FUN_80015614(param_1,*(undefined4 *)(iVar5 + 0x38),*(undefined4 *)(iVar5 + 0x40),
                             0x400);
        uVar2 = 0x20000;
        if (iVar1 == 0) {
          uVar2 = 0x10000;
        }
        uVar4 = uVar4 | uVar2;
        uVar9 = uVar9 + 1;
      }
      iVar5 = FUN_80015614(&DAT_800cfbf8,*(undefined4 *)(iVar5 + 0x38),*(undefined4 *)(iVar5 + 0x40)
                           ,0x400);
      if (iVar5 == 0) {
        uVar4 = uVar4 | 1;
      }
      else {
        uVar4 = uVar4 | 2;
      }
      puVar8 = puVar8 + 1;
      piVar3 = piVar3 + 1;
    } while (uVar6 != 0);
    if (uVar7 != 0) {
      if ((uVar4 & 0xffff0000) == 0) {
        if (uVar7 < 2) {
          return uVar4 * 2;
        }
        return uVar4 * 2 + 4;
      }
      if (1 < uVar9) {
        return (uVar4 >> 0xf) + 3;
      }
      return (uVar4 >> 0xf) - 1;
    }
  }
  return 0;
}


