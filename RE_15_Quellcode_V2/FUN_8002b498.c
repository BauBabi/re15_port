void FUN_8002b498(int param_1)

{
  short *psVar1;
  undefined2 *puVar2;
  SVECTOR local_50;
  VECTOR local_48;
  MATRIX MStack_38;
  
  psVar1 = *(short **)(param_1 + 0x78);
  puVar2 = *(undefined2 **)(param_1 + 0x7c);
  local_50.vy = *(short *)(param_1 + 0x6a);
  local_50.vz = 0;
  local_50.vx = 0;
  RotMatrix(&local_50,&MStack_38);
  local_50.vx = *psVar1;
  local_50.vz = psVar1[2];
  ApplyMatrix(&MStack_38,&local_50,&local_48);
  *puVar2 = (undefined2)local_48.vx;
  puVar2[1] = psVar1[1];
  puVar2[2] = (undefined2)local_48.vz;
  *(undefined1 *)(param_1 + 0x1c2) = 0;
  return;
}
