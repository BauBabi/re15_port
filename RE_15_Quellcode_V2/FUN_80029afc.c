void FUN_80029afc(void)

{
  undefined2 *puVar1;
  
  puVar1 = DAT_800b2b24;
  *DAT_800b2b24 = 0;
  FUN_8006e170();
  CloseTh(*(undefined4 *)(puVar1 + 4));
  FUN_8006e468();
  ChangeTh(0xff000000);
  return;
}
