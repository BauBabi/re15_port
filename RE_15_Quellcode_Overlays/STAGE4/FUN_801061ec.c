/* FUN_801061ec @ 0x801061ec  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_801061ec(undefined4 param_1,undefined4 param_2,int param_3)

{
  bool bVar1;
  short sVar2;
  byte bVar3;
  char cVar4;
  int iVar5;
  uint uVar6;
  undefined4 uVar7;
  int unaff_s2;
  uint *puVar8;
  
  uVar6 = (uint)*(byte *)(_DAT_800ac784 + 7);
  if (uVar6 != 1) {
    if (1 < uVar6) {
      iVar5 = 2;
      bVar1 = uVar6 == 2;
      uVar6 = 3;
      if (bVar1) {
        *(undefined4 *)(_DAT_800ac784 + 4) = 1;
        if ((*(byte *)(_DAT_800ac784 + 9) & 0x1f) == 3) {
          *(undefined1 *)(_DAT_800ac784 + 5) = 5;
        }
        *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfe;
        return;
      }
      goto LAB_80106b48;
    }
    iVar5 = 1;
    if (uVar6 != 0) goto LAB_80106b48;
    *(undefined1 *)(_DAT_800ac784 + 7) = 1;
    bVar3 = func_0x8001af20();
    *(byte *)(_DAT_800ac784 + 0x94) = (bVar3 & 1) + 0x25;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 4;
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x40,&LAB_801193c4);
    *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
    uVar6 = func_0x8001af20();
    if ((uVar6 & 7) == 0) {
      func_0x800453d0(6);
    }
  }
  param_3 = 0;
  iVar5 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x200);
  uVar6 = (uint)*(byte *)(_DAT_800ac784 + 7) + iVar5;
  *(char *)(_DAT_800ac784 + 7) = (char)uVar6;
LAB_80106b48:
  puVar8 = (uint *)((iVar5 * 0x10 + uVar6 * -5) * 4 + *(int *)(param_3 + 0x188));
  func_0x80019700();
  func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),puVar8 + 0x10);
  puVar8[0x1a] = 0x8f;
  *(undefined2 *)((int)puVar8 + 0x96) = 0xffce;
  *(undefined2 *)(puVar8 + 0x25) = 0;
  *(undefined2 *)(puVar8 + 0x26) = 0;
  *(undefined2 *)((int)puVar8 + 0x9a) = 3;
  *(undefined2 *)(puVar8 + 0x27) = 0;
  *(undefined2 *)((int)puVar8 + 0x9e) = 0;
  *puVar8 = *puVar8 | 0x4a;
  func_0x800453d0(9);
  *(char *)(_DAT_800ac784 + 0x1b8) = (char)unaff_s2;
  iVar5 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar5 + 0x548) = 0x80;
  *(undefined4 *)(iVar5 + 0x54c) = 0x20;
  *(uint *)(iVar5 + 0x4b4) = *(uint *)(iVar5 + 0x4b4) | 0x80;
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) & 0xfd;
  if (_DAT_800b0fe0 < 3) {
    FUN_80107464(0x800b1038,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
    return;
  }
  func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  if ((*(byte *)(_DAT_800ac784 + 0x93) & 2) != 0) {
    func_0x80019700(0x2000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    *(int *)(_DAT_800ac784 + 0x188) + 0x4f4,&DAT_801196a4);
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
  cVar4 = *(char *)(_DAT_800ac784 + 0x9f);
  *(char *)(_DAT_800ac784 + 0x9f) = cVar4 + -1;
  if (cVar4 == '\0') {
    *(undefined2 *)(_DAT_800ac784 + 6) = 0x101;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xb;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0xf;
  }
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) == 0) {
    FUN_80109350(0,0);
    return;
  }
  iVar5 = *(int *)(iVar5 + -0x387c);
  if (*(char *)(iVar5 + 0x95) == '#') {
    func_0x80019700(0x2000,(int)*(short *)(iVar5 + 0x6a),
                    (uint)(byte)(&LAB_80118cac)[*(byte *)(iVar5 + 8)] * 0xac +
                    *(int *)(iVar5 + 0x188) + 0x40,&LAB_80119694);
  }
  cVar4 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar4;
  func_0x80022da0();
  if ((*(ushort *)(unaff_s2 + 0x82) & 1) != 0) {
    func_0x80019d50(0x2000);
    return;
  }
  iVar5 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),
                          *(undefined4 *)(_DAT_800ac784 + 0x174),0,0x100);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + (char)iVar5;
  if (iVar5 == 0) {
    uVar6 = func_0x8001af20();
    uVar7 = 5;
    if ((uVar6 & 1) != 0) {
      uVar7 = 4;
    }
    func_0x800453d0(uVar7);
  }
  func_0x8001aac4((int)*(short *)(_DAT_800ac784 + 0x1bc),(int)*(short *)(_DAT_800ac784 + 0x1be),8);
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x100);
  if ((**(uint **)(_DAT_800ac784 + 0x168) & 0x2000) == 0) {
    FUN_80109350(0,0);
    return;
  }
  FUN_80109290(0,1);
  return;
}


