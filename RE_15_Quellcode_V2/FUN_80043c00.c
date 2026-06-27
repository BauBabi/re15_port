void FUN_80043c00(char param_1)

{
  CdlATV local_10 [2];
  
  if (param_1 == '\0') {
    FUN_8005ad40();
    local_10[0].val0 = '?';
    local_10[0].val1 = '?';
    local_10[0].val2 = '?';
    local_10[0].val3 = '?';
  }
  else {
    FUN_8005ad54();
    local_10[0].val0 = '\x7f';
    local_10[0].val1 = '\0';
    local_10[0].val2 = '\x7f';
    local_10[0].val3 = '\0';
  }
  CdMix(local_10);
  return;
}
