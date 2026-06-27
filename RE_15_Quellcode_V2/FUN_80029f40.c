void FUN_80029f40(uint param_1)

{
  ushort *puVar1;
  uint uVar2;
  
  if ((sRam0000001c == 0) && (uRam00000004 <= param_1)) {
    puVar1 = (ushort *)(iRam00000000 + 0x10);
    if (param_1 < uRam00000006) {
      uVar2 = (uint)uRam0000000a;
      if (DAT_800aca34 != '\0') {
        uVar2 = uVar2 + 0xf0;
      }
      FUN_8002a050(uRam00000008,uVar2);
    }
    else if (*(int *)puVar1 == -1) {
      sRam0000001c = 1;
    }
    else {
      uRam00000004 = *puVar1;
      uRam00000006 = *puVar1 + *(short *)(iRam00000000 + 4);
      iRam00000010 = (uint)*(ushort *)(iRam00000000 + 0x18) * 0x4380 + iRam00000014;
      iRam0000000c = (uint)*(ushort *)(iRam00000000 + 0x1a) * 0x12;
      uRam00000008 = *(undefined2 *)(iRam00000000 + 0x1c);
      uRam0000000a = *(ushort *)(iRam00000000 + 0x1e);
    }
  }
  return;
}
