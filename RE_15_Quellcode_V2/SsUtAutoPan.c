short SsUtAutoPan(short param_1,short param_2,short param_3,short param_4)

{
  short sVar1;
  
  if (0x17 < (ushort)param_1) {
    sVar1 = VMANAGER_OBJ_4C64();
    return sVar1;
  }
  SeAutoPan((int)param_1,(int)param_2,(int)param_3,(int)param_4);
  return 0;
}
