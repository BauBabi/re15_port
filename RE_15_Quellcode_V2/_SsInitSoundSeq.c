undefined4 _SsInitSoundSeq(ushort param_1,undefined2 param_2,char *param_3)

{
  char cVar1;
  byte bVar2;
  byte bVar3;
  undefined4 uVar4;
  uint uVar5;
  undefined2 uVar6;
  int iVar7;
  short sVar8;
  int iVar9;
  byte *pbVar10;
  int iVar11;
  uint uVar12;
  
  iVar7 = 0;
  iVar11 = *(int *)((int)&DAT_800bb500 + ((int)((uint)param_1 << 0x10) >> 0xe));
  *(undefined2 *)(iVar11 + 0x4c) = param_2;
  *(undefined2 *)(iVar11 + 0x4a) = 0;
  *(undefined1 *)(iVar11 + 0x13) = 0;
  *(undefined1 *)(iVar11 + 0x14) = 0;
  *(undefined1 *)(iVar11 + 0x29) = 0;
  *(undefined1 *)(iVar11 + 0x15) = 0;
  *(undefined1 *)(iVar11 + 0x16) = 0;
  *(undefined1 *)(iVar11 + 0x2a) = 0;
  *(undefined1 *)(iVar11 + 0x12) = 0;
  *(undefined4 *)(iVar11 + 0x7c) = 0;
  *(undefined4 *)(iVar11 + 0x80) = 0;
  *(undefined4 *)(iVar11 + 0x84) = 0;
  *(undefined2 *)(iVar11 + 0x72) = 0;
  *(undefined2 *)(iVar11 + 0x48) = 0;
  *(undefined1 *)(iVar11 + 0x2b) = 0;
  *(undefined4 *)(iVar11 + 0x88) = 0;
  *(undefined1 *)(iVar11 + 0x27) = 0;
  *(undefined1 *)(iVar11 + 0x28) = 0;
  *(undefined1 *)(iVar11 + 0x10) = 0;
  *(undefined1 *)(iVar11 + 0x11) = 0;
  *(undefined2 *)(iVar11 + 0xa8) = 0x7f;
  *(undefined2 *)(iVar11 + 0xaa) = 0;
  iVar9 = iVar11;
  do {
    *(char *)(iVar11 + iVar7 + 0x2c) = (char)iVar7;
                    /* Possible PsyQ macro: setLineF2() */
    *(undefined1 *)(iVar11 + iVar7 + 0x17) = 0x40;
    *(undefined2 *)(iVar9 + 0x4e) = 0x7f;
    iVar7 = iVar7 + 1;
    iVar9 = iVar9 + 2;
  } while (iVar7 < 0x10);
  *(undefined2 *)(iVar11 + 0x6e) = 1;
  *(char **)(iVar11 + 4) = param_3;
  if ((*param_3 != 'S') && (*param_3 != 'p')) {
    printf("This is an old SEQ Data Format.\n");
    uVar4 = SEQINIT_OBJ_314();
    return uVar4;
  }
  *(char **)(iVar11 + 4) = param_3 + 8;
  if (param_3[7] != '\x01') {
    printf("This is not SEQ Data.\n");
    uVar4 = SEQINIT_OBJ_314();
    return uVar4;
  }
  *(char **)(iVar11 + 4) = param_3 + 9;
  cVar1 = param_3[8];
  *(char **)(iVar11 + 4) = param_3 + 10;
  pbVar10 = *(byte **)(iVar11 + 4);
  *(ushort *)(iVar11 + 0x4a) = CONCAT11(cVar1,param_3[9]);
  *(byte **)(iVar11 + 4) = pbVar10 + 1;
  bVar2 = *pbVar10;
  *(byte **)(iVar11 + 4) = pbVar10 + 2;
  bVar3 = pbVar10[1];
  *(byte **)(iVar11 + 4) = pbVar10 + 3;
  uVar5 = (uint)bVar2 << 0x10 | (uint)bVar3 << 8 | (uint)pbVar10[2];
  if (uVar5 == 0) {
    trap(0x1c00);
  }
  *(uint *)(iVar11 + 0x84) = uVar5;
  if (60000000U % uVar5 <= uVar5 >> 1) {
    *(uint *)(iVar11 + 0x84) = 60000000 / uVar5;
    *(undefined4 *)(iVar11 + 0x8c) = *(undefined4 *)(iVar11 + 0x84);
    *(int *)(iVar11 + 4) = *(int *)(iVar11 + 4) + 2;
    uVar4 = _SsReadDeltaValue((int)(short)param_1,0);
    uVar12 = (int)*(short *)(iVar11 + 0x4a) * *(int *)(iVar11 + 0x84);
    *(undefined4 *)(iVar11 + 0xc) = *(undefined4 *)(iVar11 + 4);
    iVar9 = DAT_800bb4bc;
    *(undefined4 *)(iVar11 + 0x7c) = uVar4;
    *(undefined4 *)(iVar11 + 0x88) = uVar4;
    *(undefined4 *)(iVar11 + 8) = *(undefined4 *)(iVar11 + 4);
    uVar5 = iVar9 * 0x3c;
    if (uVar5 <= uVar12 * 10) {
      if (uVar5 == 0) {
        trap(0x1c00);
      }
      if (uVar5 == 0) {
        trap(0x1c00);
      }
      *(undefined2 *)(iVar11 + 0x6e) = 0xffff;
      sVar8 = (short)((uint)((int)*(short *)(iVar11 + 0x4a) * *(int *)(iVar11 + 0x84) * 10) / uVar5)
      ;
      *(short *)(iVar11 + 0x70) = sVar8;
      if ((uint)(iVar9 * 0x1e) <
          (uint)((int)*(short *)(iVar11 + 0x4a) * *(int *)(iVar11 + 0x84) * 10) % uVar5) {
        *(short *)(iVar11 + 0x70) = sVar8 + 1;
      }
      *(undefined2 *)(iVar11 + 0x72) = *(undefined2 *)(iVar11 + 0x70);
      return 0;
    }
    if (uVar12 == 0) {
      trap(0x1c00);
    }
    uVar6 = (undefined2)((uint)(iVar9 * 600) / uVar12);
    *(undefined2 *)(iVar11 + 0x6e) = uVar6;
    *(undefined2 *)(iVar11 + 0x70) = uVar6;
    uVar4 = SEQINIT_OBJ_308();
    return uVar4;
  }
  *(uint *)(iVar11 + 0x84) = 60000000 / uVar5 + 1;
  uVar4 = SEQINIT_OBJ_1D0();
  return uVar4;
}
