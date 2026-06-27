int DOOR3104(void) {

	Obj_model_move(0, 0, 0, 1, 0, 32778, 4096, -12281, -10226, -18421, 0, 8, 0);
	Obj_model_move(1, 0, 0, 1, 2, 32770, 4096, -12281, -10226, 18676, 0, 0, 0);
	Obj_model_move(2, 0, 0, 1, 3, 53248, 4096, 16126, 3316, -2041, 0, 8, 0);
	Obj_model_move(3, 0, 0, 1, 4, 53504, 4096, 0, 12021, 1025, 0, 0, 0);
	Sce_fade_set(0, 2, 7, 0, 254);
	Sce_fade_adjust(0, 0, 28);
	Sleep(15360);
	goto Sub17();
	Se_on(0, 0, 0, 1, 0, 0, 0, 0);
	Evt_exec(12, 0x1805);
	Sleep(12800);
	Evt_exec(13, 0x180B);
	Sleep(14080);
	Evt_exec(12, 0x180A);
	goto Sub09();
	return 0;
}