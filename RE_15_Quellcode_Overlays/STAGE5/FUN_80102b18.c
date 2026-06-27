/* FUN_80102b18 @ 0x80102b18  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80102b18(void)

{
  if (*(byte *)(_DAT_800ac784 + 6) < 7) {
                    /* WARNING: Jumptable at 0x80102b50 did not pass sanity check. */
                    /* WARNING: Treating indirect jump as call */
    (**(code **)(&LAB_8010004c + (uint)*(byte *)(_DAT_800ac784 + 6) * 4))();
    return;
  }
  return;
}


