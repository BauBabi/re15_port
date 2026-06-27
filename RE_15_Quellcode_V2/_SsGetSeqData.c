void _SsGetSeqData(short param_1,short param_2)

{
  byte bVar1;
  undefined1 uVar2;
  undefined4 uVar3;
  undefined1 *puVar4;
  byte bVar5;
  byte *pbVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  
  iVar9 = (int)param_1;
  iVar8 = (int)param_2;
  iVar7 = iVar8 * 0xac + (&DAT_800bb500)[iVar9];
  pbVar6 = *(byte **)(iVar7 + 4);
  *(byte **)(iVar7 + 4) = pbVar6 + 1;
  bVar1 = *pbVar6;
  if ((bVar1 & 0x80) == 0) {
    bVar5 = *(byte *)(iVar7 + 0x11);
    if (bVar5 == 0xc0) {
      _SsSetProgramChange(iVar9,iVar8,bVar1);
      SEQREAD_OBJ_398();
      return;
    }
    if (0xc0 < bVar5) {
      if (bVar5 == 0xe0) {
        _SsSetPitchBend(iVar9,iVar8);
        SEQREAD_OBJ_398();
        return;
      }
      if (bVar5 != 0xff) {
        SEQREAD_OBJ_398();
        return;
      }
      _SsGetMetaEvent(iVar9,iVar8,bVar1);
      return;
    }
    if (bVar5 == 0x90) {
      *(byte **)(iVar7 + 4) = pbVar6 + 2;
      bVar5 = pbVar6[1];
      uVar3 = _SsReadDeltaValue(iVar9,iVar8);
      *(undefined4 *)(iVar7 + 0x88) = uVar3;
      _SsNoteOn(iVar9,iVar8,bVar1,bVar5);
      SEQREAD_OBJ_398();
      return;
    }
    if (bVar5 != 0xb0) {
      SEQREAD_OBJ_398();
      return;
    }
    _SsSetControlChange(iVar9,iVar8,bVar1);
    SEQREAD_OBJ_398();
    return;
  }
  bVar5 = bVar1 & 0xf0;
  *(byte *)(iVar7 + 0x12) = bVar1 & 0xf;
  if (bVar5 == 0xc0) {
    puVar4 = *(undefined1 **)(iVar7 + 4);
    *(undefined1 *)(iVar7 + 0x11) = 0xc0;
    *(undefined1 **)(iVar7 + 4) = puVar4 + 1;
    _SsSetProgramChange(iVar9,iVar8,*puVar4);
    SEQREAD_OBJ_398();
    return;
  }
  if (0xc0 < bVar5) {
    if (bVar5 == 0xe0) {
      *(undefined1 *)(iVar7 + 0x11) = 0xe0;
      *(int *)(iVar7 + 4) = *(int *)(iVar7 + 4) + 1;
      _SsSetPitchBend(iVar9,iVar8);
      SEQREAD_OBJ_398();
      return;
    }
    if (bVar5 != 0xf0) {
      SEQREAD_OBJ_398();
      return;
    }
    puVar4 = *(undefined1 **)(iVar7 + 4);
    *(undefined1 *)(iVar7 + 0x11) = 0xff;
    *(byte *)(iVar7 + 0x12) = bVar1 & 0xf;
    *(undefined1 **)(iVar7 + 4) = puVar4 + 1;
    SEQREAD_OBJ_390(iVar9,iVar8,*puVar4);
    return;
  }
  if (bVar5 == 0x90) {
    puVar4 = *(undefined1 **)(iVar7 + 4);
    *(undefined1 *)(iVar7 + 0x11) = 0x90;
    *(undefined1 **)(iVar7 + 4) = puVar4 + 1;
    uVar2 = *puVar4;
    *(undefined1 **)(iVar7 + 4) = puVar4 + 2;
    uVar3 = _SsReadDeltaValue(iVar9,iVar8);
    *(undefined4 *)(iVar7 + 0x88) = uVar3;
    SEQREAD_OBJ_338(iVar9,iVar8,uVar2);
    return;
  }
  if (bVar5 != 0xb0) {
    SEQREAD_OBJ_398();
    return;
  }
  puVar4 = *(undefined1 **)(iVar7 + 4);
  *(undefined1 *)(iVar7 + 0x11) = 0xb0;
  *(undefined1 **)(iVar7 + 4) = puVar4 + 1;
  _SsSetControlChange(iVar9,iVar8,*puVar4);
  SEQREAD_OBJ_398();
  return;
}
