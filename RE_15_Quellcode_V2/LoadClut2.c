u_short LoadClut2(u_long *param_1,int param_2,int param_3)

{
  u_short uVar1;
  RECT local_18;
  
  local_18.w = 0x10;
  local_18.x = (short)param_2;
  local_18.y = (short)param_3;
  local_18.h = 1;
  LoadImage(&local_18,param_1);
  uVar1 = GetClut(param_2,param_3);
  return uVar1;
}
