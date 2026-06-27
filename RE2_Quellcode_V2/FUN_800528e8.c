/* FUN_800528e8 @ 0x800528e8  (Ghidra headless, raw MIPS overlay) */

undefined1 FUN_800528e8(undefined4 param_1,undefined4 param_2,uint param_3)

{
  int iVar1;
  short *psVar2;
  short *psVar3;
  int iVar4;
  undefined4 local_28 [2];
  undefined4 local_20;
  
  iVar4 = (int)**(short **)(DAT_800ce324 + 0x34);
  psVar3 = *(short **)(DAT_800ce324 + 0x34) + iVar4 * 6 + 1;
  if (iVar4 != 0) {
    psVar2 = psVar3 + 4;
    local_28[0] = param_1;
    local_20 = param_2;
    do {
      iVar4 = iVar4 + -1;
      psVar3 = psVar3 + -6;
      if (((((int)psVar2[-5] & 0x80U) == 0) && (param_3 == ((int)psVar2[-5] & 0x7fU))) &&
         (iVar1 = FUN_8002c904(local_28,psVar3), iVar1 != 0)) {
        return (char)psVar2[-6];
      }
      psVar2 = psVar2 + -6;
    } while (iVar4 != 0);
  }
  return 0x1d;
}


