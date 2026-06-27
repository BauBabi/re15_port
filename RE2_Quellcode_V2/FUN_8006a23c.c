/* FUN_8006a23c @ 0x8006a23c  (Ghidra headless, raw MIPS overlay) */

uint FUN_8006a23c(void)

{
  uint uVar1;
  
  uVar1 = FUN_800696cc(*(&PTR_DAT_800a9e20)[(uint)(byte)(&DAT_800d4a3c)[(uint)DAT_800d5bf8 * 4] * 2]
                      );
  if (DAT_800d5bfa == '\x11') {
    uVar1 = 0;
  }
  else {
    uVar1 = ~uVar1 >> 0x1f;
  }
  return uVar1;
}


