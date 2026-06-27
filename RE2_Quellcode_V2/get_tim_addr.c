/* get_tim_addr @ 0x800932d8  (Ghidra headless, raw MIPS overlay) */

int get_tim_addr(int *param_1,uint *param_2)

{
  int iVar1;
  uint uVar2;
  uint *puVar3;
  
  if (*param_1 != 0x10) {
    iVar1 = TMD_OBJ_468();
    return iVar1;
  }
  puVar3 = (uint *)(param_1 + 2);
  *param_2 = param_1[1];
  iVar1 = GetGraphDebug();
  if (iVar1 == 2) {
    printf("id  =%08x\n",0x10);
  }
  iVar1 = GetGraphDebug();
  if (iVar1 == 2) {
    printf("mode=%08x\n",*param_2);
  }
  iVar1 = GetGraphDebug();
  if (iVar1 == 2) {
    printf("timaddr=%08x\n",puVar3);
  }
  if ((*param_2 & 8) != 0) {
    uVar2 = *puVar3;
    param_2[1] = (uint)(param_1 + 3);
    param_2[2] = (uint)(param_1 + 5);
    iVar1 = TMD_OBJ_448(uVar2 >> 2);
    return iVar1;
  }
  param_2[1] = 0;
  param_2[2] = 0;
  uVar2 = *puVar3;
  param_2[3] = (uint)(param_1 + 3);
  param_2[4] = (uint)(param_1 + 5);
  return (uVar2 >> 2) + 2;
}


