/* FUN_80104264 @ 0x80104264  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104264(void)

{
  char cVar1;
  int in_v0;
  
  func_0x8001aac4((int)*(short *)(in_v0 + 0x1bc),(int)*(short *)(in_v0 + 0x1be),0x20);
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),1,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  FUN_80104b24();
  return;
}


