/* FUN_8011388c @ 0x8011388c  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_8011388c(uint param_1,int param_2)

{
  undefined2 uVar1;
  byte bVar2;
  int iVar3;
  int local_48;
  int local_44;
  int local_40;
  undefined1 auStack_38 [20];
  int local_24 [3];
  
  iVar3 = *(int *)(_DAT_800ac784 + 0x188);
  func_0x80068098(_DAT_800ac784 + 0x68,_DAT_800ac784 + 0x20);
  func_0x80022da0(_DAT_800ac784 + 0x20,iVar3 + 0x18,auStack_38);
  if (param_2 != 0) {
    local_48 = param_2;
    local_44 = param_2;
    local_40 = param_2;
    func_0x80065ff0(auStack_38,&local_48);
  }
  iVar3 = iVar3 + (param_1 & 0xff) * 0x560 + 0x2b0;
  bVar2 = 2;
  if ((*(uint *)(iVar3 + -0xac) & 1) == 0) {
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = 0;
  }
  else {
    do {
      func_0x80022da0(auStack_38,iVar3 + (uint)bVar2 * -0xac + 0xc4,auStack_38);
      bVar2 = bVar2 - 1;
    } while (bVar2 != 0);
    local_24[1] = 0;
    local_24[0] = local_24[0] - *(int *)(iVar3 + 0x54);
    local_24[2] = local_24[2] - *(int *)(iVar3 + 0x5c);
    func_0x800662e8(local_24,&local_48);
    uVar1 = func_0x80065f60(local_48 + local_40);
    *(undefined2 *)(_DAT_800ac784 + 0x8c) = uVar1;
  }
  return;
}


