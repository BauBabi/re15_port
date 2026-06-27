void SsSetTableSize(char *param_1,short param_2,short param_3)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  int iVar6;
  
  iVar6 = 0;
  DAT_800bbd84 = param_2;
  DAT_800bbd8c = param_3;
  if (0 < param_2) {
    piVar5 = &DAT_800bb500;
    iVar2 = 0;
    do {
      *piVar5 = (int)(param_1 + iVar2 * 0xac);
      piVar5 = piVar5 + 1;
      iVar6 = iVar6 + 1;
      iVar2 = iVar2 + param_3;
    } while (iVar6 < param_2);
  }
  iVar6 = 0;
  if (0 < DAT_800bbd84) {
    piVar5 = &DAT_800bb500;
    do {
      iVar2 = 0;
      if (0 < DAT_800bbd8c) {
        iVar4 = 0;
        do {
          *(undefined4 *)(iVar4 + *piVar5 + 0x90) = 0;
          *(undefined1 *)(iVar4 + *piVar5 + 0x3c) = 0xff;
          *(undefined1 *)(iVar4 + *piVar5) = 0;
          *(undefined2 *)(iVar4 + *piVar5 + 0x3e) = 0;
          *(undefined2 *)(iVar4 + *piVar5 + 0x40) = 0;
          *(undefined4 *)(iVar4 + *piVar5 + 0x94) = 0;
          *(undefined4 *)(iVar4 + *piVar5 + 0x98) = 0;
          *(undefined2 *)(iVar4 + *piVar5 + 0x42) = 0;
          *(undefined4 *)(iVar4 + *piVar5 + 0xa4) = 0;
          *(undefined4 *)(iVar4 + *piVar5 + 0xa0) = 0;
          *(undefined4 *)(iVar4 + *piVar5 + 0x9c) = 0;
          *(undefined2 *)(iVar4 + *piVar5 + 0x44) = 0;
          *(undefined2 *)(iVar4 + *piVar5 + 0x74) = 0x7f;
          *(undefined2 *)(iVar4 + *piVar5 + 0x76) = 0x7f;
          iVar2 = iVar2 + 1;
                    /* Possible PsyQ macro: setSprt16() + setSemiTrans(sprt16, 1) +
                       setShadeTex(sprt16, 1) */
          *(undefined2 *)(iVar4 + *piVar5 + 0x78) = 0x7f;
          iVar1 = (int)DAT_800bbd8c;
          iVar3 = iVar4 + *piVar5;
          iVar4 = iVar4 + 0xac;
          *(undefined2 *)(iVar3 + 0x7a) = 0x7f;
        } while (iVar2 < iVar1);
      }
      iVar6 = iVar6 + 1;
      piVar5 = piVar5 + 1;
    } while (iVar6 < DAT_800bbd84);
  }
  return;
}
