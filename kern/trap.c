#include <inc/mmu.h>
#include <inc/x86.h>
#include <inc/assert.h>
#include <inc/string.h>

#include <kern/pmap.h>
#include <kern/trap.h>
#include <kern/console.h>
#include <kern/monitor.h>
#include <kern/env.h>
#include <kern/syscall.h>
#include <kern/sched.h>
#include <kern/kclock.h>
#include <kern/picirq.h>
#include <kern/cpu.h>
#include <kern/spinlock.h>



static struct Taskstate ts;

/* For debugging, so print_trapframe can distinguish between printing
 * a saved trapframe and printing the current trapframe and print some
 * additional information in the latter case.
 */
static struct Trapframe *last_tf;

/* Interrupt descriptor table.  (Must be built at run time because
 * shifted function addresses can't be represented in relocation records.)
 */
struct Gatedesc idt[256] = { { 0 } };
struct Pseudodesc idt_pd = {
	sizeof(idt) - 1, (uint32_t) idt
};

/*Funtion Declarations of Interrupt functions...*/
extern void interrupt_0() ;
extern void interrupt_1() ;
extern void interrupt_2() ;
extern void interrupt_3() ;
extern void interrupt_4() ;
extern void interrupt_5() ;
extern void interrupt_6() ;
extern void interrupt_7() ;
extern void interrupt_8() ;
extern void interrupt_9() ;
extern void interrupt_10() ;
extern void interrupt_11() ;
extern void interrupt_12() ;
extern void interrupt_13() ;
extern void interrupt_14() ;
extern void interrupt_15() ;
extern void interrupt_16() ;
extern void interrupt_17() ;
extern void interrupt_18() ;
extern void interrupt_19() ;
extern void interrupt_20() ;
extern void interrupt_21() ;
extern void interrupt_22() ;
extern void interrupt_23() ;
extern void interrupt_24() ;
extern void interrupt_25() ;
extern void interrupt_26() ;
extern void interrupt_27() ;
extern void interrupt_28() ;
extern void interrupt_29() ;
extern void interrupt_30() ;
extern void interrupt_31() ;

extern void interrupt_32() ;
extern void interrupt_33() ;
extern void interrupt_34() ;
extern void interrupt_35() ;
extern void interrupt_36() ;
extern void interrupt_37() ;
extern void interrupt_38() ;
extern void interrupt_39() ;
extern void interrupt_40() ;
extern void interrupt_41() ;
extern void interrupt_42() ;
extern void interrupt_43() ;
extern void interrupt_44() ;
extern void interrupt_45() ;
extern void interrupt_46() ;
extern void interrupt_47() ;


extern void interrupt_48() ;

static const char *trapname(int trapno)
{
	static const char * const excnames[] = {
		"Divide error",
		"Debug",
		"Non-Maskable Interrupt",
		"Breakpoint",
		"Overflow",
		"BOUND Range Exceeded",
		"Invalid Opcode",
		"Device Not Available",
		"Double Fault",
		"Coprocessor Segment Overrun",
		"Invalid TSS",
		"Segment Not Present",
		"Stack Fault",
		"General Protection",
		"Page Fault",
		"(unknown trap)",
		"x87 FPU Floating-Point Error",
		"Alignment Check",
		"Machine-Check",
		"SIMD Floating-Point Exception"
	};

	if (trapno < sizeof(excnames)/sizeof(excnames[0]))
		return excnames[trapno];
	if (trapno == T_SYSCALL)
		return "System call";
	if (trapno >= IRQ_OFFSET && trapno < IRQ_OFFSET + 16)
		return "Hardware Interrupt";
	return "(unknown trap)";
}


