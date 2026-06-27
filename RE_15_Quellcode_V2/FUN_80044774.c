undefined4 FUN_80044774(void)

{
  short sVar1;
  undefined4 uVar2;
  int iVar3;
  char cVar4;
  int iVar5;
  int iVar6;
  uchar *puVar7;
  
  if (-1 < DAT_800b52b6) {
    SsSetNck((short)DAT_800b52b6);
    DAT_800b52b6 = -1;
  }
  if (-1 < DAT_800b52be) {
    SsSetNck((short)DAT_800b52be);
    DAT_800b52be = -1;
  }
  if (-1 < DAT_800b21f2) {
    SsVabClose((short)DAT_800b21f2);
    DAT_800b21f2 = -1;
  }
  if (*(char *)(DAT_800b5570 + 1) == -1) {
    uVar2 = 0xffffffff;
  }
  else {
    do {
      do {
        iVar5 = DAT_800ac77c;
        iVar3 = FUN_80013b60(*(undefined2 *)
                              (&DAT_80074a0c + (*(byte *)(DAT_800b5570 + 1) & 0x3f) * 2),
                             DAT_800ac77c,1,"SUB BGM");
        iVar5 = iVar5 + iVar3;
        DAT_800b2588 = &DAT_801eed00;
        iVar3 = *(int *)(iVar5 + -8);
        iVar6 = *(int *)(iVar5 + -0xc);
        puVar7 = (uchar *)(DAT_800ac77c + *(int *)(iVar5 + -0x10));
        DAT_800b3f8c = &DAT_801eed00 + iVar6;
        FUN_8004ee38(&DAT_801eed00,DAT_800ac77c);
        sVar1 = SsVabOpenHeadSticky(&DAT_801eed00 + iVar6,6,DAT_800bbdec + 0x42fc0);
        DAT_800b21f2 = (char)sVar1;
      } while (DAT_800b21f2 == -1);
      sVar1 = SsVabTransBody(puVar7,(short)DAT_800b21f2);
    } while (sVar1 == -1);
    do {
      FUN_80029ac8(1);
      sVar1 = SsVabTransCompleted(0);
    } while (sVar1 == 0);
    sVar1 = SsSeqOpen((ulong *)&DAT_801eed00,(short)DAT_800b21f2);
    cVar4 = (char)sVar1;
    DAT_800b52b6 = cVar4;
    SsSeqSetVol((short)cVar4,0,0);
    SsSeqPlay((short)cVar4,'\0',1);
    sVar1 = SsSeqOpen((ulong *)(&DAT_801eed00 + iVar3),(short)DAT_800b21f2);
    cVar4 = (char)sVar1;
    DAT_800b52be = cVar4;
    SsSeqSetVol((short)cVar4,0,0);
    SsSeqPlay((short)cVar4,'\0',1);
    uVar2 = 0;
  }
  return uVar2;
}
