/* FUN_80108e88 @ 0x80108e88  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80108e88(void)

{
  if (*(byte *)(_DAT_800ac784 + 5) < 8) {
                    /* WARNING: Jumptable at 0x80108ec8 did not pass sanity check. */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(&LAB_80100144 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
    return;
  }
  return;
}


