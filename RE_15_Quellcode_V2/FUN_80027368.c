undefined4 FUN_80027368(undefined4 param_1,char *param_2,int param_3,undefined4 param_4)

{
  undefined1 *puVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  undefined1 auStack_38 [4];
  undefined1 local_34;
  char local_33;
  uchar auStack_32 [18];
  
  puVar1 = auStack_38 + 3;
  uVar4 = (uint)puVar1 & 3;
  *(uint *)(puVar1 + -uVar4) =
       *(uint *)(puVar1 + -uVar4) & -1 << (uVar4 + 1) * 8 | 0x30307562U >> (3 - uVar4) * 8;
  auStack_38 = (undefined1  [4])0x30307562;
  local_34 = 0x3a;
  local_33 = '\0';
  memset(auStack_32,'\0',0xe);
  uVar4 = 0;
  auStack_38[2] = auStack_38[2] + DAT_801ff404;
  strcpy(&local_33,param_2);
  while (((iVar2 = open(auStack_38,1), iVar2 == -1 || (iVar3 = lseek(iVar2,param_4,0), iVar3 == -1))
         || (iVar3 = read(iVar2,param_1,param_3), iVar3 != param_3))) {
    uVar4 = uVar4 + 1;
    if (2 < uVar4) {
      return 2;
    }
  }
  close(iVar2);
  return 0;
}
