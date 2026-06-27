void FUN_80049390(uint param_1,uint param_2)

{
  RECT local_20 [3];
  
  local_20[0].w = 0x14;
  local_20[0].h = 0x1e;
  local_20[0].x = (short)((int)(((param_1 & 0xff) % 3) * 0x28 + -0x80) >> 1) + 0x2c0;
  local_20[0].y = (short)((param_1 & 0xff) / 3) * 0x1e + 0x100;
  MoveImage(local_20,((int)(((param_2 & 0xffff) % 3) * 0x28 + -0x80) >> 1) + 0x2c0U & 0xfffc,
            ((param_2 & 0xffff) / 3) * 0x1e + 0x100 & 0xfffe);
  DrawSync(0);
  return;
}
