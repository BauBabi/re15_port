/* FUN_8010d43c @ 0x8010d43c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010d43c(void)

{
  if (*(byte *)(_DAT_800ac784 + 6) < 8) {
                    /* WARNING: Jumptable at 0x8010d478 did not pass sanity check. */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(&LAB_801001d4 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
    return;
  }
  return;
}


