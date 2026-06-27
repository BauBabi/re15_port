/* FUN_80035ad0 @ 0x80035ad0  (Ghidra headless, raw MIPS overlay) */

void FUN_80035ad0(undefined1 *param_1,short *param_2,undefined4 param_3)

{
  char cVar1;
  byte bVar2;
  
  switch(param_3) {
  case 0:
    *param_2 = (ushort)(byte)param_1[3] << 4;
    break;
  case 1:
    if ((byte)param_1[2] == 0) {
      trap(0x1c00);
    }
    *param_2 = (short)((uint)(byte)param_1[1] / (uint)(byte)param_1[2] << 4);
    if ((uint)(byte)param_1[1] != (uint)(byte)param_1[3] * (uint)(byte)param_1[2] - 1) {
      param_1[1] = param_1[1] + 1;
    }
    break;
  case 2:
    if ((byte)param_1[2] == 0) {
      trap(0x1c00);
    }
    *param_2 = (short)((uint)(byte)param_1[1] / (uint)(byte)param_1[2] << 4);
    cVar1 = param_1[1];
    param_1[1] = cVar1 + 1U;
    if ((uint)(byte)(cVar1 + 1U) == (uint)(byte)param_1[3] * (uint)(byte)param_1[2]) {
      param_1[1] = 0;
    }
    break;
  case 3:
    if ((byte)param_1[2] == 0) {
      trap(0x1c00);
    }
    *param_2 = (short)((uint)(byte)param_1[1] / (uint)(byte)param_1[2] << 4);
    bVar2 = param_1[1] + 1;
    param_1[1] = bVar2;
    if ((uint)bVar2 == (uint)(byte)param_1[2] * (uint)(byte)param_1[3]) {
      param_1[1] = bVar2 - param_1[2];
      *param_1 = 0x84;
    }
    break;
  case 4:
    cVar1 = param_1[1];
    if ((byte)param_1[2] == 0) {
      trap(0x1c00);
    }
    param_1[1] = cVar1 - 1U;
    *param_2 = (short)((uint)(byte)(cVar1 - 1U) / (uint)(byte)param_1[2] << 4);
    if (param_1[1] == '\0') {
      *param_1 = 0x83;
      param_1[1] = param_1[2];
    }
  }
  return;
}


