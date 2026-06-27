int DOOR1D03(void) {

	Obj_model_move(0, 0, 0, 1, 0, 40970, 4096, -12281, -10226, -4083, 0, 0, 0);
	Obj_model_move(1, 0, 0, 1, 0, 40962, 4096, -12281, -10226, -24335, 0, 8, 0);
	Obj_model_move(2, 0, 182, 1, 1, 53248, 4096, -32256, -8460, -20750, -2820, 0, 0);
	Obj_model_move(3, 0, 182, 1, 1, 53504, 4096, 32511, -8460, -5390, -2828, 8, 0);
	Sce_fade_set(0, 2, 7, 0, 254);
	Sce_fade_adjust(0, 0, 28);
	Sleep(17920);
	Evt_exec(13, 0x1805);
	Sleep(7680);
	goto Sub16();
	Se_on(0, 0, 0, 1, 0, 0, 0, 0);
	Sleep(2560);
	Evt_exec(11, 0x180D);
	Sleep(15360);
	Evt_exec(12, 0x180A);
	goto Sub09();
	return 0;
}