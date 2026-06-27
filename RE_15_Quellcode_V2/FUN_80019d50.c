void FUN_80019d50(char param_1,char param_2,undefined2 param_3,int param_4)

{
  undefined2 *puVar1;
  uint uVar2;
  
  uVar2 = 0;
  if (param_4 == 0) {
    puVar1 = &DAT_800a73b8;
    do {
      uVar2 = uVar2 + 1;
      if ((*(char *)(puVar1 + 0x38) == param_1) && (*(char *)((int)puVar1 + 0x71) == param_2)) {
        *puVar1 = param_3;
      }
      puVar1 = puVar1 + 0x42;
    } while (uVar2 < 0x60);
  }
  else {
    puVar1 = &DAT_800a73b8;
    do {
      uVar2 = uVar2 + 1;
      if (((*(char *)(puVar1 + 0x38) == param_1) && (*(char *)((int)puVar1 + 0x71) == param_2)) &&
         (*(int *)(puVar1 + 0x3a) == param_4)) {
        *puVar1 = param_3;
      }
      puVar1 = puVar1 + 0x42;
    } while (uVar2 < 0x60);
  }
  return;
}
