void FUN_80044da4(uint param_1)

{
  char cVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;
  
  uVar4 = param_1 >> 0x1c;
  switch(param_1 >> 0x18 & 0xf) {
  case 1:
    SsSeqSetVol((short)(char)(&DAT_800b52ae)[uVar4 * 8],(&DAT_800b52b0)[uVar4 * 4],
                (&DAT_800b52b0)[uVar4 * 4]);
    SsSeqPlay((short)(char)(&DAT_800b52ae)[uVar4 * 8],'\x01',1);
    goto LAB_80044ed0;
  case 2:
    SsSeqStop((short)(char)(&DAT_800b52ae)[uVar4 * 8]);
    (&DAT_800b52ac)[uVar4 * 8] = 2;
    break;
  case 3:
    SsSeqSetVol((short)(char)(&DAT_800b52ae)[uVar4 * 8],(&DAT_800b52b0)[uVar4 * 4],
                (&DAT_800b52b0)[uVar4 * 4]);
    SsSeqReplay((short)(char)(&DAT_800b52ae)[uVar4 * 8]);
LAB_80044ed0:
    (&DAT_800b52ac)[uVar4 * 8] = 1;
    break;
  case 4:
    SsSeqPause((short)(char)(&DAT_800b52ae)[uVar4 * 8]);
    (&DAT_800b52ac)[uVar4 * 8] = 4;
    break;
  case 5:
    FUN_800449f4(0x3c);
  default:
  }
  if ((param_1 >> 8 & 0xff) != 0) {
    uVar2 = param_1 >> 0x10 & 0xff;
    iVar3 = DAT_800b3f88;
    if (uVar4 != 0) {
      iVar3 = DAT_800b3f8c;
    }
    cVar1 = (char)(param_1 >> 8);
    if (uVar2 == 0) {
      *(char *)(iVar3 + 0x18) = cVar1 + -1;
    }
    else {
      *(char *)(uVar2 * 0x10 + iVar3 + 0x11) = cVar1 + -1;
    }
  }
  if ((param_1 & 0xff) != 0) {
    uVar2 = param_1 >> 0x10 & 0xff;
    iVar3 = DAT_800b3f88;
    if (uVar4 != 0) {
      iVar3 = DAT_800b3f8c;
    }
    if (uVar2 == 0) {
      *(char *)(iVar3 + 0x19) = (char)param_1 + -1;
    }
    else {
      *(char *)(uVar2 * 0x10 + iVar3 + 0x14) = (char)param_1 + -1;
    }
  }
  return;
}
