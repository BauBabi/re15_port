/* SetGraphQueue @ 0x800901dc  (Ghidra headless, raw MIPS overlay) */

byte SetGraphQueue(uint param_1)

{
  byte bVar1;
  
  bVar1 = DAT_800b2701;
  if (1 < DAT_800b2702) {
    (*(code *)PTR_printf_800b26fc)("SetGrapQue(%d)...\n",param_1);
  }
  if (param_1 != DAT_800b2701) {
    (**(code **)(PTR_PTR_800b26f8 + 0x34))(1);
    DAT_800b2701 = (byte)param_1;
    DMACallback(2,0);
  }
  return bVar1;
}


