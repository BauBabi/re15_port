void FUN_8002ad64(void)

{
  RECT local_10;
  
  FUN_80061fc0(0);
  local_10.x = 0;
  local_10.y = 0;
  local_10.w = 0x1e0;
  local_10.h = 0x1e0;
  ClearImage(&local_10,'\0','\0','\0');
  DrawSync(0);
  return;
}
