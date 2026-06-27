/* FUN_80015350 @ 0x80015350  (Ghidra headless, raw MIPS overlay) */

void FUN_80015350(int param_1,short param_2,short param_3)

{
  SVECTOR local_38;
  MATRIX MStack_30;
  
  local_38.vx = 0;
  local_38.vy = *(short *)(param_1 + 0x76) + param_2;
  local_38.vz = *(short *)(param_1 + 0x78) + param_3;
  RotMatrix(&local_38,&MStack_30);
  ApplyMatrixSV(&MStack_30,(SVECTOR *)(param_1 + 0x144),&local_38);
  *(int *)(param_1 + 0x38) = *(int *)(param_1 + 0x38) + (int)local_38.vx;
  *(int *)(param_1 + 0x3c) = *(int *)(param_1 + 0x3c) + (int)local_38.vy;
  *(int *)(param_1 + 0x40) = *(int *)(param_1 + 0x40) + (int)local_38.vz;
  return;
}


