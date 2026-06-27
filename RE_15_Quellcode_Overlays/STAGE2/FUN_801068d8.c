/* FUN_801068d8 @ 0x801068d8  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801068d8(void)

{
  short sVar1;
  byte bVar2;
  char cVar3;
  uint uVar4;
  int iVar5;
  undefined4 uVar6;
  uint *puVar7;
  
  if (*(char *)(_DAT_800ac784 + 7) == '\0') {
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    *(undefined1 *)(_DAT_800ac784 + 0x9e) = 0x28;
    bVar2 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x9f) = (bVar2 & 0x3f) + 0x3c;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = *(undefined1 *)(_DAT_800ac784 + 0x1d4);
    bVar2 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x95) = bVar2 & 3;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x9c) = 4;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_80117888)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_80118270);
    puVar7 = (uint *)((uint)(byte)(&LAB_80117888)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                     *(int *)(_DAT_800ac784 + 0x188));
    func_0x80019700(0x5002800,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar7 + 0x10,&LAB_80118270);
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar7 + 0x10,&LAB_80118270);
    puVar7[0x1a] = 0x8f;
    *(undefined2 *)((int)puVar7 + 0x96) = 0xffce;
    *(undefined2 *)(puVar7 + 0x25) = 0;
    *(undefined2 *)(puVar7 + 0x26) = 0;
    *(undefined2 *)((int)puVar7 + 0x9a) = 3;
    *(undefined2 *)(puVar7 + 0x27) = 0;
    *(undefined2 *)((int)puVar7 + 0x9e) = 0;
    *puVar7 = *puVar7 | 0x4a;
    func_0x800453d0(9);
    *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 1;
    iVar5 = *(int *)(_DAT_800ac784 + 0x188);
    *(undefined4 *)(iVar5 + 0x548) = 0x80;
    *(undefined4 *)(iVar5 + 0x54c) = 0x20;
    *(uint *)(iVar5 + 0x4b4) = *(uint *)(iVar5 + 0x4b4) | 0x80;
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfd;
    if (_DAT_800b0fe0 < 3) {
      FUN_80107344(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
      return;
    }
    func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  }
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&LAB_80118280);
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfd;
    sVar1 = *(short *)(_DAT_800ac784 + 0x9c);
    *(short *)(_DAT_800ac784 + 0x9c) = sVar1 + -1;
    if (sVar1 == 0) {
      *(undefined1 *)(_DAT_800ac784 + 0x9f) = 0;
    }
  }
  func_0x8001a8f8(&DAT_800aca88,(int)*(char *)(_DAT_800ac784 + 0x9e));
  iVar5 = 0;
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x200);
  cVar3 = *(char *)(_DAT_800ac784 + 0x9f);
  *(char *)(_DAT_800ac784 + 0x9f) = cVar3 + -1;
  if (cVar3 == '\0') {
    *(undefined2 *)(_DAT_800ac784 + 6) = 0x101;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xb;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
  }
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) == 0) {
    FUN_80109230(0,0);
    return;
  }
  iVar5 = *(int *)(iVar5 + -0x387c);
  if (*(char *)(iVar5 + 0x95) == '#') {
    func_0x80019700(0x2000,(int)*(short *)(iVar5 + 0x6a),
                    (uint)(byte)(&LAB_80117888)[*(byte *)(iVar5 + 8)] * 0xac +
                    *(int *)(iVar5 + 0x188) + 0x40,&LAB_80118270);
  }
  cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar3;
  func_0x80022da0();
  if ((_DAT_00000083 & 1) != 0) {
    func_0x80019d50(0x2000);
    return;
  }
  iVar5 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + (char)iVar5;
  if (iVar5 == 0) {
    uVar4 = func_0x8001af20();
    uVar6 = 5;
    if ((uVar4 & 1) != 0) {
      uVar6 = 4;
    }
    func_0x800453d0(uVar6);
  }
  func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),8);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x100);
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0) {
    FUN_80109170(0,1);
    return;
  }
  FUN_80109230(0,0);
  return;
}