void
trap_init(void)
{
	extern struct Segdesc gdt[];

	// LAB 3: Your code here.
	void (*fun_ptr)();
	fun_ptr = interrupt_0;
	SETGATE(idt[0],0,GD_KT, (uint32_t)fun_ptr,0);

	fun_ptr = interrupt_1;
	SETGATE(idt[1],0,GD_KT, (uint32_t)fun_ptr,0);

	fun_ptr = interrupt_2;
	SETGATE(idt[2],0,GD_KT, (uint32_t)fun_ptr,0);

	fun_ptr = interrupt_3;
	SETGATE(idt[3],0,GD_KT, (uint32_t)fun_ptr,3);

	fun_ptr = interrupt_4;
	SETGATE(idt[4],0,GD_KT, (uint32_t)fun_ptr,0);

	fun_ptr = interrupt_5;
	SETGATE(idt[5],0,GD_KT, (uint32_t)fun_ptr,0);

	fun_ptr = interrupt_6;
	SETGATE(idt[6],0,GD_KT, (uint32_t)fun_ptr,0);

	fun_ptr = interrupt_7;
	SETGATE(idt[7],0,GD_KT, (uint32_t)fun_ptr,0);

	fun_ptr = interrupt_8;
	SETGATE(idt[8],0,GD_KT, (uint32_t)fun_ptr,0);

	fun_ptr = interrupt_9;
	SETGATE(idt[9],0,GD_KT, (uint32_t)fun_ptr,0);

	fun_ptr = interrupt_10;
	SETGATE(idt[10],0,GD_KT, (uint32_t)fun_ptr,0);

	fun_ptr = interrupt_11;
	SETGATE(idt[11],0,GD_KT, (uint32_t)fun_ptr,0);

	fun_ptr = interrupt_12;
	SETGATE(idt[12],0,GD_KT, (uint32_t)fun_ptr,0);

	fun_ptr = interrupt_13;
	SETGATE(idt[13],0,GD_KT, (uint32_t)fun_ptr,0);

	fun_ptr = interrupt_14;
	SETGATE(idt[14],0,GD_KT, (uint32_t)fun_ptr,0);

	fun_ptr = interrupt_15;
	SETGATE(idt[15],0,GD_KT, (uint32_t)fun_ptr,0);

	fun_ptr = interrupt_16;
	SETGATE(idt[16],0,GD_KT, (uint32_t)fun_ptr,0);

	fun_ptr = interrupt_17;
	SETGATE(idt[17],0,GD_KT, (uint32_t)fun_ptr,0);

	fun_ptr = interrupt_18;
	SETGATE(idt[18],0,GD_KT, (uint32_t)fun_ptr,0);

	fun_ptr = interrupt_19;
	SETGATE(idt[19],0,GD_KT, (uint32_t)fun_ptr,0);

	fun_ptr = interrupt_20;
	SETGATE(idt[20],0,GD_KT, (uint32_t)fun_ptr,0);

	fun_ptr = interrupt_21;
	SETGATE(idt[21],0,GD_KT, (uint32_t)fun_ptr,0);

	fun_ptr = interrupt_22;
	SETGATE(idt[22],0,GD_KT, (uint32_t)fun_ptr,0);

	fun_ptr = interrupt_23;
	SETGATE(idt[23],0,GD_KT, (uint32_t)fun_ptr,0);

	fun_ptr = interrupt_24;
	SETGATE(idt[24],0,GD_KT, (uint32_t)fun_ptr,0);

	fun_ptr = interrupt_25;
	SETGATE(idt[25],0,GD_KT, (uint32_t)fun_ptr,0);

	fun_ptr = interrupt_26;
	SETGATE(idt[26],0,GD_KT, (uint32_t)fun_ptr,0);

	fun_ptr = interrupt_27;
	SETGATE(idt[27],0,GD_KT, (uint32_t)fun_ptr,0);

	fun_ptr = interrupt_28;
	SETGATE(idt[28],0,GD_KT, (uint32_t)fun_ptr,0);

	fun_ptr = interrupt_29;
	SETGATE(idt[29],0,GD_KT, (uint32_t)fun_ptr,0);

	fun_ptr = interrupt_30;
	SETGATE(idt[30],0,GD_KT, (uint32_t)fun_ptr,0);

	fun_ptr = interrupt_31;
	SETGATE(idt[31],0,GD_KT, (uint32_t)fun_ptr,0);
	fun_ptr = interrupt_32;
	SETGATE(idt[32],0,GD_KT, (uint32_t)fun_ptr,3);
	fun_ptr = interrupt_33;
	SETGATE(idt[33],0,GD_KT, (uint32_t)fun_ptr,3);
	fun_ptr = interrupt_34;
	SETGATE(idt[34],0,GD_KT, (uint32_t)fun_ptr,3);
	fun_ptr = interrupt_35;
	SETGATE(idt[35],0,GD_KT, (uint32_t)fun_ptr,3);
	fun_ptr = interrupt_36;
	SETGATE(idt[36],0,GD_KT, (uint32_t)fun_ptr,3);
	fun_ptr = interrupt_37;
	SETGATE(idt[37],0,GD_KT, (uint32_t)fun_ptr,3);
	fun_ptr = interrupt_38;
	SETGATE(idt[38],0,GD_KT, (uint32_t)fun_ptr,3);
	fun_ptr = interrupt_39;
	SETGATE(idt[39],0,GD_KT, (uint32_t)fun_ptr,3);
	fun_ptr = interrupt_40;
	SETGATE(idt[40],0,GD_KT, (uint32_t)fun_ptr,3);
	fun_ptr = interrupt_41;
	SETGATE(idt[41],0,GD_KT, (uint32_t)fun_ptr,3);
	fun_ptr = interrupt_42;
	SETGATE(idt[42],0,GD_KT, (uint32_t)fun_ptr,3);
	fun_ptr = interrupt_43;
	SETGATE(idt[43],0,GD_KT, (uint32_t)fun_ptr,3);
	fun_ptr = interrupt_44;
	SETGATE(idt[44],0,GD_KT, (uint32_t)fun_ptr,3);
	fun_ptr = interrupt_45;
	SETGATE(idt[45],0,GD_KT, (uint32_t)fun_ptr,3);
	fun_ptr = interrupt_46;
	SETGATE(idt[46],0,GD_KT, (uint32_t)fun_ptr,3);
	fun_ptr = interrupt_47;
	SETGATE(idt[47],0,GD_KT, (uint32_t)fun_ptr,3);


	fun_ptr = interrupt_48;
	SETGATE(idt[48],0,GD_KT, (uint32_t)fun_ptr,3);	

	// Per-CPU setup 
	trap_init_percpu();
}

