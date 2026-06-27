/* FUN_8005ce30 @ 0x8005ce30  (Ghidra headless, raw MIPS overlay) */

void FUN_8005ce30(int param_1)

{
  bool bVar1;
  short sVar2;
  int iVar3;
  char *pcVar4;
  char *pcVar5;
  int iVar6;
  
  pcVar4 = (char *)&DAT_800d75da;
  pcVar5 = &DAT_800d75d0;
  iVar6 = 0x18;
  do {
    pcVar4 = pcVar4 + -8;
    if (param_1 == 0) {
      if ((*pcVar5 == '\x01') && (sVar2 = SsIsEos((short)*pcVar4,0), sVar2 != 0)) {
        FUN_8007f018((int)*pcVar4,0,0);
        SsSeqPause((short)*pcVar4);
        *pcVar5 = '\x05';
      }
    }
    else if (*pcVar5 == '\x05') {
      iVar3 = (int)((((uint)*(ushort *)(&UNK_800d75bc + iVar6) * (uint)DAT_800eae3c) / 100) *
                   0x10000) >> 0x10;
      FUN_8007f018((int)*pcVar4,iVar3,iVar3);
      SsSeqReplay((short)*pcVar4);
      *pcVar5 = '\x01';
    }
    bVar1 = pcVar5 != &DAT_800d75c0;
    pcVar5 = pcVar5 + -8;
    iVar6 = iVar6 + -8;
  } while (bVar1);
  return;
}


