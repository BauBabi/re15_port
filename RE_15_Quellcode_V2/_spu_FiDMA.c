void _spu_FiDMA(void)

{
  if (DAT_80076da8 == 0) {
    SPU_OBJ_92C();
    return;
  }
  SPU_CTRL_REG_CPUCNT = SPU_CTRL_REG_CPUCNT & 0xffcf;
  if (DAT_80076d90 == (code *)0x0) {
    DeliverEvent(0xf0000009,0x20);
    return;
  }
  (*DAT_80076d90)();
  SPU_OBJ_A6C();
  return;
}
