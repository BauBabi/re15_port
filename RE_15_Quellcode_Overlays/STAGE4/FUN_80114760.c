/* FUN_80114760 @ 0x80114760  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114760(void)

{
  byte *in_at;
  undefined1 in_v0;
  char cVar1;
  uint *puVar2;
  int iVar3;
  int in_v1;
  int unaff_s0;
  
  *(undefined1 *)(in_v1 + 0x8f) = in_v0;
  cVar1 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  iVar3 = _DAT_800ac784;
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar1;
  func_0x80019700(iVar3,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)*in_at * 0xac + *(int *)(_DAT_800ac784 + 0x188) + 0x40);
  func_0x80019700(0x8031500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)*(byte *)(unaff_s0 + -0x7fee5d74) * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40);
  func_0x80019700(0x8031500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)*(byte *)(unaff_s0 + -0x7fee5d73) * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40);
  func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)*(byte *)(unaff_s0 + -0x7fee5d72) * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40);
  func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)*(byte *)(unaff_s0 + -0x7fee5d71) * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40);
  puVar2 = *(uint **)(_DAT_800ac784 + 0x188);
  puVar2[0x25] = 0;
  puVar2[0x26] = 0;
  *puVar2 = *puVar2 | 0x80;
  iVar3 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar3 + 0x3f0) = 0;
  *(undefined4 *)(iVar3 + 0x3f4) = 0;
  *(uint *)(iVar3 + 0x35c) = *(uint *)(iVar3 + 0x35c) | 0x80;
  iVar3 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar3 + 0xd58) = 0;
  *(undefined4 *)(iVar3 + 0xd5c) = 0;
  *(uint *)(iVar3 + 0xcc4) = *(uint *)(iVar3 + 0xcc4) | 0x80;
  iVar3 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar3 + 0xc00) = 0;
  *(undefined4 *)(iVar3 + 0xc04) = 0;
  *(uint *)(iVar3 + 0xb6c) = *(uint *)(iVar3 + 0xb6c) | 0x80;
  iVar3 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar3 + 0x8a4) = 0;
  *(undefined4 *)(iVar3 + 0x8a8) = 0;
  *(uint *)(iVar3 + 0x810) = *(uint *)(iVar3 + 0x810) | 0x80;
  iVar3 = *(int *)(_DAT_800ac784 + 0x188);
  *(uint *)(iVar3 + 0x60c) = *(uint *)(iVar3 + 0x60c) | 0x80;
  *(undefined4 *)(iVar3 + 0x6a0) = 0;
  *(undefined4 *)(iVar3 + 0x6a4) = 0;
  iVar3 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar3 + 0xf5c) = 0;
  *(undefined4 *)(iVar3 + 0xf60) = 0;
  *(uint *)(iVar3 + 0xec8) = *(uint *)(iVar3 + 0xec8) | 0x80;
  iVar3 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar3 + 0x1160) = 0;
  *(undefined4 *)(iVar3 + 0x1164) = 0;
  *(uint *)(iVar3 + 0x10cc) = *(uint *)(iVar3 + 0x10cc) | 0x80;
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),1,
                  0x200);
  *(undefined4 *)(_DAT_800ac784 + 4) = 7;
  return;
}


