void _SsContPortaTime(ushort param_1,short param_2,uchar param_3)

{
  undefined4 uVar1;
  short sVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  ProgAtr local_60;
  VagAtr VStack_50;
  
  iVar6 = param_2 * 0xac + *(int *)((int)&DAT_800bb500 + ((int)((uint)param_1 << 0x10) >> 0xe));
  iVar3 = iVar6 + (uint)*(byte *)(iVar6 + 0x12);
  SsUtGetProgAtr(*(short *)(iVar6 + 0x4c),(ushort)*(byte *)(iVar3 + 0x2c),&local_60);
  iVar5 = 0;
  if (local_60.tones != 0) {
    iVar4 = 0;
    do {
      sVar2 = (short)((uint)iVar4 >> 0x10);
      SsUtGetVagAtr(*(short *)(iVar6 + 0x4c),(ushort)*(byte *)(iVar3 + 0x2c),sVar2,&VStack_50);
      VStack_50.porT = param_3;
      SsUtSetVagAtr(*(short *)(iVar6 + 0x4c),(ushort)*(byte *)(iVar3 + 0x2c),sVar2,&VStack_50);
      iVar5 = iVar5 + 1;
      iVar4 = iVar5 * 0x10000;
    } while (iVar5 < (int)(uint)local_60.tones);
  }
  uVar1 = _SsReadDeltaValue((int)(short)param_1,(int)param_2);
  *(undefined4 *)(iVar6 + 0x88) = uVar1;
  return;
}
