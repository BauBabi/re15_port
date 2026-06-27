void FUN_8001e8c8(uint *param_1)

{
  bool bVar1;
  int iVar2;
  char cVar3;
  uint uVar4;
  VECTOR local_20;
  
  uVar4 = param_1[0x62];
  RotMatrix((SVECTOR *)(DAT_800ac784 + 0x68),(MATRIX *)(DAT_800ac784 + 0x20));
  if ((*param_1 & 0x800) != 0) {
    local_20.vx = (long)*(short *)(DAT_800ac784 + 0x166);
    local_20.vy = (long)*(short *)(DAT_800ac784 + 0x166);
    local_20.vz = (long)*(short *)(DAT_800ac784 + 0x166);
    ScaleMatrix((MATRIX *)(DAT_800ac784 + 0x20),&local_20);
  }
  FUN_80053fc0(DAT_800ac784 + 0x34);
  SetColorMatrix((MATRIX *)&DAT_80076d34);
  cVar3 = *(char *)((int)param_1 + 0x83);
  iVar2 = FUN_80014368(DAT_800ac784 + 0x34,DAT_800ac790);
  if (iVar2 == 0) {
    while (bVar1 = cVar3 != '\0', cVar3 = cVar3 + -1, bVar1) {
      FUN_8001ef54(uVar4);
      uVar4 = uVar4 + 0xac;
    }
  }
  else {
    for (; cVar3 != '\0'; cVar3 = cVar3 + -1) {
      FUN_8001e9ec(uVar4);
      uVar4 = uVar4 + 0xac;
    }
  }
  return;
}
