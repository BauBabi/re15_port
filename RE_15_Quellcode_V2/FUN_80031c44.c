void FUN_80031c44(void)

{
  int iVar1;
  
  DAT_800ac784 = &DAT_800aca54;
  DAT_800b0fec = DAT_800acaee;
  if (-1 < DAT_800aca40) {
    DAT_800aca52 = DAT_800aca52 & 0xfffe;
    (*(code *)(&PTR_LAB_80073f90)[DAT_800aca58])();
    FUN_8002b544();
    DAT_800ac788 = 0;
    DAT_800aca3c = DAT_800aca3c & 0xffffbfff;
    iVar1 = FUN_8002d100(&DAT_800aca54,0x12);
    if ((iVar1 != 0) && (*(byte *)(iVar1 + 0x82) + 1 == (uint)DAT_800acad6)) {
      DAT_800aca3c = DAT_800aca3c | 0x4000;
      DAT_800ac788 = iVar1;
    }
    FUN_8002dc48(&DAT_800aca54);
    if ((DAT_800aca3c & 0x4000) == 0) {
      FUN_8002b498(&DAT_800aca54);
      FUN_8003b0a4(&DAT_800aca88,*(undefined2 *)(DAT_800acacc + 6),1);
    }
    FUN_80037358();
    if ((DAT_800aca5c & 4) != 0) {
      FUN_80024c30(&DAT_800aca54);
    }
    DAT_800acadc = 0;
  }
  if ((DAT_800aca54 & 0x400) == 0) {
    FUN_8001b064(&DAT_800acb04,(int)DAT_800acc0e);
  }
  return;
}
