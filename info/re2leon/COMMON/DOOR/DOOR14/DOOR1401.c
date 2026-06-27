int DOOR1401(void) {

	Obj_model_move(0, 0, 0, 1, 0, 32778, 4096, -12281, -10226, -14092, 0, 0, 0);
	Obj_model_move(1, 0, 0, 1, 1, 53248, 4096, -32256, 24822, 18963, 0, 0, 0);
	Sce_fade_set(0, 2, 7, 0, 254);
	Sce_fade_adjust(0, 0, 28);
	Sleep(15360);
	Sleep(12800);
	goto Sub08();
	Se_on(0, 0, 0, 0, 0, 0, 0, 0);
	Evt_exec(11, 0x1803);
	Sleep(17920);
	goto Sub06();
	return 0;
}