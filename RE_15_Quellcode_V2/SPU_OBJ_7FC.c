void SPU_OBJ_7FC(void)

{
  int iVar1;
  int unaff_s0;
  int unaff_s1;
  uint uVar2;
  word *unaff_s2;
  ushort unaff_s4;
  int in_stack_00000010;
  
  for (; in_stack_00000010 < 0xf0; in_stack_00000010 = in_stack_00000010 + 1) {
  }
  uVar2 = unaff_s1 - unaff_s0;
  if (uVar2 == 0) {
    SPU_CTRL_REG_CPUCNT = SPU_CTRL_REG_CPUCNT & 0xffcf;
    DAT_80076d54 = 0;
    do {
      if ((SPU_STATUS_REG_SPUSTAT & 0x7ff) == unaff_s4) {
        return;
      }
      DAT_80076d54 = DAT_80076d54 + 1;
    } while (DAT_80076d54 < 0x1389);
    printf("SPU:T/O [%s]\n","wait (dmaf clear/W)");
    SPU_OBJ_8B8();
    return;
  }
  if (0x40 < uVar2) {
    uVar2 = 0x40;
  }
  iVar1 = 0;
  if (0 < (int)uVar2) {
    do {
      SOUND_RAM_DATA_TRANSFER_FIFO = *unaff_s2;
      unaff_s2 = unaff_s2 + 1;
      iVar1 = iVar1 + 2;
    } while (iVar1 < (int)uVar2);
  }
  SPU_CTRL_REG_CPUCNT = SPU_CTRL_REG_CPUCNT & 0xffcf | 0x10;
  SPU_OBJ_6DC();
  return;
}
