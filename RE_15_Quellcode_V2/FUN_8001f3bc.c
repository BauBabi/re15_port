bool FUN_8001f3bc(int param_1,undefined4 param_2,uint *param_3,uint param_4,short param_5)

{
  short sVar1;
  bool bVar2;
  byte bVar3;
  VECTOR *r0;
  short *psVar4;
  uint uVar5;
  MATRIX *m;
  SVECTOR *r;
  uint *puVar6;
  uint uVar7;
  uint uVar8;
  undefined1 *puVar9;
  int iVar10;
  undefined1 auStack_38 [16];
  int local_28;
  int local_24;
  int local_20;
  
  puVar9 = auStack_38;
  uVar5 = (uint)*(byte *)(DAT_800ac784 + 0x8f);
  puVar6 = *(uint **)(DAT_800ac784 + 0x188);
  psVar4 = (short *)(param_1 + (uint)*(ushort *)(param_1 + 2) +
                    (*(uint *)(param_1 + 4) >> 0x12) * (*param_3 & 0xfff) * 4);
  if (uVar5 == 0) {
    sVar1 = *psVar4;
    puVar6[0xb] = (int)sVar1;
    *(short *)(puVar6 + 0xe) = sVar1;
    sVar1 = psVar4[1];
    puVar6[0xc] = (int)sVar1;
    *(short *)((int)puVar6 + 0x3a) = sVar1;
    uVar7 = (uint)*(byte *)(param_1 + 4);
    sVar1 = psVar4[2];
    puVar6[0xd] = (int)sVar1;
    *(short *)(puVar6 + 0xf) = sVar1;
    uVar8 = uVar7;
  }
  else {
    puVar6[0xb] = (int)*psVar4;
    iVar10 = (int)param_5 * uVar5;
    puVar6[0xc] = (int)psVar4[1];
    uVar7 = (uint)*(byte *)(param_1 + 4);
    local_28 = (int)(short)puVar6[0xe];
    puVar6[0xd] = (int)psVar4[2];
    local_24 = (int)*(short *)((int)puVar6 + 0x3a);
    local_20 = (int)(short)puVar6[0xf];
    gte_ldIR0(0x1000 - iVar10);
    r0 = (VECTOR *)(puVar6 + 0xb);
    uVar8 = uVar7;
    gte_ldlvl(r0);
    gte_gpf12();
    gte_ldIR0(iVar10);
    gte_ldlvl((VECTOR *)(puVar9 + 0x10));
    gte_gpl12();
    gte_stlvl(r0);
    *(short *)(puVar6 + 0xe) = (short)puVar6[0xb];
    *(short *)((int)puVar6 + 0x3a) = (short)puVar6[0xc];
    *(short *)(puVar6 + 0xf) = (short)puVar6[0xd];
  }
  FUN_8001f664(uVar7,puVar6,psVar4 + 6);
  if ((uVar5 & 0xff) == 0) {
    m = (MATRIX *)(puVar6 + 6);
    do {
      sVar1 = m[3].m[0][2];
      *(undefined4 *)(m[2].m[1] + 1) = *(undefined4 *)m[3].m[0];
      m[2].m[2][0] = sVar1;
      uVar8 = uVar8 - 1;
      if ((*puVar6 & 0x200) == 0) {
        RotMatrix((SVECTOR *)(puVar6 + 0x1e),m);
      }
      m = (MATRIX *)(m[5].m + 2);
      puVar6 = puVar6 + 0x2b;
    } while ((uVar8 & 0xff) != 0);
  }
  else {
    *(char *)(DAT_800ac784 + 0x8f) = *(char *)(DAT_800ac784 + 0x8f) + -1;
    do {
      r = (SVECTOR *)(puVar6 + 0x18);
      FUN_80020510(r,puVar6 + 0x1e,r,0x1000 - (int)param_5 * (uVar5 & 0xff));
      uVar8 = uVar8 - 1;
      if ((*puVar6 & 0x200) == 0) {
        RotMatrix(r,(MATRIX *)(puVar6 + 6));
      }
      puVar6 = puVar6 + 0x2b;
    } while ((uVar8 & 0xff) != 0);
  }
  bVar3 = *(char *)(DAT_800ac784 + 0x95) + 1;
  *(byte *)(DAT_800ac784 + 0x95) = bVar3;
  bVar2 = param_4 <= bVar3;
  if (bVar2) {
    *(undefined1 *)(DAT_800ac784 + 0x95) = 0;
  }
  return bVar2;
}
