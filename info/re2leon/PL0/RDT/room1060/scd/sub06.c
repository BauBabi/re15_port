int sub06(void) {

	Evt_exec(255, 0x1807);
	Sleep(7680);
	Set(5, 0, 1);
	Sce_bgm_control(1, 1, 0, 90, 65);
	Evt_exec(255, 0x1809);
	return 0;
}