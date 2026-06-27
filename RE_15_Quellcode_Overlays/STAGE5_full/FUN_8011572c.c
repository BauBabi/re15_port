/* FUN_8011572c @ 0x8011572c  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011572c(void)

{
  uint uVar1;
  uint *puVar2;
  int iVar3;
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 local_38;
  undefined4 local_34;
  undefined4 local_30;
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 local_24;
  undefined4 local_20;
  undefined4 local_1c;
  undefined4 local_18;
  undefined4 local_14;
  
  local_40 = DAT_80100358;
  local_3c = DAT_8010035c;
  local_38 = DAT_80100360;
  local_34 = DAT_80100364;
  local_30 = DAT_80100368;
  local_2c = DAT_8010036c;
  local_28 = DAT_80100370;
  local_24 = DAT_80100374;
  local_20 = DAT_80100014;
  local_1c = DAT_80100018;
  local_18 = DAT_8010001c;
  local_14 = DAT_80100020;
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 1;
  *(byte *)(_DAT_800ac784 + 0x93) = *(byte *)(_DAT_800ac784 + 0x93) | 2;
  *(undefined1 *)(_DAT_800ac784 + 7) = 1;
  *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
  *(undefined1 *)(_DAT_800ac784 + 0x94) = 10;
  *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
  uVar1 = func_0x8001af20();
  iVar3 = (uVar1 & 1) * 8;
  func_0x80019700(0x8031800,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&DAT_8011fdf0)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) + 0x40
                  ,&local_20);
  func_0x80019700(0x8032000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&DAT_8011fdf1)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) + 0x40
                  ,&local_30);
  func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&DAT_8011fdf2)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) + 0x40
                  ,&local_20);
  func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&DAT_8011fdf3)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) + 0x40
                  ,&local_40);
  func_0x80019700(0x8031500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&DAT_8011fdf4)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) + 0x40
                  ,&local_20);
  func_0x80019700(0x8031500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&DAT_8011fdf5)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) + 0x40
                  ,&local_20);
  func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&DAT_8011fdf6)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) + 0x40
                  ,&local_20);
  func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&DAT_8011fdf7)[iVar3] * 0xac + *(int *)(_DAT_800ac784 + 0x188) + 0x40
                  ,&local_20);
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


