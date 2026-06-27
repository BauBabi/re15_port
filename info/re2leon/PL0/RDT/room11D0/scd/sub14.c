int sub14(void) {

	Work_set(3, 1);
	Set(5, 5, 1);
	nop();
	Member_set(23, 64);
	Sleep(256);
	Member_copy(16, 23);
	nop();
	Calc(16 && 65471)
	Member_set2(23, 16);
	nop();
	return 0;
}