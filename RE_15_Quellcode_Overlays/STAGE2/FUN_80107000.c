/* FUN_80107000 @ 0x80107000  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80107000(void)

{
  func_0x8001a8f8(&DAT_800aca88,0x20);
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0) {
    FUN_80109230(0,1);
    FUN_80107b30();
    return;
  }
  FUN_80109230(0,0);
  FUN_80107b30();
  return;
}


