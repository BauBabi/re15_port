void FUN_800438c8(void)

{
  long lVar1;
  
  FUN_80043a34();
  FUN_8005abf8();
  SsSetTableSize("",3,1);
  SsSetTickMode(1);
  SsUtSetReverbType(3);
  do {
    lVar1 = SpuClearReverbWorkArea(3);
  } while (lVar1 == -1);
  SsUtReverbOn();
  SsUtSetReverbDepth(DAT_800b3f94,DAT_800b3f96);
  SsSetReservedVoice('\x10');
  FUN_80043b80(1);
  return;
}
