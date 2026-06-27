void FUN_800458d4(void)

{
  long lVar1;
  int iVar2;
  short *psVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  
  iVar6 = 8;
  psVar3 = (short *)&DAT_800b24b0;
  iVar5 = 0x180000;
  uVar4 = 0x17;
  iVar2 = 0x90;
  do {
    psVar3 = psVar3 + -9;
    iVar5 = iVar5 + -0x10000;
    iVar6 = iVar6 + -1;
    if (*psVar3 != 0) {
      SsUtKeyOnV((short)((uint)iVar5 >> 0x10),*(short *)(&UNK_800b2412 + iVar2),
                 *(short *)(&UNK_800b2414 + iVar2),*(short *)(&UNK_800b2416 + iVar2),
                 *(short *)(&UNK_800b2418 + iVar2),*(short *)(&UNK_800b241a + iVar2),
                 *(short *)(&UNK_800b241c + iVar2),*(short *)(&UNK_800b241e + iVar2));
      *psVar3 = 0;
    }
    lVar1 = SpuGetKeyStatus(1 << (uVar4 & 0x1f));
    if (lVar1 == 0) {
      (&DAT_800b22bc)[uVar4] = 0;
    }
    uVar4 = uVar4 - 1;
    iVar2 = iVar2 + -0x12;
  } while (iVar6 != 0);
  FUN_80044ab8();
  return;
}
