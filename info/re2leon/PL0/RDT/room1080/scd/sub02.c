int sub02(void) {

	Set(4, 179, 1);
	Set(1, 9, 1);
	Set(2, 7, 1);
	Evt_next();
	Sce_bgm_control(0, 0, 1, 1, 49);
	Sce_bgm_control(0, 0, 2, 1, 64);
	Sce_bgm_control(0, 0, 3, 1, 35);
	Sce_bgm_control(0, 0, 4, 1, 50);
	Sce_bgm_control(0, 4, 0, 1, 65);
	nop();
	Sce_fade_set(2, 2, 7, 0, 254);
	Movie_on(8);
	Sce_fade_adjust(2, 255, 127);
	Evt_next();
	Set(5, 5, 0);
	nop();
	Sleep(16128);
	Sce_bgm_control(0, 3, 0, 128, 65);
	Set(2, 7, 0);
	return 0;
}