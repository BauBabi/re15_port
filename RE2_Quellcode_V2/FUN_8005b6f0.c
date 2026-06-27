/* FUN_8005b6f0 @ 0x8005b6f0  (Ghidra headless, raw MIPS overlay) */

void FUN_8005b6f0(uint param_1)

{
  undefined1 uVar1;
  short sVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  char cVar6;
  
  if ((DAT_800cfb74 & 0x2000) != 0) {
    return;
  }
  uVar4 = param_1 >> 0x1c;
  if ((&DAT_800d75c1)[uVar4 * 8] == -1) {
    return;
  }
  if ((&DAT_800d75c2)[uVar4 * 8] == -1) {
    return;
  }
  iVar5 = DAT_800d75b4;
  if (uVar4 != 0) {
    iVar5 = DAT_800d75b8;
  }
  switch(param_1 >> 0x18 & 0xf) {
  default:
    goto switchD_8005b7a8_caseD_0;
  case 1:
    iVar3 = (int)((((uint)(ushort)(&DAT_800d75c4)[uVar4 * 4] * (uint)DAT_800eae3c) / 100) * 0x10000)
            >> 0x10;
    FUN_8007f018((int)(char)(&DAT_800d75c2)[uVar4 * 8],iVar3,iVar3);
    SsSeqPlay((short)(char)(&DAT_800d75c2)[uVar4 * 8],'\x01',1);
    uVar1 = 1;
    break;
  case 2:
    iVar3 = uVar4 * 8;
    if ((&DAT_800d75c0)[iVar3] == '\0') goto LAB_8005b978;
    sVar2 = SsIsEos((short)(char)(&DAT_800d75c2)[iVar3],0);
    if (sVar2 != 0) {
      FUN_8007f018((int)(char)(&DAT_800d75c2)[iVar3],0,0);
      FUN_8007eaf8((int)(char)(&DAT_800d75c2)[iVar3]);
    }
    uVar1 = 2;
    break;
  case 3:
    iVar3 = (int)((((uint)(ushort)(&DAT_800d75c4)[uVar4 * 4] * (uint)DAT_800eae3c) / 100) * 0x10000)
            >> 0x10;
    FUN_8007f018((int)(char)(&DAT_800d75c2)[uVar4 * 8],iVar3,iVar3);
    SsSeqReplay((short)(char)(&DAT_800d75c2)[uVar4 * 8]);
    uVar1 = 1;
    break;
  case 4:
    FUN_8007f018((int)(char)(&DAT_800d75c2)[uVar4 * 8],0,0);
    SsSeqPause((short)(char)(&DAT_800d75c2)[uVar4 * 8]);
    uVar1 = 4;
    break;
  case 5:
    FUN_8007a120((int)(char)(&DAT_800d75c2)[uVar4 * 8],0x7f,0x5a);
    uVar1 = 0x32;
  }
  (&DAT_800d75c0)[uVar4 * 8] = uVar1;
switchD_8005b7a8_caseD_0:
LAB_8005b978:
  cVar6 = (char)(param_1 >> 8) + -1;
  if ((param_1 >> 8 & 0xff) != 0) {
    uVar4 = param_1 >> 0x10 & 0xff;
    if (uVar4 == 0) {
      *(char *)(iVar5 + 0x18) = cVar6;
    }
    else {
      *(char *)(uVar4 * 0x10 + iVar5 + 0x11) = cVar6;
    }
  }
  uVar4 = param_1 >> 0x10 & 0xff;
  if ((param_1 & 0xff) != 0) {
    if (uVar4 == 0) {
      *(char *)(iVar5 + 0x19) = (char)param_1 + -1;
    }
    else {
      *(char *)(uVar4 * 0x10 + iVar5 + 0x14) = (char)param_1 + -1;
    }
  }
  return;
}


