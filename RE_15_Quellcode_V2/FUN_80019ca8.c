void FUN_80019ca8(char param_1,char param_2,int param_3)

{
  undefined2 *puVar1;
  uint uVar2;
  
  uVar2 = 0;
  if (param_3 == 0) {
    puVar1 = &DAT_800a73b8;
    do {
      uVar2 = uVar2 + 1;
      if ((*(char *)(puVar1 + 0x38) == param_1) && (*(char *)((int)puVar1 + 0x71) == param_2)) {
        *(undefined1 *)(puVar1 + 0x36) = 0;
      }
      puVar1 = puVar1 + 0x42;
    } while (uVar2 < 0x60);
  }
  else {
    puVar1 = &DAT_800a73b8;
    do {
      uVar2 = uVar2 + 1;
      if (((*(char *)(puVar1 + 0x38) == param_1) && (*(char *)((int)puVar1 + 0x71) == param_2)) &&
         (*(int *)(puVar1 + 0x3a) == param_3)) {
        *(undefined1 *)(puVar1 + 0x36) = 0;
      }
      puVar1 = puVar1 + 0x42;
    } while (uVar2 < 0x60);
  }
  return;
}
