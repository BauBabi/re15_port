void _SsGetMetaEvent(short param_1,short param_2,char param_3)

{
  byte bVar1;
  byte bVar2;
  undefined4 uVar3;
  undefined2 uVar4;
  short sVar5;
  int iVar6;
  byte *pbVar7;
  int *piVar8;
  undefined1 *puVar9;
  int iVar10;
  uint uVar11;
  uint uVar12;
  uint uVar13;
  
  uVar11 = (uint)param_1;
  piVar8 = &DAT_800bb500 + uVar11;
  iVar10 = (int)param_2;
  iVar6 = iVar10 * 0xac;
  puVar9 = (undefined1 *)(iVar6 + *piVar8);
  if (param_3 == '/') {
    sVar5 = *(short *)(puVar9 + 0x48);
    *(short *)(puVar9 + 0x48) = sVar5 + 1;
    if (*(short *)(puVar9 + 0x46) == 0) {
      *(undefined4 *)(puVar9 + 0x80) = 0;
      puVar9[0x27] = 0;
      *(undefined4 *)(puVar9 + 0x88) = 0;
      *(undefined4 *)(puVar9 + 4) = *(undefined4 *)(puVar9 + 8);
      SEQREAD_OBJ_1C84();
      return;
    }
    if ((short)(sVar5 + 1) < *(short *)(puVar9 + 0x46)) {
      *(undefined4 *)(puVar9 + 0x80) = 0;
      puVar9[0x27] = 0;
      *(undefined4 *)(puVar9 + 0x88) = 0;
      *(undefined4 *)(puVar9 + 4) = *(undefined4 *)(puVar9 + 8);
      *(undefined4 *)(puVar9 + 0xc) = *(undefined4 *)(puVar9 + 8);
      SEQREAD_OBJ_1C84();
      return;
    }
    *(uint *)(iVar6 + *piVar8 + 0x90) = *(uint *)(iVar6 + *piVar8 + 0x90) & 0xfffffffe;
    *(uint *)(iVar6 + *piVar8 + 0x90) = *(uint *)(iVar6 + *piVar8 + 0x90) & 0xfffffff7;
    *(uint *)(iVar6 + *piVar8 + 0x90) = *(uint *)(iVar6 + *piVar8 + 0x90) & 0xfffffffd;
    *(uint *)(iVar6 + *piVar8 + 0x90) = *(uint *)(iVar6 + *piVar8 + 0x90) | 0x200;
    *(uint *)(iVar6 + *piVar8 + 0x90) = *(uint *)(iVar6 + *piVar8 + 0x90) | 4;
    puVar9[0x2b] = 0;
    *(undefined4 *)(puVar9 + 0xc) = *(undefined4 *)(puVar9 + 8);
    if (puVar9[0x3c] != -1) {
      puVar9[0x2b] = 0;
      _SsSndNextSep(puVar9[0x3c],*puVar9);
      SpuVmSeqKeyOff(iVar10 << 8 | uVar11);
    }
    SpuVmSeqKeyOff(iVar10 << 8 | uVar11);
    *(int *)(puVar9 + 0x88) = (int)*(short *)(puVar9 + 0x70);
  }
  else if (param_3 == 'Q') {
    pbVar7 = *(byte **)(puVar9 + 4);
    *(byte **)(puVar9 + 4) = pbVar7 + 1;
    bVar1 = *pbVar7;
    *(byte **)(puVar9 + 4) = pbVar7 + 2;
    bVar2 = pbVar7[1];
    *(byte **)(puVar9 + 4) = pbVar7 + 3;
    iVar6 = DAT_800bb4bc;
    uVar11 = (uint)bVar1 << 0x10 | (uint)bVar2 << 8 | (uint)pbVar7[2];
    uVar12 = 60000000 / uVar11;
    if (uVar11 == 0) {
      trap(0x1c00);
    }
    uVar13 = (int)*(short *)(puVar9 + 0x4a) * uVar12;
    uVar11 = DAT_800bb4bc * 0x3c;
    *(uint *)(puVar9 + 0x8c) = uVar12;
    if (uVar11 <= uVar13 * 10) {
      if (uVar11 == 0) {
        trap(0x1c00);
      }
      if (uVar11 == 0) {
        trap(0x1c00);
      }
      *(undefined2 *)(puVar9 + 0x6e) = 0xffff;
      sVar5 = (short)((uint)((int)*(short *)(puVar9 + 0x4a) * *(int *)(puVar9 + 0x8c) * 10) / uVar11
                     );
      *(short *)(puVar9 + 0x70) = sVar5;
      if ((uint)(iVar6 * 0x1e) <
          (uint)((int)*(short *)(puVar9 + 0x4a) * *(int *)(puVar9 + 0x8c) * 10) % uVar11) {
        *(short *)(puVar9 + 0x70) = sVar5 + 1;
      }
      uVar3 = _SsReadDeltaValue((int)param_1,(int)param_2);
      *(undefined4 *)(puVar9 + 0x88) = uVar3;
      SEQREAD_OBJ_1C84();
      return;
    }
    if (uVar13 == 0) {
      trap(0x1c00);
    }
    uVar4 = (undefined2)((uint)(iVar6 * 600) / uVar13);
    *(undefined2 *)(puVar9 + 0x6e) = uVar4;
    *(undefined2 *)(puVar9 + 0x70) = uVar4;
    SEQREAD_OBJ_1B2C();
    return;
  }
  return;
}
