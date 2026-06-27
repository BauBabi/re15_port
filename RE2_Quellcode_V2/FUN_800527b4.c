/* FUN_800527b4 @ 0x800527b4  (Ghidra headless, raw MIPS overlay) */

int FUN_800527b4(undefined4 param_1,undefined4 param_2)

{
  bool bVar1;
  int iVar2;
  char *pcVar3;
  uint uVar4;
  undefined4 *puVar5;
  uint uVar6;
  undefined4 local_48;
  undefined4 local_44;
  undefined4 local_40;
  undefined1 auStack_38 [4];
  undefined2 local_34;
  undefined2 local_32;
  undefined2 local_30;
  undefined2 local_2e;
  undefined2 local_2c;
  undefined2 local_2a;
  undefined2 local_28;
  undefined2 local_26;
  
  uVar6 = (uint)DAT_800d4230;
  uVar4 = 0;
  local_44 = 0;
  if (uVar6 != 0) {
    puVar5 = &DAT_800ce558;
    local_48 = param_1;
    local_40 = param_2;
    do {
      pcVar3 = (char *)*puVar5;
      puVar5 = puVar5 + 1;
      if ((pcVar3 == (char *)0x0) || (uVar4 = uVar4 + 1, *pcVar3 != '\a')) {
        bVar1 = uVar4 < uVar6;
      }
      else if ((pcVar3[1] & 0x80U) == 0) {
        iVar2 = FUN_8002c904(&local_48,pcVar3 + 4);
        bVar1 = uVar4 < uVar6;
        if (iVar2 != 0) {
          return (int)*(short *)(pcVar3 + 0xc);
        }
      }
      else {
        local_34 = *(undefined2 *)(pcVar3 + 4);
        local_32 = *(undefined2 *)(pcVar3 + 6);
        local_30 = *(undefined2 *)(pcVar3 + 8);
        local_2e = *(undefined2 *)(pcVar3 + 10);
        local_2c = *(undefined2 *)(pcVar3 + 0xc);
        local_2a = *(undefined2 *)(pcVar3 + 0xe);
        local_28 = *(undefined2 *)(pcVar3 + 0x10);
        local_26 = *(undefined2 *)(pcVar3 + 0x12);
        iVar2 = FUN_8002c820(&local_48,auStack_38);
        bVar1 = uVar4 < uVar6;
        if (iVar2 != 0) {
          return (int)*(short *)(pcVar3 + 0x14);
        }
      }
    } while (bVar1);
  }
  return 0;
}


