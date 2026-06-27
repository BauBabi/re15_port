/* FUN_8003b918 @ 0x8003b918  (Ghidra headless, raw MIPS overlay) */

void FUN_8003b918(int param_1)

{
  int iVar1;
  int iVar2;
  
  iVar1 = DAT_800ea23c;
  if (param_1 != 0) {
    FUN_8005ba28(0x2070000,0);
    DAT_800d4237 = 1;
    while (iVar2 = FUN_8003a148(0xc,10,0xfc,0x8b), iVar2 != 0) {
      FUN_80031f94(1);
    }
  }
  DAT_800d4234 = 0;
  DAT_800d4235 = 0;
  DAT_800ce32c = DAT_800ce32c + -0x53e0;
  DAT_800cfbf2 = *(undefined1 *)(iVar1 + 0x2dd);
  DAT_800dfc1b = 1;
  DAT_800dfc1a = 2;
  FUN_8003210c(0);
  FUN_80031fe4();
  return;
}


