/* _spu_init @ 0x80078660  (Ghidra headless, raw MIPS overlay) */

undefined4 _spu_init(int param_1)

{
  undefined *puVar1;
  undefined *puVar2;
  undefined4 uVar3;
  uint uVar4;
  int iVar5;
  undefined2 *puVar6;
  
  *(uint *)PTR_DMA_DPCR_800ab22c = *(uint *)PTR_DMA_DPCR_800ab22c | 0xb0000;
  puVar1 = PTR_VOICE_00_LEFT_RIGHT_800ab21c;
  DAT_800ab238 = 0;
  DAT_800ab23c = 0;
  DAT_800ab234 = 0;
  *(undefined2 *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x180) = 0;
  *(undefined2 *)(puVar1 + 0x182) = 0;
  *(undefined2 *)(puVar1 + 0x1aa) = 0;
  _spu_Fw1ts();
  puVar1 = PTR_VOICE_00_LEFT_RIGHT_800ab21c;
  *(undefined2 *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x180) = 0;
  *(undefined2 *)(puVar1 + 0x182) = 0;
  puVar2 = PTR_VOICE_00_LEFT_RIGHT_800ab21c;
  if ((*(ushort *)(puVar1 + 0x1ae) & 0x7ff) != 0) {
    uVar4 = 1;
    do {
      if (0xf00 < uVar4) {
        printf("SPU:T/O [%s]\n","wait (reset)");
        uVar3 = SPU_OBJ_DC(0);
        return uVar3;
      }
      uVar4 = uVar4 + 1;
    } while ((*(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1ae) & 0x7ff) != 0);
  }
  iVar5 = 0;
  puVar6 = &DAT_800d5b88;
  DAT_800ab240 = 2;
  DAT_800ab244 = 3;
  DAT_800ab248 = 8;
  DAT_800ab24c = 7;
  *(undefined2 *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1ac) = 4;
  *(undefined2 *)(puVar2 + 0x184) = 0;
  *(undefined2 *)(puVar2 + 0x186) = 0;
  *(undefined2 *)(puVar2 + 0x18c) = 0xffff;
  *(undefined2 *)(puVar2 + 0x18e) = 0xffff;
  *(undefined2 *)(puVar2 + 0x198) = 0;
  *(undefined2 *)(puVar2 + 0x19a) = 0;
  do {
    *puVar6 = 0;
    puVar1 = PTR_VOICE_00_LEFT_RIGHT_800ab21c;
    iVar5 = iVar5 + 1;
    puVar6 = puVar6 + 1;
  } while (iVar5 < 10);
  if (param_1 == 0) {
    DAT_800ab234 = 0x200;
    *(undefined2 *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 400) = 0;
    *(undefined2 *)(puVar1 + 0x192) = 0;
    *(undefined2 *)(puVar1 + 0x194) = 0;
    *(undefined2 *)(puVar1 + 0x196) = 0;
    *(undefined2 *)(puVar1 + 0x1b0) = 0;
    *(undefined2 *)(puVar1 + 0x1b2) = 0;
    *(undefined2 *)(puVar1 + 0x1b4) = 0;
    *(undefined2 *)(puVar1 + 0x1b6) = 0;
    _spu_FwriteByIO(&DAT_800ab25c,0x10);
    iVar5 = 0;
    puVar6 = (undefined2 *)PTR_VOICE_00_LEFT_RIGHT_800ab21c;
    do {
      *puVar6 = 0;
      puVar6[1] = 0;
      puVar6[2] = 0x3fff;
      puVar6[3] = 0x200;
      puVar6[4] = 0;
      puVar6[5] = 0;
      puVar1 = PTR_VOICE_00_LEFT_RIGHT_800ab21c;
      iVar5 = iVar5 + 1;
      puVar6 = puVar6 + 8;
    } while (iVar5 < 0x18);
    *(undefined2 *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x188) = 0xffff;
    *(undefined2 *)(puVar1 + 0x18a) = 0xff;
    _spu_Fw1ts();
    _spu_Fw1ts();
    _spu_Fw1ts();
    _spu_Fw1ts();
    puVar1 = PTR_VOICE_00_LEFT_RIGHT_800ab21c;
    *(undefined2 *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x18c) = 0xffff;
    *(undefined2 *)(puVar1 + 0x18e) = 0xff;
    _spu_Fw1ts();
    _spu_Fw1ts();
    _spu_Fw1ts();
    _spu_Fw1ts();
  }
  DAT_800ab250 = 1;
  *(undefined2 *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) = 0xc000;
  DAT_800ab254 = 0;
  DAT_800ab258 = 0;
  return 0;
}


