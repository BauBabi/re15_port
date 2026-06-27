/* RATAN_OBJ_15C @ 0x8008f478  (Ghidra headless, raw MIPS overlay) */

int RATAN_OBJ_15C(undefined4 param_1,undefined4 param_2,int param_3,int param_4)

{
  int in_v1;
  
  if (param_3 != 0) {
    in_v1 = 0x800 - in_v1;
  }
  if (param_4 != 0) {
    in_v1 = -in_v1;
  }
  return in_v1;
}


