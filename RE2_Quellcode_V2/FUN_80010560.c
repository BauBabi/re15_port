/* FUN_80010560 @ 0x80010560  (Ghidra headless, raw MIPS overlay) */

void FUN_80010560(void)

{
  int iVar1;
  int unaff_s2;
  int unaff_s3;
  int unaff_s4;
  
  iVar1 = unaff_s2 - unaff_s4;
  if (-1 < unaff_s4 - unaff_s2) {
    iVar1 = 0;
  }
                    /* WARNING: Could not recover jumptable at 0x80010590. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)(unaff_s3 + iVar1 * 0xc))();
  return;
}


