/* FUN_800534c4 @ 0x800534c4  (Ghidra headless, raw MIPS overlay) */

int FUN_800534c4(undefined4 param_1,int param_2)

{
  int in_v1;
  
  switch(param_1) {
  case 0:
    in_v1 = 0;
    break;
  case 1:
    in_v1 = 0x1e;
    break;
  case 2:
    in_v1 = 0x3a;
    break;
  case 3:
    in_v1 = 0x48;
    break;
  case 4:
    in_v1 = 0x59;
    break;
  case 5:
    in_v1 = 99;
    break;
  case 6:
    in_v1 = 0x7b;
  }
  return in_v1 + param_2;
}


