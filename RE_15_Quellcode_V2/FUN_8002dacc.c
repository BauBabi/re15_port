void FUN_8002dacc(int param_1,SVECTOR *param_2)

{
  long lVar1;
  int iVar2;
  int iVar3;
  int x;
  int iVar4;
  int y;
  MATRIX MStack_68;
  MATRIX MStack_48;
  SVECTOR local_28;
  VECTOR local_20;
  
  MStack_68.t[0] = *(long *)(param_1 + 0x34);
  MStack_68.t[1] = *(long *)(param_1 + 0x38);
  MStack_68.t[2] = *(long *)(param_1 + 0x3c);
  RotMatrix(param_2,&MStack_68);
  MulMatrix2(&MStack_68,(MATRIX *)(param_1 + 0x20));
  iVar2 = (int)*(short *)(param_1 + 0x24);
  iVar3 = (int)*(short *)(param_1 + 0x2a);
  x = (int)*(short *)(param_1 + 0x30);
  iVar4 = iVar2 * iVar2 + iVar3 * iVar3 + x * x;
  if (iVar4 < 0) {
    iVar4 = iVar4 + 0xfff;
  }
  iVar4 = iVar4 >> 0xc;
  y = (iVar2 << 0xc) / iVar4;
  if (iVar4 == 0) {
    trap(0x1c00);
  }
  if ((iVar4 == -1) && (iVar2 << 0xc == -0x80000000)) {
    trap(0x1800);
  }
  lVar1 = ratan2(-iVar3,x);
  iVar4 = y * y;
  *(short *)(param_1 + 0x68) = (short)lVar1;
  if (iVar4 < 0) {
    iVar4 = iVar4 + 0xfff;
  }
  lVar1 = SquareRoot12(0x1000 - (iVar4 >> 0xc));
  lVar1 = ratan2(y,lVar1);
  *(short *)(param_1 + 0x6a) = (short)lVar1;
  *(undefined2 *)(param_1 + 0x6c) = 0;
  MStack_68.t[0] = 0;
  MStack_68.t[1] = 0;
  MStack_68.t[2] = 0;
  RotMatrix((SVECTOR *)(param_1 + 0x68),&MStack_68);
  TransposeMatrix(&MStack_68,&MStack_48);
  local_28.vx = *(short *)(param_1 + 0x20);
  local_28.vy = *(short *)(param_1 + 0x26);
  local_28.vz = *(short *)(param_1 + 0x2c);
  ApplyMatrix(&MStack_48,&local_28,&local_20);
  lVar1 = ratan2(local_20.vy,local_20.vx);
  *(short *)(param_1 + 0x6c) = (short)lVar1;
  return;
}
