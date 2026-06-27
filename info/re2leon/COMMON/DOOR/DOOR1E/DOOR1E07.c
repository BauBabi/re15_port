int DOOR1E07(void) {

	Obj_model_move(0, 0, 0, 1, 1, 32768, 4096, 27680, -17406, 23804, 0, 0, 24831);
	Obj_model_move(1, 0, 0, 1, 1, 36864, 4096, -28440, 0, 0, 0, 0, 24831);
	Obj_model_move(2, 0, 0, 1, 1, 37120, 4096, -28440, 0, 0, 0, 0, 0);
	Obj_model_move(3, 0, 0, 1, 1, 37376, 4096, -28440, 0, 0, 0, 0, -24576);
	Sce_fade_set(0, 2, 7, 0, 254);
	Sce_fade_adjust(0, 0, 28);
	Sleep(12800);
	Evt_exec(11, 0x1808);
	Sleep(56320);
	Sce_fade_set(0, 2, 7, 0, 4);
	Sleep(7680);
	return 0;
}