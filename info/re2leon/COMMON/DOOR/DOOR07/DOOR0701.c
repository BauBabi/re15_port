int DOOR0701(void) {

	Obj_model_move(0, 0, 0, 1, 0, 40970, 4096, -12281, -10226, 8, 0, 0, 0);
	Obj_model_move(1, 0, 0, 1, 1, 53248, 4096, -32256, -5902, -19214, 0, 0, 0);
	Sce_fade_set(0, 2, 7, 0, 254);
	Sce_fade_adjust(0, 0, 28);
	Sleep(17920);
	goto Sub04();
	Se_on(0, 0, 0, 1, 0, 0, 0, 0);
	Evt_exec(11, 0x1805);
	Sleep(17920);
	Evt_exec(13, 0x1803);
	Sleep(17920);
	goto Sub07();
	return 0;
}