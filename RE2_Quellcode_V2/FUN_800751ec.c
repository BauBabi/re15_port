/* FUN_800751ec @ 0x800751ec  (Ghidra headless, raw MIPS overlay) */

void FUN_800751ec(void)

{
  u_short uVar1;
  u_short *puVar2;
  int iVar3;
  
  uVar1 = GetClut(0,0x1e5);
  iVar3 = 0x46;
  puVar2 = &DAT_8019920e;
  do {
    iVar3 = iVar3 + -1;
    *(undefined1 *)((int)puVar2 + -0xb) = 4;
                    /* Possible PsyQ macro: setSprt() */
    *(undefined1 *)((int)puVar2 + -7) = 100;
    *(undefined1 *)(puVar2 + -5) = 0x80;
    *(undefined1 *)((int)puVar2 + -9) = 0x80;
    *(undefined1 *)(puVar2 + -4) = 0x80;
    *(undefined1 *)(puVar2 + -1) = 0x50;
    *(undefined1 *)((int)puVar2 + -1) = 0x96;
    puVar2[1] = 0x30;
    puVar2[2] = 0x30;
    *puVar2 = uVar1;
    puVar2 = puVar2 + 10;
  } while (iVar3 != 0);
  return;
}


