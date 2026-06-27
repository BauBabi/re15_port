/* FUN_8011c654 @ 0x8011c654  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011c654(void)

{
  char cVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  short in_stack_fffffff0;
  
  cVar1 = func_0x800509e4(15000,&stack0xfffffff0,0x800,0);
  if (cVar1 != '\0') {
    uVar2 = func_0x8005070c(0x5dc,(int)in_stack_fffffff0,-(int)in_stack_fffffff0);
    *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar2;
    *(undefined1 *)(_DAT_800ac784 + 9) = 1;
    if (((_DAT_800aca40 & 0x20000000) == 0) && ((*(byte *)(_DAT_800ac784 + 9) & 0x20) == 0)) {
      iVar3 = (int)(((uint)_DAT_800aca88 - (uint)*(ushort *)(_DAT_800ac784 + 0x34)) * 0x10000) >>
              0x10;
      iVar4 = (int)(((uint)_DAT_800aca90 - (uint)*(ushort *)(_DAT_800ac784 + 0x3c)) * 0x10000) >>
              0x10;
      uVar2 = func_0x80065f60(iVar3 * iVar3 + iVar4 * iVar4);
      *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar2;
      func_0x8001bd60(0xfffffff6,0x14);
      func_0x80039e7c(&DAT_800aca88,0,0);
      (**(code **)((uint)*(byte *)(_DAT_800ac784 + 4) * 4 + -0x7fede860))();
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
  uVar2 = func_0x8005070c(0x5dc,(int)in_stack_fffffff0,-(int)in_stack_fffffff0);
  *(undefined4 *)(_DAT_800ac784 + 0x1d0) = uVar2;
  *(undefined1 *)(_DAT_800ac784 + 9) = 0;
  (**(code **)((*(byte *)(_DAT_800ac784 + 9) & 0xf) * 4 + -0x7fede984))();
  return;
}


