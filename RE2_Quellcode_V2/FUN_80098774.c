/* FUN_80098774 @ 0x80098774  (Ghidra headless, raw MIPS overlay) */

bool FUN_80098774(void)

{
  uint uVar1;
  
  uVar1 = (uint)(ushort)TMR_SYSCLOCK_VAL;
  if (uVar1 < DAT_800cbc2c) {
    if ((ushort)TMR_SYSCLOCK_MAX == 0) {
      uVar1 = uVar1 + 0x10000;
    }
    else {
      uVar1 = uVar1 + (ushort)TMR_SYSCLOCK_MAX;
    }
  }
  if (((ushort)TMR_SYSCLOCK_MODE & 0x200) == 0) {
    uVar1 = uVar1 - DAT_800cbc2c >> 3;
  }
  else {
    uVar1 = uVar1 - DAT_800cbc2c;
  }
  return DAT_800e8990 <= uVar1;
}


