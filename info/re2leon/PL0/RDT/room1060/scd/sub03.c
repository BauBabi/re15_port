int sub03(void) {

	if(Ck(5, 0, 1)) {
		Ck(6, 66, 1)
		Ck(6, 67, 1)
		Set(5, 0, 0);
		Sce_bgm_control(1, 5, 0, 90, 65);
	}
	if(Ck(5, 1, 1)) {
		Ck(6, 66, 1)
		Ck(6, 67, 1)
		Aot_reset(9, 9, 49, 3328, 0, 0x00);
	}
	return 0;
}