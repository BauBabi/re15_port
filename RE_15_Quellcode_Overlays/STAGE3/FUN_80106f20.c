/* FUN_80106f20 @ 0x80106f20  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106f20(void)

{
  if (*(byte *)(_DAT_800ac784 + 7) < 0xc) {
                    /* WARNING: Jumptable at 0x80106f5c did not pass sanity check. */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(&LAB_801000fc + (uint)*(byte *)(_DAT_800ac784 + 7) * 4))();
    return;
  }
  return;
}


