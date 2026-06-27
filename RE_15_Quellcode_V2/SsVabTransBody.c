short SsVabTransBody(uchar *param_1,short param_2)

{
  short sVar1;
  ulong addr;
  int iVar2;
  
  if ((ushort)param_2 < 0x11) {
    iVar2 = (int)param_2;
    if ((&DAT_800b5334)[iVar2] == '\x02') {
      addr = *(ulong *)(&DAT_800bbe08 + iVar2 * 4);
      SpuSetTransferMode(0);
      SpuSetTransferStartAddr(addr);
      SpuRead(param_1,*(ulong *)(&DAT_800bbda8 + iVar2 * 4));
      (&DAT_800b5334)[iVar2] = 1;
      sVar1 = VS_VTB_OBJ_A0();
      return sVar1;
    }
  }
  FUN_8005a970(0);
  return -1;
}
