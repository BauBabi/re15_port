/* FUN_80015fe8 @ 0x80015fe8  (Ghidra headless, raw MIPS overlay) */

uint FUN_80015fe8(void)

{
  uint uVar1;
  uint uVar2;
  
  uVar2 = DAT_800ce318 >> 7 & 0xff;
  uVar1 = uVar2 + DAT_800ce318 & 0xff;
  DAT_800ce318 = uVar1 | uVar2 << 8;
  return uVar1;
}


