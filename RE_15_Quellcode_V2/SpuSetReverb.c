long SpuSetReverb(long on_off)

{
  long lVar1;
  int iVar2;
  
  if (on_off == 0) {
    DAT_80076dc8 = 0;
    lVar1 = S_SR_OBJ_B0();
    return lVar1;
  }
  if (on_off != 1) {
    lVar1 = S_SR_OBJ_B4();
    return lVar1;
  }
  if ((DAT_80076dcc != 1) && (iVar2 = _SpuIsInAllocateArea_(DAT_80076dd0), iVar2 != 0)) {
    DAT_80076dc8 = 0;
    lVar1 = S_SR_OBJ_B0();
    return lVar1;
  }
  SPU_CTRL_REG_CPUCNT = SPU_CTRL_REG_CPUCNT | 0x80;
  DAT_80076dc8 = on_off;
  return 1;
}
