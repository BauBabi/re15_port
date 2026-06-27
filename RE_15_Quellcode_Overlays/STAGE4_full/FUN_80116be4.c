/* FUN_80116be4 @ 0x80116be4  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80116be4(void)

{
  int local_18 [4];
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    func_0x8001bd60(0xfffffff6,0x14);
    local_18[0] = (int)*(short *)(_DAT_800ac784 + 0x1dc);
    local_18[2] = (int)*(short *)(_DAT_800ac784 + 0x1de);
    local_18[1] = 0;
    func_0x80039e7c(local_18,0,0);
    (*(code *)(&PTR_FUN_8011a2d8)[*(byte *)(_DAT_800ac784 + 4)])();
    func_0x8002b498(_DAT_800ac784);
    func_0x8002aec4(&DAT_800aca54,_DAT_800ac784);
    func_0x8002b544();
    func_0x8003b0a4(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6),4);
    func_0x80037358();
    func_0x8001b38c();
  }
  func_0x8001b064(_DAT_800ac784 + 0xb0,(int)*(short *)(_DAT_800ac784 + 0x1ba));
  return;
}


