/* FUN_8011308c @ 0x8011308c  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011308c(uint param_1,int param_2,undefined4 param_3,undefined1 param_4)

{
  short sVar1;
  char cVar2;
  undefined2 uVar3;
  int iVar4;
  uint uVar5;
  undefined1 *puVar6;
  int in_t0;
  undefined2 in_t1;
  undefined1 in_t2;
  byte bVar7;
  int iVar8;
  int local_48;
  int local_44;
  int local_40;
  undefined1 auStack_38 [20];
  int local_24 [3];
  
  iVar8 = *(int *)(_DAT_800ac784 + 0x188);
  func_0x80068098(_DAT_800ac784 + 0x68,_DAT_800ac784 + 0x20);
  puVar6 = auStack_38;
  func_0x80022da0(_DAT_800ac784 + 0x20,iVar8 + 0x18);
  uVar3 = SUB42(puVar6,0);
  if (param_2 != 0) {
    local_48 = param_2;
    local_44 = param_2;
    local_40 = param_2;
    func_0x80065ff0(auStack_38,&local_48);
  }
  iVar4 = _DAT_800ac784;
  iVar8 = iVar8 + (param_1 & 0xff) * 0x560 + 0x2b0;
  bVar7 = 2;
  if ((*(uint *)(iVar8 + -0xac) & 1) != 0) {
    do {
      func_0x80022da0(auStack_38,iVar8 + (uint)bVar7 * -0xac + 0xc4,auStack_38);
      bVar7 = bVar7 - 1;
    } while (bVar7 != 0);
    local_24[1] = 0;
    local_24[0] = local_24[0] - *(int *)(iVar8 + 0x54);
    local_24[2] = local_24[2] - *(int *)(iVar8 + 0x5c);
    func_0x800662e8(local_24,&local_48);
    uVar3 = func_0x80065f60(local_48 + local_40);
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = uVar3;
    return;
  }
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
  do {
    iVar8 = *(short *)(iVar4 + 0x1e0) * 0x158 + in_t0;
    uVar5 = *(uint *)(iVar8 + 0xac);
    if ((uVar5 & 0x41) == 1) {
      *(uint *)(iVar8 + 0xac) = uVar5 | 0x1062;
      iVar8 = *(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + in_t0;
      *(uint *)(iVar8 + 0x158) = *(uint *)(iVar8 + 0x158) | 0x1062;
      *(undefined1 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + in_t0 + 0x121) = in_t2;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + in_t0 + 0x12e) = in_t1;
      *(undefined1 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + in_t0 + 0x122) = param_4;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + in_t0 + 0xe4) = 0;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + in_t0 + 0xe6) = uVar3;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + in_t0 + 0xe8) = 0;
      *(undefined1 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + in_t0 + 0x1cd) = in_t2;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + in_t0 + 0x1da) = in_t1;
      *(undefined1 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + in_t0 + 0x1ce) = param_4;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + in_t0 + 400) = 0;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + in_t0 + 0x192) = uVar3;
      *(undefined2 *)(*(short *)(_DAT_800ac784 + 0x1e0) * 0x158 + in_t0 + 0x194) = 0;
    }
    sVar1 = *(short *)(_DAT_800ac784 + 0x1e0);
    *(short *)(_DAT_800ac784 + 0x1e0) = sVar1 + -1;
    iVar4 = _DAT_800ac784;
  } while (sVar1 != 1);
  cVar2 = func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),
                          *(undefined4 *)(_DAT_800ac784 + 0x16c),0,0x200);
  *(char *)(_DAT_800ac784 + 7) = *(char *)(_DAT_800ac784 + 7) + cVar2;
  return;
}


