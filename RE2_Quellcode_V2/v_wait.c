/* v_wait @ 0x80085fe8  (Ghidra headless, raw MIPS overlay) */

void v_wait(int param_1,int param_2)

{
  param_2 = param_2 << 0xf;
  do {
    if (param_1 <= DAT_800acd00) {
      return;
    }
    param_2 = param_2 + -1;
  } while (param_2 != -1);
  FUN_8009881c("VSync: timeout\n");
  ChangeClearPAD(0);
  ChangeClearRCnt(3,0);
  VSYNC_OBJ_1D4();
  return;
}


