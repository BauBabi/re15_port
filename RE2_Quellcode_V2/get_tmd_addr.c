/* get_tmd_addr @ 0x800933f8  (Ghidra headless, raw MIPS overlay) */

int get_tmd_addr(undefined4 *param_1,int param_2,int *param_3,int *param_4,int *param_5)

{
  int iVar1;
  int *piVar2;
  
  iVar1 = GetGraphDebug();
  if (iVar1 == 2) {
    printf("analizing TMD...\n");
  }
  iVar1 = GetGraphDebug();
  if (iVar1 == 2) {
    printf("\tid=%08X, flags=%d, nobj=%d, objid=%d\n",*param_1,param_1[1],param_1[2],param_2);
  }
  iVar1 = GetGraphDebug();
  if (iVar1 == 2) {
    printf("\tvert=%08X, nvert=%d\n",param_1[param_2 * 7 + 3],(param_1 + param_2 * 7 + 3)[1]);
  }
  iVar1 = GetGraphDebug();
  if (iVar1 == 2) {
    printf("\tnorm=%08X, nnorm=%d\n",param_1[param_2 * 7 + 5],param_1[param_2 * 7 + 6]);
  }
  iVar1 = GetGraphDebug();
  if (iVar1 == 2) {
    printf("\tprim=%08X, nprim=%d\n",param_1[param_2 * 7 + 7],param_1[param_2 * 7 + 8]);
  }
  piVar2 = param_1 + param_2 * 7 + 3;
  *param_4 = (int)param_1 + *piVar2 + 0xc;
  *param_5 = (int)param_1 + piVar2[2] + 0xc;
  *param_3 = (int)param_1 + piVar2[4] + 0xc;
  return piVar2[5];
}


