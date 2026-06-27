short SsUtGetVVol(short param_1,short *param_2,short *param_3)

{
  short sVar1;
  int iVar2;
  
  if (0x17 < (ushort)param_1) {
    sVar1 = VMANAGER_OBJ_4B38();
    return sVar1;
  }
  iVar2 = (int)((uint)(ushort)param_1 << 0x10) >> 0xc;
  sVar1 = *(short *)((int)&VOICE_00_LEFT_RIGHT + iVar2 + 2);
  *param_2 = *(short *)((int)&VOICE_00_LEFT_RIGHT + iVar2) / 0x81;
  *param_3 = sVar1 / 0x81;
  return 0;
}
