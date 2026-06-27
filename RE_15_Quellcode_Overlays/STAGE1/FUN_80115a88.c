/* FUN_80115a88 @ 0x80115a88  (Ghidra headless overlay RE) */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_80115a88(void)

{
  undefined4 *puVar1;
  char cVar2;
  short sVar3;
  int iVar4;
  uint uVar5;
  
  if (((_DAT_800aca40 & 0x20000000) == 0) || ((*(byte *)((int)_DAT_800ac784 + 9) & 0x20) != 0)) {
    puVar1 = (undefined4 *)((uint)(byte)_DAT_800ac784[1] * 4 + -0x7feded98);
    (*(code *)*puVar1)();
    func_0x8002b498(_DAT_800ac784);
    if ((byte)_DAT_800ac784[0x74] < 0xd) {
      *_DAT_800ac784 = *_DAT_800ac784 | 2;
      *_DAT_800ac784 = *_DAT_800ac784 | 0x40;
    }
    else {
      *_DAT_800ac784 = *_DAT_800ac784 & 0xfffffffd;
      *_DAT_800ac784 = *_DAT_800ac784 & 0xffffffbf;
      iVar4 = func_0x8002aec4(&DAT_800aca54,_DAT_800ac784);
      if (iVar4 != 0) {
        if (DAT_800acae7 == '\0') {
          DAT_800aca58 = 2;
          cVar2 = func_0x8001a780(&DAT_800aca54);
          DAT_800aca59 = cVar2 + '\x02';
          puVar1 = (undefined4 *)0x800b0000;
          _DAT_800aca5a = 0;
        }
        uVar5 = (uint)_DAT_800acaee;
        sVar3 = _DAT_800acaee + -2;
        if (3 < (int)uVar5) {
          _DAT_800acaee = sVar3;
          if (uVar5 < 5) {
            (**(code **)(&LAB_80100334 + uVar5 * 4))();
            return;
          }
          return;
        }
      }
    }
    _DAT_800ac784[0xd] = (int)(short)_DAT_800ac784[0x76];
    _DAT_800ac784[0xf] = (int)*(short *)((int)_DAT_800ac784 + 0x1da);
    if ((*(byte *)((int)_DAT_800ac784 + 9) & 0x80) != 0) {
                    /* WARNING: Could not recover jumptable at 0x801164a4. Too many branches */
                    /* WARNING: Treating indirect jump as call */
      (**(code **)((int)puVar1 + *(short *)((int)_DAT_800ac784 + 0x1d6) + 0x34c))();
      return;
    }
    _DAT_800ac784[0xe] =
         (int)*(short *)((int)_DAT_800ac784 + 0x1d6) + ((byte)_DAT_800ac784[0x74] - 1) * -0x14;
  }
  return;
}


