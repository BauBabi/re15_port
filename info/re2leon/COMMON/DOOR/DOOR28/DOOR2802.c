int DOOR2802(void) {

	Obj_model_move(0, 0, 0, 1, 0, 32768, 4096, -7156, 18945, -28417, -20226, -16384, -4094);
	Obj_model_move(1, 0, 226, 1, 1, 53249, 4096, 2301, -17670, 28412, 0, 0, 0);
	Obj_model_move(2, 0, 0, 1, 2, 53248, 4096, -25347, 21755, -8964, 0, 0, 0);
	Sce_fade_set(0, 2, 7, 0, 254);
	Sce_fade_adjust(0, 0, 28);
	Sleep(26880);
	goto Sub05();
	// unknown opcode (8A) found at 0x00000058
	// unknown opcode (8C) found at 0x00000060
	// unknown opcode (8B) found at 0x00000066
	Se_on(0, 0, 0, 0, 0, 0, 0, 0);
	Sleep(1280);
	Evt_exec(11, 0x1804);
	Sleep(43520);
	return 0;
}