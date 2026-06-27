short SsUtGetDetVVol(short param_1,short *param_2,short *param_3)

{
  short sVar1;
  int iVar2;
  
  if (0x17 < (ushort)param_1) {
    sVar1 = VMANAGER_OBJ_4A40();
    return sVar1;
  }
  iVar2 = (int)((uint)(ushort)param_1 << 0x10) >> 0xc;
  *param_2 = *(short *)((int)&VOICE_00_LEFT_RIGHT + iVar2);
  *param_3 = *(short *)((int)&VOICE_00_LEFT_RIGHT + iVar2 + 2);
  return 0;
}
