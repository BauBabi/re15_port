byte FUN_8001b9b4(int *param_1)

{
  bool bVar1;
  byte bVar2;
  char cVar3;
  byte bVar4;
  int local_20;
  int local_1c;
  int local_18;
  
  *(undefined4 *)(DAT_800ac784 + 0x1b4) = 0;
  local_20 = *param_1 - *(int *)(DAT_800ac784 + 0x34);
  bVar4 = 0;
  local_1c = param_1[1] - *(int *)(DAT_800ac784 + 0x38);
  cVar3 = '\x03';
  local_18 = param_1[2] - *(int *)(DAT_800ac784 + 0x3c);
  do {
    bVar2 = FUN_8003dcc4(&local_20,cVar3,0xf00,0x300);
    bVar4 = bVar4 | bVar2;
    bVar1 = cVar3 != '\0';
    cVar3 = cVar3 + -1;
  } while (bVar1);
  return bVar4;
}
