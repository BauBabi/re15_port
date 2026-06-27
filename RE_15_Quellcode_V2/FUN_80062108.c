void FUN_80062108(int param_1,int param_2)

{
  param_2 = param_2 << 0xf;
  do {
    if (param_1 <= DAT_800787dc) {
      return;
    }
    param_2 = param_2 + -1;
  } while (param_2 != -1);
  puts("VSync: timeout\n");
  ChangeClearPAD(0);
  ChangeClearRCnt(3,0);
  return;
}
