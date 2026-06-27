/* SsSetSerialVol @ 0x8007eb4c  (Ghidra headless, raw MIPS overlay) */

void SsSetSerialVol(char param_1,short param_2,short param_3)

{
  SpuCommonAttr local_30;
  
  if (param_1 == '\0') {
    local_30.mask = 0xc0;
    if (0x7f < param_2) {
      param_2 = 0x7f;
    }
    if (0x7f < param_3) {
      param_3 = 0x7f;
    }
    local_30.cd.volume.left = param_2 * 0x102;
    local_30.cd.volume.right = param_3 * 0x102;
  }
  if (param_1 == '\x01') {
    local_30.mask = 0xc00;
    if (0x7f < param_2) {
      param_2 = 0x7f;
    }
    if (0x7f < param_3) {
      param_3 = 0x7f;
    }
    local_30.ext.volume.left = param_2 * 0x102;
    local_30.ext.volume.right = param_3 * 0x102;
  }
  SpuSetCommonAttr(&local_30);
  return;
}


