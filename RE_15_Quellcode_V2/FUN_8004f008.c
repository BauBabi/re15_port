void FUN_8004f008(short param_1,SVECTOR *param_2,undefined2 *param_3)

{
  SVECTOR local_48;
  VECTOR local_40;
  MATRIX MStack_30;
  
  local_48.vz = 0;
  local_48.vx = 0;
  local_48.vy = param_1;
  RotMatrix(&local_48,&MStack_30);
  ApplyMatrix(&MStack_30,param_2,&local_40);
  *param_3 = (undefined2)local_40.vx;
  param_3[1] = (undefined2)local_40.vy;
  param_3[2] = (undefined2)local_40.vz;
  return;
}
