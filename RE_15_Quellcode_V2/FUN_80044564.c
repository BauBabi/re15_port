undefined4 FUN_80044564(void)

{
  short sVar1;
  undefined4 uVar2;
  int iVar3;
  char cVar4;
  int *piVar5;
  int iVar6;
  uchar *puVar7;
  
  DAT_800bbdec = 0;
  if (-1 < DAT_800b52ae) {
    SsSetNck((short)DAT_800b52ae);
  }
  if (-1 < DAT_800b21f1) {
    SsVabClose((short)DAT_800b21f1);
    DAT_800b21f1 = -1;
  }
  if (*DAT_800b5570 == 0xff) {
    uVar2 = 0xffffffff;
  }
  else {
    do {
      do {
        iVar6 = DAT_800ac77c;
        iVar3 = FUN_80013b60(*(undefined2 *)(&DAT_8007498c + (*DAT_800b5570 & 0x3f) * 2),
                             DAT_800ac77c,1,"MAIN BGM");
        piVar5 = (int *)(iVar6 + iVar3 + -8);
        iVar6 = *piVar5;
        piVar5 = piVar5 + -1;
        puVar7 = (uchar *)(DAT_800ac77c + *piVar5);
        DAT_800b2588 = &DAT_801f5500 + *piVar5;
        DAT_800bbdec = (int)piVar5 + (0x10 - (int)puVar7) & 0xfffffff0;
        DAT_800b3f88 = &DAT_801f5500 + iVar6;
        FUN_8004ee38(&DAT_801f5500,DAT_800ac77c,*piVar5);
        sVar1 = SsVabOpenHeadSticky(&DAT_801f5500 + iVar6,5,0x42fc0);
        DAT_800b21f1 = (char)sVar1;
      } while (DAT_800b21f1 == -1);
      sVar1 = SsVabTransBody(puVar7,(short)DAT_800b21f1);
    } while (sVar1 == -1);
    do {
      FUN_80029ac8(1);
      sVar1 = SsVabTransCompleted(0);
    } while (sVar1 == 0);
    sVar1 = SsSeqOpen((ulong *)&DAT_801f5500,(short)DAT_800b21f1);
    cVar4 = (char)sVar1;
    DAT_800b52ae = cVar4;
    SsSeqSetVol((short)cVar4,0,0);
    SsSeqPlay((short)cVar4,'\0',1);
    uVar2 = 0;
  }
  return uVar2;
}
