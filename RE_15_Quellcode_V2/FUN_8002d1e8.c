bool FUN_8002d1e8(int param_1,int param_2,int param_3)

{
  int iVar1;
  short local_18 [2];
  short local_14;
  
  local_18[0] = 0x233;
  local_14 = 0;
  FUN_8004f008((int)*(short *)(param_1 + 0x6a),local_18,local_18);
  iVar1 = 900;
  if (param_3 != 0) {
    iVar1 = 700;
  }
  return (uint)(((int)(short)(local_18[0] + *(short *)(param_1 + 0x34)) -
                ((int)**(short **)(param_2 + 0x78) + *(int *)(param_2 + 0x34))) + iVar1) <=
         (uint)(iVar1 << 1) &&
         (uint)(((int)(short)(local_14 + *(short *)(param_1 + 0x3c)) -
                ((int)(*(short **)(param_2 + 0x78))[2] + *(int *)(param_2 + 0x3c))) + iVar1) <=
         (uint)(iVar1 << 1);
}
