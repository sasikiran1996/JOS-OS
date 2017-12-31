/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>

#include <kern/picirq.h>


###################################################################
# exceptions/interrupts
###################################################################

/* TRAPHANDLER defines a globally-visible function for handling a trap.
 * It pushes a trap number onto the stack, then jumps to _alltraps.
 * Use TRAPHANDLER for traps where the CPU automatically pushes an error code.
 *
 * You shouldn't call a TRAPHANDLER function from C, but you may
 * need to _declare_ one in C (for instance, to get a function pointer
 * during IDT setup).  You can declare the function with
 *   void NAME();
 * where NAME is the argument passed to TRAPHANDLER.
 */
#define TRAPHANDLER(name, num)						\
	.globl name;		/* define global symbol for 'name' */	\
	.type name, @function;	/* symbol type is function */		\
	.align 2;		/* align function definition */		\
	name:			/* function starts here */		\
	pushl $(num);							\
	jmp _alltraps

/* Use TRAPHANDLER_NOEC for traps where the CPU doesn't push an error code.
 * It pushes a 0 in place of the error code, so the trap frame has the same
 * format in either case.
 */
#define TRAPHANDLER_NOEC(name, num)					\
	.globl name;							\
	.type name, @function;						\
	.align 2;							\
	name:								\
	pushl $0;							\
	pushl $(num);							\
	jmp _alltraps

.text

/*
 * Lab 3: Your code here for generating entry points for the different traps.
 * The various interrupt error codes are obtained from http://wiki.osdev.org/Exceptions#Page_Fault
 */
_INTERRUPT_0 :
		TRAPHANDLER_NOEC(interrupt_0, 0);
_INTERRUPT_1 :
		TRAPHANDLER_NOEC(interrupt_1, 1);
_INTERRUPT_2 :
		TRAPHANDLER_NOEC(interrupt_2, 2);
_INTERRUPT_3 :
		TRAPHANDLER_NOEC(interrupt_3, 3);
_INTERRUPT_4 :
		TRAPHANDLER_NOEC(interrupt_4, 4);
_INTERRUPT_5 :
		TRAPHANDLER_NOEC(interrupt_5, 5);
_INTERRUPT_6 :
		TRAPHANDLER_NOEC(interrupt_6, 6);
_INTERRUPT_7 :
		TRAPHANDLER_NOEC(interrupt_7, 7);
_INTERRUPT_8 :
		TRAPHANDLER(interrupt_8, 8);
_INTERRUPT_9 :
		TRAPHANDLER_NOEC(interrupt_9, 9);
_INTERRUPT_10 :
		TRAPHANDLER(interrupt_10, 10);
_INTERRUPT_11 :
		TRAPHANDLER(interrupt_11, 11);
_INTERRUPT_12 :
		TRAPHANDLER(interrupt_12, 12);
_INTERRUPT_13 :
		TRAPHANDLER(interrupt_13, 13);
_INTERRUPT_14 :
		TRAPHANDLER(interrupt_14, 14);
_INTERRUPT_15 :
		TRAPHANDLER_NOEC(interrupt_15, 15);
_INTERRUPT_16 :
		TRAPHANDLER_NOEC(interrupt_16, 16);
_INTERRUPT_17 :
		TRAPHANDLER(interrupt_17, 17);
_INTERRUPT_18 :
		TRAPHANDLER_NOEC(interrupt_18, 18);
_INTERRUPT_19 :
		TRAPHANDLER_NOEC(interrupt_19, 19);
_INTERRUPT_20 :
		TRAPHANDLER_NOEC(interrupt_20, 20);
_INTERRUPT_21 :
		TRAPHANDLER_NOEC(interrupt_21, 21);
_INTERRUPT_22 :
		TRAPHANDLER_NOEC(interrupt_22, 22);
_INTERRUPT_23 :
		TRAPHANDLER_NOEC(interrupt_23, 23);
_INTERRUPT_24 :
		TRAPHANDLER_NOEC(interrupt_24, 24);
_INTERRUPT_25 :
		TRAPHANDLER_NOEC(interrupt_25, 25);
_INTERRUPT_26 :
		TRAPHANDLER_NOEC(interrupt_26, 26);
_INTERRUPT_27 :
		TRAPHANDLER_NOEC(interrupt_27, 27);
_INTERRUPT_28 :
		TRAPHANDLER_NOEC(interrupt_28, 28);
_INTERRUPT_29 :
		TRAPHANDLER_NOEC(interrupt_29, 29);
_INTERRUPT_30 :
		TRAPHANDLER(interrupt_30, 30);
_INTERRUPT_31 :
		TRAPHANDLER_NOEC(interrupt_31, 31);


_INTERRUPT_32 :
	TRAPHANDLER_NOEC(interrupt_32, 32);
_INTERRUPT_33 :
	TRAPHANDLER_NOEC(interrupt_33, 33);
_INTERRUPT_34 :
	TRAPHANDLER_NOEC(interrupt_34, 34);
_INTERRUPT_35 :
	TRAPHANDLER_NOEC(interrupt_35, 35);
_INTERRUPT_36 :
	TRAPHANDLER_NOEC(interrupt_36, 36);
_INTERRUPT_37 :
	TRAPHANDLER_NOEC(interrupt_37, 37);
_INTERRUPT_38 :
	TRAPHANDLER_NOEC(interrupt_38, 38);
_INTERRUPT_39 :
	TRAPHANDLER_NOEC(interrupt_39, 39);
_INTERRUPT_40 :
	TRAPHANDLER_NOEC(interrupt_40, 40);
_INTERRUPT_41 :
	TRAPHANDLER_NOEC(interrupt_41, 41);
_INTERRUPT_42 :
	TRAPHANDLER_NOEC(interrupt_42, 42);
_INTERRUPT_43 :
	TRAPHANDLER_NOEC(interrupt_43, 43);
_INTERRUPT_44 :
	TRAPHANDLER_NOEC(interrupt_44, 44);
_INTERRUPT_45 :
	TRAPHANDLER_NOEC(interrupt_45, 45);
_INTERRUPT_46 :
	TRAPHANDLER_NOEC(interrupt_46, 46);
_INTERRUPT_47 :
	TRAPHANDLER_NOEC(interrupt_47, 47);




_INTERRUPT_48 :
		TRAPHANDLER_NOEC(interrupt_48, 48);

/*
 * Lab 3: Your code here for _alltraps
 * pushing the remaining for a trapFrame
 */


_alltraps : 
		pushl %ds
		pushl %es
		pushal

		movl $GD_KD, %eax
		movw %ax, %es
		movw %ax, %ds

		pushl %esp
		call trap



