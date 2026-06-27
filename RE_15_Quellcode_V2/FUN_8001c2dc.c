int FUN_8001c2dc(int param_1,short param_2,undefined1 *param_3)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  
  uVar3 = -(*(int *)(param_1 + 4) / 0x708);
  while( true ) {
    uVar4 = uVar3 & 0xff;
    uVar1 = FUN_8003b7f0(param_1,(int)param_2,uVar4);
    *param_3 = 0;
    if ((uVar1 & 0xffff) != 0) break;
    iVar2 = FUN_8003bc2c(uVar4);
    if (iVar2 != 0) {
      *param_3 = 0;
      goto LAB_8001c3d8;
    }
    uVar4 = uVar3 & 0xff;
    uVar3 = uVar3 - 1;
    if (uVar4 == 0) {
      *param_3 = 0;
      return 0;
    }
  }
  if ((uVar1 & 1) == 0) {
    if ((uVar1 & 2) != 0) {
      uVar3 = uVar3 - (((uVar1 & 0xc) >> 2) + 1);
      *param_3 = 0;
      goto LAB_8001c3c8;
    }
    if ((uVar1 & 0x600) == 0) goto LAB_8001c3c8;
    uVar3 = 7;
  }
  else {
    uVar3 = uVar3 + 1;
  }
  *param_3 = 1;
LAB_8001c3c8:
  uVar4 = uVar3 & 0xff;
LAB_8001c3d8:
  return (int)(uVar4 * -0x7080000) >> 0x10;
}
