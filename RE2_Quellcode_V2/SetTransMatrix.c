/* SetTransMatrix @ 0x8008ddc4  (Ghidra headless, raw MIPS overlay) */

void SetTransMatrix(MATRIX *m)

{
  gte_ldtr(m->t[0],m->t[1],m->t[2]);
  return;
}


