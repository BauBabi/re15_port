/* FUN_8011388c @ 0x8011388c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011388c(undefined4 param_1)

{
  short sVar1;
  char cVar2;
  int iVar3;
  uint uVar4;
  int iVar5;
  uint *puVar6;
  
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x9c) = 0x1e;
  func_0x80019700(param_1,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  *(int *)(_DAT_800ac784 + 0x188) + 0x40,0x80118f18);
  puVar6 = *(uint **)(_DAT_800ac784 + 0x188);
  puVar6[0x1a] = 0x207f60;
  *(undefined2 *)(puVar6 + 0x25) = 0;
  *(undefined2 *)((int)puVar6 + 0x96) = 0xffce;
  *(undefined2 *)(puVar6 + 0x26) = 0;
  *(undefined2 *)((int)puVar6 + 0x9a) = 3;
  *(undefined2 *)(puVar6 + 0x27) = 0;
  *(undefined2 *)((int)puVar6 + 0x9e) = 0;
  *puVar6 = *puVar6 | 0x4a;
  iVar3 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar3 + 0xd2c) = 0x207f60;
  *(undefined2 *)(iVar3 + 0xd58) = 0;
  *(undefined2 *)(iVar3 + 0xd5a) = 0xffce;
  *(undefined2 *)(iVar3 + 0xd5c) = 0;
  *(undefined2 *)(iVar3 + 0xd5e) = 3;
  *(undefined2 *)(iVar3 + 0xd60) = 0;
  *(undefined2 *)(iVar3 + 0xd62) = 0;
  *(uint *)(iVar3 + 0xcc4) = *(uint *)(iVar3 + 0xcc4) | 0x4a;
  func_0x80019700(0x82000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  *(int *)(_DAT_800ac784 + 0x188) + 0x40,0x80118f18);
  func_0x80019700(0x82000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  *(int *)(_DAT_800ac784 + 0x188) + 0xd04,0x80118f18);
  func_0x800453d0(5);
  iVar3 = _DAT_800ac784;
  *(undefined2 *)(_DAT_800ac784 + 0x1e0) = 9;
  sVar1 = *(short *)(_DAT_800ac784 + 0x1e0);
  iVar3 = *(int *)(iVar3 + 0x188);
  *(short *)(_DAT_800ac784 + 0x1e0) = sVar1 + -1;
  while (sVar1 != 1) {
    iVar5 = *(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar3;
    uVar4 = *(uint *)(iVar5 + 0xac);
    if ((uVar4 & 0x41) == 1) {
      *(uint *)(iVar5 + 0xac) = uVar4 | 0x1062;
      iVar5 = *(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar3;
      *(uint *)(iVar5 + 0x158) = *(uint *)(iVar5 + 0x158) | 0x1062;
      *(undefined1 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar3 + 0x121) = 0x14;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar3 + 0x12e) = 0xff80;
      *(undefined1 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar3 + 0x122) = 8;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar3 + 0xe4) = 0;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar3 + 0xe6) = 0xff9c;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar3 + 0xe8) = 0;
      *(undefined1 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar3 + 0x1cd) = 0x14;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar3 + 0x1da) = 0xff80;
      *(undefined1 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar3 + 0x1ce) = 8;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar3 + 400) = 0;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar3 + 0x192) = 0xff9c;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + iVar3 + 0x194) = 0;
    }
    sVar1 = *(short *)(_DAT_800ac784 + 0x1e0);
    *(short *)(_DAT_800ac784 + 0x1e0) = sVar1 + -1;
  }
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  return;
}


