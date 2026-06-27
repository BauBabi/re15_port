/* FUN_80028dac @ 0x80028dac  (Ghidra headless, raw MIPS overlay) */

void FUN_80028dac(undefined4 *param_1)

{
  short sVar1;
  MATRIX MStack_40;
  SVECTOR local_20;
  SVECTOR local_18;
  
  if ((*(ushort *)(param_1 + 0x28) & 0x7fff) < 0x1d) {
    *(ushort *)(param_1 + 0x28) = *(ushort *)(param_1 + 0x28) + 1;
  }
  else {
    *param_1 = 0;
  }
  local_20.vy = *(short *)(param_1 + 0x26);
  sVar1 = *(short *)((int)param_1 + 0x9a) + *(short *)((int)param_1 + 0x9e);
  *(short *)((int)param_1 + 0x9a) = sVar1;
  param_1[0x18] = param_1[0x18] + (int)sVar1;
  local_20.vz = 0;
  local_20.vx = 0;
  RotMatrix(&local_20,&MStack_40);
  sVar1 = *(short *)(param_1 + 0x27);
  *(short *)(param_1 + 0x27) = sVar1 + *(short *)(param_1 + 0x29);
  if ((int)((uint)(ushort)(sVar1 + *(short *)(param_1 + 0x29)) << 0x10) < 0) {
    *(undefined2 *)(param_1 + 0x27) = 0;
  }
  local_20.vx = *(short *)(param_1 + 0x27);
  local_20.vy = 0;
  ApplyMatrixSV(&MStack_40,&local_20,&local_18);
  param_1[0x17] = param_1[0x17] + (int)local_18.vx;
  param_1[0x19] = param_1[0x19] + (int)local_18.vz;
  return;
}


