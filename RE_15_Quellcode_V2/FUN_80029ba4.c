void FUN_80029ba4(undefined4 param_1)

{
  undefined2 *puVar1;
  
  puVar1 = DAT_800b2b24;
  *DAT_800b2b24 = 2;
  *(undefined4 *)(puVar1 + 2) = param_1;
  FUN_8006e170();
  CloseTh(*(undefined4 *)(puVar1 + 4));
  FUN_8006e468();
  ChangeTh(0xff000000);
  return;
}
