/* FUN_80036710 @ 0x80036710  (Ghidra headless, raw MIPS overlay) */

void FUN_80036710(int param_1)

{
  short sVar1;
  byte bVar2;
  short sVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  
  sVar1 = DAT_800dcba2;
  sVar3 = DAT_800dcb98;
  switch(*(undefined1 *)(param_1 + 4)) {
  case 0:
    goto switchD_80036744_caseD_0;
  case 1:
    DAT_800c3b14 = 0xd;
    DAT_800c3b10 = 0xd;
    DAT_800c3b1c = 8;
    DAT_800c3b24 = 0;
    DAT_800c3b18 = 0;
    DAT_800c3b2e = 0;
    DAT_800c3b20 = 0;
    DAT_800c3b34 = '\0';
    *(undefined1 *)(param_1 + 4) = 2;
    *(undefined1 *)(param_1 + 5) = 0;
  case 2:
    sVar1 = DAT_800dcba2;
    sVar3 = DAT_800dcb98;
    iVar4 = (int)DAT_800c3b10;
    iVar6 = (int)DAT_800dcba2;
    *(int *)(param_1 + 0x38) = *(int *)(param_1 + 0x38) + iVar4 * DAT_800dcb98;
    iVar5 = (int)DAT_800c3b24;
    *(int *)(param_1 + 0x40) = *(int *)(param_1 + 0x40) + iVar4 * iVar6;
    if (iVar5 < 0x400) {
      iVar5 = iVar5 + (uint)DAT_800c3b18;
      DAT_800c3b24 = (short)iVar5;
      if (0x400 < iVar5 * 0x10000 >> 0x10) {
        DAT_800c3b18 = (DAT_800c3b18 + 0x400) - DAT_800c3b24;
      }
      DAT_800c3b2c = DAT_800c3b18 * -sVar1;
      DAT_800c3b30 = sVar3 * DAT_800c3b18;
    }
    else {
      DAT_800c3b30 = 0;
      DAT_800c3b2c = 0;
    }
    DAT_800c3b18 = DAT_800c3b18 + DAT_800c3b1c;
    DAT_800c3b10 = DAT_800c3b10 + DAT_800c3b14;
    bVar2 = *(char *)(param_1 + 5) + 1;
    *(byte *)(param_1 + 5) = bVar2;
    if (bVar2 < 0xd) {
      *(int *)(param_1 + 0x3c) =
           *(int *)(param_1 + 0x3c) + (((int)DAT_800c3b10 / 3) * 0x10000 >> 0x10);
    }
    else {
      iVar4 = (int)DAT_800c3b20;
      DAT_800c3b14 = 0;
      DAT_800c3b20 = DAT_800c3b20 + 0xa0;
      *(int *)(param_1 + 0x3c) = *(int *)(param_1 + 0x3c) + iVar4;
      if (*(char *)(param_1 + 5) == '\r') {
        sVar3 = FUN_8004fba0(param_1 + 0x84,0x1c2,0x4000);
        DAT_800c3b28 = sVar3;
        *(short *)(param_1 + 0x44) = (short)*(undefined4 *)(param_1 + 0x38);
        *(short *)(param_1 + 0x48) = (short)*(undefined4 *)(param_1 + 0x40);
        *(char *)(param_1 + 0x106) =
             (char)(sVar3 >> 0xf) - ((char)((int)sVar3 / 0x708) + (char)(sVar3 >> 7));
      }
      if (((DAT_800c3b34 == '\0') && (*(short *)(param_1 + 0x10c) != 0)) &&
         ((int)*(short *)(param_1 + 0x10c) < *(int *)(param_1 + 0x3c))) {
        DAT_800c3b34 = '\x01';
      }
      if ((int)DAT_800c3b28 < *(int *)(param_1 + 0x3c)) {
        *(int *)(param_1 + 0x3c) = (int)DAT_800c3b28;
        *(undefined1 *)(param_1 + 4) = 3;
      }
    }
    break;
  case 3:
    iVar5 = (int)DAT_800dcb98;
    iVar4 = (int)DAT_800dcba2;
    *(int *)(param_1 + 0x3c) = *(int *)(param_1 + 0x3c) + -0x1e;
    *(int *)(param_1 + 0x38) = *(int *)(param_1 + 0x38) + iVar5 * 0x14;
    DAT_800c3b2c = sVar1 * -0x1e;
    *(int *)(param_1 + 0x40) = *(int *)(param_1 + 0x40) + iVar4 * 0x14;
    DAT_800c3b30 = sVar3 * 0x1e;
    *(char *)(param_1 + 4) = *(char *)(param_1 + 4) + '\x01';
    break;
  case 4:
    iVar5 = (int)DAT_800dcb98;
    iVar4 = (int)DAT_800dcba2;
    *(int *)(param_1 + 0x3c) = *(int *)(param_1 + 0x3c) + 0x1e;
    *(int *)(param_1 + 0x38) = *(int *)(param_1 + 0x38) + iVar5 * 0x14;
    DAT_800c3b2c = sVar1 * 0x1e;
    *(int *)(param_1 + 0x40) = *(int *)(param_1 + 0x40) + iVar4 * 0x14;
    DAT_800c3b30 = sVar3 * -0x1e;
    *(undefined1 *)(param_1 + 4) = 0;
  }
  FUN_8007746c(param_1 + 0x38,param_1 + 0x74,param_1 + 0x24,&DAT_800c3b2c);
switchD_80036744_caseD_0:
  return;
}


