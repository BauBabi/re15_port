/* FUN_80072c1c @ 0x80072c1c  (Ghidra headless, raw MIPS overlay) */

void FUN_80072c1c(void)

{
  undefined2 *puVar1;
  int iVar2;
  
  iVar2 = 0xa0;
  puVar1 = &DAT_80199612;
  do {
    iVar2 = iVar2 + -1;
    *(undefined1 *)((int)puVar1 + -0xf) = 4;
                    /* Possible PsyQ macro: setSprt() */
    *(undefined1 *)((int)puVar1 + -0xb) = 100;
    *(undefined1 *)(puVar1 + -7) = 0x80;
    *(undefined1 *)((int)puVar1 + -0xd) = 0x80;
    *(undefined1 *)(puVar1 + -6) = 0x80;
    puVar1[-1] = 8;
    *puVar1 = 9;
    puVar1 = puVar1 + 10;
  } while (iVar2 != 0);
  return;
}


