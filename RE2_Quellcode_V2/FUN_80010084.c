/* FUN_80010084 @ 0x80010084  (Ghidra headless, raw MIPS overlay) */

void FUN_80010084(void)

{
  undefined4 unaff_s0;
  undefined4 unaff_s1;
  undefined4 unaff_s2;
  undefined4 unaff_s3;
  undefined4 unaff_s4;
  undefined4 unaff_s5;
  undefined4 unaff_s6;
  undefined4 unaff_s7;
  
  DAT_80010758 = unaff_s0;
  DAT_8001075c = unaff_s1;
  DAT_80010760 = unaff_s2;
  DAT_80010764 = unaff_s3;
  DAT_80010768 = unaff_s4;
  DAT_8001076c = unaff_s5;
  DAT_80010770 = unaff_s6;
  DAT_80010774 = unaff_s7;
                    /* WARNING: Could not recover jumptable at 0x80010168. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (**(code **)((int)&PTR_LAB_80010170 + DAT_80010748))();
  return;
}


