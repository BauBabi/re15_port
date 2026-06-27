/* FUN_80053244 @ 0x80053244  (Ghidra headless, raw MIPS overlay) */

void FUN_80053244(void)

{
  byte *pbVar1;
  uint uVar2;
  uint uVar3;
  
  uVar3 = (uint)DAT_800d7532;
  uVar2 = 0;
  if (uVar3 != 0) {
    pbVar1 = &DAT_800d6c48;
    do {
      if ((*pbVar1 & 0x80) == 0) {
        if ((*pbVar1 & 0x40) != 0) {
          FUN_800358f0(uVar2,pbVar1);
        }
      }
      else {
        FUN_800357d8(uVar2,pbVar1);
      }
      uVar2 = uVar2 + 1;
      pbVar1 = pbVar1 + 0x4c;
    } while (uVar2 < uVar3);
  }
  return;
}


