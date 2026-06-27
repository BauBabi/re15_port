int SetGraphQueue(int mode)

{
  uint uVar1;
  
  uVar1 = (uint)DAT_8007e351;
  if (1 < DAT_8007e352) {
    printf("SetGrapQue(%d)...\n",mode);
  }
  if (mode != (uint)DAT_8007e351) {
    _reset(1);
    DAT_8007e351 = (byte)mode;
    DMACallback(2,0);
  }
  return uVar1;
}
