/* SPU_OBJ_3AC @ 0x80078a0c  (Ghidra headless, raw MIPS overlay) */

void SPU_OBJ_3AC(void)

{
  undefined2 uVar1;
  ushort uVar2;
  undefined *puVar3;
  int iVar4;
  uint uVar5;
  uint unaff_s0;
  uint unaff_s1;
  undefined2 *unaff_s2;
  ushort unaff_s3;
  
  do {
    do {
      unaff_s1 = unaff_s1 - unaff_s0;
      _spu_Fw1ts();
      _spu_Fw1ts();
      puVar3 = PTR_VOICE_00_LEFT_RIGHT_800ab21c;
      if (unaff_s1 == 0) {
        *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) =
             *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) & 0xffcf;
        uVar2 = *(ushort *)(puVar3 + 0x1ae);
        uVar5 = 1;
        while( true ) {
          if ((uVar2 & 0x7ff) == unaff_s3) {
            return;
          }
          if (0xf00 < uVar5) break;
          uVar2 = *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1ae);
          uVar5 = uVar5 + 1;
        }
        printf("SPU:T/O [%s]\n","wait (dmaf clear/W)");
        SPU_OBJ_440();
        return;
      }
      unaff_s0 = 0x40;
      if (unaff_s1 < 0x41) {
        unaff_s0 = unaff_s1;
      }
      iVar4 = 0;
      if (0 < (int)unaff_s0) {
        do {
          uVar1 = *unaff_s2;
          unaff_s2 = unaff_s2 + 1;
          iVar4 = iVar4 + 2;
          *(undefined2 *)(puVar3 + 0x1a8) = uVar1;
        } while (iVar4 < (int)unaff_s0);
      }
      *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) =
           *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) & 0xffcf | 0x10;
      _spu_Fw1ts();
    } while ((*(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1ae) & 0x400) == 0);
    uVar5 = 1;
    do {
      if (0xf00 < uVar5) {
        printf("SPU:T/O [%s]\n","wait (wrdy H -> L)");
        SPU_OBJ_3AC();
        return;
      }
      uVar5 = uVar5 + 1;
    } while ((*(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1ae) & 0x400) != 0);
  } while( true );
}


