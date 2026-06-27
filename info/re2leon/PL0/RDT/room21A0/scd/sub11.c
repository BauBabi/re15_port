int sub11(void) {

	Set(2, 7, 1);
	Cut_chg(0x07);
	Set(5, 2, 1);
	Work_set(1, 0);
	nop();
	Pos_set(0, 9178, 0, 20198);
	Dir_set(0, 0, 4, 0);
	Plc_dest(0, 1056, 12247, -6979);
	Work_set(3, 1);
	nop();
	Member_set(5, 256);
	Sleep(3328);
	// unknown opcode (8A) found at 0x00000038
	// unknown opcode (8B) found at 0x0000003E
	Work_set(1, 0);
	nop();
	Plc_dest(0, 2336, 28179, -18714);
	Plc_rot(0, -14336);
	Sleep(1280);
	Work_set(1, 0);
	Plc_motion(1, 3, 0);
	nop();
	Sleep(5120);
	Work_set(1, 0);
	Plc_ret();
	Set(2, 7, 0);
	Cut_chg(0x04);
	Cut_auto(1);
	return 0;
}