/* FUN_8010b880 @ 0x8010b880  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b880(void)

{
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    (**(code **)(&LAB_80118a20 + (uint)*(byte *)(_DAT_800ac784 + 4) * 4))();
  }
  return;
}


