int sub24(void) {

	Set(1, 25, 1);
	Set(2, 7, 1);
	Aot_reset(9, 0, 0, 0, 0, 0x00);
	Aot_reset(0, 0, 0, 0, 0, 0x00);
	Aot_reset(10, 5, 49, 65280, 6162, 0x00);
	// unknown opcode (8A) found at 0x0000002C
	// unknown opcode (8B) found at 0x00000032
	// unknown opcode (8C) found at 0x0000003A
	// unknown opcode (8B) found at 0x00000040
	Se_on(2, 12, 1, 1, 0, 0, 0, 0);
	Evt_exec(255, 0x1821);
	Sleep(2560);
	Set(1, 27, 1);
	Work_set(1, 0);
	nop();
	Plc_neck(1, 52, 233, -5910, 18434, -32640);
	Sleep(1024);
	Plc_dest(0, 5408, 29451, -32768);
	goto Sub36();
	Plc_motion(11, 1, 0);
	Sleep(2560);
	Evt_chain(0, 24, 25);
	return 0;
}