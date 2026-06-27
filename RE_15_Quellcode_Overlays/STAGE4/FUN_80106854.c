/* FUN_80106854 @ 0x80106854  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80106854(undefined4 param_1)

{
  char cVar1;
  int in_v0;
  int iVar2;
  undefined1 unaff_s2;
  uint *unaff_s3;
  
  func_0x80019700(param_1,(int)*(short *)(in_v0 + 0x6a));
  unaff_s3[0x1a] = 0x8f;
  *(undefined2 *)((int)unaff_s3 + 0x96) = 0xffce;
  *(undefined2 *)(unaff_s3 + 0x25) = 0;
  *(undefined2 *)(unaff_s3 + 0x26) = 0;
  *(undefined2 *)((int)unaff_s3 + 0x9a) = 3;
  *(undefined2 *)(unaff_s3 + 0x27) = 0;
  *(undefined2 *)((int)unaff_s3 + 0x9e) = 0;
  *unaff_s3 = *unaff_s3 | 0x4a;
  func_0x800453d0(9);
  *(undefined1 *)(_DAT_800ac784 + 0x1b8) = unaff_s2;
  iVar2 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar2 + 0x548) = 0x80;
  *(undefined4 *)(iVar2 + 0x54c) = 0x20;
  *(uint *)(iVar2 + 0x4b4) = *(uint *)(iVar2 + 0x4b4) | 0x80;
  if (_DAT_800b0fe0 < 3) {
    FUN_80107120(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    return;
  }
  func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
  FUN_801071b4();
  return;
}


