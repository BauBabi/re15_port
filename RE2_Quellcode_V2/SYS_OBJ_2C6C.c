/* SYS_OBJ_2C6C @ 0x80092b4c  (Ghidra headless, raw MIPS overlay) */

undefined4 SYS_OBJ_2C6C(void)

{
  undefined4 uVar1;
  uint unaff_s0;
  
  SetIntrMask(DAT_800b2814);
  uVar1 = 0;
  if ((unaff_s0 & 7) == 0) {
    uVar1 = _version();
  }
  return uVar1;
}


