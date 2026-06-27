/* COR_06_OBJ_A4 @ 0x8008d234  (Ghidra headless, raw MIPS overlay) */

undefined4 COR_06_OBJ_A4(undefined4 param_1,int *param_2,uint param_3,int *param_4)

{
  undefined4 uVar1;
  int in_v0;
  int iVar2;
  int *piVar3;
  int *in_t0;
  int *in_t1;
  undefined4 in_stack_000000a0;
  
  while( true ) {
    *in_t1 = in_v0;
    in_t1 = in_t1 + 1;
    in_t0 = in_t0 + 1;
    param_4 = param_4 + 1;
    param_3 = param_3 + 1;
    piVar3 = param_2 + 1;
    if (0xb < (int)param_3) {
      return in_stack_000000a0;
    }
    iVar2 = param_2[0xf];
    if (iVar2 < 0) break;
    param_2[2] = *piVar3 + (iVar2 >> (param_3 & 0x1f));
    *param_4 = param_2[0xf] - (*piVar3 >> (param_3 & 0x1f));
    in_v0 = param_2[0x1d] + *in_t0;
    param_2 = piVar3;
  }
  param_2[2] = *piVar3 - (iVar2 >> (param_3 & 0x1f));
  *param_4 = param_2[0xf] + (*piVar3 >> (param_3 & 0x1f));
  uVar1 = COR_06_OBJ_A4();
  return uVar1;
}


