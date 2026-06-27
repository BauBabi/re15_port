undefined4 FUN_800271a8(int param_1,char param_2,char *param_3,char *param_4,uint param_5)

{
  undefined1 *puVar1;
  undefined4 uVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  undefined1 auStack_238 [512];
  undefined1 auStack_38 [4];
  undefined1 local_34;
  char local_33;
  uchar auStack_32 [18];
  
  puVar1 = auStack_38 + 3;
  uVar6 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar6) =
       *(uint *)(puVar1 + -uVar6) & -1 << (uVar6 + 1) * 8 | 0x30307562U >> (3 - uVar6) * 8;
  auStack_38 = (undefined1  [4])0x30307562;
  local_34 = 0x3a;
  local_33 = '\0';
  memset(auStack_32,'\0',0xe);
  uVar6 = 0;
  *(char *)(param_1 + 3) = (char)(param_5 >> 0xd) + ((param_5 & 0x1fff) != 0);
  auStack_38[2] = param_2 + auStack_38[2];
  strcpy(&local_33,param_3);
  strcpy((char *)(param_1 + 4),param_4);
  uVar2 = open(auStack_38,(uint)*(byte *)(param_1 + 3) << 0x10 | 0x200);
  close(uVar2);
  do {
    iVar5 = 0x1e;
    while (iVar3 = open(auStack_38,2), iVar3 == -1) {
      FUN_80061fc0(2);
      iVar5 = iVar5 + -1;
      if (iVar5 == 0) {
        return 1;
      }
    }
    iVar5 = 0x1e;
    while (uVar4 = write(iVar3,param_1,param_5), uVar4 != param_5) {
      FUN_80061fc0(2);
      iVar5 = iVar5 + -1;
      if (iVar5 == 0) {
        return 2;
      }
    }
    close(iVar3);
    iVar5 = open(auStack_38,1);
    if (iVar5 == -1) {
      return 2;
    }
    iVar3 = read(iVar5,auStack_238,0x200);
    close(iVar5);
    uVar6 = uVar6 + 1;
    if (iVar3 == 0x200) {
      return 0;
    }
  } while (uVar6 < 3);
  return 2;
}
