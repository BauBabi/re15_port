void FUN_8001f314(undefined4 param_1,int param_2,int param_3,short param_4)

{
  uint uVar1;
  uint uVar2;
  uint *puVar3;
  
  uVar1 = *(uint *)(param_2 + (uint)*(byte *)(DAT_800ac784 + 0x94) * 4);
  if (param_3 == 0) {
    uVar2 = (uint)*(byte *)(DAT_800ac784 + 0x95);
  }
  else {
    uVar2 = ((uVar1 & 0xffff) - (uint)*(byte *)(DAT_800ac784 + 0x95)) - 1;
  }
  puVar3 = (uint *)(param_2 + (uVar1 >> 0x10) + uVar2 * 4);
  *(uint **)(DAT_800ac784 + 0x168) = puVar3;
  if ((*puVar3 & 0x8000) == 0) {
    FUN_8001f3bc();
  }
  else {
    FUN_8001f8b4(param_1,param_2,param_3,(int)param_4);
  }
  return;
}
