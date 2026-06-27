byte FUN_8001bafc(int *param_1)

{
  bool bVar1;
  uint uVar2;
  uint *puVar3;
  byte bVar4;
  char cVar5;
  byte bVar6;
  int local_28;
  int local_24;
  int local_20;
  short local_18;
  undefined2 local_16;
  undefined1 auStack_15 [4];
  undefined1 uStack_11;
  
  uVar2 = (uint)((int)register0x00000074 + -0x15) & 3;
  puVar3 = (uint *)((undefined1 *)((int)register0x00000074 + -0x15) + -uVar2);
  *puVar3 = *puVar3 & -1 << (uVar2 + 1) * 8 | 5000U >> (3 - uVar2) * 8;
  _local_18 = 5000;
  uVar2 = (uint)&uStack_11 & 3;
  puVar3 = (uint *)(&uStack_11 + -uVar2);
  *puVar3 = *puVar3 & -1 << (uVar2 + 1) * 8 | 0U >> (3 - uVar2) * 8;
  stack0xffffffec = 0;
  *(undefined4 *)(DAT_800ac784 + 0x1b4) = 0;
  FUN_8004f008((int)*(short *)(DAT_800ac784 + 0x6a),&local_18,&local_18);
  local_28 = ((int)local_18 + *param_1) - *(int *)(DAT_800ac784 + 0x34);
  bVar6 = 0;
  local_24 = ((int)(short)local_16 + param_1[1]) - *(int *)(DAT_800ac784 + 0x38);
  cVar5 = '\x03';
  local_20 = ((int)(short)auStack_15._1_2_ + param_1[2]) - *(int *)(DAT_800ac784 + 0x3c);
  do {
    bVar4 = FUN_8003dcc4(&local_28,cVar5,0x400,0x400);
    bVar6 = bVar6 | bVar4;
    bVar1 = cVar5 != '\0';
    cVar5 = cVar5 + -1;
  } while (bVar1);
  return bVar6;
}
