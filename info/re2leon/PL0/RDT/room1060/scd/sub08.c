int sub08(void) {

	Save(6, 0);
	for(n = 0; n < 3840; n++) {
		Work_copy(4, 8, 1);
		Work_copy(6, 2, 0);
		Parts_set(0, 0, 0, 0);
		Work_copy(5, 8, 1);
		Work_copy(6, 2, 0);
		Parts_set(0, 0, 1, 0);
		Copy(16, 6);
		nop();
		Calc(16 + 256)
		Copy(6, 16);
		nop();
	}
	return 0;
}