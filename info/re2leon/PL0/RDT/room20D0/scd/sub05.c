int sub05(void) {

	if(Ck(4, 84, 0)) {
		Aot_reset(8, 0, 0, 0, 0, 0x00);
		Set(4, 84, 1);
		Set(5, 10, 1);
		Sleep(768);
		Sleep(768);
		Se_on(2, 11, 0, 0, 0, 0, 0, 0);
		Sce_espr_on(0x00, 0x1E, 0, 0, 0, 0x10, 31896, 12536, -9005, 0);
		Sce_espr_on(0x00, 0x1E, 0, 0, 0, 0x10, 31896, 12536, 5331, 0);
		Save(4, 0);
	} else {
		nop();
		nop();
		return 0;
}