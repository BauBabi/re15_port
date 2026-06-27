void S_SCA_OBJ_330(int param_1)

{
  int in_v0;
  undefined2 in_v1;
  uint in_t1;
  int in_t2;
  
  *(undefined2 *)(in_v0 + 0x1aa) = in_v1;
  if ((in_t2 != 0) || ((in_t1 & 0x2000) != 0)) {
    if (*(int *)(param_1 + 0x24) == 0) {
      S_SCA_OBJ_384();
      return;
    }
    SPU_CTRL_REG_CPUCNT = SPU_CTRL_REG_CPUCNT | 2;
  }
  return;
}
