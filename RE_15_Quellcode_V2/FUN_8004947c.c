void FUN_8004947c(uint param_1)

{
  RECT local_18 [2];
  
  local_18[0].x = 0x2a8;
  local_18[0].y = 0x15a;
  local_18[0].w = 0x14;
  local_18[0].h = 0x1e;
  MoveImage(local_18,((int)(((param_1 & 0xffff) % 3) * 0x28 + -0x80) >> 1) + 0x2c0U & 0xfffc,
            ((param_1 & 0xffff) / 3) * 0x1e + 0x100 & 0xfffe);
  DrawSync(0);
  return;
}
