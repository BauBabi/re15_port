/* FUN_801042f4 @ 0x801042f4  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801042f4(void)

{
  if (*(byte *)(_DAT_800ac784 + 6) < 8) {
                    /* WARNING: Jumptable at 0x80104364 did not pass sanity check. */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(&LAB_801000a4 + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
    return;
  }
  return;
}


