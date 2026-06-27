void SpuVmSeKeyOn(undefined4 param_1,short param_2,undefined2 param_3,undefined4 param_4,
                 ushort param_5,ushort param_6)

{
  uint uVar1;
  uint uVar2;
  
  uVar1 = (uint)param_5;
  uVar2 = (uint)param_6;
  if (uVar1 == uVar2) {
    VMANAGER_OBJ_31F8(param_1,uVar1);
    return;
  }
  if (uVar2 < uVar1) {
    if (uVar1 == 0) {
      trap(0x1c00);
    }
    if ((uVar1 == 0xffffffff) && (uVar2 == 0x2000000)) {
      trap(0x1800);
    }
    VMANAGER_OBJ_31F8(param_1,uVar1);
    return;
  }
  if (uVar2 == 0) {
    trap(0x1c00);
  }
  if ((uVar2 == 0xffffffff) && (uVar1 == 0x2000000)) {
    trap(0x1800);
  }
  SpuVmKeyOn(0x21,(int)(short)param_1,(int)param_2,param_3,uVar2,
             0x7f - (uVar1 << 6) / uVar2 & 0xffff);
  return;
}