// Initialize and load the per-CPU TSS and IDT
void
trap_init_percpu(void)
{
	// The example code here sets up the Task State Segment (TSS) and
	// the TSS descriptor for CPU 0. But it is incorrect if we are
	// running on other CPUs because each CPU has its own kernel stack.
	// Fix the code so that it works for all CPUs.
	//
	// Hints:
	//   - The macro "thiscpu" always refers to the current CPU's
	//     struct CpuInfo;
	//   - The ID of the current CPU is given by cpunum() or
	//     thiscpu->cpu_id;
	//   - Use "thiscpu->cpu_ts" as the TSS for the current CPU,
	//     rather than the global "ts" variable;
	//   - Use gdt[(GD_TSS0 >> 3) + i] for CPU i's TSS descriptor;
	//   - You mapped the per-CPU kernel stacks in mem_init_mp()
	//
	// ltr sets a 'busy' flag in the TSS selector, so if you
	// accidentally load the same TSS on more than one CPU, you'll
	// get a triple fault.  If you set up an individual CPU's TSS
	// wrong, you may not get a fault until you try to return from
	// user space on that CPU.
	//
	// LAB 4: Your code here:
	
	thiscpu->cpu_ts.ts_esp0 = KSTACKTOP - (cpunum())*(KSTKSIZE + KSTKGAP);
	thiscpu->cpu_ts.ts_ss0 = GD_KD;

	gdt[(GD_TSS0 >> 3) + cpunum()] = SEG16(STS_T32A, (uint32_t) (&(thiscpu->cpu_ts)),
					sizeof(struct Taskstate) , 0);
	gdt[(GD_TSS0 >> 3) + cpunum()].sd_s = 0;

	ltr( GD_TSS0 + cpunum()*sizeof(struct Segdesc));
	
	// Setup a TSS so that we get the right stack
	// when we trap to the kernel.
	/*
	ts.ts_esp0 = KSTACKTOP;
	ts.ts_ss0 = GD_KD;

	// Initialize the TSS slot of the gdt.
	gdt[GD_TSS0 >> 3] = SEG16(STS_T32A, (uint32_t) (&ts),
					sizeof(struct Taskstate) - 1, 0);
	gdt[GD_TSS0 >> 3].sd_s = 0;

	// Load the TSS selector (like other segment selectors, the
	// bottom three bits are special; we leave them 0)
	ltr(GD_TSS0);
	*/
	// Load the IDT
	lidt(&idt_pd);
}

