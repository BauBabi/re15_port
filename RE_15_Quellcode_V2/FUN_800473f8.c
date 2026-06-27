undefined * FUN_800473f8(void)

{
  ushort uVar1;
  undefined1 uVar2;
  short sVar3;
  short sVar4;
  short sVar5;
  int iVar6;
  undefined *puVar7;
  uint uVar8;
  undefined *puVar9;
  
  iVar6 = (uint)DAT_800b260d * 8;
  puVar9 = &DAT_8019a900;
  uVar1 = *(ushort *)(&DAT_80076840 + (uint)DAT_800b260e * 8);
  sVar5 = (short)((((int)((DAT_800aca88 + 32000) * 10 * (uint)*(ushort *)(&DAT_800768b4 + iVar6)) >>
                   0x14) + 5) / 10) + *(short *)(&DAT_800768b0 + iVar6);
  DAT_800b2606 = (short)(-(((int)((DAT_800aca90 + 32000) *
                                 (uint)*(ushort *)(&DAT_800768b6 + iVar6) * 10) >> 0x14) + 5) / 10)
                 + *(short *)(&DAT_800768b2 + iVar6);
  puVar7 = &UNK_8019a800;
  if (DAT_800aca34 == 0) {
    puVar7 = &DAT_8019a828;
  }
  puVar7[4] = DAT_800b2602;
  sVar4 = sVar5 + -4;
  puVar7[5] = DAT_800b2602;
  uVar2 = DAT_800b2602;
  sVar5 = sVar5 + 4;
  *(short *)(puVar7 + 8) = sVar4;
  puVar7[6] = uVar2;
  sVar3 = DAT_800b2606;
  *(short *)(puVar7 + 0x10) = sVar5;
  *(short *)(puVar7 + 10) = sVar3 + -4;
  sVar3 = DAT_800b2606;
  *(short *)(puVar7 + 0x18) = sVar4;
  *(short *)(puVar7 + 0x12) = sVar3 + -4;
  sVar3 = DAT_800b2606;
  uVar8 = 0;
  *(short *)(puVar7 + 0x20) = sVar5;
  *(short *)(puVar7 + 0x1a) = sVar3 + 4;
  *(short *)(puVar7 + 0x22) = DAT_800b2606 + 4;
  AddPrim(&DAT_800aa6d8 + (uint)DAT_800aca34 * 0x1000,puVar7);
  puVar7 = puVar9;
  if (uVar1 != 0xfffffffe) {
    do {
      puVar9 = puVar7 + 0x28;
      uVar8 = uVar8 + 1;
      AddPrim(&DAT_800aa6d8 + (uint)DAT_800aca34 * 0x1000,puVar7);
      puVar7 = puVar9;
    } while ((uVar8 & 0xffff) < uVar1 + 2);
  }
  return puVar9;
}
