/* FUN_80075360 @ 0x80075360  (Ghidra headless, raw MIPS overlay) */

void FUN_80075360(void)

{
  bool bVar1;
  byte bVar2;
  u_short uVar3;
  undefined1 *puVar4;
  u_short *puVar5;
  int iVar6;
  int iVar7;
  undefined *puVar8;
  
  puVar8 = &UNK_80198100;
  uVar3 = GetClut(0,0x1e4);
  iVar6 = 4;
  do {
    iVar7 = 2;
    puVar5 = (u_short *)(puVar8 + 0xe);
    do {
      *(undefined1 *)((int)puVar5 + -0xb) = 4;
                    /* Possible PsyQ macro: setSprt() + setSemiTrans(sprt, 1) */
      *(undefined1 *)((int)puVar5 + -7) = 0x66;
      *(undefined1 *)(puVar5 + -5) = 0x80;
      *(undefined1 *)((int)puVar5 + -9) = 0x80;
      *(undefined1 *)(puVar5 + -4) = 0x80;
      *(undefined1 *)(puVar5 + -1) = (&DAT_800ab194)[iVar6];
      *(undefined1 *)((int)puVar5 + -1) = (&DAT_800ab195)[iVar6];
      puVar8 = puVar8 + 0x14;
      puVar5[1] = (ushort)(byte)(&UNK_800ab196)[iVar6];
      bVar2 = (&UNK_800ab197)[iVar6];
      iVar7 = iVar7 + -1;
      *puVar5 = uVar3;
      puVar5[2] = (ushort)bVar2;
      puVar5 = puVar5 + 10;
    } while (iVar7 != 0);
    bVar1 = iVar6 != 0;
    iVar6 = iVar6 + -4;
  } while (bVar1);
  iVar6 = 0x40;
  puVar4 = &DAT_80198186;
  do {
    puVar4[-3] = 3;
    puVar4[1] = 0x42;
    puVar4[-2] = 0x80;
    puVar4[-1] = 0x80;
    *puVar4 = 0x80;
    iVar6 = iVar6 + -1;
    puVar4 = puVar4 + 0x10;
  } while (iVar6 != 0);
  DAT_800d5c54 = 10;
  DAT_800d5c56 = 0x46;
  DAT_800d5c2c = 0x3a;
  DAT_800d5c50 = 0;
  DAT_800d5c53 = 0;
  DAT_800d5c52 = 0;
  DAT_800d5c2e = 0x1d;
  return;
}


