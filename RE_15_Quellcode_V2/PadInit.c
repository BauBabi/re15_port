void PadInit(int mode)

{
  DAT_8008fdf4 = 0xffffffff;
  DAT_800b3f84 = mode;
  ResetCallback();
  PAD_init(0x20000001,&DAT_8008fdf4);
  ChangeClearPAD(0);
  return;
}
