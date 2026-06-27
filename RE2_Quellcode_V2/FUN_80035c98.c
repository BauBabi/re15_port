/* FUN_80035c98 @ 0x80035c98  (Ghidra headless, raw MIPS overlay) */

void FUN_80035c98(undefined1 *param_1,short *param_2,undefined4 param_3)

{
  char cVar1;
  undefined1 uVar2;
  byte bVar3;
  
  switch(param_3) {
  case 0:
    *param_2 = (ushort)((byte)param_1[3] >> 2) << 5;
    param_2[1] = ((byte)param_1[3] & 3) << 6;
    break;
  case 1:
    if ((byte)param_1[2] == 0) {
      trap(0x1c00);
    }
    *param_2 = *(short *)(param_1 + 4) +
               (short)((uint)(byte)param_1[1] / (uint)(byte)param_1[2] >> 2) * 0x20;
    if (param_1[2] == 0) {
      trap(0x1c00);
    }
    param_2[1] = *(short *)(param_1 + 6) +
                 ((ushort)(byte)param_1[1] / (ushort)(byte)param_1[2] & 3) * 0x40;
    if ((uint)(byte)param_1[1] != (uint)(byte)param_1[3] * (uint)(byte)param_1[2] - 1) {
      param_1[1] = param_1[1] + 1;
    }
    break;
  case 2:
    if ((byte)param_1[2] == 0) {
      trap(0x1c00);
    }
    *param_2 = *(short *)(param_1 + 4) +
               (short)((uint)(byte)param_1[1] / (uint)(byte)param_1[2] >> 2) * 0x20;
    if (param_1[2] == 0) {
      trap(0x1c00);
    }
    param_2[1] = *(short *)(param_1 + 6) +
                 ((ushort)(byte)param_1[1] / (ushort)(byte)param_1[2] & 3) * 0x40;
    cVar1 = param_1[1];
    param_1[1] = cVar1 + 1U;
    if ((uint)(byte)(cVar1 + 1U) == (uint)(byte)param_1[2] * (uint)(byte)param_1[3]) {
      param_1[1] = param_1[2];
    }
    break;
  case 3:
    if ((byte)param_1[2] == 0) {
      trap(0x1c00);
    }
    *param_2 = *(short *)(param_1 + 4) +
               (short)((uint)(byte)param_1[1] / (uint)(byte)param_1[2] >> 2) * 0x20;
    if (param_1[2] == 0) {
      trap(0x1c00);
    }
    param_2[1] = *(short *)(param_1 + 6) +
                 ((ushort)(byte)param_1[1] / (ushort)(byte)param_1[2] & 3) * 0x40;
    bVar3 = param_1[1] + 1;
    param_1[1] = bVar3;
    if ((uint)bVar3 != (uint)(byte)param_1[2] * (uint)(byte)param_1[3]) {
      return;
    }
    param_1[1] = bVar3 - param_1[2];
    uVar2 = 0x44;
    goto LAB_80035f5c;
  case 4:
    cVar1 = param_1[1];
    if ((byte)param_1[2] == 0) {
      trap(0x1c00);
    }
    param_1[1] = cVar1 - 1U;
    *param_2 = *(short *)(param_1 + 4) +
               (short)((uint)(byte)(cVar1 - 1U) / (uint)(byte)param_1[2] >> 2) * 0x20;
    if (param_1[2] == 0) {
      trap(0x1c00);
    }
    param_2[1] = *(short *)(param_1 + 6) +
                 ((ushort)(byte)param_1[1] / (ushort)(byte)param_1[2] & 3) * 0x40;
    if (param_1[1] != param_1[2]) {
      return;
    }
    param_1[1] = param_1[1] + param_1[2];
    uVar2 = 0x43;
LAB_80035f5c:
    *param_1 = uVar2;
  }
  return;
}


