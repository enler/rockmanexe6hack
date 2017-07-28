hook入口<br>
HOOK_VECTOR_ENTRY全部定义在头部(0x88c0000开始，entry.S里)，地址全部固定了，每次修改代码后编译都不会变动<br><br>
红版(括号里为蓝版)<br>
0x42060 (0x42034) 指针改为<br>
HOOK_VECTOR_ENTRY(abi_convert_hook_sub80420CC)的地址(0x88C0001)<br><br>
0x1D7018 (0x1D7018)指针改为<br>
HOOK_VECTOR_ENTRY(abi_convert_hook_sub3006C38)的地址(0x88C0005)<br><br>
0x47134 (0x47104)指针改为<br>
HOOK_VECTOR_ENTRY(abi_convert_hook_sub3006C38_2)的地址(0x88C0009)<br><br>
0x128100 (0x129EC8)进行inline hook<br>
ldr r1, =HOOK_VECTOR_ENTRY(abi_convert_hook_sub8128100) #0x88C000D<br>
bx r1<br><br>
0x12E21C (0x12FFE4)进行inline hook<br>
ldr r1, =HOOK_VECTOR_ENTRY(abi_convert_hook_sub812E21C) #0x88C0011<br>
bx r1<br><br>
0x44F1C (0x44EEC) 指针改为<br>
HOOK_VECTOR_ENTRY(InitCodeList)的地址(0x88C0015)<br>
※注1<br>
0x44EEC (0x44EBC)是放了一个hook函数，hook的是显存字库的初始化函数(0x8000B31)<br>
但这个函数不适合inline hook，所以修改了调用方(将bl #0x8000B30改成bl #0x8044EEC(0x8044EBC))<br>
然后再在这里添加了InitCodeList函数<br><br>
※注2<br>
0x44EEC是存放了字库的宽度表，但因为字库扩容了，这个宽度表的位置也随之变动，成为一块无用的区域<br><br>
0x415BC (0x41590)进行inline hook<br>
push r6<br>
ldr r6 HOOK_VECTOR_ENTRY(abi_convert_hook_LoadText)<br>
bx r6<br><br>
在HOOK_VECTOR_ENTRY(abi_convert_hook_LoadText)前面加一条pop r6以保持栈平衡
