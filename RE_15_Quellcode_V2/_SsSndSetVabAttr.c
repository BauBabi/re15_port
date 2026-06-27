void _SsSndSetVabAttr(short param_1,short param_2,short param_3,undefined4 param_4)

{
  int iVar1;
  byte bVar2;
  uint uVar3;
  uint uVar4;
  byte local_resc;
  byte bStackX_d;
  undefined2 uStackX_e;
  ushort uStack0000001c;
  ushort uStack0000001e;
  ushort uStack0000002c;
  ushort uStack0000002e;
  ushort in_stack_00000030;
  ushort uStack00000032;
  ushort in_stack_00000034;
  undefined2 uStack00000036;
  undefined2 in_stack_00000038;
  undefined2 uStack0000003a;
  undefined2 in_stack_0000003c;
  int in_stack_00000040;
  byte in_stack_00000044;
  
  bVar2 = in_stack_00000044;
  iVar1 = in_stack_00000040;
  uVar4 = (uint)in_stack_00000044;
  _local_resc = param_4;
  SsUtGetVagAtr(param_1,param_2,param_3,(VagAtr *)&local_resc);
  switch((short)iVar1) {
  case 0:
    local_resc = bVar2;
    break;
  case 1:
    bStackX_d = bVar2;
    SsUtSetVagAtr(param_1,param_2,param_3,(VagAtr *)&local_resc);
    if (uVar4 == 0) {
      FUN_8005f87c();
      SEQREAD_OBJ_1878();
      return;
    }
    if (uVar4 == 1) {
      return;
    }
    if (uVar4 == 2) {
      return;
    }
    if (uVar4 == 3) {
      return;
    }
    if (uVar4 != 4) {
      return;
    }
    SsUtReverbOn();
    SEQREAD_OBJ_1878();
    return;
  case 2:
    break;
  case 3:
    break;
  case 4:
  case 5:
  case 6:
  case 7:
  case 8:
  case 9:
  case 10:
  case 0xb:
  case 0xc:
  case 0xd:
  case 0xe:
    _SsUtResolveADSR(uStack0000001c,uStack0000001e,&stack0x0000002c);
    uVar3 = (iVar1 + -4) * 0x10000 >> 0x10;
    if (uVar3 < 0xb) {
      uStack00000032 = (ushort)bVar2;
      switch(uVar3) {
      case 0:
        uStack00000036 = 0;
        uStack0000002c = uStack00000032;
        SEQREAD_OBJ_1800();
        return;
      case 1:
        uStack00000036 = 1;
        uStack0000002c = uStack00000032;
        SEQREAD_OBJ_1800();
        return;
      case 2:
        uStack0000002e = uStack00000032;
        break;
      case 3:
        in_stack_00000030 = uStack00000032;
        break;
      case 4:
        in_stack_00000038 = 0;
        SEQREAD_OBJ_1800();
        return;
      case 5:
        in_stack_00000038 = 1;
        SEQREAD_OBJ_1800();
        return;
      case 6:
        uStack0000003a = 0;
        in_stack_00000034 = uStack00000032;
        SEQREAD_OBJ_1800();
        return;
      case 7:
        uStack0000003a = 1;
        in_stack_00000034 = uStack00000032;
        SEQREAD_OBJ_1800();
        return;
      case 8:
        if ((uVar4 != 0) && (uVar4 < 0x40)) {
          in_stack_0000003c = 0;
          SEQREAD_OBJ_1800();
          return;
        }
        if (uVar4 - 0x40 < 0x40) {
          in_stack_0000003c = 1;
          SEQREAD_OBJ_1800();
          return;
        }
        break;
      case 9:
        break;
      case 10:
      }
    }
    _SsUtBuildADSR(&stack0x0000002c,&stack0x0000001c,(int)&stack0x0000001c + 2);
    SsUtSetVagAtr(param_1,param_2,param_3,(VagAtr *)&local_resc);
    SEQREAD_OBJ_1878();
    return;
  case 0xf:
    SsUtSetReverbType((ushort)bVar2);
    SEQREAD_OBJ_1878();
    return;
  case 0x10:
    SsUtSetReverbDepth((ushort)bVar2,(ushort)bVar2);
    SEQREAD_OBJ_1878();
    return;
  case 0x11:
    SsUtSetReverbFeedback((ushort)bVar2);
    SEQREAD_OBJ_1878();
    return;
  case 0x12:
  case 0x13:
    SsUtSetReverbDelay((ushort)bVar2);
  default:
    return;
  }
  SsUtSetVagAtr(param_1,param_2,param_3,(VagAtr *)&local_resc);
  SEQREAD_OBJ_1878();
  return;
}
