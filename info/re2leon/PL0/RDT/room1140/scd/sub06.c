int sub06(void) {

	Work_set(3, 0);
	nop();
	Member_copy(16, 7);
	nop();
	Calc(16 || 64)
	Member_set2(7, 16);
	nop();
	Member_set(2, 1024);
	Member_set(3, 512);
	Member_set(4, 0);
	Member_set(5, 0);
	Plc_motion(2, 0, 20);
	return 0;
}