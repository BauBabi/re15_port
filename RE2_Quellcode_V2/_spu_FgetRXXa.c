/* _spu_FgetRXXa @ 0x80079098  (Ghidra headless, raw MIPS overlay) */

uint _spu_FgetRXXa(int param_1,int param_2)

{
  uint uVar1;
  
  if (param_2 != -1) {
    uVar1 = SPU_OBJ_A6C();
    return uVar1;
  }
  return (uint)*(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + param_1 * 2);
}


