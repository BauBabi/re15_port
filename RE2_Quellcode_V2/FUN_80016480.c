/* FUN_80016480 @ 0x80016480  (Ghidra headless, raw MIPS overlay) */

void FUN_80016480(undefined4 *param_1,undefined4 param_2,undefined4 param_3,uint param_4,
                 undefined4 param_5)

{
  char *pcVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  undefined4 *puVar6;
  
  puVar5 = (undefined4 *)&UNK_800ce698;
  iVar4 = 0x32;
  if (DAT_800ce6a6 != '\0') {
    iVar2 = 0x31;
    puVar6 = puVar5;
    do {
      iVar4 = iVar2;
      puVar5 = puVar6 + 0x1a;
      if (iVar4 == 0) goto LAB_800164c8;
      pcVar1 = (char *)((int)puVar6 + 0x76);
      iVar2 = iVar4 + -1;
      puVar6 = puVar5;
    } while (*pcVar1 != '\0');
  }
  if (iVar4 == 0) {
LAB_800164c8:
    *param_1 = 0;
  }
  else {
    *param_1 = puVar5;
    *(undefined1 *)((int)puVar5 + 0xe) = 5;
    puVar5[5] = param_5;
    if (param_4 == 0) {
      uVar3 = puVar5[7] & 0xff000000 | 0x808080;
      param_4 = puVar5[0x11] & 0xff000000 | 0x808080;
    }
    else {
      uVar3 = puVar5[7] & 0xff000000 | param_4;
      param_4 = puVar5[0x11] & 0xff000000 | param_4;
    }
    puVar5[7] = uVar3;
    puVar5[0x11] = param_4;
    *puVar5 = param_2;
    puVar5[1] = param_3;
  }
  return;
}


