/* FUN_80052f3c @ 0x80052f3c  (Ghidra headless, raw MIPS overlay) */

void FUN_80052f3c(void)

{
  undefined1 *puVar1;
  uint uVar2;
  int iVar3;
  undefined4 *puVar4;
  
  uVar2 = 0;
  puVar1 = &DAT_800d7868;
  do {
    puVar1[-5] = (char)uVar2;
    puVar1[-7] = 0;
    puVar1[-6] = 0;
    puVar1[-4] = 0xff;
    *puVar1 = 0xff;
    uVar2 = uVar2 + 1;
    puVar1 = puVar1 + 0x174;
  } while (uVar2 < 10);
  FUN_800530c0();
  puVar4 = &DAT_800d4928;
  iVar3 = 0x40;
  DAT_800d4874 = 0;
  DAT_800d8ccd = 0;
  DAT_800d8cce = 0;
  DAT_800d8ccf = 0;
  DAT_800d482e = 0xffff;
  DAT_800d4830 = 0xffff;
  DAT_800cfbf4 = 0;
  DAT_800cfbf6 = 0;
  DAT_800d7533 = 0xff;
  DAT_800d8cca = 0xff;
  DAT_800d8ccb = 0xff;
  DAT_800d8ccc = 0xff;
  DAT_800d8cd2 = 0xff;
  DAT_800d7532 = 0;
  DAT_800d8cd0 = 0;
  DAT_800d8cd1 = 0;
  DAT_800d5b50 = 0x78;
  DAT_800cfbd8 = DAT_800cfbd8 & 0xfffff3ff;
  DAT_800d8cc0 = DAT_800cfe1c;
  do {
    *puVar4 = 0xffffffff;
    iVar3 = iVar3 + -1;
    puVar4 = puVar4 + 1;
  } while (iVar3 != 0);
  if ((DAT_800cfbf8 & 0x400) != 0) {
    DAT_800cfbf8 = DAT_800cfbf8 & 0xfffffbff;
  }
  DAT_800cfd06 = DAT_800cfd06 & 0xfff;
  return;
}


