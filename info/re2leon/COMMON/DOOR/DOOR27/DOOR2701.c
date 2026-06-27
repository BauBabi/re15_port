int DOOR2701(void) {

	Obj_model_move(0, 0, 0, 1, 0, 32778, 4096, -12281, -10226, 18676, 0, 0, 0);
	Obj_model_move(1, 0, 0, 1, 1, 32770, 4096, -12281, -10226, -18421, 0, 0, 0);
	Sce_fade_set(0, 2, 7, 0, 254);
	Sce_fade_adjust(0, 0, 28);
	Sleep(17920);
	goto Sub09();
	Evt_exec(12, 0x1802);
	Evt_exec(13, 0x1803);
	Se_on(0, 0, 0, 1, 0, 0, 0, 0);
	Sleep(20480);
	Evt_exec(14, 0x1805);
	goto Sub04();
	return 0;
}