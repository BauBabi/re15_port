/* FUN_8002c378 @ 0x8002c378  (Ghidra headless, raw MIPS overlay) */

void FUN_8002c378(void)

{
  byte bVar1;
  int iVar2;
  ushort *puVar3;
  byte *pbVar4;
  int iVar5;
  
  puVar3 = &DAT_800dfc1c;
  iVar5 = 4;
  pbVar4 = &DAT_800dfc24;
  do {
    if (-1 < (int)((uint)*puVar3 << 0x10)) {
      bVar1 = (byte)((int)((uint)*puVar3 << 0x10) >> 0x17);
      *(byte *)(puVar3 + (uint)DAT_800ce5e0 * 8 + 8) = pbVar4[-3] & bVar1;
      *(byte *)((int)puVar3 + (uint)DAT_800ce5e0 * 0x10 + 0x11) = pbVar4[-2] & bVar1;
      *(byte *)(puVar3 + (uint)DAT_800ce5e0 * 8 + 9) = pbVar4[-1] & bVar1;
      *puVar3 = *puVar3 + *(short *)(pbVar4 + -6);
      *(undefined4 *)(puVar3 + (uint)DAT_800ce5e0 * 8 + 10) = *(undefined4 *)(pbVar4 + 0x3c);
      *(undefined4 *)(puVar3 + (uint)DAT_800ce5e0 * 8 + 0xc) = *(undefined4 *)(pbVar4 + 0x40);
      if (iVar5 < 2) {
        AddPrim((void *)(DAT_800ce2b0 + (uint)*pbVar4 * 4),puVar3 + (uint)DAT_800ce5e0 * 8 + 6);
        bVar1 = *pbVar4;
        iVar2 = DAT_800ce2b0;
      }
      else {
        AddPrim((void *)(DAT_800cc228 + (uint)*pbVar4 * 4),puVar3 + (uint)DAT_800ce5e0 * 8 + 6);
        bVar1 = *pbVar4;
        iVar2 = DAT_800cc228;
      }
      AddPrim((void *)(iVar2 + (uint)bVar1 * 4),puVar3 + (uint)DAT_800ce5e0 * 6 + 0x16);
    }
    pbVar4 = pbVar4 + 0x4c;
    iVar5 = iVar5 + -1;
    puVar3 = puVar3 + 0x26;
  } while (0 < iVar5);
  if ((DAT_800cfbd8 & 0x10) == 0) {
    if (0xf < DAT_800dfd55) {
      DAT_800dfd55 = DAT_800dfd55 - 0x10;
    }
  }
  else if (DAT_800dfd55 < 0xf0) {
    DAT_800dfd55 = DAT_800dfd55 + 0x10;
  }
  bVar1 = DAT_800dfd55;
  iVar5 = (uint)DAT_800ce5e0 * 0x10;
  (&DAT_8009dbd0)[iVar5] = DAT_800dfd55;
  (&DAT_8009dbd1)[iVar5] = bVar1;
  (&DAT_8009dbd2)[iVar5] = bVar1;
  (&DAT_8009dbb0)[iVar5] = bVar1;
  (&DAT_8009dbb1)[iVar5] = bVar1;
  (&DAT_8009dbb2)[iVar5] = bVar1;
  if (((DAT_800cfb74 & 0x4000) == 0) && (DAT_800dfd55 != 0)) {
    AddPrim((void *)(DAT_800cc228 + 0x10),&DAT_8009dbac + (uint)DAT_800ce5e0 * 0x10);
    AddPrim((void *)(DAT_800cc228 + 0x10),&DAT_8009dbcc + (uint)DAT_800ce5e0 * 0x10);
    AddPrim((void *)(DAT_800cc228 + 0x10),(void *)(DAT_800dfc10 + 0x70));
  }
  return;
}


