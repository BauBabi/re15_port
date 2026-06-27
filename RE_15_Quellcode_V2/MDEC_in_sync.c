undefined4 MDEC_in_sync(void)

{
  undefined4 uVar1;
  int local_10;
  
  local_10 = 0x100000;
  do {
    if ((MDEC_REG1 & 0x20000000) == 0) {
      return 0;
    }
    local_10 = local_10 + -1;
  } while (local_10 != -1);
  timeout("MDEC_in_sync");
  uVar1 = LIBPRESS_OBJ_538();
  return uVar1;
}
