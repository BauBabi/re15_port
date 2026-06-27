/* FUN_80100000 @ 0x80100000  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80100000(int param_1)

{
  ushort uVar1;
  ushort uVar2;
  int iVar3;
  uint uVar4;
  byte bVar5;
  
  iVar3 = (uint)(byte)(&LAB_80118cac)[*(byte *)(param_1 + 8)] * 0xac + *(int *)(param_1 + 0x188);
  *(undefined2 *)(iVar3 + 0x98) = 0x60;
  *(undefined2 *)(iVar3 + 0x9a) = 0x30;
  *(undefined2 *)(iVar3 + 0x9c) = 0x390;
  *(undefined2 *)(iVar3 + 0x94) = 0;
  *(undefined2 *)(iVar3 + 0x96) = 0;
  *(undefined2 *)(iVar3 + 0x9e) = 0x138;
  uVar4 = func_0x8001af20();
  if ((uVar4 & 3) == 0) {
    uVar4 = func_0x8001af20();
    (**(code **)((uVar4 & 7) * 4 + -0x7fee72ec))();
  }
  if ((_DAT_800aca3c & 1) != 0) {
    *(ushort *)(_DAT_800ac784 + 0x1d8) = *(ushort *)(_DAT_800ac784 + 0x1d8) | 0x100;
    uVar4 = func_0x8001af20();
    *(ushort *)(_DAT_800ac784 + 0x1d8) =
         *(ushort *)(_DAT_800ac784 + 0x1d8) | (ushort)((uVar4 & 1) << 9);
    uVar1 = func_0x8001af20();
    uVar2 = func_0x8001af20();
    iVar3 = (uVar4 & 1) * 8;
    *(ushort *)(_DAT_800ac784 + 0x1da) = (uVar2 & 0xff) + (uVar1 & 0xff) + 600;
    func_0x80019700(0x8031800,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_80118ccc)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                    0x40,&stack0x00000028);
    func_0x80019700(0x8032000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_80118ccc_1)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                    0x40,&stack0x00000018);
    func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_80118ccc_2)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                    0x40,&LAB_80118cfc);
    func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_80118ccc_3)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                    0x40,&LAB_80118cfc);
    func_0x80019700(0x8031500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80118cd0)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                    0x40,&LAB_80118cfc);
    func_0x80019700(0x8031500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80118cd1)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                    0x40,&stack0x00000018);
    func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80118cd2)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                    0x40,&LAB_80118cfc);
    func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&DAT_80118cd3)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                    0x40,&LAB_80118cfc);
  }
  func_0x8001af5c(0,0,600,700);
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                  0x100);
  bVar5 = *(byte *)(_DAT_800ac784 + 9) & 0x1f;
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x80) != 0) {
    if (((bVar5 == 4) || (bVar5 == 7)) || (bVar5 == 9)) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xc;
    }
    bVar5 = *(byte *)(_DAT_800ac784 + 9) & 0x1f;
    if (((bVar5 == 5) || (bVar5 == 8)) || (bVar5 == 10)) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xe;
    }
    bVar5 = *(byte *)(_DAT_800ac784 + 9) & 0x1f;
    if ((bVar5 == 1) || (bVar5 == 3)) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0xc;
      *(undefined1 *)(_DAT_800ac784 + 5) = 5;
    }
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),*(undefined4 *)(_DAT_800ac784 + 0x174),0,
                    0x200);
    bVar5 = *(byte *)(_DAT_800ac784 + 9) & 0x1f;
    if (((bVar5 == 4) || (bVar5 == 7)) || (bVar5 == 9)) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x12;
    }
    bVar5 = *(byte *)(_DAT_800ac784 + 9) & 0x1f;
    if (((bVar5 == 5) || (bVar5 == 8)) || (bVar5 == 10)) {
      *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x13;
    }
  }
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x1f) == 6) {
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x27;
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x20c01;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),*(undefined4 *)(_DAT_800ac784 + 0x174),0,
                    0x200);
    *(undefined1 *)(_DAT_800ac784 + 9) = 0;
  }
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x1f) == 0xb) {
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 3;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),*(undefined4 *)(_DAT_800ac784 + 0x174),0,
                    0x200);
  }
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x1f) == 0xd) {
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x27;
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x201;
    *(undefined1 *)(_DAT_800ac784 + 9) = 0;
  }
  if ((*(byte *)(_DAT_800ac784 + 9) & 0x1f) == 0xe) {
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 0x2a;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x170),*(undefined4 *)(_DAT_800ac784 + 0x174),0,
                    0x200);
    *(undefined4 *)(_DAT_800ac784 + 4) = 0x1201;
    *(undefined1 *)(_DAT_800ac784 + 9) = 0;
  }
  func_0x8001ee68(_DAT_800ac784);
  return;
}


