/* _SsClose @ 0x80079e34  (Ghidra headless, raw MIPS overlay) */

void _SsClose(short param_1)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int *piVar4;
  int iVar5;
  uint uVar6;
  
  uVar6 = (uint)param_1;
  FUN_800840ac(uVar6,0,0,1);
  FUN_800846dc(uVar6);
  DAT_800dcbd4 = DAT_800dcbd4 & ~(1 << (uVar6 & 0x1f));
  iVar5 = 0;
  if (0 < DAT_800ead78) {
    piVar4 = &DAT_800ea250 + uVar6;
    iVar3 = 0;
    do {
      *(undefined4 *)(iVar3 + *piVar4 + 0x98) = 0;
      *(undefined1 *)(iVar3 + *piVar4 + 0x22) = 0xff;
      *(undefined1 *)(iVar3 + *piVar4 + 0x23) = 0;
      *(undefined2 *)(iVar3 + *piVar4 + 0x48) = 0;
      *(undefined2 *)(iVar3 + *piVar4 + 0x4a) = 0;
      *(undefined4 *)(iVar3 + *piVar4 + 0x9c) = 0;
      *(undefined4 *)(iVar3 + *piVar4 + 0xa0) = 0;
      *(undefined2 *)(iVar3 + *piVar4 + 0x4c) = 0;
      *(undefined4 *)(iVar3 + *piVar4 + 0xac) = 0;
      *(undefined4 *)(iVar3 + *piVar4 + 0xa8) = 0;
      *(undefined4 *)(iVar3 + *piVar4 + 0xa4) = 0;
      *(undefined2 *)(iVar3 + *piVar4 + 0x4e) = 0;
      iVar5 = iVar5 + 1;
                    /* Possible PsyQ macro: setSprt16() + setSemiTrans(sprt16, 1) +
                       setShadeTex(sprt16, 1) */
      *(undefined2 *)(iVar3 + *piVar4 + 0x58) = 0x7f;
      iVar1 = (int)DAT_800ead78;
      iVar2 = iVar3 + *piVar4;
      iVar3 = iVar3 + 0xb0;
      *(undefined2 *)(iVar2 + 0x5a) = 0x7f;
    } while (iVar5 < iVar1);
  }
  return;
}


