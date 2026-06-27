void _SsSeqPlay(short param_1,char param_2,short param_3)

{
  short sVar1;
  short sVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined3 in_register_00000015;
  int iVar6;
  
  iVar3 = (int)(short)CONCAT31(in_register_00000015,param_2);
  iVar6 = iVar3 * 0xac +
          *(int *)((int)&DAT_800bb500 + ((int)((uint)(ushort)param_1 << 0x10) >> 0xe));
  sVar1 = *(short *)(iVar6 + 0x70);
  iVar4 = *(int *)(iVar6 + 0x88);
  iVar5 = iVar4 - sVar1;
  if (iVar5 < 1) {
    if (iVar4 <= sVar1) {
      do {
        do {
          _SsGetSeqData((int)((uint)(ushort)param_1 << 0x10) >> 0x10,iVar3);
        } while (*(int *)(iVar6 + 0x88) == 0);
        iVar4 = iVar4 + *(int *)(iVar6 + 0x88);
      } while (iVar4 < *(short *)(iVar6 + 0x70));
      *(int *)(iVar6 + 0x88) = iVar4 - *(short *)(iVar6 + 0x70);
    }
    return;
  }
  sVar2 = *(short *)(iVar6 + 0x6e);
  if (0 < sVar2) {
    *(short *)(iVar6 + 0x6e) = sVar2 + -1;
    SEQREAD_OBJ_F0();
    return;
  }
  if (sVar2 == 0) {
    *(short *)(iVar6 + 0x6e) = sVar1;
    SEQREAD_OBJ_EC();
    return;
  }
  *(int *)(iVar6 + 0x88) = iVar5;
  SEQREAD_OBJ_F0();
  return;
}
