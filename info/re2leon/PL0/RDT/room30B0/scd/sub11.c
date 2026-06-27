int sub11(void) {

	Set(2, 7, 1);
	Set(1, 27, 1);
	Work_set(1, 0);
	nop();
	Pos_set(0, 29396, 0, -20245);
	Dir_set(0, 0, -16385, 0);
	Work_set(2, 0);
	nop();
	Pos_set(0, 27896, 0, -20245);
	Dir_set(0, 0, -26616, 0);
	Evt_exec(255, 0x180C);
	return 0;
}