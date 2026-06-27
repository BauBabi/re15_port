int sub02(void) {

	if(Ck(1, 1, 0)) {
		Sce_bgmtbl_set(0, 0x1A02, 0x3FF, 0x00);
	} else {
		Sce_bgmtbl_set(0, 0x1A02, 0x3FF, 0x00);
		nop();
		nop();
		return 0;
}