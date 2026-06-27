void SetDispMask(int mask)

{
  undefined4 uVar1;
  
  if (1 < DAT_8007e352) {
    printf("SetDispMask(%d)...\n",mask);
  }
  if (mask == 0) {
    memset("",0xff,0x14);
  }
  uVar1 = 0x3000001;
  if (mask != 0) {
    uVar1 = 0x3000000;
  }
  _ctl(uVar1);
  return;
}
