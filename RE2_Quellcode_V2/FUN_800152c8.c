/* FUN_800152c8 @ 0x800152c8  (Ghidra headless, raw MIPS overlay) */

void FUN_800152c8(int param_1,short param_2)

{
  SVECTOR local_38;
  MATRIX MStack_30;
  
  local_38.vx = 0;
  local_38.vz = 0;
  local_38.vy = *(short *)(param_1 + 0x76) + param_2;
  RotMatrix(&local_38,&MStack_30);
  ApplyMatrixSV(&MStack_30,(SVECTOR *)(param_1 + 0x144),&local_38);
  *(int *)(param_1 + 0x38) = *(int *)(param_1 + 0x38) + (int)local_38.vx;
  *(int *)(param_1 + 0x40) = *(int *)(param_1 + 0x40) + (int)local_38.vz;
  return;
}


