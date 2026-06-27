/* FUN_80104c5c @ 0x80104c5c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80104c5c(void)

{
  char cVar1;
  int iVar2;
  undefined1 in_v0;
  uint uVar3;
  int iVar4;
  int in_v1;
  undefined4 uVar5;
  int iVar6;
  int iVar7;
  
  *(undefined1 *)(in_v1 + 6) = in_v0;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x12;
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)(_DAT_800ac784 + 6) = 5;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 0;
  uVar3 = func_0x8001af20();
  if ((uVar3 & 7) == 0) {
    uVar3 = func_0x8001af20();
    uVar5 = 8;
    if ((uVar3 & 1) != 0) {
      uVar5 = 5;
    }
    func_0x800453d0(uVar5);
  }
  iVar6 = 0;
  iVar7 = 0x100;
  iVar4 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  iVar2 = _DAT_800ac784;
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + (char)iVar4;
  iVar4 = (uint)(byte)(&LAB_8011e814)[iVar4] * 0xac + *(int *)(iVar6 + 0x188);
  func_0x80019700(iVar2,(int)*(short *)(iVar6 + 0x6a),iVar4 + 0x40,iVar7 + -0x10ec);
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    iVar4 = *(int *)(_DAT_800ac784 + 0x188) + 0x158;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011ef14);
  }
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  func_0x800453d0(6);
  (**(code **)(&LAB_8011eec0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  *(short *)(iVar4 + 100) = *(short *)(iVar4 + 100) + *(short *)(_DAT_800ac784 + 0x9c);
  *(short *)(_DAT_800ac784 + 0x9c) = *(short *)(_DAT_800ac784 + 0x9c) + -0x80;
  func_0x80068098(iVar4 + 0x60,iVar4 + 0x18);
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


