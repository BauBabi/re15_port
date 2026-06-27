/* FUN_80075dc4 @ 0x80075dc4  (Ghidra headless, raw MIPS overlay) */

void FUN_80075dc4(void)

{
  u_short uVar1;
  u_short *puVar2;
  int iVar3;
  char cVar4;
  undefined *puVar5;
  int iVar6;
  
  puVar5 = &DAT_8019c000;
  iVar6 = 2;
  cVar4 = '8';
  do {
    iVar6 = iVar6 + -1;
    iVar3 = 2;
    puVar2 = (u_short *)(puVar5 + 0xe);
    do {
      *(undefined1 *)((int)puVar2 + -0xb) = 4;
                    /* Possible PsyQ macro: setSprt() + setSemiTrans(sprt, 1) */
      *(undefined1 *)((int)puVar2 + -7) = 0x66;
      *(undefined1 *)(puVar2 + -5) = 0x80;
      *(undefined1 *)((int)puVar2 + -9) = 0x80;
      *(undefined1 *)(puVar2 + -4) = 0x80;
      *(char *)(puVar2 + -1) = cVar4;
      *(undefined1 *)((int)puVar2 + -1) = 0x80;
      puVar2[1] = 8;
      puVar2[2] = 8;
      uVar1 = GetClut(0,0x1e9);
      *puVar2 = uVar1;
      puVar2 = puVar2 + 10;
      iVar3 = iVar3 + -1;
      puVar5 = puVar5 + 0x14;
    } while (iVar3 != 0);
    cVar4 = cVar4 + -8;
  } while (iVar6 != 0);
  return;
}


