/* FUN_801194b4 @ 0x801194b4  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801194b4(void)

{
  short sVar1;
  
  sVar1 = func_0x8001a9cc(&DAT_800aca88,0x2c8);
  if ((*(uint *)(_DAT_800ac784 + 0x1d0) < 2000) && (sVar1 == 0)) {
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x701;
  }
  return;
}


