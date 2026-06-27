/* FUN_80059438 @ 0x80059438  (Ghidra headless, raw MIPS overlay) */

void FUN_80059438(void)

{
  long lVar1;
  
  FUN_80059654();
  FUN_8007a42c();
  SsSetTableSize("",3,1);
  SsSetTickMode(1);
  SsUtSetReverbType(3);
  do {
    lVar1 = SpuClearReverbWorkArea(3);
  } while (lVar1 == -1);
  FUN_80081348();
  SsUtSetReverbDepth((short)DAT_800d8ce4,DAT_800d8ce4._2_2_);
  SsSetReservedVoice('\x16');
  FUN_8005990c(1);
  return;
}


