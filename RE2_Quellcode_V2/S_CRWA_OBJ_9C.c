/* S_CRWA_OBJ_9C @ 0x80078560  (Ghidra headless, raw MIPS overlay) */

undefined4 S_CRWA_OBJ_9C(void)

{
  int iVar1;
  undefined4 uVar2;
  uint unaff_s1;
  undefined4 unaff_s2;
  int unaff_s5;
  int in_stack_00000010;
  
  iVar1 = DAT_800ab238;
  if (DAT_800ab238 == 1) {
    DAT_800ab238 = 0;
    unaff_s5 = 1;
  }
  if (DAT_800ab254 != 0) {
    in_stack_00000010 = DAT_800ab254;
    DAT_800ab254 = 0;
  }
  if (unaff_s1 < 0x401) {
    _spu_t(2,unaff_s2);
    _spu_t(1);
    _spu_t(3,&DAT_800ab2e4,unaff_s1);
    WaitEvent(DAT_800ab280);
    if (unaff_s5 != 0) {
      DAT_800ab238 = iVar1;
    }
    if (in_stack_00000010 != 0) {
      DAT_800ab254 = in_stack_00000010;
    }
    return 0;
  }
  uVar2 = S_CRWA_OBJ_100();
  return uVar2;
}


