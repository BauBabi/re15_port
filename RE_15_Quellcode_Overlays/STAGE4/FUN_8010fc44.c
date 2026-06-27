/* FUN_8010fc44 @ 0x8010fc44  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8010fc44(undefined4 param_1)

{
  char cVar1;
  int in_v0;
  int iVar2;
  int extraout_v1;
  undefined4 uStack00000010;
  undefined4 uStack00000014;
  undefined4 uStack00000018;
  undefined4 uStack0000001c;
  
  iVar2 = (*(byte *)(*(int *)(in_v0 + -0x387c) + 6) & 1) * 0x20;
  uStack00000010 = *(undefined4 *)(iVar2 + -0x7fee616c);
  uStack00000014 = *(undefined4 *)(iVar2 + -0x7fee6168);
  uStack00000018 = *(undefined4 *)(iVar2 + -0x7fee6164);
  uStack0000001c = *(undefined4 *)(iVar2 + -0x7fee6160);
  func_0x80019700(param_1,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  *(int *)(_DAT_800ac784 + 0x188) + 0x40,&stack0x00000010);
  func_0x800453d0(2);
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
  *(short *)(_DAT_800ac784 + 0x8c) = *(short *)(_DAT_800ac784 + 0x9c) * -4 + 0x50;
  func_0x800245d8(0x800);
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 0x80) == 0) {
    return;
  }
  iVar2 = 0;
  func_0x800245d8();
  if (extraout_v1 != 0) {
    FUN_80110f78();
    return;
  }
  *(undefined1 *)(iVar2 + 6) = 1;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x1f;
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0;
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 6) = *(char *)(_DAT_800ac784 + 6) + cVar1;
  FUN_80110f78();
  return;
}


