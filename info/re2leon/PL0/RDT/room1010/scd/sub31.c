int sub31(void) {

Evt_next();
nop();
// unknown opcode (8A) found at 0x00000008
// unknown opcode (8B) found at 0x0000000E
Se_on(2, 12, 1, 3, 0, 0, 0, 0);
Sce_espr_on(0x00, 0x18, 0, 203, 0, 0x1D, -27136, 5125, 0, 0);
Sce_espr_on(0x00, 0x01, 4, 203, 0, 0x10, -27136, 5125, 0, 0);
Sce_espr_on(0x00, 0x03, 12, 203, 0, 0x18, -27136, 5125, 0, 0);
Sce_espr_on(0x00, 0x03, 14, 203, 0, 0x10, -27136, 2055, 0, 0);
Sce_espr_on(0x00, 0x01, 2, 203, 0, 0x50, -27136, 2055, 0, 0);
Sce_espr_on(0x00, 0x03, 14, 203, 0, 0x1D, -27136, -1016, 0, 0);
Evt_exec(255, 0x1820);
Sleep(6400);
Work_set(3, 0);
nop();
Member_copy(16, 15);
nop();
Calc(16 + 5125)
Copy(4, 16);
nop();
Sce_espr_on(0x00, 0x03, 8, 203, 0, 0x0D, -18432, 11778, -20225, 0);
Work_copy(4, 14, 1);
Sce_espr_on(0x00, 0x02, 2, 203, 0, 0x09, -18432, 11778, -20225, 0);
Evt_next();
nop();
Sce_espr_on(0x00, 0x03, 8, 203, 0, 0x0D, -18432, 6658, -20225, 0);
return 0;
}