/* FUN_80056a38 @ 0x80056a38  (Ghidra headless, raw MIPS overlay) */

undefined4 * FUN_80056a38(uint param_1,byte param_2,undefined4 param_3,int param_4)

{
  undefined4 *puVar1;
  uint uVar2;
  int iVar3;
  
  uVar2 = (uint)(char)param_1;
  if ((uVar2 & 0x80) == 0) {
    puVar1 = (undefined4 *)(uint)(uVar2 < 5);
    if (puVar1 != (undefined4 *)0x0) {
      switch((uint)(&switchD_80056a70::switchdataD_80011470)[uVar2] & 0xfffffffe) {
      case 0x80056a78:
        puVar1 = &DAT_8009db44;
        break;
      case 0x80056a88:
        puVar1 = (undefined4 *)(DAT_800cfe14 + 0x24);
        break;
      case 0x80056a98:
        puVar1 = (undefined4 *)(DAT_800cfe18 + 0x24);
        break;
      case 0x80056aa8:
        puVar1 = (undefined4 *)
                 (*(int *)((int)&DAT_800cfe1c + ((int)((uint)param_2 << 0x18) >> 0x16)) + 0x24);
        break;
      case 0x80056ac4:
        puVar1 = (undefined4 *)(&DAT_800d0378 + (char)param_2 * 0x1f8);
      }
    }
  }
  else {
    uVar2 = (param_1 & 0x60) >> 5;
    iVar3 = DAT_800cfe18;
    if (uVar2 != 1) {
      if (uVar2 < 2) {
        iVar3 = DAT_800cfe14;
        if (uVar2 != 0) {
          iVar3 = param_4;
        }
      }
      else {
        iVar3 = param_4;
        if (uVar2 == 2) {
          iVar3 = *(int *)((int)&DAT_800cfe1c + ((int)((uint)param_2 << 0x18) >> 0x16));
        }
      }
    }
    puVar1 = (undefined4 *)(*(int *)(iVar3 + 0x198) + (param_1 & 0x1f) * 0xac + 0x48);
  }
  return puVar1;
}


