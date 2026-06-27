int DOOR1D01(void) {

	Obj_model_move(0, 0, 0, 1, 0, 40970, 4096, -12281, -12786, 8, 0, 0, 0);
	Obj_model_move(1, 0, 226, 1, 1, 53248, 4096, -32256, -8460, -20750, -2820, 0, 0);
	Sce_fade_set(0, 2, 7, 0, 254);
	Sce_fade_adjust(0, 0, 28);
	Sleep(17920);
	goto Sub16();
	Se_on(0, 0, 0, 0, 0, 0, 0, 0);
	Evt_exec(11, 0x1807);
	Sleep(12800);
	Evt_exec(11, 0x180D);
	Sleep(15360);
	goto Sub14();
	return 0;
}