/* FUN_80105bc4 @ 0x80105bc4  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80105bc4(void)

{
  byte bVar1;
  char cVar2;
  int iVar3;
  uint uVar4;
  undefined1 *puVar5;
  uint unaff_s1;
  
  bVar1 = *(byte *)(_DAT_800ac784 + 7);
  if (bVar1 == 1) {
LAB_80105d40:
    iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
    if (iVar3 == 0) goto LAB_80105f3c;
    *(undefined1 *)(_DAT_800ac784 + 7) = 2;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 7;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
      func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      (uint)(byte)(&LAB_8011e814)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,1);
      uVar4 = (uint)*(byte *)(_DAT_800ac784 + 6);
      if (uVar4 == unaff_s1) {
        func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        *(int *)(_DAT_800ac784 + 0x188) + 0x40,1);
        uVar4 = (uint)*(byte *)(_DAT_800ac784 + 6);
      }
      if (uVar4 == 0) {
        func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        *(int *)(_DAT_800ac784 + 0x188) + 0x198,1);
      }
      func_0x8001af20();
      func_0x800453d0(5);
      puVar5 = &DAT_800b0fe0;
      *(undefined1 *)(_DAT_800ac784 + 0x1b8) = 1;
      if (_DAT_800b0fe0 < 3) {
        FUN_801094d0(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
        return;
      }
      goto LAB_8010673c;
    }
  }
  else {
    if (bVar1 < 2) {
      if (bVar1 != 0) {
        puVar5 = (undefined1 *)0x0;
        goto LAB_8010673c;
      }
      *(undefined1 *)(_DAT_800ac784 + 7) = 1;
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 6;
      *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
      *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
      if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
        func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        (uint)(byte)(&LAB_8011e814)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                        *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011ef14);
      }
      if (*(char *)(_DAT_800ac784 + 6) == '\0') {
        func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                        *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011ef14);
      }
      *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
      func_0x800453d0(6);
      (**(code **)(&LAB_8011eec0 + (uint)*(byte *)(_DAT_800ac784 + 5) * 4))();
      goto LAB_80105d40;
    }
    if (bVar1 != 2) {
      if (bVar1 != 3) {
        puVar5 = (undefined1 *)0x0;
        goto LAB_8010673c;
      }
      iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                              *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
      if (iVar3 != 0) {
        *(undefined4 *)(_DAT_800ac784 + 4) = 0x201;
        *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
        uVar4 = func_0x8001af20();
        if ((uVar4 & 7) == 0) {
          *(undefined1 *)(_DAT_800ac784 + 5) = 8;
        }
        iVar3 = func_0x8001a780(&DAT_800aca54);
        if (iVar3 != 0) {
          *(undefined1 *)(_DAT_800ac784 + 5) = 7;
        }
      }
      goto LAB_80105f3c;
    }
    iVar3 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
    if (iVar3 == 0) goto LAB_80105f3c;
    *(undefined1 *)(_DAT_800ac784 + 7) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 8;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 7;
    if (*(char *)(_DAT_800ac784 + 6) == '\x01') {
      func_0x80019700(0x1000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                      (uint)(byte)(&LAB_8011e814)[*(byte *)(_DAT_800ac784 + 8)] * 0xac +
                      *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_8011ef14);
    }
  }
  if (*(char *)(_DAT_800ac784 + 6) == '\0') {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x198,&LAB_8011ef14);
    puVar5 = (undefined1 *)0x0;
LAB_8010673c:
    func_0x8004ef90(puVar5 + 0x78);
    cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                            *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
    *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
    FUN_80106fd4();
    return;
  }
LAB_80105f3c:
  FUN_801094d0(0,0);
  return;
}


