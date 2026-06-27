int sub23(void) {

	Set(1, 25, 1);
	Aot_reset(9, 0, 0, 0, 0, 0x00);
	Aot_reset(0, 0, 0, 0, 0, 0x00);
	Aot_reset(10, 5, 49, 65280, 6162, 0x00);
	// unknown opcode (8A) found at 0x00000028
	// unknown opcode (8B) found at 0x0000002E
	// unknown opcode (8C) found at 0x00000036
	// unknown opcode (8B) found at 0x0000003C
	Se_on(2, 12, 1, 1, 0, 0, 0, 0);
	Evt_exec(255, 0x1821);
	Sleep(1280);
	Set(2, 7, 1);
	Sleep(1280);
	Set(1, 27, 1);
	Work_set(1, 0);
	nop();
	Plc_neck(1, 52, 233, -5910, 18434, -32640);
	Sleep(1024);
	Plc_dest(0, 5408, -3320, -32768);
	goto Sub36();
	Evt_chain(0, 24, 25);
	return 0;
}