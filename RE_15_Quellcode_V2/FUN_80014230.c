void FUN_80014230(void)

{
  int iVar1;
  char *pcVar2;
  int iVar3;
  
  if (*(char *)(DAT_800ac794 + 0x16) == DAT_800afbb5) {
    pcVar2 = (char *)(DAT_800ac794 + 0x16);
    iVar3 = DAT_800ac794;
    do {
      iVar3 = iVar3 + 0x14;
      if (((pcVar2[-1] == -1) || (pcVar2[-1] == DAT_800acad6)) &&
         (iVar1 = FUN_80014368(&DAT_800aca88,iVar3), iVar1 != 0)) {
        FUN_800142f4(pcVar2[1]);
        return;
      }
      pcVar2 = pcVar2 + 0x14;
    } while (*pcVar2 == DAT_800afbb5);
  }
  return;
}
