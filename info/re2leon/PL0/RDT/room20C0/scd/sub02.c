int sub02(void) {

	if(Ck(8, 207, 0)) {
		Sleep(38656);
		if(Ck(1, 0, 0)) {
			Aot_reset(10, 2, 49, 5376, 1792, 0xCF00);
		} else {
			Aot_reset(10, 2, 49, 7424, 9216, 0xCF00);
			nop();
			nop();
		}
		return 0;
}