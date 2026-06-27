int sub17(void) {

	Sce_rnd();
	nop();
	Copy(6, 29);
	nop();
	Copy(16, 6);
	nop();
	Calc(16 && 65295)
	Copy(6, 16);
	nop();
	Copy(16, 6);
	nop();
	Calc(16 - 52838)
	Copy(6, 16);
	nop();
	Sce_rnd();
	nop();
	Copy(5, 29);
	nop();
	Copy(16, 5);
	nop();
	Calc(16 && 65343)
	Copy(5, 16);
	nop();
	Copy(16, 5);
	nop();
	Calc(16 - 14410)
	Copy(5, 16);
	nop();
	Work_copy(6, 16, 1);
	Work_copy(5, 8, 1);
	Sce_espr_on(0x00, 0x19, 0, 0, 0, 0x06, 0, -5910, 0, 0);
	return 0;
}