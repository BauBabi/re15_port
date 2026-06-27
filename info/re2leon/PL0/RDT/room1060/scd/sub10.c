int sub10(void) {

	Save(9, 0);
	for(n = 0; n < 3840; n++) {
		Work_copy(7, 8, 1);
		Work_copy(9, 2, 0);
		Parts_set(0, 0, 0, 0);
		Work_copy(8, 8, 1);
		Work_copy(9, 2, 0);
		Parts_set(0, 0, 1, 0);
		Copy(16, 9);
		nop();
		Calc(16 + 256)
		Copy(9, 16);
		nop();
	}
	return 0;
}