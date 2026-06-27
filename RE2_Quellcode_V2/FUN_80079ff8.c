/* FUN_80079ff8 @ 0x80079ff8  (Ghidra headless, raw MIPS overlay) */

void FUN_80079ff8(ushort param_1,short param_2,ushort param_3,undefined4 param_4)

{
  ushort uVar1;
  int iVar2;
  int iVar3;
  ushort local_20;
  ushort local_1e [3];
  
  FUN_800845e8((int)(short)(param_1 | param_2 << 8),&local_20,local_1e);
  uVar1 = local_20;
  if (((int)(uint)local_20 < (int)(short)param_3) ||
     (uVar1 = local_1e[0], (int)(uint)local_1e[0] < (int)(short)param_3)) {
    param_3 = uVar1;
  }
  _SsSndSetVolData((int)(short)param_1,(int)param_2,(int)(short)param_3,param_4);
  iVar2 = param_2 * 0xb0;
  iVar3 = iVar2 + (&DAT_800ea250)[(short)param_1];
  *(uint *)(iVar3 + 0x98) = *(uint *)(iVar3 + 0x98) | 0x20;
  iVar2 = iVar2 + (&DAT_800ea250)[(short)param_1];
  *(uint *)(iVar2 + 0x98) = *(uint *)(iVar2 + 0x98) & 0xffffffef;
  return;
}


