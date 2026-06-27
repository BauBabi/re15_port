/* FUN_80111f0c @ 0x80111f0c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80111f0c(void)

{
  int in_v0;
  
  if ((in_v0 != 0) && (*(short *)(_DAT_800ac784 + 0x1ec) < 0x1518)) {
    FUN_80115d74(4);
  }
  if ((_DAT_800aca50 & 0x1000) != 0) {
    FUN_80115d74((uint)*(byte *)(_DAT_800ac784 + 0x1d4) % 3 + 1);
  }
  return;
}


