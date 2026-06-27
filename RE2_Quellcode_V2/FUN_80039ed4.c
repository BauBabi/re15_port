/* FUN_80039ed4 @ 0x80039ed4  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_80039ed4(void)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  
  switch(DAT_800d4236) {
  case 1:
    FUN_8005ba28(0x2060000,0);
LAB_80039f18:
    DAT_800d4237 = 0;
    DAT_800d4236 = 5;
    iVar1 = FUN_8003a148(0x30,0x8e,0xf7,0x57);
    uVar2 = 1;
    if (iVar1 == 0) {
      FUN_80039b30(1,0xf7,0x57,0x80);
LAB_8003a018:
      DAT_800d4236 = 0;
      uVar2 = 0;
    }
    return uVar2;
  case 2:
    FUN_8005ba28(0x2070000,0);
  case 6:
    FUN_80039b30(0,0,0,0x80);
    uVar2 = 0x30;
    uVar3 = 0x8e;
    uVar4 = 0xf7;
    uVar5 = 0x57;
    DAT_800d4237 = 1;
    DAT_800d4236 = 6;
LAB_8003a000:
    iVar1 = FUN_8003a148(uVar2,uVar3,uVar4,uVar5);
    if (iVar1 != 0) {
      return 1;
    }
    goto LAB_8003a018;
  case 3:
    FUN_8005ba28(0x2060000,0);
  case 7:
    uVar2 = 0x10;
    uVar3 = 0x15;
    uVar4 = 0xf0;
    uVar5 = 0x78;
    DAT_800d4237 = 0;
    DAT_800d4236 = 7;
    goto LAB_8003a000;
  case 4:
    FUN_8005ba28(0x2070000,0);
  case 8:
    uVar2 = 0x10;
    uVar3 = 0x15;
    uVar4 = 0xf0;
    uVar5 = 0x78;
    DAT_800d4237 = 1;
    DAT_800d4236 = 8;
    goto LAB_8003a000;
  case 5:
    goto LAB_80039f18;
  default:
    goto LAB_8003a018;
  }
}


