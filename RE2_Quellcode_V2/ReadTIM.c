/* ReadTIM @ 0x80092f84  (Ghidra headless, raw MIPS overlay) */

TIM_IMAGE * ReadTIM(TIM_IMAGE *timimg)

{
  int iVar1;
  TIM_IMAGE *pTVar2;
  
  iVar1 = get_tim_addr(DAT_800c4060,timimg);
  if (iVar1 != -1) {
    DAT_800c4060 = DAT_800c4060 + iVar1 * 4;
    pTVar2 = (TIM_IMAGE *)TMD_OBJ_64();
    return pTVar2;
  }
  return (TIM_IMAGE *)0x0;
}


