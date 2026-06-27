int DOOR3400(void) {

	Obj_model_move(0, 0, 0, 0, 0, 49152, 4096, -12281, -10226, 8, 0, 0, 0);
	Sce_fade_set(0, 2, 7, 0, 252);
	Sleep(5120);
	goto Sub01();
	Se_on(0, 0, 0, 0, 0, 0, 0, 0);
	Sleep(1025);
	Sce_fade_set(0, 2, 7, 0, 4);
	Sleep(10240);
	return 0;
}