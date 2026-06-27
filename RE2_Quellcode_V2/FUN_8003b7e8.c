/* FUN_8003b7e8 @ 0x8003b7e8  (Ghidra headless, raw MIPS overlay) */

void FUN_8003b7e8(undefined1 param_1,int param_2)

{
  int iVar1;
  int iVar2;
  undefined2 *puVar3;
  uint uVar4;
  
  iVar1 = DAT_800ea23c;
  DAT_800dfc1a = 0;
  FUN_8003a668();
  uVar4 = 0;
  puVar3 = &DAT_800a2708;
  iVar2 = 0x300;
  do {
    FUN_8003a8cc(iVar1 + iVar2,puVar3);
    puVar3 = puVar3 + 8;
    uVar4 = uVar4 + 1;
    iVar2 = iVar2 + 0x10;
  } while (uVar4 < 0x16c);
                    /* Possible PsyQ macro: setLineF2() */
  *(undefined1 *)(iVar1 + 0x2da) = 0x40;
  *(undefined1 *)(iVar1 + 3) = 0;
  *(undefined1 *)(iVar1 + 2) = 0;
  *(undefined1 *)(iVar1 + 0x2d5) = 0;
  *(undefined1 *)(iVar1 + 0x2d8) = 0;
  *(undefined1 *)(iVar1 + 0x2db) = 4;
  *(undefined1 *)(iVar1 + 0x2dc) = 0;
  *(undefined1 *)(iVar1 + 0x2dd) = DAT_800cfbf2;
  DAT_800dfc1b = 1;
  DAT_800cfbf2 = param_1;
  FUN_80031f94(1);
  if (param_2 != 0) {
    FUN_8005ba28(0x2060000,0);
    DAT_800d4237 = 0;
    while (iVar1 = FUN_8003a148(0xc,10,0xfc,0x8b), iVar1 != 0) {
      FUN_8003a794(0x16c);
      FUN_80031f94(1);
    }
  }
  return;
}


