void FUN_80043958(void)

{
  char *pcVar1;
  char *pcVar2;
  uint uVar3;
  
  uVar3 = 0;
  pcVar1 = &DAT_800b52ae;
  pcVar2 = &DAT_800b52ac;
  DAT_800bbdec = 0;
  do {
    if (*pcVar2 != '\0') {
      SsSeqStop((short)*pcVar1);
      *pcVar2 = '\0';
    }
    pcVar2 = pcVar2 + 8;
    if (-1 < *pcVar1) {
      SsSetNck((short)*pcVar1);
      *pcVar1 = -1;
    }
    uVar3 = uVar3 + 1;
    pcVar1 = pcVar1 + 8;
  } while (uVar3 < 3);
  pcVar1 = &DAT_800b21ec;
  do {
    if (-1 < *pcVar1) {
      SsVabClose((short)*pcVar1);
      *pcVar1 = -1;
    }
    pcVar1 = pcVar1 + 1;
  } while (pcVar1 < "");
  return;
}
