/* FUN_8010ee40 @ 0x8010ee40  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010ee40(short param_1,uint param_2)

{
  int iVar1;
  char *pcVar2;
  
  iVar1 = (param_2 & 0xff) * 4;
  pcVar2 = &DAT_80118d44 + iVar1;
  if (param_1 == 0) {
    pcVar2 = &DAT_80118d45 + iVar1;
  }
  else if (0 < param_1) {
    pcVar2 = &DAT_80118d46 + iVar1;
  }
  if (*(char *)(_DAT_800ac784 + 0x94) != *pcVar2) {
    *(char *)(_DAT_800ac784 + 0x94) = *pcVar2;
  }
  return;
}


