/* FUN_80104b24 @ 0x80104b24  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104b24(void)

{
  char cVar1;
  int in_v0;
  int iVar2;
  int iVar3;
  
  cVar1 = func_0x8001f314(*(undefined4 *)(*(int *)(in_v0 + -0x387c) + 0x170),
                          *(undefined4 *)(*(int *)(in_v0 + -0x387c) + 0x174),0,0x100);
  iVar3 = _DAT_800ac784;
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  iVar2 = FUN_80109488(0,0);
  iVar3 = (uint)(byte)(&LAB_8011d8d4)[iVar2] * 0xac + *(int *)(iVar3 + 0x188);
  func_0x80019700();
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    iVar3 = *(int *)(_DAT_800ac784 + 0x188) + 0x158;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011dfd4);
  }
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  func_0x800453d0(6);
  (**(code **)(&LAB_8011df80 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  *(short *)(iVar3 + 100) = *(short *)(iVar3 + 100) + *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + -0x80;
  func_0x80068098(iVar3 + 0x60,iVar3 + 0x18);
  cVar1 = *(char *)(_DAT_800ac784 + 0x9e);
  *(char *)(_DAT_800ac784 + 0x9e) = cVar1 + -1;
  if (cVar1 == '\0') {
    *(undefined1 *)(_DAT_800ac784 + 7) = 2;
    *(short *)(_DAT_800ac784 + 0x8c) =
         *(short *)(_DAT_800ac784 + 0x8c) - (short)*(char *)(_DAT_800ac784 + 0x9e);
    *(char *)(_DAT_800ac784 + 0x9e) = *(char *)(_DAT_800ac784 + 0x9e) + -1;
    if (*(char *)(_DAT_800ac784 + 0x9e) < '\0') {
      *(undefined1 *)(_DAT_800ac784 + 0x9e) = 0;
    }
    return;
  }
  func_0x800245d8(0);
  return;
}


