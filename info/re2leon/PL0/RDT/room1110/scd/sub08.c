int sub08(void) {

	goto Sub07();
	Sce_bgm_control(1, 0, 1, 80, 31);
	Work_set(4, 10);
	nop();
	Speed_set(0, -2305);
	// unknown opcode (8A) found at 0x00000016
	// unknown opcode (8B) found at 0x0000001C
	Se_on(2, 16, 1, 4, 10, 0, 0, 0);
	for(n = 0; n < 5120; n++) {
		Add_speed();
		Evt_next();
	}
	Sleep(2560);
	Evt_exec(255, 0x1809);
	Cut_chg(0x09);
	Speed_set(0, 0);
	Speed_set(2, 11520);
	Sleep(7680);
	// unknown opcode (8B) found at 0x0000004E
	// unknown opcode (8A) found at 0x00000054
	// unknown opcode (8B) found at 0x0000005A
	Se_on(2, 17, 1, 4, 10, 0, 0, 0);
	for(n = 0; n < 10240; n++) {
		Add_speed();
		Evt_next();
	}
	goto Sub21();
	Sce_bgm_control(1, 0, 1, 121, 65);
	return 0;
}