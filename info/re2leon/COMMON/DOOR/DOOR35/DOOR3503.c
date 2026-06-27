int DOOR3503(void) {

	Obj_model_move(1, 0, 0, 1, 1, 32768, 4096, -14311, 7170, 23804, 0, 0, 0);
	Sce_fade_set(0, 2, 7, 0, 254);
	Sce_fade_adjust(0, 0, 28);
	Sleep(12800);
	Sleep(17920);
	goto Sub09();
	Se_on(0, 0, 0, 0, 0, 0, 0, 0);
	Evt_exec(12, 0x1806);
	Sleep(25600);
	Sleep(12800);
	Sce_fade_set(0, 2, 7, 0, 4);
	Sleep(7680);
	return 0;
}