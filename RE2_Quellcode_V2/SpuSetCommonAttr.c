/* SpuSetCommonAttr @ 0x8007cf04  (Ghidra headless, raw MIPS overlay) */

void SpuSetCommonAttr(SpuCommonAttr *attr)

{
  bool bVar1;
  ushort uVar2;
  ushort uVar3;
  ushort uVar4;
  uint uVar5;
  
  uVar3 = 0;
  uVar5 = attr->mask;
  bVar1 = uVar5 == 0;
  uVar4 = 0;
  if (bVar1) {
S_SCA_OBJ_30:
    switch((attr->mvolmode).left) {
    default:
S_SCA_OBJ_94:
      uVar3 = (attr->mvol).left;
      uVar2 = 0;
      break;
    case 1:
      uVar2 = 0x8000;
      break;
    case 2:
      uVar2 = 0x9000;
      break;
    case 3:
      uVar2 = 0xa000;
      break;
    case 4:
      uVar2 = 0xb000;
      break;
    case 5:
      uVar2 = 0xc000;
      break;
    case 6:
      uVar2 = 0xd000;
      break;
    case 7:
      uVar2 = 0xe000;
    }
    if (uVar2 != 0) {
      uVar3 = (attr->mvol).left;
      if (0x7f < (short)uVar3) {
        S_SCA_OBJ_CC(attr,uVar2,0x7f);
        return;
      }
      if ((short)uVar3 < 0) {
        uVar3 = 0;
      }
    }
    *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x180) = uVar3 & 0x7fff | uVar2;
  }
  else if ((uVar5 & 1) != 0) {
    if ((uVar5 & 4) != 0) goto S_SCA_OBJ_30;
    goto S_SCA_OBJ_94;
  }
  if (bVar1) {
S_SCA_OBJ_F8:
    switch((attr->mvolmode).right) {
    default:
      goto S_SCA_OBJ_15C;
    case 1:
      uVar3 = 0x8000;
      break;
    case 2:
      uVar3 = 0x9000;
      break;
    case 3:
      uVar3 = 0xa000;
      break;
    case 4:
      uVar3 = 0xb000;
      break;
    case 5:
      uVar3 = 0xc000;
      break;
    case 6:
      uVar3 = 0xd000;
      break;
    case 7:
      uVar3 = 0xe000;
    }
  }
  else {
    if ((uVar5 & 2) == 0) goto S_SCA_OBJ_1A8;
    if ((uVar5 & 8) != 0) goto S_SCA_OBJ_F8;
S_SCA_OBJ_15C:
    uVar4 = (attr->mvol).right;
    uVar3 = 0;
  }
  if (uVar3 != 0) {
    uVar4 = (attr->mvol).right;
    if (0x7f < (short)uVar4) {
      S_SCA_OBJ_194();
      return;
    }
    if ((short)uVar4 < 0) {
      uVar4 = 0;
    }
  }
  *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x182) = uVar4 & 0x7fff | uVar3;
S_SCA_OBJ_1A8:
  if ((bVar1) || ((uVar5 & 0x40) != 0)) {
    *(short *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1b0) = (attr->cd).volume.left;
  }
  if ((bVar1) || ((uVar5 & 0x80) != 0)) {
    *(short *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1b2) = (attr->cd).volume.right;
  }
  if ((bVar1) || ((uVar5 & 0x400) != 0)) {
    *(short *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1b4) = (attr->ext).volume.left;
  }
  if ((bVar1) || ((uVar5 & 0x800) != 0)) {
    *(short *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1b6) = (attr->ext).volume.right;
  }
  if ((bVar1) || ((uVar5 & 0x100) != 0)) {
    if ((attr->cd).reverb == 0) {
      S_SCA_OBJ_288();
      return;
    }
    *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) =
         *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) | 4;
  }
  if ((bVar1) || ((uVar5 & 0x200) != 0)) {
    if ((attr->cd).mix == 0) {
      S_SCA_OBJ_2DC();
      return;
    }
    *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) =
         *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) | 1;
  }
  if ((bVar1) || ((uVar5 & 0x1000) != 0)) {
    if ((attr->ext).reverb == 0) {
      S_SCA_OBJ_330();
      return;
    }
    *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) =
         *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) | 8;
  }
  if ((bVar1) || ((uVar5 & 0x2000) != 0)) {
    if ((attr->ext).mix == 0) {
      S_SCA_OBJ_384();
      return;
    }
    *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) =
         *(ushort *)(PTR_VOICE_00_LEFT_RIGHT_800ab21c + 0x1aa) | 2;
  }
  return;
}


