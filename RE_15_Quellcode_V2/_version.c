undefined4 _version(uint param_1)

{
  undefined4 uVar1;
  
  GPU_REG1 = 0x10000007;
  if ((GPU_REG0 & 0xffffff) != 2) {
    GPU_REG0 = 0xe1001007;
    uVar1 = SYS_OBJ_2D18();
    return uVar1;
  }
  if ((param_1 & 8) == 0) {
    return 3;
  }
  GPU_REG1 = 0x9000001;
  uVar1 = SYS_OBJ_2D18();
  return uVar1;
}
