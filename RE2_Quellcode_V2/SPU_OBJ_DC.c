/* SPU_OBJ_DC @ 0x8007873c  (Ghidra headless, raw MIPS overlay) */

undefined4 SPU_OBJ_DC(int param_1)

{
  undefined *puVar1;
  int iVar2;
  undefined2 *puVar3;
  int unaff_s0;
  
  puVar1 = PTR_VOICE_00_LEFT_RIGHT_800ab21c;
  puVar3 = &DAT_800d5b88;
  DAT_800ab240 = 2;
  DAT_800ab244 = 3;
  DAT_800ab248 = 8;
  DAT_800ab24c = 7;
  *(undefined2 *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1ac) = 4;
  *(undefined2 *)(puVar1 + 0x184) = 0;
  *(undefined2 *)(puVar1 + 0x186) = 0;
  *(undefined2 *)(puVar1 + 0x18c) = 0xffff;
  *(undefined2 *)(puVar1 + 0x18e) = 0xffff;
  *(undefined2 *)(puVar1 + 0x198) = 0;
  *(undefined2 *)(puVar1 + 0x19a) = 0;
  do {
    *puVar3 = 0;
    puVar1 = PTR_VOICE_00_LEFT_RIGHT_800ab21c;
    param_1 = param_1 + 1;
    puVar3 = puVar3 + 1;
  } while (param_1 < 10);
  if (unaff_s0 == 0) {
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
    iVar2 = 0;
    puVar3 = (undefined2 *)PTR_VOICE_00_LEFT_RIGHT_800ab21c;
    do {
      *puVar3 = 0;
      puVar3[1] = 0;
      puVar3[2] = 0x3fff;
      puVar3[3] = 0x200;
      puVar3[4] = 0;
      puVar3[5] = 0;
      puVar1 = PTR_VOICE_00_LEFT_RIGHT_800ab21c;
      iVar2 = iVar2 + 1;
      puVar3 = puVar3 + 8;
    } while (iVar2 < 0x18);
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


