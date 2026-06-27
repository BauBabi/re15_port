int DOOR2B01(void) {

	Obj_model_move(0, 0, 0, 1, 0, 32794, 4096, -20476, 21515, -12050, 0, 0, 0);
	Sce_fade_set(0, 2, 7, 0, 254);
	Sce_fade_adjust(0, 0, 28);
	Sleep(7680);
	Evt_exec(11, 0x1802);
	Sleep(7680);
	goto Sub03();
	Se_on(0, 0, 0, 0, 0, 0, 0, 0);
	Sleep(61440);
	return 0;
}