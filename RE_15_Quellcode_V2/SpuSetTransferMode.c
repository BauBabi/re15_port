long SpuSetTransferMode(long mode)

{
  long lVar1;
  
  if ((mode != 0) && (mode == 1)) {
    lVar1 = S_STM_OBJ_1C();
    return lVar1;
  }
  DAT_80076dc4 = mode;
  DAT_80076d74 = 0;
  return 0;
}
