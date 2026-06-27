int sub06(void) {

	goto Sub11();
	Work_set(3, 0);
	nop();
	Pos_set(0, -8273, 0, 14006);
	Dir_set(0, 0, -28449, 0);
	Plc_dest(0, 1057, 10935, 31416);
	goto Sub30();
	Plc_dest(0, 2337, -24650, -2098);
	goto Sub30();
	Plc_motion(0, 22, 4);
	Plc_neck(2, 0, 0, 0, 1, 8224);
	Member_copy(16, 0);
	nop();
	Calc(16 || 1024)
	Member_set2(0, 16);
	nop();
	goto Sub16();
	return 0;
}