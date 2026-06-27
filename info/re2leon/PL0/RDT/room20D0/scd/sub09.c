int sub09(void) {

	Set(5, 12, 0);
	Se_on(2, 12, 0, 0, 0, 0, 0, 0);
	Sce_espr_kill2(2);
	Set(5, 13, 0);
	Se_on(2, 12, 0, 0, 0, 0, 0, 0);
	Sce_espr_kill2(3);
	Set(5, 11, 0);
	Se_on(2, 12, 0, 0, 0, 0, 0, 0);
	Sce_espr_kill2(1);
	Save(4, 0);
	Aot_reset(4, 5, 49, 65280, 6150, 0x00);
	Aot_reset(5, 5, 49, 65280, 6151, 0x00);
	Aot_reset(6, 5, 49, 65280, 6152, 0x00);
	return 0;
}