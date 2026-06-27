void FUN_80044210(void)

{
  byte bVar1;
  byte bVar2;
  char cVar3;
  
  DAT_800b5570 = &UNK_80074828 + _DAT_800b0fe2 * 2 + (uint)(byte)(&DAT_800748fc)[DAT_800b0fe0] * 2;
  bVar1 = *DAT_800b5570;
  bVar2 = DAT_800b5570[1];
  if ((bVar1 & 0x3f) == (DAT_800b2b44 & 0x3f)) {
    if ((bVar2 & 0x3f) == (DAT_800b2b45 & 0x3f)) goto LAB_800443b0;
    if (DAT_800b52b4 != '\0') {
      SsSeqStop((short)DAT_800b52b6);
      DAT_800b52b4 = '\0';
    }
    if (DAT_800b52bc != '\0') {
      SsSeqStop((short)DAT_800b52be);
      DAT_800b52bc = '\0';
    }
    cVar3 = FUN_80044774();
  }
  else {
    if (DAT_800b5218 != '\0') {
      do {
        FUN_80029ac8(1);
      } while (DAT_800b5218 != '\0');
    }
    cVar3 = FUN_80044564();
    DAT_800b52ad = 0xff;
    if (cVar3 == '\0') {
      DAT_800b52ad = bVar1 >> 6;
    }
    cVar3 = FUN_80044774();
  }
  if (cVar3 == '\0') {
    DAT_800b52bd = bVar2 >> 7;
    DAT_800b52b5 = bVar2 >> 6 & 1;
  }
  else {
    DAT_800b52b5 = 0xff;
    DAT_800b52bd = 0xff;
  }
LAB_800443b0:
  DAT_800b2b44 = *DAT_800b5570;
  DAT_800b2b45 = DAT_800b5570[1];
  return;
}