void
print_trapframe(struct Trapframe *tf)
{
	cprintf("TRAP frame at %p from CPU %d\n", tf, cpunum());
	print_regs(&tf->tf_regs);
	cprintf("  es   0x----%04x\n", tf->tf_es);
	cprintf("  ds   0x----%04x\n", tf->tf_ds);
	cprintf("  trap 0x%08x %s\n", tf->tf_trapno, trapname(tf->tf_trapno));
	// If this trap was a page fault that just happened
	// (so %cr2 is meaningful), print the faulting linear address.
	if (tf == last_tf && tf->tf_trapno == T_PGFLT)
		cprintf("  cr2  0x%08x\n", rcr2());
	cprintf("  err  0x%08x", tf->tf_err);
	// For page faults, print decoded fault error code:
	// U/K=fault occurred in user/kernel mode
	// W/R=a write/read caused the fault
	// PR=a protection violation caused the fault (NP=page not present).
	if (tf->tf_trapno == T_PGFLT)
		cprintf(" [%s, %s, %s]\n",
			tf->tf_err & 4 ? "user" : "kernel",
			tf->tf_err & 2 ? "write" : "read",
			tf->tf_err & 1 ? "protection" : "not-present");
	else
		cprintf("\n");
	cprintf("  eip  0x%08x\n", tf->tf_eip);
	cprintf("  cs   0x----%04x\n", tf->tf_cs);
	cprintf("  flag 0x%08x\n", tf->tf_eflags);
	if ((tf->tf_cs & 3) != 0) {
		cprintf("  esp  0x%08x\n", tf->tf_esp);
		cprintf("  ss   0x----%04x\n", tf->tf_ss);
	}
}

void
print_regs(struct PushRegs *regs)
{
	cprintf("  edi  0x%08x\n", regs->reg_edi);
	cprintf("  esi  0x%08x\n", regs->reg_esi);
	cprintf("  ebp  0x%08x\n", regs->reg_ebp);
	cprintf("  oesp 0x%08x\n", regs->reg_oesp);
	cprintf("  ebx  0x%08x\n", regs->reg_ebx);
	cprintf("  edx  0x%08x\n", regs->reg_edx);
	cprintf("  ecx  0x%08x\n", regs->reg_ecx);
	cprintf("  eax  0x%08x\n", regs->reg_eax);
}

