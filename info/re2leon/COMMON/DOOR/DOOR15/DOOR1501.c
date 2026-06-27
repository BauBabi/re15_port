int DOOR1501(void) {

	Obj_model_move(0, 0, 0, 1, 0, 40970, 4096, -12281, -10226, 8, 0, 0, 0);
	Obj_model_move(1, 0, 226, 1, 1, 53248, 4096, -20991, -9999, -4877, 1, 252, 0);
	Obj_model_move(2, 0, 0, 1, 1, 20480, 4096, -18690, -9999, -4877, 1, 4, 0);
	Sce_fade_set(0, 2, 7, 0, 254);
	Sce_fade_adjust(0, 0, 28);
	Sleep(15360);
	goto Sub13();
	Se_on(0, 0, 0, 0, 0, 0, 0, 0);
	Sleep(12800);
	Evt_exec(11, 0x180A);
	Sleep(14080);
	goto Sub11();
	return 0;
}