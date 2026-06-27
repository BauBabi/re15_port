void FUN_80053240(void)

{
  int iVar1;
  undefined *puVar2;
  MATRIX *r0;
  byte *pbVar3;
  int iVar4;
  int iVar5;
  undefined2 *puVar6;
  undefined *puVar7;
  MATRIX *r0_00;
  undefined1 *puVar8;
  uint uVar9;
  undefined1 auStack_58 [40];
  undefined *local_30;
  
  puVar2 = DAT_800a73a0;
  puVar8 = auStack_58;
  puVar6 = (undefined2 *)&UNK_800aa538;
  r0_00 = (MATRIX *)&DAT_800b5288;
  local_30 = &DAT_8009d394 + (uint)DAT_800aca34 * 0xa000;
  puVar7 = &DAT_800aa6d8 + (uint)DAT_800aca34 * 0x1000;
  uVar9 = (uint)(*(ushort *)(DAT_800ac778 + DAT_800b0fe4 * 0x20 + 0x62) >> 7);
  SetRotMatrix((MATRIX *)&DAT_800b5288);
  SetTransMatrix((MATRIX *)&DAT_800b5288);
  pbVar3 = &DAT_800aa520;
  do {
    puVar6 = puVar6 + -0x42;
    if (((*pbVar3 & 1) != 0) && ((*pbVar3 & 2) != 0)) {
      *(int *)(puVar8 + 0x18) = (int)*(short *)(pbVar3 + -0x44);
      *(int *)(puVar8 + 0x1c) = (int)*(short *)(pbVar3 + -0x42);
      *(int *)(puVar8 + 0x20) = (int)*(short *)(pbVar3 + -0x40);
      iVar1 = FUN_80014368(puVar8 + 0x18,DAT_800ac790);
      if (iVar1 != 0) {
        iVar5 = (uint)pbVar3[2] * 8 + *(int *)(pbVar3 + 0xc);
        iVar1 = *(int *)(puVar8 + 0x28);
        iVar4 = *(byte *)(iVar5 + 1) - 1;
        if (puVar2 < (undefined *)(iVar1 + iVar4 * -0x28)) {
          *(uint *)(puVar8 + 0x10) = uVar9;
          *(undefined **)(puVar8 + 0x14) = puVar7;
          puVar2 = (undefined *)FUN_800534c4(puVar2,puVar6,iVar5,iVar4);
          if ((*pbVar3 & 8) != 0) {
            if ((undefined *)(iVar1 + iVar4 * -0x28) <= puVar2) goto LAB_800533b4;
            r0 = (MATRIX *)&DAT_800b5288;
            gte_SetRotMatrix((MATRIX *)&DAT_800b5288);
            gte_SetTransMatrix(r0);
            *(uint *)(puVar8 + 0x10) = uVar9;
            *(undefined **)(puVar8 + 0x14) = puVar7;
            puVar2 = (undefined *)FUN_800534c4(puVar2,puVar6,iVar5,iVar4);
            gte_SetRotMatrix(r0_00);
            gte_SetTransMatrix(r0_00);
          }
        }
        else {
LAB_800533b4:
          *pbVar3 = 0;
        }
      }
    }
    pbVar3 = pbVar3 + -0x84;
    if (puVar6 == &DAT_800a73b8) {
      DAT_800a73a0 = puVar2;
      return;
    }
  } while( true );
}
