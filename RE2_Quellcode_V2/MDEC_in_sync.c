/* MDEC_in_sync @ 0x80094d58  (Ghidra headless, raw MIPS overlay) */

undefined4 MDEC_in_sync(void)

{
  uint uVar1;
  undefined4 uVar2;
  int local_10;
  
  local_10 = 0x100000;
  uVar1 = *(uint *)PTR_MDEC_REG1_800b2964;
  while( true ) {
    if ((uVar1 & 0x20000000) == 0) {
      return 0;
    }
    local_10 = local_10 + -1;
    if (local_10 == -1) break;
    uVar1 = *(uint *)PTR_MDEC_REG1_800b2964;
  }
  timeout("MDEC_in_sync");
  uVar2 = LIBPRESS_OBJ_570();
  return uVar2;
}


