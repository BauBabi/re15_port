/* FUN_80100000 @ 0x80100000  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100000(void)

{
  ushort uVar1;
  ushort uVar2;
  int in_v0;
  uint uVar3;
  int iVar4;
  byte bVar5;
  int in_v1;
  
  *(undefined2 *)(in_v1 + 0x1bc) = *(undefined2 *)(in_v0 + -0x3578);
  *(undefined2 *)((int)_DAT_800ac784 + 0x1be) = _DAT_800aca90;
  *_DAT_800ac784 = *_DAT_800ac784 | 0x40000000;
  *(undefined2 *)(_DAT_800ac784 + 0x27) = 0x14;
  _DAT_800ac784[0x1e] = DAT_8011d8e0;
  uVar3 = func_0x8001af20();
  *(undefined1 *)(_DAT_800ac784 + 0x75) = (&LAB_8011d934)[uVar3 & 7];
  *(undefined1 *)(_DAT_800ac784 + 0x6e) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1b9) = 0xe;
  uVar3 = func_0x8001af20();
  *(undefined2 *)((int)_DAT_800ac784 + 0x9a) =
       *(undefined2 *)((uVar3 & 0xf) * 2 + (uint)(byte)_DAT_800ac784[2] * 0x20 + -0x7fee2e38);
  *(undefined1 *)((int)_DAT_800ac784 + 0x1d5) = 0;
  *(undefined2 *)(_DAT_800ac784 + 0x76) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x1d7) = 4;
  uVar1 = func_0x8001af20();
  *(ushort *)(_DAT_800ac784 + 0x77) = (uVar1 & 3) + 4;
  iVar4 = (uint)(byte)(&LAB_8011d8d4)[(byte)_DAT_800ac784[2]] * 0xac + _DAT_800ac784[0x62];
  *(undefined2 *)(iVar4 + 0x98) = 0x60;
  *(undefined2 *)(iVar4 + 0x9a) = 0x30;
  *(undefined2 *)(iVar4 + 0x9c) = 0x390;
  *(undefined2 *)(iVar4 + 0x94) = 0;
  *(undefined2 *)(iVar4 + 0x96) = 0;
  *(undefined2 *)(iVar4 + 0x9e) = 0x138;
  uVar3 = func_0x8001af20();
  if ((uVar3 & 3) == 0) {
    uVar3 = func_0x8001af20();
    (**(code **)((uVar3 & 7) * 4 + -0x7fee26c4))();
  }
  if ((_DAT_800aca3c & 1) != 0) {
    *(ushort *)(_DAT_800ac784 + 0x76) = (ushort)_DAT_800ac784[0x76] | 0x100;
    uVar3 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x76) = (ushort)_DAT_800ac784[0x76] | (ushort)((uVar3 & 1) << 9);
    uVar1 = func_0x8001af20();
    uVar2 = func_0x8001af20();
    iVar4 = (uVar3 & 1) * 8;
    *(ushort *)((int)_DAT_800ac784 + 0x1da) = (uVar2 & 0xff) + (uVar1 & 0xff) + 600;
    func_0x80019700(0x8031800,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_8011d8f4)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &stack0x00000028);
    func_0x80019700(0x8032000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_8011d8f4_1)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &stack0x00000018);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_8011d8f4_2)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_8011d924);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_8011d8f4_3)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_8011d924);
    func_0x80019700(0x8031500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011d8f8)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_8011d924);
    func_0x80019700(0x8031500,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011d8f9)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &stack0x00000018);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011d8fa)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_8011d924);
    func_0x80019700(0x8031000,(int)*(short *)((int)_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_8011d8fb)[iVar4] * 0xac + _DAT_800ac784[0x62] + 0x40,
                    &LAB_8011d924);
  }
  func_0x8001af5c(0,0,600,700);
  *(undefined1 *)(_DAT_800ac784 + 0x25) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
  func_0x8001f314(_DAT_800ac784[0x21],_DAT_800ac784[0x5b],0,0x100);
  bVar5 = *(byte *)((int)_DAT_800ac784 + 9) & 0x1f;
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x80) != 0) {
    if (((bVar5 == 4) || (bVar5 == 7)) || (bVar5 == 9)) {
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 0xc;
    }
    bVar5 = *(byte *)((int)_DAT_800ac784 + 9) & 0x1f;
    if (((bVar5 == 5) || (bVar5 == 8)) || (bVar5 == 10)) {
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 0xe;
    }
    bVar5 = *(byte *)((int)_DAT_800ac784 + 9) & 0x1f;
    if ((bVar5 == 1) || (bVar5 == 3)) {
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 0xc;
      *(undefined1 *)((int)_DAT_800ac784 + 5) = 5;
    }
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
    func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x200);
    bVar5 = *(byte *)((int)_DAT_800ac784 + 9) & 0x1f;
    if (((bVar5 == 4) || (bVar5 == 7)) || (bVar5 == 9)) {
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x12;
    }
    bVar5 = *(byte *)((int)_DAT_800ac784 + 9) & 0x1f;
    if (((bVar5 == 5) || (bVar5 == 8)) || (bVar5 == 10)) {
      *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x13;
    }
  }
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x1f) == 6) {
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x27;
    _DAT_800ac784[1] = 0x20c01;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
    func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x200);
    *(undefined1 *)((int)_DAT_800ac784 + 9) = 0;
  }
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x1f) == 0xb) {
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 3;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
    func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x200);
  }
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x1f) == 0xd) {
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x27;
    _DAT_800ac784[1] = 0x201;
    *(undefined1 *)((int)_DAT_800ac784 + 9) = 0;
  }
  if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x1f) == 0xe) {
    *(undefined1 *)(_DAT_800ac784 + 0x25) = 0x2a;
    *(undefined1 *)((int)_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)((int)_DAT_800ac784 + 0x8f) = 0;
    func_0x8001f314(_DAT_800ac784[0x5c],_DAT_800ac784[0x5d],0,0x200);
    _DAT_800ac784[1] = 0x1201;
    *(undefined1 *)((int)_DAT_800ac784 + 9) = 0;
  }
  func_0x8001ee68(_DAT_800ac784);
  return;
}


