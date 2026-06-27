/* FUN_80027434 @ 0x80027434  (Ghidra headless, raw MIPS overlay) */

void FUN_80027434(int param_1,undefined4 *param_2,uint param_3,MATRIX *param_4)

{
  short sVar1;
  uint uVar2;
  MATRIX *pMVar3;
  int iVar4;
  int iVar5;
  long lVar6;
  long lVar7;
  VECTOR *pVVar8;
  undefined4 uVar9;
  MATRIX *pMVar10;
  undefined4 uVar11;
  MATRIX *r0;
  MATRIX *r0_00;
  MATRIX *pMVar12;
  MATRIX *pMVar13;
  int iVar14;
  undefined4 uVar15;
  undefined1 *puVar16;
  undefined1 *puVar17;
  undefined4 uVar18;
  undefined1 auStack_70 [72];
  
  puVar16 = auStack_70;
  pMVar10 = (MATRIX *)param_2[0x1d];
  pMVar3 = (MATRIX *)(param_2 + 6);
  if ((*(uint *)param_2[0x25] & 0x21) == 0x20) {
    *param_2 = 0x20;
  }
  else {
    uVar2 = param_3 & 0x800;
    puVar17 = auStack_70;
    if ((param_3 & 0x40) == 0) {
      gte_SetRotMatrix(pMVar10);
      gte_SetTransMatrix(pMVar10);
      gte_ldclmv(pMVar3);
      gte_rtir();
      pMVar3 = (MATRIX *)((int)param_2 + 0x1a);
      pMVar12 = (MATRIX *)(param_4->m[0] + 1);
      gte_stclmv(param_4);
      gte_ldclmv(pMVar3);
      gte_rtir();
      pMVar3 = (MATRIX *)(param_2 + 7);
      pMVar13 = (MATRIX *)(param_4->m[0] + 2);
      gte_stclmv(pMVar12);
      gte_ldclmv(pMVar3);
      gte_rtir();
      pVVar8 = (VECTOR *)(param_2 + 0xb);
      gte_stclmv(pMVar13);
      gte_ldlv0(pVVar8);
      gte_rt();
      gte_stlvnl((VECTOR *)param_4->t);
      uVar2 = param_3 & 0x800;
      puVar17 = puVar16;
    }
    if (uVar2 != 0) {
      sVar1 = *(short *)(param_2[0x25] + 0x8c);
      iVar4 = param_2[0xb];
      *(undefined4 *)(puVar17 + 0x10) = param_2[6];
      *(undefined4 *)(puVar17 + 0x14) = param_2[7];
      *(undefined4 *)(puVar17 + 0x18) = param_2[8];
      *(undefined4 *)(puVar17 + 0x1c) = param_2[9];
      uVar11 = param_2[10];
      *(int *)(puVar17 + 0x24) = iVar4 * sVar1 >> 0xc;
      *(int *)(puVar17 + 0x28) = param_2[0xc] * (int)*(short *)(param_2[0x25] + 0x8e) >> 0xc;
      sVar1 = *(short *)(param_2[0x25] + 0x90);
      iVar4 = param_2[0xd];
      *(undefined4 *)(puVar17 + 0x20) = uVar11;
      *(int *)(puVar17 + 0x2c) = iVar4 * sVar1 >> 0xc;
      FUN_8002ce94(pMVar10,puVar17 + 0x10,param_4);
    }
    if ((param_3 & 0x20) != 0) {
      FUN_80028ad8(param_2,param_4);
    }
    gte_SetRotMatrix((MATRIX *)&DAT_8009db64);
    gte_ldclmv(param_4);
    gte_rtir();
    r0_00 = (MATRIX *)(puVar17 + 0x10);
    gte_stclmv(r0_00);
    pMVar3 = (MATRIX *)(param_4->m[0] + 1);
    gte_ldclmv(pMVar3);
    gte_rtir();
    pMVar12 = (MATRIX *)(puVar17 + 0x12);
    gte_stclmv(pMVar12);
    pMVar10 = (MATRIX *)(param_4->m[0] + 2);
    gte_ldclmv(pMVar10);
    gte_rtir();
    pMVar13 = (MATRIX *)(puVar17 + 0x14);
    gte_stclmv(pMVar13);
    gte_SetLightMatrix(r0_00);
    if ((param_3 & 0x1000) != 0) {
      FUN_80076f88(param_4->t);
      MulMatrix0((MATRIX *)&DAT_8009db64,param_4,r0_00);
      SetLightMatrix(r0_00);
      FUN_80076f88(*(int *)(param_1 + 0x198) + 0x5c);
    }
    r0 = (MATRIX *)&DAT_800dcba8;
    gte_SetRotMatrix((MATRIX *)&DAT_800dcba8);
    gte_SetTransMatrix(r0);
    gte_ldclmv(param_4);
    gte_rtir();
    uVar11 = param_2[2];
    gte_stclmv(r0_00);
    gte_ldclmv(pMVar3);
    gte_rtir();
    uVar15 = param_2[4];
    gte_stclmv(pMVar12);
    gte_ldclmv(pMVar10);
    gte_rtir();
    uVar18 = param_2[0x1c];
    pVVar8 = (VECTOR *)param_4->t;
    *(undefined4 *)(puVar17 + 0x40) = param_2[1];
    gte_stclmv(pMVar13);
    gte_ldlv0(pVVar8);
    gte_rt();
    iVar4 = param_2[3] + (uint)DAT_800ce5e0 * 0x28;
    iVar14 = param_2[5] + (uint)DAT_800ce5e0 * 0x34;
    gte_stlvnl((VECTOR *)(puVar17 + 0x24));
    if ((param_3 & 0x400) != 0) {
      *(int *)(puVar17 + 0x30) = (int)*(short *)(param_2 + 0x23);
      *(int *)(puVar17 + 0x34) = (int)*(short *)((int)param_2 + 0x8e);
      *(int *)(puVar17 + 0x38) = (int)*(short *)(param_2 + 0x24);
      ScaleMatrix(r0_00,(VECTOR *)(puVar17 + 0x30));
    }
    if ((param_3 & 0x2000) != 0) {
      *(int *)(puVar17 + 0x30) = (int)*(short *)(param_2 + 0x23);
      *(int *)(puVar17 + 0x34) = (int)*(short *)((int)param_2 + 0x8e);
      sVar1 = *(short *)(param_2 + 0x24);
      *(undefined4 *)(puVar17 + 0x10) = 0x1000;
      *(undefined4 *)(puVar17 + 0x14) = 0;
      *(undefined4 *)(puVar17 + 0x18) = 0x1000;
      *(undefined4 *)(puVar17 + 0x1c) = 0;
      *(undefined4 *)(puVar17 + 0x20) = 0x1000;
      *(int *)(puVar17 + 0x38) = (int)sVar1;
      ScaleMatrix(r0_00,(VECTOR *)(puVar17 + 0x30));
      MulMatrix0(r0_00,param_4,r0_00);
      lVar6 = param_4->t[1];
      lVar7 = param_4->t[2];
      uVar9 = *(undefined4 *)param_4[1].m[0];
      *(long *)(puVar17 + 0x24) = param_4->t[0];
      *(long *)(puVar17 + 0x28) = lVar6;
      *(long *)(puVar17 + 0x2c) = lVar7;
      *(undefined4 *)(puVar17 + 0x30) = uVar9;
      FUN_8002ce94(&DAT_800dcba8,r0_00,r0_00);
    }
    if (((param_3 & 0x8000) != 0) && (iVar5 = FUN_8002c820(param_2 + 0x17,DAT_800ce338), iVar5 == 0)
       ) {
      param_3 = param_3 | 0x4000;
    }
    pMVar3 = (MATRIX *)(puVar17 + 0x10);
    gte_SetRotMatrix(pMVar3);
    gte_SetTransMatrix(pMVar3);
    if ((param_3 & 0x4000) == 0) {
      if ((param_3 & 0x18) == 0) {
        uVar2 = *(uint *)(puVar17 + 0x40);
        FUN_80027bec(uVar11,iVar4,uVar18,(uVar2 & 2) >> 1);
        FUN_80027dbc(uVar15,iVar14,uVar18,(uVar2 & 2) >> 1);
        if ((param_3 & 0x80) == 0) {
          return;
        }
        FUN_8002940c(param_2 + 1,param_2[0x26],param_2[0x27]);
        return;
      }
      if ((param_3 & 8) != 0) {
        FUN_8002d3c8(uVar11,iVar4,param_2);
        FUN_8002d718(uVar15,iVar14,param_2);
      }
      if ((param_3 & 0x10) != 0) {
        uVar2 = *(uint *)(puVar17 + 0x40);
        FUN_8002da80(uVar11,iVar4,param_2,(uVar2 & 2) >> 1);
        FUN_8002ddf0(uVar15,iVar14,param_2,(uVar2 & 2) >> 1);
      }
    }
    if ((param_3 & 8) == 0) {
      if ((param_3 & 0x10) != 0) {
        FUN_80028ea4(param_2);
      }
    }
    else {
      FUN_80028dac(param_2);
    }
  }
  return;
}


