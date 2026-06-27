/* trapIntrVSync @ 0x800867ec  (Ghidra headless, raw MIPS overlay) */

void trapIntrVSync(void)

{
  undefined4 *puVar1;
  int iVar2;
  
  iVar2 = 0;
  puVar1 = &DAT_800acce0;
  DAT_800acd00 = DAT_800acd00 + 1;
  do {
    if ((code *)*puVar1 != (code *)0x0) {
      (*(code *)*puVar1)();
    }
    iVar2 = iVar2 + 1;
    puVar1 = puVar1 + 1;
  } while (iVar2 < 8);
  return;
}


