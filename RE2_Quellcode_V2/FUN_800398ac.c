/* FUN_800398ac @ 0x800398ac  (Ghidra headless, raw MIPS overlay) */

undefined4 FUN_800398ac(uint param_1,int param_2)

{
  char *pcVar1;
  char cVar2;
  undefined1 uVar3;
  int iVar4;
  undefined4 uVar5;
  char *pcVar6;
  uint uVar7;
  undefined4 local_20 [4];
  
  pcVar1 = DAT_800ea23c;
  local_20[0] = 0;
  iVar4 = FUN_80039b88();
  if (iVar4 == 0) {
LAB_80039b10:
    uVar5 = 0;
  }
  else {
    cVar2 = pcVar1[0x2d9];
    pcVar1[0x2de] = '\x01';
    if (cVar2 == '\x01') {
      pcVar1[0x2d8] = '\0';
    }
    else if (param_1 == 0) {
      if (param_2 == 1) {
        pcVar1[(byte)pcVar1[0x2d8] + 0x2e4] = cVar2;
        cVar2 = pcVar1[0x2d9];
        if (cVar2 == '\x02') {
          pcVar1[0x2d8] = '\x01';
          FUN_8003a56c(0x41);
        }
        else if (cVar2 == '\x03') {
          pcVar1[0x2d8] = '\x01';
          FUN_8003a56c(0x42);
        }
        else if (cVar2 == '\x04') {
          pcVar1[0x2d8] = '\x01';
          FUN_8003a56c(0x43);
        }
        else {
          if (cVar2 != '\x05') {
            return 0;
          }
          pcVar1[0x2d8] = '\x01';
          FUN_8003a56c(0x44);
        }
      }
      else if (param_2 == 0) {
        pcVar1[(byte)pcVar1[0x2d8] + 0x2e4] = cVar2;
        if (pcVar1[0x2d9] == '\x1a') {
          pcVar1[0x2d8] = '\x01';
          FUN_8003a56c(0x59);
        }
        else {
          if (pcVar1[0x2d9] != '\x0f') {
            return 0;
          }
          pcVar1[0x2d8] = '\x01';
          FUN_8003a56c(0x4e);
        }
      }
      else {
        if (param_2 != 2) {
          return 0;
        }
        pcVar1[0x2d8] = '\x01';
      }
    }
    else if (cVar2 != '\x1c') {
      if (cVar2 == '\x1d') {
        if (pcVar1[0x2d8] != '\0') {
          *pcVar1 = *pcVar1 + -1;
          pcVar1[0x2d8] = pcVar1[0x2d8] + -1;
          FUN_8003a56c(0x20);
          return 0;
        }
      }
      else if ((byte)pcVar1[0x2d8] < param_1) {
        uVar3 = 0x2a;
        if (param_2 == 0) {
          uVar3 = FUN_8003a28c(pcVar1[0x2d9]);
        }
        FUN_8003a56c(uVar3);
        pcVar1[(byte)pcVar1[0x2d8] + 0x2e4] = pcVar1[0x2d9];
        *pcVar1 = *pcVar1 + '\x01';
        pcVar1[0x2d8] = pcVar1[0x2d8] + '\x01';
      }
      goto LAB_80039b10;
    }
    FUN_8003a298(0,&DAT_80010ba4,local_20);
    uVar7 = 0;
    pcVar6 = pcVar1;
    if (pcVar1[0x2d8] != '\0') {
      do {
        uVar7 = uVar7 + 1;
        cVar2 = FUN_8003a28c(pcVar6[0x2e4]);
        pcVar6[0x2e4] = cVar2;
        pcVar6 = pcVar1 + uVar7;
      } while (uVar7 < (byte)pcVar1[0x2d8]);
    }
    uVar5 = 1;
    pcVar1[uVar7 + 0x2e4] = '\0';
    pcVar1[0x2d8] = '\0';
  }
  return uVar5;
}


