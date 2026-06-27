int sub26(void) {

	Work_set(1, 0);
	Plc_motion(0, 22, 4);
	nop();
	Member_copy(16, 0);
	nop();
	Calc(16 && 65023)
	Member_set2(0, 16);
	nop();
	Plc_neck(5, 3, 0, 0, 0, 4112);
	return 0;
}