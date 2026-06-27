undefined4 DecDCTvlcSize(int param_1)

{
  undefined4 uVar1;
  
  uVar1 = VLC_OBJ_0;
  if (0 < param_1 + -1) {
    VLC_OBJ_0 = param_1 << 1;
    return uVar1;
  }
  VLC_OBJ_0 = 0xffffff;
  return uVar1;
}
