/* SYS_OBJ_1578 @ 0x80091458  (Ghidra headless, raw MIPS overlay) */

void SYS_OBJ_1578(void)

{
  undefined2 in_v0;
  int in_a3;
  int unaff_s0;
  int unaff_s1;
  short sStack00000010;
  short sStack00000012;
  undefined2 in_stack_00000014;
  
  _in_stack_00000014 = CONCAT22(in_v0,in_stack_00000014);
  _sStack00000010 =
       CONCAT22(sStack00000012 - *(short *)(unaff_s0 + 10),sStack00000010 - *(short *)(unaff_s0 + 8)
               );
  *(uint *)(in_a3 * 4 + unaff_s1) =
       (uint)*(byte *)(unaff_s0 + 0x1b) << 0x10 | (uint)*(byte *)(unaff_s0 + 0x1a) << 8 | 0x60000000
       | (uint)*(byte *)(unaff_s0 + 0x19);
  *(undefined4 *)((in_a3 + 1) * 4 + unaff_s1) = _sStack00000010;
  *(undefined4 *)((in_a3 + 2) * 4 + unaff_s1) = _in_stack_00000014;
  *(char *)(unaff_s1 + 3) = (char)in_a3 + '\x02';
  return;
}


