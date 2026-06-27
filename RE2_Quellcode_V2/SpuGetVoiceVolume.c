/* SpuGetVoiceVolume @ 0x80081714  (Ghidra headless, raw MIPS overlay) */

void SpuGetVoiceVolume(int vNum,short *volL,short *volR)

{
  ushort uVar1;
  ushort uVar2;
  
  uVar1 = *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + vNum * 0x10);
  uVar2 = *(ushort *)((int)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + vNum * 0x10) + 2);
  if (0x3fff < uVar1) {
    *volL = uVar1 + 0x8000;
    S_GVV_OBJ_38();
    return;
  }
  *volL = uVar1;
  if (0x3fff < uVar2) {
    *volR = uVar2 + 0x8000;
    S_GVV_OBJ_58();
    return;
  }
  *volR = uVar2;
  return;
}


