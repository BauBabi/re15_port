/* FUN_8005998c @ 0x8005998c  (Ghidra headless, raw MIPS overlay) */

void FUN_8005998c(uint param_1)

{
  DslATV local_10 [2];
  
  if ((param_1 & 1) == 0) {
    FUN_80013a7c(1);
    FUN_80084e80();
    local_10[0].val0 = '\x7f';
    local_10[0].val1 = '\0';
    local_10[0].val2 = '\x7f';
    local_10[0].val3 = '\0';
  }
  else {
    FUN_80013a7c(0);
    FUN_80084e6c();
    local_10[0].val0 = '?';
    local_10[0].val1 = '?';
    local_10[0].val2 = '?';
    local_10[0].val3 = '?';
  }
  DsMix(local_10);
  return;
}


