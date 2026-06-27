void FUN_8003ee3c(uint param_1)

{
  char *pcVar1;
  int iVar2;
  undefined4 *puVar3;
  
  if (DAT_800b3f7a == '\0') {
    iVar2 = param_1 << 1;
    if (param_1 < 10) goto LAB_8003eef8;
    param_1 = 2;
  }
  else {
    iVar2 = param_1 << 1;
    if (param_1 < 0xe) goto LAB_8003eef8;
    param_1 = 10;
    if (DAT_800b39ad != '\0') {
      iVar2 = 0xe60;
      do {
        if (param_1 == 0xd) break;
        pcVar1 = &DAT_800b2cbd + iVar2;
        param_1 = param_1 + 1;
        iVar2 = iVar2 + 0x170;
      } while (*pcVar1 != '\0');
    }
  }
  iVar2 = param_1 << 1;
LAB_8003eef8:
  iVar2 = (iVar2 * 8 + param_1 * 7) * 0x10;
  (&DAT_800b2b4e)[iVar2] = 0;
  puVar3 = (undefined4 *)(&DAT_800b2ca4 + iVar2);
  iVar2 = 6;
  do {
    *puVar3 = 0;
    iVar2 = iVar2 + -1;
    puVar3 = puVar3 + 1;
  } while (iVar2 != 0);
  FUN_8003edec(&DAT_800b2b4c + param_1 * 0x170);
  return;
}
