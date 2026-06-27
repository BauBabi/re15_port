/* FUN_80053f50 @ 0x80053f50  (Ghidra headless, raw MIPS overlay) */

uint FUN_80053f50(int param_1,byte *param_2,uint param_3)

{
  short sVar1;
  uint uVar2;
  uint uVar3;
  byte *pbVar4;
  
  uVar2 = (*(code *)(&PTR_LAB_800a74c8)[*param_2])();
  pbVar4 = *(byte **)(param_1 + 0x1c);
  while (pbVar4 != param_2 + (param_3 & 0xff)) {
    sVar1 = *(short *)pbVar4;
    *(byte **)(param_1 + 0x1c) = pbVar4 + 2;
    uVar3 = (*(code *)(&PTR_LAB_800a74c8)[pbVar4[2]])(param_1);
    pbVar4 = *(byte **)(param_1 + 0x1c);
    if (sVar1 == 0) {
      uVar2 = uVar2 & uVar3;
    }
    else {
      uVar2 = uVar2 | uVar3;
    }
  }
  return uVar2;
}


