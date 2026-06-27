/* FUN_800333c4 @ 0x800333c4  (Ghidra headless, raw MIPS overlay) */

void FUN_800333c4(uint param_1)

{
  DslATV *pDVar1;
  u_char uVar2;
  
  pDVar1 = DAT_800e8980;
  if (DAT_800ce5e1 == '\0') {
    DAT_800e8980->val2 = (u_char)param_1;
    pDVar1->val0 = (u_char)param_1;
    pDVar1 = DAT_800e8980;
    DAT_800e8980->val3 = '\0';
    pDVar1->val1 = '\0';
  }
  else {
    uVar2 = (u_char)(param_1 >> 1);
    DAT_800e8980->val3 = uVar2;
    pDVar1->val2 = uVar2;
    pDVar1->val1 = uVar2;
    pDVar1->val0 = uVar2;
  }
  DsMix(DAT_800e8980);
  return;
}


