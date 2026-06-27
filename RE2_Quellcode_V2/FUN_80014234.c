/* FUN_80014234 @ 0x80014234  (Ghidra headless, raw MIPS overlay) */

void FUN_80014234(void)

{
  ushort uVar1;
  uint uVar2;
  MATRIX *pMVar3;
  int iVar4;
  MATRIX *r0;
  int *piVar5;
  int *piVar6;
  undefined1 *puVar7;
  undefined1 auStack_40 [48];
  
  piVar5 = DAT_800c3a80;
  puVar7 = auStack_40;
  if (DAT_800ce5e0 == '\0') {
    iVar4 = *DAT_800c3a80;
  }
  else {
    iVar4 = *DAT_800c3a80 + 0x5000;
  }
  DAT_800c3a80[2] = iVar4;
  *(undefined2 *)(piVar5 + 0x8b) = 0;
  iVar4 = 0;
  piVar6 = DAT_800d4dd8 + 5;
  piVar5 = DAT_800d4dd8;
  do {
    if (*piVar5 != 0) {
      RotMatrix((SVECTOR *)(piVar5 + 0x1d),(MATRIX *)(piVar5 + 9));
      if ((*(ushort *)(piVar6 + 0x4c) & 0x1000) == 0) {
        gte_SetBackColor(0x44,0x44,0x44);
      }
      else {
        gte_SetBackColor(0x88,0x88,0x88);
      }
      gte_SetColorMatrix((MATRIX *)&DAT_8009a490);
      gte_SetRotMatrix((MATRIX *)&UNK_8009a470);
      r0 = (MATRIX *)(piVar5 + 0x15);
      gte_ldclmv(r0);
      gte_rtir();
      pMVar3 = (MATRIX *)(puVar7 + 0x10);
      gte_stclmv(pMVar3);
      gte_ldclmv((MATRIX *)((int)piVar5 + 0x56));
      gte_rtir();
      gte_stclmv((MATRIX *)(puVar7 + 0x12));
      gte_ldclmv((MATRIX *)(piVar5 + 0x16));
      gte_rtir();
      gte_stclmv((MATRIX *)(puVar7 + 0x14));
      gte_SetLightMatrix(pMVar3);
      if ((*(ushort *)(piVar6 + 0x4c) & 0x100) != 0) {
        uVar1 = (short)piVar6[0x4d] + 8;
        *(ushort *)(piVar6 + 0x4d) = uVar1 & 0x1ff;
        if ((uVar1 & 0x100) == 0) {
          uVar1 = uVar1 & 0xff;
        }
        else {
          uVar1 = 0x100 - (uVar1 & 0xff);
        }
        *(ushort *)((int)piVar6 + 0x136) = uVar1;
        uVar2 = (uint)*(ushort *)((int)piVar6 + 0x136);
        piVar6[0x1a] = uVar2 << 0x10 | uVar2 << 8 | uVar2;
      }
      pMVar3 = (MATRIX *)piVar6[0x1b];
      gte_SetRotMatrix(pMVar3);
      gte_ldclmv((MATRIX *)(piVar5 + 9));
      gte_rtir();
      gte_stclmv(r0);
      gte_ldclmv((MATRIX *)((int)piVar5 + 0x26));
      gte_rtir();
      gte_stclmv((MATRIX *)(r0->m[0] + 1));
      gte_ldclmv((MATRIX *)(piVar5 + 10));
      gte_rtir();
      gte_stclmv((MATRIX *)(r0->m[0] + 2));
      gte_SetTransMatrix(pMVar3);
      gte_ldlv0((VECTOR *)(piVar5 + 0xe));
      gte_rt();
      gte_stlvl((VECTOR *)r0->t);
      gte_SetRotMatrix(r0);
      gte_SetTransMatrix(r0);
      FUN_8001468c(piVar5 + 4,piVar5,*piVar6);
    }
    iVar4 = iVar4 + 1;
    piVar6 = piVar6 + 0x53;
    piVar5 = piVar5 + 0x53;
  } while (iVar4 < 10);
  return;
}


