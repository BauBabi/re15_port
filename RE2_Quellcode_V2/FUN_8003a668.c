/* FUN_8003a668 @ 0x8003a668  (Ghidra headless, raw MIPS overlay) */

void FUN_8003a668(void)

{
  undefined1 *puVar1;
  undefined1 *puVar2;
  undefined1 *puVar3;
  uint uVar4;
  
  puVar1 = DAT_800ea23c;
  uVar4 = 0;
  puVar3 = DAT_800ea23c + 4;
  puVar2 = DAT_800ea23c + 0x16c;
  do {
    *puVar3 = 0x20;
    puVar3 = puVar3 + 1;
    *puVar2 = 0;
    uVar4 = uVar4 + 1;
    puVar2 = puVar2 + 1;
  } while (uVar4 < 0x168);
  puVar1[0x2d7] = 0;
  puVar1[1] = 0;
  *puVar1 = 0;
  return;
}


