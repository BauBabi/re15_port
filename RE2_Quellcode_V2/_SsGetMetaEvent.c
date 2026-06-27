/* _SsGetMetaEvent @ 0x8007c324  (Ghidra headless, raw MIPS overlay) */

/* WARNING: Removing unreachable block (ram,0x8007c3c8) */

void _SsGetMetaEvent(short param_1,short param_2,uchar param_3)

{
  byte bVar1;
  byte bVar2;
  int iVar3;
  uint uVar4;
  undefined4 uVar5;
  short sVar6;
  byte *pbVar7;
  undefined4 *puVar8;
  uint uVar9;
  uint uVar10;
  
  puVar8 = (undefined4 *)
           (*(int *)((int)&DAT_800ea250 + ((int)((uint)(ushort)param_1 << 0x10) >> 0xe)) +
           param_2 * 0xb0);
  pbVar7 = (byte *)*puVar8;
  *puVar8 = pbVar7 + 1;
  bVar1 = *pbVar7;
  *puVar8 = pbVar7 + 2;
  bVar2 = pbVar7[1];
  *puVar8 = pbVar7 + 3;
  iVar3 = DAT_800ea228;
  uVar4 = (uint)bVar1 << 0x10 | (uint)bVar2 << 8 | (uint)pbVar7[2];
  uVar9 = 60000000 / uVar4;
  if (uVar4 == 0) {
    trap(0x1c00);
  }
  uVar10 = (int)*(short *)(puVar8 + 0x14) * uVar9;
  puVar8[0x25] = uVar9;
  uVar4 = iVar3 * 0x3c;
  if (uVar4 <= uVar10 * 10) {
    if (uVar4 == 0) {
      trap(0x1c00);
    }
    if (uVar4 == 0) {
      trap(0x1c00);
    }
    *(undefined2 *)((int)puVar8 + 0x52) = 0xffff;
    sVar6 = (short)((uint)((int)*(short *)(puVar8 + 0x14) * puVar8[0x25] * 10) / uVar4);
    *(short *)(puVar8 + 0x15) = sVar6;
    if ((uint)(iVar3 * 0x1e) < (uint)((int)*(short *)(puVar8 + 0x14) * puVar8[0x25] * 10) % uVar4) {
      *(short *)(puVar8 + 0x15) = sVar6 + 1;
    }
    uVar5 = _SsReadDeltaValue((int)param_1,(int)param_2);
    puVar8[0x24] = uVar5;
    return;
  }
  if (uVar10 == 0) {
    trap(0x1c00);
  }
  *(short *)((int)puVar8 + 0x52) = (short)((uint)(iVar3 * 600) / uVar10);
  MIDIMETA_OBJ_1A0();
  return;
}


