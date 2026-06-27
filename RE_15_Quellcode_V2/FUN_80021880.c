void FUN_80021880(void)

{
  byte bVar1;
  uint uVar2;
  int iVar3;
  undefined *puVar4;
  ushort *puVar5;
  byte *pbVar6;
  int iVar7;
  
  puVar5 = &DAT_800b5458;
  iVar7 = 4;
  pbVar6 = &DAT_800b5460;
  do {
    if (-1 < (int)((uint)*puVar5 << 0x10)) {
      bVar1 = (byte)((int)((uint)*puVar5 << 0x10) >> 0x17);
      *(byte *)(puVar5 + (uint)DAT_800aca34 * 8 + 8) = bVar1 & pbVar6[-3];
      *(byte *)((int)puVar5 + (uint)DAT_800aca34 * 0x10 + 0x11) = bVar1 & pbVar6[-2];
      *(byte *)(puVar5 + (uint)DAT_800aca34 * 8 + 9) = bVar1 & pbVar6[-1];
      *puVar5 = *puVar5 + *(short *)(pbVar6 + -6);
      if (iVar7 < 2) {
        AddPrim(&DAT_800ac6d8 + (uint)DAT_800aca34 * 0x40 + (uint)*pbVar6 * 4,
                puVar5 + (uint)DAT_800aca34 * 8 + 6);
        uVar2 = (uint)DAT_800aca34;
        iVar3 = uVar2 << 6;
        puVar4 = &DAT_800ac6d8 + (uint)*pbVar6 * 4;
      }
      else {
        AddPrim(&UNK_800aa698 + (uint)DAT_800aca34 * 0x20 + (uint)*pbVar6 * 4,
                puVar5 + (uint)DAT_800aca34 * 8 + 6);
        uVar2 = (uint)DAT_800aca34;
        iVar3 = uVar2 << 5;
        puVar4 = &UNK_800aa698 + (uint)*pbVar6 * 4;
      }
      AddPrim(puVar4 + iVar3,puVar5 + uVar2 * 6 + 0x16);
    }
    pbVar6 = pbVar6 + 0x44;
    iVar7 = iVar7 + -1;
    puVar5 = puVar5 + 0x22;
  } while (0 < iVar7);
  return;
}
