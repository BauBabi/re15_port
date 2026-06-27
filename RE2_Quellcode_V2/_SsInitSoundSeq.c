/* _SsInitSoundSeq @ 0x8007c6e8  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Removing unreachable block (ram,0x8007c894) */

undefined4 _SsInitSoundSeq(ushort param_1,undefined1 param_2,char *param_3)

{
  char cVar1;
  byte bVar2;
  byte bVar3;
  undefined4 uVar4;
  uint uVar5;
  int iVar6;
  undefined1 *puVar7;
  undefined1 *puVar8;
  short sVar9;
  undefined4 *puVar10;
  byte *pbVar11;
  undefined4 *puVar12;
  uint uVar13;
  
  iVar6 = 0;
  puVar12 = *(undefined4 **)((int)&DAT_800ea250 + ((int)((uint)param_1 << 0x10) >> 0xe));
  *(undefined1 *)((int)puVar12 + 0x26) = param_2;
  *(undefined2 *)(puVar12 + 0x14) = 0;
  *(undefined1 *)(puVar12 + 6) = 0;
  *(undefined1 *)((int)puVar12 + 0x19) = 0;
  *(undefined1 *)((int)puVar12 + 0x1e) = 0;
  *(undefined1 *)((int)puVar12 + 0x1a) = 0;
  *(undefined1 *)((int)puVar12 + 0x1b) = 0;
  *(undefined1 *)((int)puVar12 + 0x1f) = 0;
  *(undefined1 *)((int)puVar12 + 0x17) = 0;
  puVar12[0x21] = 0;
  puVar12[0x22] = 0;
  puVar12[0x23] = 0;
  *(undefined2 *)((int)puVar12 + 0x56) = 0;
  *(undefined1 *)((int)puVar12 + 0x21) = 0;
  *(undefined1 *)(puVar12 + 8) = 1;
  *(undefined1 *)(puVar12 + 5) = 0;
  puVar12[0x24] = 0;
  *(undefined1 *)(puVar12 + 7) = 0;
  *(undefined1 *)((int)puVar12 + 0x1d) = 0;
  *(undefined1 *)((int)puVar12 + 0x15) = 0;
  *(undefined1 *)((int)puVar12 + 0x16) = 0;
  *(undefined2 *)(puVar12 + 0x20) = 0;
  *(undefined1 *)(puVar12 + 9) = 0;
  *(undefined1 *)((int)puVar12 + 0x25) = 0;
  puVar10 = puVar12;
  do {
    *(char *)((int)puVar12 + iVar6 + 0x37) = (char)iVar6;
                    /* Possible PsyQ macro: setLineF2() */
    *(undefined1 *)((int)puVar12 + iVar6 + 0x27) = 0x40;
                    /* Possible PsyQ macro: setSprt16() + setSemiTrans(sprt16, 1) +
                       setShadeTex(sprt16, 1) */
    *(undefined2 *)(puVar10 + 0x18) = 0x7f;
    iVar6 = iVar6 + 1;
    puVar10 = (undefined4 *)((int)puVar10 + 2);
  } while (iVar6 < 0x10);
  *(undefined2 *)((int)puVar12 + 0x52) = 1;
  *puVar12 = param_3;
  if ((*param_3 != 'S') && (*param_3 != 'p')) {
    printf("This is an old SEQ Data Format.\n");
    uVar4 = SEQINIT_OBJ_338();
    return uVar4;
  }
  *puVar12 = param_3 + 8;
  if (param_3[7] != '\x01') {
    printf("This is not SEQ Data.\n");
    uVar4 = SEQINIT_OBJ_338();
    return uVar4;
  }
  *puVar12 = param_3 + 9;
  cVar1 = param_3[8];
  *puVar12 = param_3 + 10;
  pbVar11 = (byte *)*puVar12;
  *(ushort *)(puVar12 + 0x14) = CONCAT11(cVar1,param_3[9]);
  *puVar12 = pbVar11 + 1;
  bVar2 = *pbVar11;
  *puVar12 = pbVar11 + 2;
  bVar3 = pbVar11[1];
  *puVar12 = pbVar11 + 3;
  uVar5 = (uint)bVar2 << 0x10 | (uint)bVar3 << 8 | (uint)pbVar11[2];
  if (uVar5 == 0) {
    trap(0x1c00);
  }
  puVar12[0x23] = uVar5;
  if (60000000U % uVar5 <= uVar5 >> 1) {
    puVar12[0x23] = 60000000 / uVar5;
    puVar7 = (undefined1 *)*puVar12;
    puVar12[0x25] = puVar12[0x23];
    *puVar12 = puVar7 + 1;
    puVar8 = (undefined1 *)*puVar12;
    *(undefined1 *)(puVar12 + 9) = *puVar7;
    *puVar12 = puVar8 + 1;
    *(undefined1 *)((int)puVar12 + 0x25) = *puVar8;
    uVar4 = _SsReadDeltaValue((int)(short)param_1,0);
    uVar13 = (int)*(short *)(puVar12 + 0x14) * puVar12[0x23];
    puVar12[2] = *puVar12;
    iVar6 = DAT_800ea228;
    puVar12[0x21] = uVar4;
    puVar12[0x24] = uVar4;
    puVar12[4] = 0;
    puVar12[3] = *puVar12;
    puVar12[1] = *puVar12;
    uVar5 = iVar6 * 0x3c;
    if (uVar5 <= uVar13 * 10) {
      if (uVar5 == 0) {
        trap(0x1c00);
      }
      if (uVar5 == 0) {
        trap(0x1c00);
      }
      *(undefined2 *)((int)puVar12 + 0x52) = 0xffff;
      sVar9 = (short)((uint)((int)*(short *)(puVar12 + 0x14) * puVar12[0x23] * 10) / uVar5);
      *(short *)(puVar12 + 0x15) = sVar9;
      if ((uint)(iVar6 * 0x1e) <
          (uint)((int)*(short *)(puVar12 + 0x14) * puVar12[0x23] * 10) % uVar5) {
        *(short *)(puVar12 + 0x15) = sVar9 + 1;
      }
      *(undefined2 *)((int)puVar12 + 0x56) = *(undefined2 *)(puVar12 + 0x15);
      return 0;
    }
    if (uVar13 == 0) {
      trap(0x1c00);
    }
    *(short *)((int)puVar12 + 0x52) = (short)((uint)(iVar6 * 600) / uVar13);
    uVar4 = SEQINIT_OBJ_328();
    return uVar4;
  }
  puVar12[0x23] = 60000000 / uVar5 + 1;
  uVar4 = SEQINIT_OBJ_1D8();
  return uVar4;
}


