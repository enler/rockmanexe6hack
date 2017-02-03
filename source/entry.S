#define VER_BLUE

//#define VER_RED

#define ABI_CONVERT(name) \
	.thumb;\
	push {r0-r7, lr};\
	mov r0, r8;\
	mov r1, r9;\
	mov r2, r10;\
	mov r3, r11;\
	mov r4, r12;\
	push {r0-r4};\
	mov r1, sp;\
	sub sp, #0x34;\
	mov r0, sp;\
	bl copyRegs;\
	bl name;\
	mov r1, sp;\
	mov r0, r1;\
	add r0, #0x34;\
	bl restoreRegs;\
	add sp, #0x34;\
	pop {r0-r4};\
	mov r8, r0;\
	mov r9, r1;\
	mov r10, r2;\
	mov r11, r3;\
	mov r12, r4;\
	pop {r0-r7};\
	pop {pc};\
	.align 2;

#define HOOK_VECTOR_ENTRY(name)\
	b name;\
	.align 2;

.section .text.start
	.thumb
	HOOK_VECTOR_ENTRY(abi_convert_hook_sub80420CC)
	HOOK_VECTOR_ENTRY(abi_convert_hook_sub3006C38)
	HOOK_VECTOR_ENTRY(abi_convert_hook_sub3006C38_2)
	HOOK_VECTOR_ENTRY(abi_convert_hook_sub8128100)
	HOOK_VECTOR_ENTRY(abi_convert_hook_sub812E21C)
	HOOK_VECTOR_ENTRY(InitCodeList)
	pop {r6}
	HOOK_VECTOR_ENTRY(abi_convert_hook_LoadText)

abi_convert_hook_sub80420CC:
	ABI_CONVERT(hook_sub80420CC)

abi_convert_hook_sub3006C38:
	ABI_CONVERT(hook_sub3006C38)

abi_convert_hook_sub3006C38_2:
	ABI_CONVERT(hook_sub3006C38_2)

abi_convert_hook_sub8128100:
	ABI_CONVERT(hook_sub8128100)

abi_convert_hook_sub812E21C:
	ABI_CONVERT(hook_sub812E21C)

abi_convert_hook_LoadText:
	ABI_CONVERT(hook_LoadText)

.global callOrignalFunc;
.type callOrignalFunc %function;
.thumb
callOrignalFunc:
	push {r4-r7,lr}
	mov r4,r8
	mov r5,r9
	mov r6,r10
	mov r7,r11
	push {r4-r7}
	push {r0}
	push {r1}
	add r0,r0,#0x20
	ldmia r0!,{r1-r5}
	mov r8,r1
	mov r9,r2
	mov r10,r3
	mov r11,r4
	mov r12,r5
	sub r0,r0,#0x34
	ldmia r0!,{r0-r7}
	mov lr,pc
	pop {r15}
	push {r0}
	ldr r0,[sp,#0x4]
	add r0,r0,#0x4
	stmia r0!,{r1-r7}
	mov r1,r8
	mov r2,r9
	mov r3,r10
	mov r4,r11
	mov r5,r12
	stmia r0!,{r1-r5}
	sub r0,r0,#0x34
	pop {r1}
	str r1,[r0]
	add sp, #4
	pop {r4-r7}
	mov r8,r4
	mov r9,r5
	mov r10,r6
	mov r11,r7
	pop {r4-r7}
	pop {r1}
	bx r1
.align 2
.global ori_loadText;
.type ori_loadText %function;
.thumb
ori_loadText:
	push {r4-r6, lr}
	mov r5, r10
	ldr r5,[r5,#0x2c]
	ldr r2,[r5,#0x4c]
	ldr r3,[r5,#0x50]
	ldr r4,[r5,#0x54]
	ldr r6, ori_loadText_addr
	bx r6
.align 2
ori_loadText_addr:
#ifdef VER_RED
	.word 0x80415C9
#endif
#ifdef VER_BLUE
	.word 0x804159D
#endif