void FUN_8004a0cc(void)

{
  short sVar1;
  undefined2 *puVar2;
  short *psVar3;
  uint uVar4;
  
  puVar2 = &DAT_800ac766;
  if ((DAT_800ac766 & 0x10) != 0) {
    (&DAT_800b10ad)[(uint)DAT_800b25c8 * 4] =
         (&DAT_80074da8)[(uint)(byte)(&DAT_800b10ac)[(uint)DAT_800b25c8 * 4] * 0xc];
  }
  uVar4 = (uint)DAT_800ac762;
  if ((DAT_800ac762 & 0x100) != 0) {
    DAT_800b2668 = 1;
    puVar2 = (undefined2 *)0x4090000;
    uVar4 = 0;
    FUN_80045024();
  }
  if ((byte)DAT_800b2668 == 1) {
    FUN_80013b60(0xb,&DAT_801a0000,0);
LAB_8004a1a4:
    FUN_800492b8(DAT_800b25bd,0,&DAT_801a0000 + (uint)DAT_800b2668._1_1_ * 0x4b0);
    (&DAT_800b10ac)[(uint)DAT_800b25bd * 4] = DAT_800b2668._1_1_;
    (&DAT_800b10ad)[(uint)DAT_800b25bd * 4] = 0xff;
    (&DAT_800b10af)[(uint)DAT_800b25bd * 4] = 0;
    DAT_800b2668 = CONCAT11(DAT_800b2668._1_1_,3);
  }
  else if (1 < (byte)DAT_800b2668) {
    if ((byte)DAT_800b2668 == 2) goto LAB_8004a1a4;
    if ((byte)DAT_800b2668 == 3) {
      if ((uVar4 & 8) == 0) {
        if ((uVar4 & 4) == 0) {
          if ((uVar4 & 2) == 0) {
            if ((uVar4 & 1) == 0) {
              if ((uVar4 & 0x20) == 0) {
                if ((uVar4 & 0x10) != 0) {
                  *(char *)((int)puVar2 + (uint)DAT_800b25bd * 4 + 0x4947) =
                       (&DAT_800b10ad)[(uint)DAT_800b25bd * 4] + '\x01';
                }
              }
              else {
                DAT_800b2668 = (ushort)DAT_800b2668._1_1_ << 8;
              }
            }
            else {
              DAT_800b2668 = CONCAT11(DAT_800b2668._1_1_ - 10,2);
            }
          }
          else {
            DAT_800b2668 = CONCAT11(DAT_800b2668._1_1_ + 10,2);
          }
        }
        else {
          DAT_800b2668 = CONCAT11(DAT_800b2668._1_1_ - 1,2);
        }
      }
      else {
        DAT_800b2668 = CONCAT11(DAT_800b2668._1_1_ + 1,2);
      }
      if (0x47 < DAT_800b2668._1_1_) {
        DAT_800b2668 = CONCAT11(0x47,(byte)DAT_800b2668);
      }
    }
  }
  sVar1 = DAT_800b25ee;
  switch(DAT_800b25c0._2_1_) {
  case '\0':
    psVar3 = &DAT_800b25ea;
    sVar1 = DAT_800b25ea;
    if (0xfa < DAT_800b25ea) {
      DAT_800b25c0._2_1_ = DAT_800b25c0._2_1_ + '\x01';
      return;
    }
    goto LAB_8004a6e0;
  case '\x01':
    FUN_800c62a0();
    break;
  case '\x02':
    psVar3 = &DAT_800b25ea;
    sVar1 = DAT_800b25ea;
    if (DAT_800b25ea < 0xa7) {
      DAT_800b25c0._1_1_ = 0;
      DAT_800b25c0._2_1_ = 0;
      DAT_800b25c0._3_1_ = 0;
      DAT_800b25ca = 0;
      return;
    }
    goto LAB_8004a440;
  case '\x03':
    psVar3 = &DAT_800b25ee;
    if (DAT_800b25ee < 0xa7) {
      DAT_800b25c0._2_1_ = DAT_800b25c0._2_1_ + '\x01';
      return;
    }
LAB_8004a440:
    *psVar3 = sVar1 + -0xe;
    break;
  case '\x04':
    if ((DAT_800ac762 & 0x2000) == 0) {
      if ((DAT_800ac762 & 0x8000) == 0) {
        if ((DAT_800ac762 & 0x4000) == 0) {
          if ((DAT_800ac762 & 0x1000) == 0) {
            if ((DAT_800ac76c & 0x4000) != 0) {
              FUN_80045024(0x4060000,0);
                    /* WARNING: Could not recover jumptable at 0x8004a548. Too many branches */
                    /* WARNING: Treating indirect jump as call */
              (*(code *)(&PTR_LAB_8004a73c)[DAT_800b25d6])();
              return;
            }
            if ((DAT_800ac76c & 0x8000) != 0) {
              FUN_80045024(0x4050000,0);
              DAT_800b25c0._2_1_ = '\x06';
            }
          }
          else {
            DAT_800b25d6 = 0;
            FUN_80045024(0x4040000,0);
          }
        }
        else {
          DAT_800b25d6 = 2;
          FUN_80045024(0x4040000,0);
        }
      }
      else {
        DAT_800b25d6 = 1;
        FUN_80045024(0x4040000,0);
      }
    }
    else {
      DAT_800b25d6 = 3;
      FUN_80045024(0x4040000,0);
    }
    break;
  case '\x05':
    FUN_8004aa24();
    break;
  case '\x06':
    psVar3 = &DAT_800b25ee;
    if (0xfa < DAT_800b25ee) {
      DAT_800b25c0._2_1_ = 1;
      return;
    }
    goto LAB_8004a6e0;
  case '\a':
    FUN_8004b33c();
    break;
  case '\b':
    psVar3 = &DAT_800b25ee;
    if (0xfa < DAT_800b25ee) {
      DAT_800b25c0._2_1_ = 2;
      return;
    }
LAB_8004a6e0:
    *psVar3 = sVar1 + 0xe;
    break;
  case '\t':
    FUN_800c6630();
  }
  return;
}
