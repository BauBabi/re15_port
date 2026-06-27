int DOOR3001(void) {

	Obj_model_move(0, 0, 0, 1, 0, 32778, 4096, -21491, -29686, -30716, 0, 8, 0);
	Obj_model_move(1, 0, 0, 1, 0, 32770, 4096, -21491, -29686, -4870, 0, 0, 0);
	Sce_fade_set(0, 2, 7, 0, 254);
	Sce_fade_adjust(0, 0, 28);
	Sleep(17920);
	Sleep(12800);
	goto Sub12();
	Se_on(0, 0, 0, 1, 0, 0, 0, 0);
	Evt_exec(11, 0x1803);
	Evt_exec(12, 0x1804);
	Sleep(28160);
	Evt_exec(13, 0x1806);
	goto Sub05();
	return 0;
}