undefined4 S_M_UTIL_OBJ_94(uint param_1,uint *param_2,uint param_3,uint param_4)

{
  uint uVar1;
  uint in_t0;
  
  do {
    uVar1 = *param_2;
    if ((uVar1 & in_t0) == 0) {
      if ((uVar1 & param_4) != 0) {
        return 0;
      }
      if (param_1 <= (uVar1 & param_3)) {
        return 1;
      }
      if (param_1 < (uVar1 & param_3) + param_2[1]) {
        return 1;
      }
    }
    param_2 = param_2 + 2;
  } while( true );
}
