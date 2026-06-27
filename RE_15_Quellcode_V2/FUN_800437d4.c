undefined1 FUN_800437d4(short param_1,short param_2)

{
  int iVar1;
  undefined1 *puVar2;
  short *psVar3;
  
  psVar3 = (short *)(*(int *)(DAT_800ac778 + 0x34) + 2);
  puVar2 = (undefined1 *)(*(int *)(DAT_800ac778 + 0x34) + 10);
  iVar1 = (int)*psVar3;
  while (((uint)(int)*(short *)(puVar2 + -4) <= (uint)(param_1 - iVar1) ||
         ((uint)(int)*(short *)(puVar2 + -2) <= (uint)((int)param_2 - (int)*(short *)(puVar2 + -6)))
         )) {
    psVar3 = psVar3 + 6;
    iVar1 = (int)*psVar3;
    puVar2 = puVar2 + 0xc;
    if (iVar1 == 0) {
      return 0;
    }
  }
  return *puVar2;
}
