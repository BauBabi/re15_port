/* FUN_80073880 @ 0x80073880  (Ghidra headless, raw MIPS overlay) */

void FUN_80073880(void)

{
  bool bVar1;
  byte bVar2;
  u_short uVar3;
  u_short *puVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  undefined *puVar8;
  
  puVar8 = &DAT_80198700;
  uVar3 = GetClut(0,0x1e4);
  if (DAT_800cfc00 == 0xd) {
    DAT_800aaea0 = 0x2a;
    DAT_800aaea8 = 0;
    DAT_800aaea9 = 0xb4;
    DAT_800aaea5 = 0xb4;
    DAT_800aaea1 = 0xb4;
  }
  else {
    if (DAT_800cfc00 < 0xe) {
      DAT_800aaea8 = 0x58;
      if (DAT_800cfc00 != 0xc) {
        DAT_800aaea8 = 0;
      }
    }
    else {
      DAT_800aaea8 = 0x2c;
    }
    DAT_800aaea1 = 200;
    DAT_800aaea5 = 0xd6;
    DAT_800aaea9 = 0xeb;
    DAT_800aaea0 = DAT_800aaea8;
  }
  iVar6 = 0x34;
  DAT_800aaea4 = DAT_800aaea8;
  do {
    iVar5 = iVar6 + -4;
    iVar7 = 2;
    puVar4 = (u_short *)(puVar8 + 0xe);
    do {
      *(undefined1 *)((int)puVar4 + -0xb) = 4;
                    /* Possible PsyQ macro: setSprt() */
      *(undefined1 *)((int)puVar4 + -7) = 100;
      *(undefined1 *)(puVar4 + -5) = 0x80;
      *(undefined1 *)((int)puVar4 + -9) = 0x80;
      *(undefined1 *)(puVar4 + -4) = 0x80;
      *(undefined *)(puVar4 + -1) = (&UNK_800aae9c)[iVar6];
      *(undefined *)((int)puVar4 + -1) = (&UNK_800aae9d)[iVar6];
      puVar8 = puVar8 + 0x14;
      puVar4[1] = (ushort)(byte)(&UNK_800aae9e)[iVar6];
      bVar2 = (&UNK_800aae9f)[iVar6];
      iVar7 = iVar7 + -1;
      *puVar4 = uVar3;
      puVar4[2] = (ushort)bVar2;
      puVar4 = puVar4 + 10;
    } while (iVar7 != 0);
    iVar6 = iVar5;
  } while (iVar5 != 0);
  puVar8 = &DAT_80198a00;
  iVar6 = 0x24;
  do {
    iVar5 = 2;
    puVar4 = (u_short *)(puVar8 + 0xe);
    do {
      *(undefined1 *)((int)puVar4 + -0xb) = 4;
                    /* Possible PsyQ macro: setSprt() */
      *(undefined1 *)((int)puVar4 + -7) = 100;
      *(undefined1 *)(puVar4 + -5) = 0x80;
      *(undefined1 *)((int)puVar4 + -9) = 0x80;
      *(undefined1 *)(puVar4 + -4) = 0x80;
      *(undefined *)(puVar4 + -1) = (&UNK_800aaed4)[iVar6];
      *(undefined *)((int)puVar4 + -1) = (&UNK_800aaed5)[iVar6];
      puVar8 = puVar8 + 0x14;
      puVar4[1] = (ushort)(byte)(&UNK_800aaed6)[iVar6];
      bVar2 = (&UNK_800aaed7)[iVar6];
      iVar5 = iVar5 + -1;
      *puVar4 = uVar3;
      puVar4[2] = (ushort)bVar2;
      puVar4 = puVar4 + 10;
    } while (iVar5 != 0);
    bVar1 = iVar6 != 0;
    iVar6 = iVar6 + -4;
  } while (bVar1);
  return;
}


