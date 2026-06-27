int DOOR3501(void) {

	Obj_model_move(0, 0, 0, 1, 0, 32776, 4096, -14311, 27901, 23804, 0, 0, 0);
	Obj_model_move(1, 0, 0, 1, 1, 16384, 4096, -14311, 7170, 23804, 0, 0, 0);
	Sce_fade_set(0, 2, 7, 0, 254);
	Sce_fade_adjust(0, 0, 28);
	Sleep(12800);
	Evt_exec(11, 0x1804);
	Sleep(5120);
	goto Sub09();
	Se_on(0, 0, 0, 0, 0, 0, 0, 0);
	Sleep(38400);
	Evt_exec(12, 0x1806);
	Evt_exec(13, 0x1805);
	Sleep(12800);
	Sce_fade_set(0, 2, 7, 0, 4);
	Sleep(7680);
	return 0;
}