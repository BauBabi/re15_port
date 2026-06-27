void FUN_80029e84(uint param_1,int param_2)

{
  int iVar1;
  
  iVar1 = (param_1 & 0xffff) * 8;
  psRam00000000 = *(short **)(&DAT_80073c60 + iVar1);
  sRam00000004 = *psRam00000000;
  sRam00000006 = *psRam00000000 + psRam00000000[2];
  iRam0000000c = (uint)(ushort)psRam00000000[5] * 0x12;
  sRam00000008 = psRam00000000[6];
  sRam0000000a = psRam00000000[7];
  iRam00000010 = param_2 + 0x8000;
  uRam0000001c = 0;
  iRam00000014 = iRam00000010;
  iRam00000018 = param_2;
  FUN_80013b60(*(undefined4 *)(&DAT_80073c64 + iVar1),iRam00000010,0);
  return;
}
