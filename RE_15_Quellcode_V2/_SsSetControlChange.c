void _SsSetControlChange(ushort param_1,short param_2,byte param_3)

{
  byte bVar1;
  undefined4 uVar2;
  byte *pbVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  
  iVar6 = param_2 * 0xac + *(int *)((int)&DAT_800bb500 + ((int)((uint)param_1 << 0x10) >> 0xe));
  pbVar3 = *(byte **)(iVar6 + 4);
  uVar4 = (uint)*(byte *)(iVar6 + 0x12);
  *(byte **)(iVar6 + 4) = pbVar3 + 1;
  bVar1 = *pbVar3;
  if (param_3 < 0x7a) {
    switch(param_3) {
    case 0:
      *(ushort *)(iVar6 + 0x4c) = (ushort)bVar1;
      SEQREAD_OBJ_804((int)(short)param_1,(int)param_2);
      return;
    case 6:
      _SsContDataEntry((int)(short)param_1,(int)param_2,bVar1);
      SEQREAD_OBJ_810();
      return;
    case 7:
      SpuVmSetVol((int)param_2 << 8 | (int)(short)param_1,(int)*(short *)(iVar6 + 0x4c),
                  *(undefined1 *)(iVar6 + uVar4 + 0x2c),bVar1,*(undefined1 *)(iVar6 + uVar4 + 0x17))
      ;
      *(ushort *)(uVar4 * 2 + iVar6 + 0x4e) = (ushort)bVar1;
      SEQREAD_OBJ_804((int)(short)param_1,(int)param_2);
      return;
    case 10:
      SpuVmSetVol((int)param_2 << 8 | (int)(short)param_1,(int)*(short *)(iVar6 + 0x4c),
                  *(undefined1 *)(iVar6 + uVar4 + 0x2c),(int)*(short *)(uVar4 * 2 + iVar6 + 0x4e),
                  bVar1);
      *(byte *)(iVar6 + uVar4 + 0x17) = bVar1;
      SEQREAD_OBJ_804((int)(short)param_1,(int)param_2);
      return;
    case 0xb:
      iVar5 = iVar6 + uVar4;
      SpuVmSetProgVol((int)*(short *)(iVar6 + 0x4c),*(undefined1 *)(iVar5 + 0x2c),bVar1);
      SpuVmSetVol((int)param_2 << 8 | (int)(short)param_1,(int)*(short *)(iVar6 + 0x4c),
                  *(undefined1 *)(iVar5 + 0x2c),(int)*(short *)(uVar4 * 2 + iVar6 + 0x4e),
                  *(undefined1 *)(iVar5 + 0x17));
      SEQREAD_OBJ_804((int)(short)param_1,(int)param_2);
      return;
    case 0x40:
      if (bVar1 < 0x40) {
        FUN_8005fb00();
        SEQREAD_OBJ_7F8((uint)param_1 << 0x10);
        return;
      }
      FUN_8005f89c();
      SEQREAD_OBJ_7F8((uint)param_1 << 0x10);
      return;
    case 0x41:
      _SsContPortamento((int)(short)param_1,(int)param_2,bVar1);
      SEQREAD_OBJ_810();
      return;
    case 0x5b:
      SsUtSetReverbDepth((ushort)bVar1,(ushort)bVar1);
      SEQREAD_OBJ_7F8((uint)param_1 << 0x10);
      return;
    case 0x62:
      _SsContNrpn1((int)(short)param_1,(int)param_2,bVar1);
      SEQREAD_OBJ_810();
      return;
    case 99:
      _SsContNrpn2((int)(short)param_1,(int)param_2,bVar1);
      SEQREAD_OBJ_810();
      return;
    case 100:
      _SsContRpn1((int)(short)param_1,(int)param_2,bVar1);
      SEQREAD_OBJ_810();
      return;
    case 0x65:
      _SsContRpn2((int)(short)param_1,(int)param_2,bVar1);
      SEQREAD_OBJ_810();
      return;
    case 0x79:
      _SsContResetAll((int)(short)param_1,(int)param_2);
      SEQREAD_OBJ_810();
      return;
    }
  }
  uVar2 = _SsReadDeltaValue((int)(short)param_1,(int)param_2);
  *(undefined4 *)(iVar6 + 0x88) = uVar2;
  return;
}