static void
trap_dispatch(struct Trapframe *tf)
{
	// Handle spurious interrupts
	// The hardware sometimes raises these because of noise on the
	// IRQ line or other reasons. We don't care.
	if (tf->tf_trapno == IRQ_OFFSET + IRQ_SPURIOUS) {
		cprintf("Spurious interrupt on irq 7\n");
		print_trapframe(tf);
		return;
	}

	// Handle clock interrupts. Don't forget to acknowledge the
	// interrupt using lapic_eoi() before calling the scheduler!
	// LAB 4: Your code here.
	if(tf->tf_trapno == IRQ_OFFSET + IRQ_TIMER ){
		cprintf("---------------------------------context switch ------------------------------------------\n");
		lapic_eoi();
		cprintf("acknowledges timer interrupt\n");
		sched_yield();
		return ;
	}



	// Handle processor exceptions.
	// LAB 3: Your code here.
	//dispatch page fault interrupt to its handler
	//Dispatch the page faults
	if(tf->tf_trapno == 14){
		page_fault_handler(tf);
		return ;
	}
	//dipatch breakpoint interrupt ot monitor
	if(tf->tf_trapno == 3){
		monitor(tf);
		return ;
	}

	//Dispatch the system calls
	if(tf->tf_trapno == 48){
		int32_t returnValue;
		//Initially the syscall num is stored in the eax
		//Later the syscall returns by storing the return value in the eax
		//We take that value and return it.
		//Arguments are in edx ,ecx , ebx , edi , esi respectively
		returnValue = syscall((tf->tf_regs).reg_eax,(tf->tf_regs).reg_edx,(tf->tf_regs).reg_ecx,(tf->tf_regs).reg_ebx,(tf->tf_regs).reg_edi,(tf->tf_regs).reg_esi);
		(tf->tf_regs).reg_eax = returnValue;
		return;
	}

	// Unexpected trap: The user process or the kernel has a bug.
	print_trapframe(tf);
	if (tf->tf_cs == GD_KT)
		panic("unhandled trap in kernel");
	else {
		env_destroy(curenv);
		return;
	}
}

void
trap(struct Trapframe *tf)
{
	// The environment may have set DF and some versions
	// of GCC rely on DF being clear
	asm volatile("cld" ::: "cc");

	// Halt the CPU if some other CPU has called panic()
	extern char *panicstr;
	if (panicstr)
		asm volatile("hlt");

	// Re-acqurie the big kernel lock if we were halted in
	// sched_yield()
	if (xchg(&thiscpu->cpu_status, CPU_STARTED) == CPU_HALTED)
		lock_kernel();
	// Check that interrupts are disabled.  If this assertion
	// fails, DO NOT be tempted to fix it by inserting a "cli" in
	// the interrupt path.
	assert(!(read_eflags() & FL_IF));
	//cprintf("success assertion\n");

	if ((tf->tf_cs & 3) == 3) {
		// Trapped from user mode.
		// Acquire the big kernel lock before doing any
		// serious kernel work.
		// LAB 4: Your code here.

		//locks the kernel before jumping into kernel mode. 
		lock_kernel();
		assert(curenv);

		// Garbage collect if current enviroment is a zombie
		if (curenv->env_status == ENV_DYING) {
			env_free(curenv);
			curenv = NULL;
			sched_yield();
		}

		// Copy trap frame (which is currently on the stack)
		// into 'curenv->env_tf', so that running the environment
		// will restart at the trap point.
		curenv->env_tf = *tf;
		// The trapframe on the stack should be ignored from here on.
		tf = &curenv->env_tf;
	}

	// Record that tf is the last real trapframe so
	// print_trapframe can print some additional information.
	last_tf = tf;

	// Dispatch based on what type of trap occurred
	trap_dispatch(tf);

	// If we made it to this point, then no other environment was
	// scheduled, so we should return to the current environment
	// if doing so makes sense.
	if (curenv && curenv->env_status == ENV_RUNNING)
		env_run(curenv);
	else
		sched_yield();
}


