void FUN_800171b4(uint param_1)

{
  undefined2 uVar1;
  undefined2 *puVar2;
  undefined2 *puVar3;
  
  puVar2 = (undefined2 *)(&DAT_801ab000 + -param_1);
  puVar3 = DAT_800b5364;
  for (param_1 = param_1 >> 1; param_1 != 0; param_1 = param_1 - 1) {
    uVar1 = *puVar3;
    puVar3 = puVar3 + 1;
    *puVar2 = uVar1;
    puVar2 = puVar2 + 1;
  }
  return;
}
