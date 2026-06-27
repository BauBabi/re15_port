int DOOR2E02(void) {

	Obj_model_move(0, 0, 0, 1, 0, 32778, 4096, -15351, -18419, -23814, 0, 8, 0);
	Obj_model_move(1, 0, 0, 1, 1, 53248, 4096, 32511, -780, 19701, 16, 0, 8);
	Obj_model_move(2, 0, 0, 1, 1, 20480, 4096, -32256, 12021, 19701, 14343, 8, 8);
	Sce_fade_set(0, 2, 7, 0, 254);
	Sce_fade_adjust(0, 0, 28);
	Sleep(17920);
	Evt_exec(11, 0x1805);
	Sleep(7680);
	goto Sub04();
	Se_on(0, 0, 0, 1, 0, 0, 0, 0);
	Sleep(7680);
	Evt_exec(12, 0x1803);
	Sleep(17920);
	goto Sub08();
	return 0;
}