/* FUN_80052af4 @ 0x80052af4  (Ghidra headless, raw MIPS overlay) */

byte FUN_80052af4(uint param_1)

{
  uint uVar1;
  byte *pbVar2;
  
  pbVar2 = &DAT_800a7704;
  uVar1 = (uint)DAT_800a7704;
  do {
    if (uVar1 == param_1) {
      return pbVar2[1];
    }
    pbVar2 = pbVar2 + 2;
    uVar1 = (uint)*pbVar2;
  } while (uVar1 == 0xff);
  return 0x80;
}


