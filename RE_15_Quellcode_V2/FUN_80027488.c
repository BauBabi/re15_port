void FUN_80027488(undefined4 param_1,undefined1 *param_2,char *param_3)

{
  char *pcVar1;
  undefined1 *puVar2;
  uint uVar3;
  char acStack_58 [24];
  int local_40;
  undefined1 auStack_30 [4];
  undefined1 local_2c;
  char local_2b;
  uchar auStack_2a [26];
  
  puVar2 = auStack_30 + 3;
  uVar3 = (uint)puVar2 & 3;
  *(uint *)(puVar2 + -uVar3) =
       *(uint *)(puVar2 + -uVar3) & -1 << (uVar3 + 1) * 8 | 0x30307562U >> (3 - uVar3) * 8;
  auStack_30 = (undefined1  [4])0x30307562;
  local_2c = 0x3a;
  local_2b = '\0';
  memset(auStack_2a,'\0',0x1a);
  strcpy(&local_2b,param_3);
  auStack_30[2] = auStack_30[2] + (char)param_1;
  param_2[0x14a] = 0;
  *(undefined4 *)(param_2 + 0x14c) = 0;
  pcVar1 = (char *)firstfile(auStack_30,acStack_58);
  if (pcVar1 == acStack_58) {
    do {
      strcpy(param_2 + (uint)(byte)param_2[0x14a] * 0x16,acStack_58);
      *(int *)(param_2 + (uint)(byte)param_2[0x14a] * 4 + 0x150) = local_40;
      *(int *)(param_2 + 0x14c) = *(int *)(param_2 + 0x14c) + local_40;
      param_2[0x14a] = param_2[0x14a] + '\x01';
      pcVar1 = (char *)nextfile(acStack_58);
    } while (pcVar1 == acStack_58);
  }
  else {
    printf("--------- Port:%d path:%s\n",param_1,auStack_30);
  }
  uVar3 = 0;
  puVar2 = param_2;
  do {
    if (uVar3 < (byte)param_2[0x14a]) {
      puVar2[0x14] = 0;
    }
    else {
      *puVar2 = 0;
    }
    uVar3 = uVar3 + 1;
    puVar2 = puVar2 + 0x16;
  } while (uVar3 < 0xf);
  return;
}
