/* FUN_8006db44 @ 0x8006db44  (Ghidra headless, raw MIPS overlay) */

void FUN_8006db44(void)

{
  bool bVar1;
  byte bVar2;
  char cVar3;
  u_short uVar4;
  u_short *puVar5;
  int iVar6;
  byte *pbVar7;
  int iVar8;
  undefined *puVar9;
  
  puVar9 = &DAT_8019c000;
  pbVar7 = &DAT_800a9b86;
  iVar6 = 0x70;
  do {
    iVar8 = 2;
    puVar5 = (u_short *)(puVar9 + 0xe);
    do {
      *(undefined1 *)((int)puVar5 + -0xb) = 4;
                    /* Possible PsyQ macro: setSprt() */
      *(undefined1 *)((int)puVar5 + -7) = 100;
      *(undefined1 *)(puVar5 + -5) = 0x80;
      *(undefined1 *)((int)puVar5 + -9) = 0x80;
      *(undefined1 *)(puVar5 + -4) = 0x80;
      if ((&UNK_800a9b17)[iVar6] == '\0') {
        bVar2 = *pbVar7;
        *(undefined1 *)((int)puVar5 + -1) = 0xf8;
        cVar3 = (bVar2 & 3) * '\b' + ' ';
      }
      else {
        bVar2 = *pbVar7;
        *(undefined1 *)((int)puVar5 + -1) = 0xf8;
        cVar3 = (bVar2 & 3) << 3;
      }
      *(char *)(puVar5 + -1) = cVar3;
      puVar9 = puVar9 + 0x14;
      iVar8 = iVar8 + -1;
      puVar5[-3] = *(u_short *)(&UNK_800a9b18 + iVar6);
      uVar4 = *(u_short *)(&UNK_800a9b1a + iVar6);
      puVar5[1] = 8;
      puVar5[2] = 8;
      puVar5[-2] = uVar4;
      uVar4 = GetClut(0x100,0x1fc);
      *puVar5 = uVar4;
      puVar5 = puVar5 + 10;
    } while (iVar8 != 0);
    bVar1 = pbVar7 != &UNK_800a9b1e;
    pbVar7 = pbVar7 + -8;
    iVar6 = iVar6 + -8;
  } while (bVar1);
  SetDrawMode((DR_MODE *)&DAT_800d6bc0,0,0,0x17,(RECT *)0x0);
  SetDrawMode((DR_MODE *)&DAT_800d6bcc,0,0,0x17,(RECT *)0x0);
  return;
}


