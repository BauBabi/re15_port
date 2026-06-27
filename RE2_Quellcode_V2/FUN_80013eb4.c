/* FUN_80013eb4 @ 0x80013eb4  (Ghidra headless, raw MIPS overlay) */

void FUN_80013eb4(void)

{
  uint uVar1;
  
  uVar1 = DAT_800cfb74 & 0x10000;
  while (uVar1 != 0) {
    FUN_80031f94(1);
    uVar1 = DAT_800cfb74 & 0x10000;
  }
  if ((*(ushort *)(DAT_800d4dd8 + 0x144) & 0x8000) == 0) {
    DAT_800dfc1a = 0;
  }
  while (DAT_800d86e9 != '\0') {
    if ((DAT_800cfbd8 & 0x20000) == 0) {
      DAT_800d4806 = 0;
    }
    FUN_80014058();
    FUN_80014234();
    if (*(ushort *)(DAT_800c3a80 + 0x22e) - 0x29 < 0xdb) {
      if (DAT_800d480a == 0x32) {
        FUN_800313a4(0x20,200,&UNK_8009a42c + DAT_8009a468,0x4000);
      }
      if (DAT_800d480a == 0x34) {
        FUN_800313a4(0x20,0xb8,&UNK_8009a42c + DAT_8009a46a,0x4000);
        FUN_800313a4(0x20,200,&UNK_8009a42c + DAT_8009a46c,0x4000);
      }
    }
    *(short *)(DAT_800c3a80 + 0x22e) = *(short *)(DAT_800c3a80 + 0x22e) + 1;
    FUN_80031f94(1);
  }
  return;
}


