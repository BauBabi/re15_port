bool FUN_8001f8b4(int param_1,int param_2,int param_3,short param_4)

{
  bool bVar1;
  byte bVar2;
  uint uVar3;
  int iVar4;
  uint *puVar5;
  uint uVar6;
  short *psVar7;
  uint *puVar8;
  uint uVar9;
  int iVar10;
  uint uVar11;
  undefined1 *puVar12;
  undefined1 auStack_50 [24];
  int local_38;
  int local_34;
  int local_30;
  VECTOR local_28;
  
  puVar12 = auStack_50;
  uVar11 = (uint)*(byte *)(param_1 + 4);
  puVar8 = (uint *)(param_2 + (uint)*(byte *)(DAT_800ac784 + 0x94) * 4);
  uVar3 = *puVar8;
  iVar10 = *(int *)(DAT_800ac784 + 0x188);
  uVar9 = uVar3 & 0xffff;
  if (param_3 == 0) {
    uVar6 = (uint)*(byte *)(DAT_800ac784 + 0x95);
    uVar3 = *(uint *)(param_2 + (uVar3 >> 0x10) + (uint)*(byte *)(DAT_800ac784 + 0x95) * 4) & 0xfff;
    do {
      uVar6 = uVar6 + 1;
      iVar4 = uVar6 * 0x10000;
      if (uVar9 <= (uint)((int)(uVar6 * 0x10000) >> 0x10)) {
        uVar6 = 0;
        iVar4 = 0;
      }
      puVar5 = (uint *)(param_2 + (*puVar8 >> 0x10) + (iVar4 >> 0xe));
    } while ((*puVar5 & 0x8000) != 0);
  }
  else {
    uVar3 = 0x1000 - (*(uint *)(param_2 + (uVar3 >> 0x10) +
                               ((uVar9 - *(byte *)(DAT_800ac784 + 0x95)) + -1) * 4) & 0xfff);
    iVar4 = (uVar9 - *(byte *)(DAT_800ac784 + 0x95)) + 0xfffe;
    do {
      if (iVar4 << 0x10 < 0) {
        iVar4 = uVar9 + 0xffff;
      }
      puVar5 = (uint *)(param_2 + (*puVar8 >> 0x10) + ((iVar4 << 0x10) >> 0xe));
      iVar4 = iVar4 + -1;
    } while ((*puVar5 & 0x8000) != 0);
  }
  psVar7 = (short *)(param_1 + (uint)*(ushort *)(param_1 + 2) +
                    (uint)(*(ushort *)(param_1 + 6) >> 2) * (*puVar5 & 0xfff) * 4);
  local_38 = (int)*psVar7;
  local_34 = (int)psVar7[1];
  local_30 = (int)psVar7[2];
  local_28.vx = (long)*(short *)(iVar10 + 0x38);
  local_28.vy = (long)*(short *)(iVar10 + 0x3a);
  local_28.vz = (long)*(short *)(iVar10 + 0x3c);
  gte_ldIR0(0x1000 - uVar3);
  gte_ldlvl(&local_28);
  gte_gpf12();
  gte_ldIR0(uVar3);
  gte_ldlvl((VECTOR *)(puVar12 + 0x18));
  gte_gpl12();
  gte_stlvl((VECTOR *)(iVar10 + 0x2c));
  if (*(char *)(DAT_800ac784 + 0x8f) == '\0') {
    FUN_8001fb94(uVar11,iVar10,psVar7 + 6);
  }
  else {
    *(char *)(DAT_800ac784 + 0x8f) = *(char *)(DAT_800ac784 + 0x8f) + -1;
    *(int *)(puVar12 + 0x10) = (int)param_4;
    FUN_8001ffd8(uVar11,iVar10,psVar7 + 6);
  }
  bVar2 = *(char *)(DAT_800ac784 + 0x95) + 1;
  *(byte *)(DAT_800ac784 + 0x95) = bVar2;
  bVar1 = uVar9 <= bVar2;
  if (bVar1) {
    *(undefined1 *)(DAT_800ac784 + 0x95) = 0;
  }
  return bVar1;
}
