/* FUN_80035994 @ 0x80035994  (Ghidra headless, raw MIPS overlay) */

void FUN_80035994(void)

{
  byte bVar1;
  uint uVar2;
  uint x;
  uint y;
  RECT *rect;
  byte *pbVar3;
  ushort *puVar4;
  uint uVar5;
  uint uVar6;
  
  uVar6 = (uint)DAT_800d7532;
  pbVar3 = &DAT_800d6c48;
  if (uVar6 != 0) {
    uVar5 = 0;
    uVar2 = (uint)DAT_800ce5e0;
    puVar4 = (ushort *)&DAT_800d6c4e;
    do {
      bVar1 = *pbVar3;
      rect = (RECT *)(pbVar3 + uVar2 * 8 + 0x38);
      if ((bVar1 & 0x80) == 0) {
        if ((bVar1 & 0x40) != 0) {
          FUN_80035c98(pbVar3,rect,bVar1 & 0xbf);
          x = (uint)puVar4[-1];
          y = (uint)*puVar4;
          goto LAB_80035a74;
        }
      }
      else {
        FUN_80035ad0(pbVar3,rect,bVar1 & 0x7f);
        x = 0x130;
        y = uVar5 + 0x1e0;
LAB_80035a74:
        SetDrawMove((DR_MOVE *)(pbVar3 + uVar2 * 0x18 + 8),rect,x,y);
      }
      AddPrim(&UNK_800ce230 + uVar2 * 0x40,(DR_MOVE *)(pbVar3 + uVar2 * 0x18 + 8));
      puVar4 = puVar4 + 0x26;
      uVar5 = uVar5 + 1;
      pbVar3 = pbVar3 + 0x4c;
    } while (uVar5 < uVar6);
  }
  return;
}


