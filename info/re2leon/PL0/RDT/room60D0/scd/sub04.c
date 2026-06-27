int sub04(void) {

switch(26) {
case 0:
Work_set(4, 4);
nop();
Member_set(12, 0);
Work_set(4, 5);
nop();
Member_set(12, 131);
Work_set(4, 6);
nop();
Member_set(12, 131);
break;
case 256:
Work_set(4, 4);
nop();
Member_set(12, 131);
Work_set(4, 5);
nop();
Member_set(12, 0);
Work_set(4, 6);
nop();
Member_set(12, 131);
break;
case 512:
case 768:
Work_set(4, 4);
nop();
Member_set(12, 131);
Work_set(4, 5);
nop();
Member_set(12, 131);
Work_set(4, 6);
nop();
Member_set(12, 0);
break;
default:
Work_set(4, 4);
nop();
Member_set(12, 131);
Work_set(4, 5);
nop();
Member_set(12, 131);
Work_set(4, 6);
nop();
Member_set(12, 131);
}
return 0;
}