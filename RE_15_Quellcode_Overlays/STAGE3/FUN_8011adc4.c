/* FUN_8011adc4 @ 0x8011adc4  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011adc4(int param_1,int param_2)

{
  undefined2 uVar1;
  int iVar2;
  int iVar3;
  int aiStack_38 [5];
  uint local_24;
  uint local_1c;
  
  iVar3 = *(int *)(_DAT_800ac784 + 0x188);
  func_0x80068098(_DAT_800ac784 + 0x68,_DAT_800ac784 + 0x20);
  func_0x80022da0(_DAT_800ac784 + 0x20,iVar3 + 0x18,aiStack_38);
  iVar3 = iVar3 + param_2 * 0x204 + 0x204;
  func_0x80022da0(aiStack_38,iVar3 + -0x140,aiStack_38);
  func_0x80022da0(aiStack_38,iVar3 + -0x94,aiStack_38);
  func_0x80022da0(aiStack_38,iVar3 + 0x18,aiStack_38);
  if (param_1 == 0) {
    *(uint *)(_DAT_800ac784 + 0x34) =
         *(int *)(_DAT_800ac784 + 0x34) - (local_24 - *(int *)(iVar3 + 0x54));
    iVar2 = local_1c - *(int *)(iVar3 + 0x5c);
    iVar3 = *(int *)(_DAT_800ac784 + 0x3c) - iVar2;
    *(int *)(_DAT_800ac784 + 0x3c) = iVar3;
    *(short *)(iVar2 + 0x5fe) = (short)iVar3;
    *(undefined1 *)(_DAT_800ac784 + 0x94) = 2;
    *(undefined1 *)(_DAT_800ac784 + 0x95) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x8f) = 0;
    *(undefined1 *)(_DAT_800ac784 + 0x93) = 0;
    func_0x8001f314(*(undefined4 *)(_DAT_800ac784 + 0x84),*(undefined4 *)(_DAT_800ac784 + 0x16c),0,
                    0x200);
    aiStack_38[1] = 0x808080;
    aiStack_38[0] = _DAT_800ac784 + 0xb0;
    func_0x8001af5c(0,0,600,700);
    if ((*(byte *)(_DAT_800ac784 + 9) & 0x40) != 0) {
      *(undefined1 *)(_DAT_800ac784 + 4) = 4;
      *(undefined1 *)(_DAT_800ac784 + 5) = 6;
    }
    return;
  }
  iVar2 = (int)(((local_24 & 0xffff) - (uint)*(ushort *)(iVar3 + 0x54)) * 0x10000) >> 0x10;
  iVar3 = (int)(((local_1c & 0xffff) - (uint)*(ushort *)(iVar3 + 0x5c)) * 0x10000) >> 0x10;
  uVar1 = func_0x80065f60(iVar2 * iVar2 + iVar3 * iVar3);
  *(undefined2 *)(_DAT_800ac784 + 0x8c) = uVar1;
  return;
}


