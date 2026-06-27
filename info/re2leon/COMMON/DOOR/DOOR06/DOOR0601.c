int DOOR0601(void) {

	Obj_model_move(0, 0, 0, 1, 0, 40970, 4096, -12281, -24562, 8, 0, 0, 0);
	Obj_model_move(1, 0, 0, 1, 1, 53248, 4096, -32256, 8948, -9486, 0, 0, 0);
	Sce_fade_set(0, 2, 7, 0, 254);
	Sce_fade_adjust(0, 0, 28);
	Sleep(17920);
	Sleep(15360);
	goto Sub12();
	Se_on(0, 0, 0, 1, 0, 0, 0, 0);
	Evt_exec(11, 0x1809);
	Sleep(15360);
	goto Sub10();
	return 0;
}