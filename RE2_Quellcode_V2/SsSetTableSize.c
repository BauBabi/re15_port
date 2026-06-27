/* SsSetTableSize @ 0x8007ec48  (Ghidra headless, raw MIPS overlay) */

void SsSetTableSize(char *param_1,short param_2,short param_3)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  int iVar6;
  
  iVar6 = 0;
  DAT_800ead72 = param_2;
  DAT_800ead78 = param_3;
  if (0 < param_2) {
    piVar5 = &DAT_800ea250;
    iVar2 = 0;
    do {
      *piVar5 = (int)(param_1 + iVar2 * 0xb0);
      piVar5 = piVar5 + 1;
      iVar6 = iVar6 + 1;
      iVar2 = iVar2 + param_3;
    } while (iVar6 < param_2);
  }
  iVar6 = 0;
  if (0 < DAT_800ead72) {
    piVar5 = &DAT_800ea250;
    do {
      iVar2 = 0;
      if (0 < DAT_800ead78) {
        iVar4 = 0;
        do {
          *(undefined4 *)(iVar4 + *piVar5 + 0x98) = 0;
          *(undefined1 *)(iVar4 + *piVar5 + 0x22) = 0xff;
          *(undefined1 *)(iVar4 + *piVar5 + 0x23) = 0;
          *(undefined2 *)(iVar4 + *piVar5 + 0x48) = 0;
          *(undefined2 *)(iVar4 + *piVar5 + 0x4a) = 0;
          *(undefined4 *)(iVar4 + *piVar5 + 0x9c) = 0;
          *(undefined4 *)(iVar4 + *piVar5 + 0xa0) = 0;
          *(undefined2 *)(iVar4 + *piVar5 + 0x4c) = 0;
          *(undefined4 *)(iVar4 + *piVar5 + 0xac) = 0;
          *(undefined4 *)(iVar4 + *piVar5 + 0xa8) = 0;
          *(undefined4 *)(iVar4 + *piVar5 + 0xa4) = 0;
          *(undefined2 *)(iVar4 + *piVar5 + 0x4e) = 0;
          *(undefined2 *)(iVar4 + *piVar5 + 0x58) = 0x7f;
          *(undefined2 *)(iVar4 + *piVar5 + 0x5a) = 0x7f;
          iVar2 = iVar2 + 1;
                    /* Possible PsyQ macro: setSprt16() + setSemiTrans(sprt16, 1) +
                       setShadeTex(sprt16, 1) */
          *(undefined2 *)(iVar4 + *piVar5 + 0x5c) = 0x7f;
          iVar1 = (int)DAT_800ead78;
          iVar3 = iVar4 + *piVar5;
          iVar4 = iVar4 + 0xb0;
          *(undefined2 *)(iVar3 + 0x5e) = 0x7f;
        } while (iVar2 < iVar1);
      }
      iVar6 = iVar6 + 1;
      piVar5 = piVar5 + 1;
    } while (iVar6 < DAT_800ead72);
  }
  return;
}


