/* SsVabFakeBody @ 0x800852c8  (Ghidra headless, raw MIPS overlay) */

short SsVabFakeBody(short param_1)

{
  short sVar1;
  
  if (((ushort)param_1 < 0x11) && ((&DAT_800dcc68)[param_1] == '\x02')) {
    FUN_80082710(0);
    (&DAT_800dcc68)[param_1] = 1;
    sVar1 = VS_VFB_OBJ_5C();
    return sVar1;
  }
  return -1;
}


