/* FUN_80101e7c @ 0x80101e7c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80101e7c(void)

{
  if (*(byte *)(_DAT_800ac784 + 6) < 9) {
                    /* WARNING: Jumptable at 0x80101eb8 did not pass sanity check. */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(&LAB_80100024 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
    return;
  }
  return;
}


