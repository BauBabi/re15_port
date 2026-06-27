/* FUN_8011bf50 @ 0x8011bf50  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011bf50(void)

{
  int in_v1;
  int iVar1;
  
  *(undefined1 *)(*(int *)(in_v1 + -0x387c) + 0x9e) = 0x78;
  *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x1b9) = 8;
  iVar1 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined2 *)(iVar1 + 0x5f8) = 0x40;
  *(undefined2 *)(iVar1 + 0x5fa) = 0x30;
  *(undefined2 *)(iVar1 + 0x5fc) = 0x2c8;
  *(undefined2 *)(iVar1 + 0x5f4) = 0;
  *(undefined2 *)(iVar1 + 0x5f6) = 0;
  *(undefined2 *)(iVar1 + 0x5fe) = 0x138;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 2;
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  func_0x8001af5c(0,0,600,700);
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x40) != 0) {
    *(undefined1 *)(_DAT_800ac784 + 4) = 4;
    *(undefined1 *)(_DAT_800ac784 + 5) = 6;
  }
  return;
}


