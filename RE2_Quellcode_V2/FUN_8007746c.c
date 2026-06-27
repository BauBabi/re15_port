/* FUN_8007746c @ 0x8007746c  (Ghidra headless, raw MIPS overlay) */

void FUN_8007746c(long *param_1,SVECTOR *param_2,MATRIX *param_3,SVECTOR *param_4)

{
  int iVar1;
  long lVar2;
  int iVar3;
  int x;
  int iVar4;
  int y;
  MATRIX MStack_68;
  MATRIX MStack_48;
  SVECTOR local_28;
  VECTOR local_20;
  
  MStack_68.t[0] = *param_1;
  MStack_68.t[1] = param_1[1];
  MStack_68.t[2] = param_1[2];
  RotMatrix(param_4,&MStack_68);
  MulMatrix2(&MStack_68,param_3);
  MatrixNormal(param_3,param_3);
  iVar1 = (int)param_3->m[0][2];
  iVar3 = (int)param_3->m[1][2];
  x = (int)param_3->m[2][2];
  iVar4 = iVar1 * iVar1 + iVar3 * iVar3 + x * x;
  if (iVar4 < 0) {
    iVar4 = iVar4 + 0xfff;
  }
  iVar4 = iVar4 >> 0xc;
  y = (iVar1 << 0xc) / iVar4;
  if (iVar4 == 0) {
    trap(0x1c00);
  }
  if ((iVar4 == -1) && (iVar1 << 0xc == -0x80000000)) {
    trap(0x1800);
  }
  lVar2 = ratan2(-iVar3,x);
  iVar1 = y * y;
  param_2->vx = (short)lVar2;
  if (iVar1 < 0) {
    iVar1 = iVar1 + 0xfff;
  }
  lVar2 = SquareRoot12(0x1000 - (iVar1 >> 0xc));
  lVar2 = ratan2(y,lVar2);
  param_2->vy = (short)lVar2;
  param_2->vz = 0;
  MStack_68.t[0] = 0;
  MStack_68.t[1] = 0;
  MStack_68.t[2] = 0;
  RotMatrix(param_2,&MStack_68);
  TransposeMatrix(&MStack_68,&MStack_48);
  local_28.vx = param_3->m[0][0];
  local_28.vy = param_3->m[1][0];
  local_28.vz = param_3->m[2][0];
  ApplyMatrix(&MStack_48,&local_28,&local_20);
  lVar2 = ratan2(local_20.vy,local_20.vx);
  param_2->vz = (short)lVar2;
  return;
}


