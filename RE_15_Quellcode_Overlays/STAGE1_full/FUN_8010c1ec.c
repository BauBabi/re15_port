/* FUN_8010c1ec @ 0x8010c1ec  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010c1ec(void)

{
  int iVar1;
  int iVar2;
  
  if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
    iVar1 = (int)(((uint)_DAT_800aca88 - (uint)*(ushort *)(_DAT_800ac784 + 0x34)) * 0x10000) >> 0x10
    ;
    iVar2 = (int)(((uint)_DAT_800aca90 - (uint)*(ushort *)(_DAT_800ac784 + 0x3c)) * 0x10000) >> 0x10
    ;
    func_0x80065f60(iVar1 * iVar1 + iVar2 * iVar2);
    func_0x8001bd60(0xfffffff6,0x14);
    (*(code *)(&PTR_FUN_8012093c)[*(byte *)(_DAT_800ac784 + 4)])();
    *(byte *)(_DAT_800ac784 + 0x1b8) = *(byte *)(_DAT_800ac784 + 0x1b8) | 0x12;
    func_0x8002b498(_DAT_800ac784);
    func_0x8002aec4(&DAT_800aca54,_DAT_800ac784);
    func_0x8002b544();
    func_0x8003b0a4(_DAT_800ac784 + 0x34,*(undefined2 *)(*(int *)(_DAT_800ac784 + 0x78) + 6),4);
  }
  return;
}


