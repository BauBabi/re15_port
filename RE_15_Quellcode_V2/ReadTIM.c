TIM_IMAGE * ReadTIM(TIM_IMAGE *timimg)

{
  int iVar1;
  TIM_IMAGE *pTVar2;
  
  iVar1 = get_tim_addr(DAT_8008ff68,timimg);
  if (iVar1 != -1) {
    DAT_8008ff68 = iVar1 * 4 + DAT_8008ff68;
    pTVar2 = (TIM_IMAGE *)TMD_OBJ_64();
    return pTVar2;
  }
  return (TIM_IMAGE *)0x0;
}
