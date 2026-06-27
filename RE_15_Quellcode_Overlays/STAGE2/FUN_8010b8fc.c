/* FUN_8010b8fc @ 0x8010b8fc  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010b8fc(void)

{
  (*(code *)(&PTR_DAT_80118a40)[*(byte *)(_DAT_800ac784 + 5)])();
  (**(code **)(&LAB_80118a80 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  return;
}


