void _spu_reset(void)

{
  SPU_CTRL_REG_CPUCNT = SPU_CTRL_REG_CPUCNT & 0x7fcf;
  SPU_OBJ_58();
  return;
}
