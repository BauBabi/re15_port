void FUN_800492b8(uint param_1,short param_2,u_long *param_3)

{
  uint uVar1;
  RECT local_20 [3];
  
  if (param_2 == 0) {
    uVar1 = (uint)((ulonglong)(param_1 & 0xffff) * 0xaaaaaaab >> 0x20);
    local_20[0].w = 0x14;
  }
  else {
    if (param_2 != 1) {
      return;
    }
    uVar1 = (uint)((ulonglong)(param_1 & 0xffff) * 0xaaaaaaab >> 0x20);
    local_20[0].w = 0x28;
  }
  local_20[0].h = 0x1e;
  local_20[0].x =
       (short)((int)((((param_1 & 0xffff) + (uVar1 >> 1) * -3 & 0xffff) * 0x28 & 0xffff) - 0x80) >>
              1) + 0x2c0;
  local_20[0].y = (short)(uVar1 >> 1) * 0x1e + 0x100;
  LoadImage(local_20,param_3);
  DrawSync(0);
  return;
}
