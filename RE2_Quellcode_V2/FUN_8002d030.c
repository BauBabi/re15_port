/* FUN_8002d030 @ 0x8002d030  (Ghidra headless, raw MIPS overlay) */

void FUN_8002d030(MATRIX *param_1,MATRIX *param_2,MATRIX *param_3)

{
  MATRIX MStack_40;
  VECTOR local_20;
  
  TransposeMatrix(param_1,&MStack_40);
  MulMatrix0(&MStack_40,param_2,param_3);
  local_20.vx = param_2->t[0] - param_1->t[0];
  local_20.vy = param_2->t[1] - param_1->t[1];
  local_20.vz = param_2->t[2] - param_1->t[2];
  ApplyMatrixLV(&MStack_40,&local_20,&local_20);
  param_3->t[0] = local_20.vx;
  param_3->t[1] = local_20.vy;
  param_3->t[2] = local_20.vz;
  return;
}


