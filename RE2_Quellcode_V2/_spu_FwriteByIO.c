/* _spu_FwriteByIO @ 0x800788f4  (Ghidra headless, raw MIPS overlay) */

void _spu_FwriteByIO(undefined2 *param_1,uint param_2)

{
  ushort uVar1;
  undefined2 uVar2;
  ushort uVar3;
  bool bVar4;
  undefined *puVar5;
  int iVar6;
  uint uVar7;
  uint uVar8;
  
  uVar1 = *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1ae);
  *(undefined2 *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1a6) = DAT_800ab234;
  _spu_Fw1ts();
  bVar4 = param_2 < 0x41;
  if (param_2 != 0) {
    do {
      puVar5 = PTR_VOICE_00_LEFT_RIGHT_800ab21c;
      uVar8 = 0x40;
      if (bVar4) {
        uVar8 = param_2;
      }
      iVar6 = 0;
      if (0 < (int)uVar8) {
        do {
          uVar2 = *param_1;
          param_1 = param_1 + 1;
          iVar6 = iVar6 + 2;
          *(undefined2 *)(puVar5 + 0x1a8) = uVar2;
        } while (iVar6 < (int)uVar8);
      }
      *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) =
           *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) & 0xffcf | 0x10;
      _spu_Fw1ts();
      if ((*(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1ae) & 0x400) != 0) {
        uVar7 = 1;
        do {
          if (0xf00 < uVar7) {
            printf("SPU:T/O [%s]\n","wait (wrdy H -> L)");
            SPU_OBJ_3AC();
            return;
          }
          uVar7 = uVar7 + 1;
        } while ((*(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1ae) & 0x400) != 0);
      }
      param_2 = param_2 - uVar8;
      _spu_Fw1ts();
      _spu_Fw1ts();
      bVar4 = param_2 < 0x41;
    } while (param_2 != 0);
  }
  puVar5 = PTR_VOICE_00_LEFT_RIGHT_800ab21c;
  *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) =
       *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) & 0xffcf;
  uVar3 = *(ushort *)(puVar5 + 0x1ae);
  uVar8 = 1;
  while( true ) {
    if ((uVar3 & 0x7ff) == (uVar1 & 0x7ff)) {
      return;
    }
    if (0xf00 < uVar8) break;
    uVar3 = *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1ae);
    uVar8 = uVar8 + 1;
  }
  printf("SPU:T/O [%s]\n","wait (dmaf clear/W)");
  SPU_OBJ_440();
  return;
}


