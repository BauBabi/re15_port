void FUN_80029ac8(undefined2 param_1)

{
  undefined2 *puVar1;
  
  puVar1 = DAT_800b2b24;
  DAT_800b2b24[1] = param_1;
  *puVar1 = 1;
  ChangeTh(0xff000000);
  return;
}
