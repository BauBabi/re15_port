/* FUN_80105934 @ 0x80105934  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105934(void)

{
  byte bVar1;
  short sVar2;
  char cVar3;
  uint uVar4;
  int iVar5;
  int iVar6;
  undefined4 uVar7;
  int iVar8;
  int unaff_s2;
  uint *puVar9;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 != 1) {
    if (1 < bVar1) {
      if (bVar1 != 2) {
        FUN_80106358(0);
        return;
      }
      *(undefined4 *)(_DAT_800ac784 + 4) = 0x201;
      uVar4 = func_0x8001af20();
      if ((uVar4 & 7) == 0) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 8;
      }
      iVar5 = func_0x8001a780(&DAT_800aca54);
      if (iVar5 != 0) {
        *(undefined1 *)(_DAT_800ac784 + 5) = 7;
      }
      *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
      FUN_80109488(0,0);
      func_0x800245d8(0x800);
      return;
    }
    if (bVar1 != 0) {
      FUN_80106358(0);
      return;
    }
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    uVar4 = func_0x8001af20();
    *(undefined1 *)(_DAT_800ac784 + 0x94) = (&LAB_8011dfe4)[uVar4 & 7];
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 10;
    cVar3 = *(char *)(_DAT_800ac784 + 6);
    if (cVar3 == '\x01') {
      func_0x80019700(0x1500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&LAB_8011dfec);
      cVar3 = *(char *)(_DAT_800ac784 + 6);
    }
    if (cVar3 == '\0') {
      func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011dfd4);
    }
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    uVar4 = func_0x8001af20();
    if ((uVar4 & 7) == 0) {
      func_0x800453d0(6);
    }
    (**(code **)(&LAB_8011df80 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
  }
  iVar8 = 0;
  iVar5 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
  iVar6 = (uint)*(byte *)(_DAT_800ac784 + 7) + iVar5;
  *(char *)(_DAT_800ac784 + 7) = (char)iVar6;
  puVar9 = (uint *)((iVar5 * 0x10 + iVar6 * -5) * 4 + *(int *)(iVar8 + 0x188));
  func_0x80019700();
  func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar9 + 0x10);
  puVar9[0x1a] = 0x8f;
  *(undefined2 *)((int)puVar9 + 0x96) = 0xffce;
  *(undefined2 *)(puVar9 + 0x25) = 0;
  *(undefined2 *)(puVar9 + 0x26) = 0;
  *(undefined2 *)((int)puVar9 + 0x9a) = 3;
  *(undefined2 *)(puVar9 + 0x27) = 0;
  *(undefined2 *)((int)puVar9 + 0x9e) = 0;
  *puVar9 = *puVar9 | 0x4a;
  func_0x800453d0(9);
  *(char *)(_DAT_800ac784 + 0x1b8) = (char)unaff_s2;
  iVar5 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar5 + 0x548) = 0x80;
  *(undefined4 *)(iVar5 + 0x54c) = 0x20;
  *(uint *)(iVar5 + 0x4b4) = *(uint *)(iVar5 + 0x4b4) | 0x80;
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfd;
  if (_DAT_800b0fe0 < 3) {
    FUN_8010759c(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    return;
  }
  func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&LAB_8011e2cc);
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfd;
    sVar2 = *(short *)(_DAT_800ac784 + 0x9c);
    *(short *)(_DAT_800ac784 + 0x9c) = sVar2 + -1;
    if (sVar2 == 0) {
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
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0) {
    iVar5 = *(int *)(iVar5 + -0x387c);
    if (*(char *)(iVar5 + 0x95) == '#') {
      func_0x80019700(0x2000,(int)*(short *)(iVar5 + 0x6a),
                      (uint)(byte)(&LAB_8011d8d4)[*(byte *)(iVar5 + 8)] * 0xac +
                      *(int *)(iVar5 + 0x188) + 0x40,&LAB_8011e2bc);
    }
    cVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar3;
    func_0x80022da0();
    if ((*(ushort *)(unaff_s2 + 0x82) & 1) != 0) {
      func_0x80019d50(0x2000);
      return;
    }
    iVar5 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + (char)iVar5;
    if (iVar5 == 0) {
      uVar4 = func_0x8001af20();
      uVar7 = 5;
      if ((uVar4 & 1) != 0) {
        uVar7 = 4;
      }
      func_0x800453d0(uVar7);
    }
    func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),8)
    ;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x100);
    if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) != 0) {
      FUN_801093c8(0,1);
      return;
    }
    FUN_80109488(0,0);
    return;
  }
  FUN_80109488(0,0);
  return;
}


