/* SetDispMask @ 0x80090304  (Ghidra headless, raw MIPS overlay) */

void SetDispMask(int mask)

{
  undefined4 uVar1;
  
  if (1 < DAT_800b2702) {
    (*(code *)PTR_printf_800b26fc)("SetDispMask(%d)...\n",mask);
  }
  if (mask == 0) {
    memset("",0xff,0x14);
  }
  uVar1 = 0x3000001;
  if (mask != 0) {
    uVar1 = 0x3000000;
  }
  (**(code **)(PTR_PTR_800b26f8 + 0x10))(uVar1);
  return;
}


