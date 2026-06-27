void FUN_80043870(short param_1)

{
  u_long *p;
  RECT local_10;
  
  p = (u_long *)&DAT_80198000;
  if (DAT_800b854c == 1) {
    p = (u_long *)&DAT_80198014;
  }
  local_10.w = 0x140;
  local_10.x = 0;
  local_10.h = 0xf0;
  local_10.y = param_1;
  LoadImage(&local_10,p);
  return;
}
