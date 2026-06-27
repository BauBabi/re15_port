/* SpuSetReverbDepth @ 0x8007d370  (Ghidra headless, raw MIPS overlay) */

long SpuSetReverbDepth(SpuReverbAttr *attr)

{
  uint uVar1;
  
  uVar1 = attr->mask;
  if ((uVar1 == 0) || ((uVar1 & 2) != 0)) {
    *(short *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x184) = (attr->depth).left;
    DAT_800ab2a0 = (attr->depth).left;
  }
  if ((uVar1 == 0) || ((uVar1 & 4) != 0)) {
    *(short *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x186) = (attr->depth).right;
    DAT_800ab2a2 = (attr->depth).right;
  }
  return 0;
}


