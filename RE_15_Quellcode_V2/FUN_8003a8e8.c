void FUN_8003a8e8(int param_1,uint *param_2)

{
  uint uVar1;
  undefined4 *puVar2;
  uint *puVar3;
  uint uVar4;
  uint uVar5;
  
  *(uint **)(param_1 + 0x1b0) = param_2;
  if (*param_2 != 0) {
    uVar5 = 0;
    puVar3 = param_2 + 3;
    puVar2 = (undefined4 *)(*(int *)(param_1 + 0x188) + 0x84);
    uVar4 = *(uint *)(*(int *)(param_1 + 0x10) + -4) >> 1;
    do {
      *(undefined2 *)((int)puVar2 + -6) = 0;
      *(undefined2 *)(puVar2 + -1) = 0;
      *puVar2 = 0;
      uVar1 = uVar5 & 0x1f;
      uVar5 = uVar5 + 1;
      if ((1 << uVar1 & *param_2) != 0) {
        *puVar2 = puVar3;
        puVar3 = puVar3 + 2;
      }
      uVar4 = uVar4 - 1;
      puVar2 = puVar2 + 0x2b;
    } while ((uVar4 & 0xff) != 0);
  }
  return;
}
