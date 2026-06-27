int DOOR2D01(void) {

	Obj_model_move(0, 0, 0, 1, 0, 32770, 4096, -9211, -27111, -2840, 0, 0, -20480);
	Obj_model_move(1, 0, 0, 1, 1, 53248, 4096, 13551, -18700, -20478, 0, 0, 0);
	Obj_model_move(2, 0, 0, 1, 2, 16384, 4096, -5910, 7180, 28406, 0, 0, 20483);
	Obj_model_move(3, 0, 0, 1, 2, 20992, 4096, 0, 0, -29166, 0, 0, 0);
	Obj_model_move(4, 0, 0, 1, 2, 20992, 4096, -26616, 0, 15606, 4, 4, 4);
	Obj_model_move(5, 0, 0, 1, 2, 20992, 4096, -26616, 0, 8220, 252, 4, 4);
	Sce_fade_set(0, 2, 7, 0, 254);
	Sce_fade_adjust(0, 0, 28);
	Sleep(15360);
	goto Sub09();
	// unknown opcode (8A) found at 0x0000009A
	// unknown opcode (8C) found at 0x000000A2
	// unknown opcode (8B) found at 0x000000A8
	// unknown opcode (8C) found at 0x000000B0
	// unknown opcode (8A) found at 0x000000B6
	Se_on(0, 0, 0, 0, 0, 0, 0, 0);
	Evt_exec(11, 0x1803);
	Sleep(12800);
	Evt_exec(12, 0x1804);
	Sleep(48640);
	Se_on(0, 1, 0, 1, 0, 0, 0, 0);
	return 0;
}