void
page_fault_handler(struct Trapframe *tf)
{
	uint32_t fault_va;

	// Read processor's CR2 register to find the faulting address
	fault_va = rcr2();

	// Handle kernel-mode page faults.

	// LAB 3: Your code here.

	if((tf->tf_cs & 0x3) == 0 ){
		panic("Kernel causing the page fault");
	}

	// We've already handled kernel-mode exceptions, so if we get here,
	// the page fault happened in user mode.

	// Call the environment's page fault upcall, if one exists.  Set up a
	// page fault stack frame on the user exception stack (below
	// UXSTACKTOP), then branch to curenv->env_pgfault_upcall.
	//
	// The page fault upcall might cause another page fault, in which case
	// we branch to the page fault upcall recursively, pushing another
	// page fault stack frame on top of the user exception stack.
	//
	// The trap handler needs one word of scratch space at the top of the
	// trap-time stack in order to return.  In the non-recursive case, we
	// don't have to worry about this because the top of the regular user
	// stack is free.  In the recursive case, this means we have to leave
	// an extra word between the current top of the exception stack and
	// the new stack frame because the exception stack _is_ the trap-time
	// stack.
	//
	// If there's no page fault upcall, the environment didn't allocate a
	// page for its exception stack or can't write to it, or the exception
	// stack overflows, then destroy the environment that caused the fault.
	// Note that the grade script assumes you will first check for the page
	// fault upcall and print the "user fault va" message below if there is
	// none.  The remaining three checks can be combined into a single test.
	//
	// Hints:
	//   user_mem_assert() and env_run() are useful here.
	//   To change what the user environment runs, modify 'curenv->env_tf'
	//   (the 'tf' variable points at 'curenv->env_tf').

	// LAB 4: Your code here.

	// Destroy the environment that caused the fault.
	if(curenv->env_pgfault_upcall == NULL){
		cprintf("[%08x] user fault va %08x ip %08x\n",
			curenv->env_id, fault_va, tf->tf_eip);
		print_trapframe(tf);
		env_destroy(curenv);
	}
	
	
	struct UTrapframe trapBuild;
	//copy the faulted va
	trapBuild.utf_fault_va = fault_va;
	//copy errorcode from trapframe
	trapBuild.utf_err = tf->tf_err;
	//copies trap-time regs, eip, eflags, esp from trapframe
	trapBuild.utf_regs = tf->tf_regs;
	trapBuild.utf_eip = tf->tf_eip;
	trapBuild.utf_eflags = tf->tf_eflags;
	trapBuild.utf_esp = tf->tf_esp;

	//recursively in this
	if (tf->tf_esp <= UXSTACKTOP-1  && tf->tf_esp >= UXSTACKTOP-PGSIZE){
		if(tf->tf_esp < UXSTACKTOP-PGSIZE){
			cprintf("Destroyed due to exception stack overflow.");
			cprintf("[%08x] user fault va %08x ip %08x\n",
				curenv->env_id, fault_va, tf->tf_eip);
			print_trapframe(tf);
			env_destroy(curenv);
		}
		user_mem_assert(curenv, (void*)((tf->tf_esp) - 4 - sizeof(struct UTrapframe)), sizeof(struct UTrapframe), PTE_W|PTE_P|PTE_U );

		memcpy( (void*)((tf->tf_esp) - 4 - sizeof(struct UTrapframe)), (void*)(&trapBuild), sizeof(struct UTrapframe) );
		curenv->env_tf.tf_eip = (uintptr_t)curenv->env_pgfault_upcall;
		curenv->env_tf.tf_esp = (tf->tf_esp) - 4 - sizeof(struct UTrapframe);
		env_run(curenv);
		
	}
	//first time
	else{

		user_mem_assert(curenv, (void*)(UXSTACKTOP - sizeof(struct UTrapframe)), sizeof(struct UTrapframe), PTE_W|PTE_P|PTE_U);
		memcpy( (void*)(UXSTACKTOP - sizeof(struct UTrapframe)), (void*)(&trapBuild), sizeof(struct UTrapframe) );
		curenv->env_tf.tf_eip = (uintptr_t)curenv->env_pgfault_upcall;
		curenv->env_tf.tf_esp = UXSTACKTOP - sizeof(struct UTrapframe);
		env_run(curenv);	
	}

}

