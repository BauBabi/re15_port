/* FUN_80114730 @ 0x80114730  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80114730(undefined4 param_1)

{
  byte bVar1;
  undefined4 *in_at;
  char cVar2;
  bool bVar3;
  int in_v0;
  uint *puVar4;
  int iVar5;
  int in_v1;
  int unaff_s0;
  undefined4 uStack00000010;
  undefined4 uStack00000014;
  undefined4 uStack00000018;
  undefined4 uStack0000001c;
  
  uStack00000010 = *in_at;
  uStack00000014 = *(undefined4 *)(&LAB_8011fcd4 + in_v0);
  uStack00000018 = *(undefined4 *)(&LAB_8011fcd8 + in_v0);
  uStack0000001c = *(undefined4 *)(&LAB_8011fcdc + in_v0);
  func_0x80019700(param_1,(int)*(short *)(in_v1 + 0x6a),*(int *)(in_v1 + 0x188) + 0x40);
  func_0x800453d0(2);
  func_0x8004ef90(0x800b1058,*(undefined1 *)(_DAT_800ac784 + 0x1c6));
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  bVar1 = *(byte *)(_DAT_800ac784 + 0x95);
  if (((bVar1 < 0xf) || (bVar3 = bVar1 < 0x11, *(char *)(_DAT_800ac784 + 0x94) != '\b')) &&
     ((bVar3 = true, bVar1 < 0x11 || (*(char *)(_DAT_800ac784 + 0x94) != '\t')))) {
    FUN_80115d6c(0,1);
    if (*(char *)(_DAT_800ac784 + 0x95) != '\x18') {
      return;
    }
    func_0x800453d0(7);
    *(undefined1 *)(_DAT_800ac784 + 7) = 2;
  }
  else {
    unaff_s0 = (uint)bVar3 * 8;
    func_0x80019700(0,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_8011fdf0)[unaff_s0] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                    0x40);
    func_0x80019700(0x8032000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_8011fdf0_1)[unaff_s0] * 0xac + *(int *)(_DAT_800ac784 + 0x188)
                    + 0x40,&stack0x00000020);
    func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                    (uint)(byte)(&LAB_8011fdf0_2)[unaff_s0] * 0xac + *(int *)(_DAT_800ac784 + 0x188)
                    + 0x40);
  }
  func_0x80019700();
  func_0x80019700(0x8031500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&LAB_8011fdf4)[unaff_s0] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40);
  func_0x80019700(0x8031500,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&LAB_8011fdf4_1)[unaff_s0] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40);
  func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&LAB_8011fdf4_2)[unaff_s0] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40);
  func_0x80019700(0x8031000,(int)*(short *)(_DAT_800ac784 + 0x6a),
                  (uint)(byte)(&LAB_8011fdf4_3)[unaff_s0] * 0xac + *(int *)(_DAT_800ac784 + 0x188) +
                  0x40);
  puVar4 = *(uint **)(_DAT_800ac784 + 0x188);
  puVar4[0x25] = 0;
  puVar4[0x26] = 0;
  *puVar4 = *puVar4 | 0x80;
  iVar5 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar5 + 0x3f0) = 0;
  *(undefined4 *)(iVar5 + 0x3f4) = 0;
  *(uint *)(iVar5 + 0x35c) = *(uint *)(iVar5 + 0x35c) | 0x80;
  iVar5 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar5 + 0xd58) = 0;
  *(undefined4 *)(iVar5 + 0xd5c) = 0;
  *(uint *)(iVar5 + 0xcc4) = *(uint *)(iVar5 + 0xcc4) | 0x80;
  iVar5 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar5 + 0xc00) = 0;
  *(undefined4 *)(iVar5 + 0xc04) = 0;
  *(uint *)(iVar5 + 0xb6c) = *(uint *)(iVar5 + 0xb6c) | 0x80;
  iVar5 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar5 + 0x8a4) = 0;
  *(undefined4 *)(iVar5 + 0x8a8) = 0;
  *(uint *)(iVar5 + 0x810) = *(uint *)(iVar5 + 0x810) | 0x80;
  iVar5 = *(int *)(_DAT_800ac784 + 0x188);
  *(uint *)(iVar5 + 0x60c) = *(uint *)(iVar5 + 0x60c) | 0x80;
  *(undefined4 *)(iVar5 + 0x6a0) = 0;
  *(undefined4 *)(iVar5 + 0x6a4) = 0;
  iVar5 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar5 + 0xf5c) = 0;
  *(undefined4 *)(iVar5 + 0xf60) = 0;
  *(uint *)(iVar5 + 0xec8) = *(uint *)(iVar5 + 0xec8) | 0x80;
  iVar5 = *(int *)(_DAT_800ac784 + 0x188);
  *(undefined4 *)(iVar5 + 0x1160) = 0;
  *(undefined4 *)(iVar5 + 0x1164) = 0;
  *(uint *)(iVar5 + 0x10cc) = *(uint *)(iVar5 + 0x10cc) | 0x80;
  func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),1,
                  0x200);
  *(undefined4 *)(_DAT_800ac784 + 4) = 7;
  return;
}


