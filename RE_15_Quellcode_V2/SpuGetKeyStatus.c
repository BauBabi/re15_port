long SpuGetKeyStatus(ulong voice_bit)

{
  uint uVar1;
  long lVar2;
  uint uVar3;
  
  uVar3 = 0;
  uVar1 = 1;
  do {
    if ((voice_bit & uVar1) != 0) {
      lVar2 = S_GKS_OBJ_2C(voice_bit,uVar3);
      return lVar2;
    }
    uVar3 = uVar3 + 1;
    uVar1 = 1 << (uVar3 & 0x1f);
  } while ((int)uVar3 < 0x18);
  lVar2 = S_GKS_OBJ_8C();
  return lVar2;
}
