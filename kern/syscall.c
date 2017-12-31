/* See COPYRIGHT for copyright information. */

#include <inc/x86.h>
#include <inc/error.h>
#include <inc/string.h>
#include <inc/assert.h>

#include <kern/env.h>
#include <kern/pmap.h>
#include <kern/trap.h>
#include <kern/syscall.h>
#include <kern/console.h>
#include <kern/sched.h>

// Print a string to the system console.
// The string is exactly 'len' characters long.
// Destroys the environment on memory errors.
static void
sys_cputs(const char *s, size_t len)
{
	// Check that the user has permission to read memory [s, s+len).
	// Destroy the environment if not.

	// LAB 3: Your code here.
	
	//Do a permission check
	user_mem_assert(curenv, (void*)s, len, PTE_U);
	
	// Print the string supplied by the user.
	cprintf("%.*s", len, s);
}

// Read a character from the system console without blocking.
// Returns the character, or 0 if there is no input waiting.
static int
sys_cgetc(void)
{
	return cons_getc();
}

// Returns the current environment's envid.
static envid_t
sys_getenvid(void)
{
	return curenv->env_id;
}

// Destroy a given environment (possibly the currently running environment).
//
// Returns 0 on success, < 0 on error.  Errors are:
//	-E_BAD_ENV if environment envid doesn't currently exist,
//		or the caller doesn't have permission to change envid.
static int
sys_env_destroy(envid_t envid)
{
	int r;
	struct Env *e;

	if ((r = envid2env(envid, &e, 1)) < 0)
		return r;
	if (e == curenv)
		cprintf("[%08x] exiting gracefully\n", curenv->env_id);
	else
		cprintf("[%08x] destroying %08x\n", curenv->env_id, e->env_id);
	env_destroy(e);
	return 0;
}

// Deschedule current environment and pick a different one to run.
static void
sys_yield(void)
{
	sched_yield();
}

// Allocate a new environment.
// Returns envid of new environment, or < 0 on error.  Errors are:
//	-E_NO_FREE_ENV if no free environment is available.
//	-E_NO_MEM on memory exhaustion.
static envid_t
sys_exofork(void)
{
	// Create the new environment with env_alloc(), from kern/env.c.
	// It should be left as env_alloc created it, except that
	// status is set to ENV_NOT_RUNNABLE, and the register set is copied
	// from the current environment -- but tweaked so sys_exofork
	// will appear to return 0.

	// LAB 4: Your code here.
	cprintf("came here\n");
	
	struct Env* newProc ;
	int error_code ;
	//A new Enviroinment is created using the env_alloc function.
	error_code = env_alloc(&newProc ,curenv->env_id ) ;  
	// Any error code is returned as it is.
	if( error_code < 0 ){
		
		return error_code ;	
	}
	// The new process has a status of ENV_NOT _RUNNABLE
	newProc->env_status = ENV_NOT_RUNNABLE ;
	// The trapframe is copied as it is ... 
	(newProc->env_tf) = curenv->env_tf ; 
	// The eax is twaeked to contain the value 0 
	(newProc->env_tf).tf_regs.reg_eax = 0 ;
	return newProc->env_id ;
	
}

// Set envid's env_status to status, which must be ENV_RUNNABLE
// or ENV_NOT_RUNNABLE.
//
// Returns 0 on success, < 0 on error.  Errors are:
//	-E_BAD_ENV if environment envid doesn't currently exist,
//		or the caller doesn't have permission to change envid.
//	-E_INVAL if status is not a valid status for an environment.
static int
sys_env_set_status(envid_t envid, int status)
{
	// Hint: Use the 'envid2env' function from kern/env.c to translate an
	// envid to a struct Env.
	// You should set envid2env's third argument to 1, which will
	// check whether the current environment has permission to set
	// envid's status.

	// LAB 4: Your code here.

	struct Env* newProc ; 
	// Inc case the enviroinment doesnot exist , or if the
	// calling env does not have permission to query this .. error cose is returned!
	if (envid2env(envid , &newProc , 1) < 0){
		return -E_BAD_ENV ;
	//panic("sys_env_set_status not implemented");
	}
	// Else if the status is not either of the following , an error code is returned.
	if(status != ENV_NOT_RUNNABLE && status != ENV_RUNNABLE){
		return -E_INVAL ;
	}
	//assign the status to the enviroinment.
	newProc->env_status = status ;
	return 0 ; 
}

// Set the page fault upcall for 'envid' by modifying the corresponding struct
// Env's 'env_pgfault_upcall' field.  When 'envid' causes a page fault, the
// kernel will push a fault record onto the exception stack, then branch to
// 'func'.
//
// Returns 0 on success, < 0 on error.  Errors are:
//	-E_BAD_ENV if environment envid doesn't currently exist,
//		or the caller doesn't have permission to change envid.
static int
sys_env_set_pgfault_upcall(envid_t envid, void *func)
{
	// LAB 4: Your code here.
	struct Env* envNow  ;
	cprintf("envid:%d\n",envid);
	//Check if the envid is proper and has permisssions.
	if( envid2env(envid, &envNow, 1) < 0){
		return -E_BAD_ENV;
	}
	// Upcall entry is assigned to the func pointer ... 
	cprintf("Here it is : %u\n" , func);
	envNow->env_pgfault_upcall = func ;
	return 0 ;

	//panic("sys_env_set_pgfault_upcall not implemented");
}

// Allocate a page of memory and map it at 'va' with permission
// 'perm' in the address space of 'envid'.
// The page's contents are set to 0.
// If a page is already mapped at 'va', that page is unmapped as a
// side effect.
//
// perm -- PTE_U | PTE_P must be set, PTE_AVAIL | PTE_W may or may not be set,
//         but no other bits may be set.  See PTE_SYSCALL in inc/mmu.h.
//
// Return 0 on success, < 0 on error.  Errors are:
//	-E_BAD_ENV if environment envid doesn't currently exist,
//		or the caller doesn't have permission to change envid.
//	-E_INVAL if va >= UTOP, or va is not page-aligned.
//	-E_INVAL if perm is inappropriate (see above).
//	-E_NO_MEM if there's no memory to allocate the new page,
//		or to allocate any necessary page tables.
static int
sys_page_alloc(envid_t envid, void *va, int perm)
{
	// Hint: This function is a wrapper around page_alloc() and
	//   page_insert() from kern/pmap.c.
	//   Most of the new code you write should be to check the
	//   parameters for correctness.
	//   If page_insert() fails, remember to free the page you
	//   allocated!

	// LAB 4: Your code here.
	//get the given env
	cprintf("started\n");
	struct Env* givenEnv;
	if( envid2env(envid, &givenEnv, true) < 0){
		return -E_BAD_ENV;
	}
	cprintf("1 ........\n");
	//allocate a page
	struct PageInfo* newpage = page_alloc(ALLOC_ZERO);
	cprintf("2 ........\n");

	//if new page was not allocated
	if (newpage == NULL){
		return -E_NO_MEM ;
	}
	//PTE_P & PTE_U must be present
	if(!(perm & PTE_U) || !(perm & PTE_P)){
		return -E_INVAL ; 
	}
	//some other bits were set
	if( (perm & (~PTE_AVAIL & ~PTE_W & ~PTE_U & ~PTE_P)) != 0){
		return -E_INVAL;
	}
	cprintf("3 ........\n");

	//va check
	if((uint32_t)va >= UTOP || (uint32_t)va % PGSIZE != 0){
		return -E_INVAL;
	}
	cprintf("4 ........\n");

	//page insert fails
	if( page_insert(givenEnv->env_pgdir, newpage, va, perm) < 0 ){
		//free the allocated page
		page_free(newpage);
		return -E_NO_MEM;
	}

	cprintf("ended\n");
	return 0;
	//panic("sys_page_alloc not implemented");
}

// Map the page of memory at 'srcva' in srcenvid's address space
// at 'dstva' in dstenvid's address space with permission 'perm'.
// Perm has the same restrictions as in sys_page_alloc, except
// that it also must not grant write access to a read-only
// page.
//
// Return 0 on success, < 0 on error.  Errors are:
//	-E_BAD_ENV if srcenvid and/or dstenvid doesn't currently exist,
//		or the caller doesn't have permission to change one of them.
//	-E_INVAL if srcva >= UTOP or srcva is not page-aligned,
//		or dstva >= UTOP or dstva is not page-aligned.
//	-E_INVAL is srcva is not mapped in srcenvid's address space.
//	-E_INVAL if perm is inappropriate (see sys_page_alloc).
//	-E_INVAL if (perm & PTE_W), but srcva is read-only in srcenvid's
//		address space.
//	-E_NO_MEM if there's no memory to allocate any necessary page tables.
static int
sys_page_map(envid_t srcenvid, void *srcva,
	     envid_t dstenvid, void *dstva, int perm)
{
	// Hint: This function is a wrapper around page_lookup() and
	//   page_insert() from kern/pmap.c.
	//   Again, most of the new code you write should be to check the
	//   parameters for correctness.
	//   Use the third argument to page_lookup() to
	//   check the current permissions on the page.

	// LAB 4: Your code here.
	//check for src and dst environments
	struct Env* srcEnv;
	if( envid2env(srcenvid, &srcEnv, true) < 0){
		return -E_BAD_ENV;
	}
	struct Env* dstEnv;
	if( envid2env(dstenvid, &dstEnv, true) < 0){
		return -E_BAD_ENV;
	}
	//va check
	if((uint32_t)srcva >= UTOP || (uint32_t)srcva % PGSIZE != 0){
		return -E_INVAL;
	}
	if((uint32_t)dstva >= UTOP || (uint32_t)dstva % PGSIZE != 0){
		return -E_INVAL;
	}
	struct PageInfo* srcPage;
	pte_t* pageEntry;
	srcPage = page_lookup(srcEnv->env_pgdir, srcva, &pageEntry);
	if(srcPage == NULL){
		return -E_INVAL;
	}
	//PTE_P & PTE_U must be present
	if(!(perm & PTE_U) || !(perm & PTE_P)){
		return -E_INVAL ; 
	}
	//some other bits were set
	if( (perm & (~PTE_AVAIL & ~PTE_W & ~PTE_U & ~PTE_P)) != 0){
		return -E_INVAL;
	}
	//src is read only but perm has PTE_W
	if( (perm & PTE_W) && !((*pageEntry) & PTE_W) ){
		return -E_INVAL;
	}
	//page insert fails
	if( page_insert(dstEnv->env_pgdir, srcPage, dstva, perm) < 0 ){
		return -E_NO_MEM;
	}
	return 0;
	//panic("sys_page_map not implemented");
}

// Unmap the page of memory at 'va' in the address space of 'envid'.
// If no page is mapped, the function silently succeeds.
//
// Return 0 on success, < 0 on error.  Errors are:
//	-E_BAD_ENV if environment envid doesn't currently exist,
//		or the caller doesn't have permission to change envid.
//	-E_INVAL if va >= UTOP, or va is not page-aligned.
static int
sys_page_unmap(envid_t envid, void *va)
{
	// Hint: This function is a wrapper around page_remove().

	// LAB 4: Your code here.
	struct Env* envNow ;
	if (envid2env(envid , &envNow , 1) < 0){
		return -E_BAD_ENV ;	
	}
	if((uint32_t)va>= UTOP){
		return -E_INVAL ;
	}
	if((uint32_t)va %PGSIZE != 0){
		return -E_INVAL ;
	}

	page_remove(envNow->env_pgdir , va);
	return 0 ;


	//panic("sys_page_unmap not implemented");
}

// Try to send 'value' to the target env 'envid'.
// If srcva < UTOP, then also send page currently mapped at 'srcva',
// so that receiver gets a duplicate mapping of the same page.
//
// The send fails with a return value of -E_IPC_NOT_RECV if the
// target is not blocked, waiting for an IPC.
//
// The send also can fail for the other reasons listed below.
//
// Otherwise, the send succeeds, and the target's ipc fields are
// updated as follows:
//    env_ipc_recving is set to 0 to block future sends;
//    env_ipc_from is set to the sending envid;
//    env_ipc_value is set to the 'value' parameter;
//    env_ipc_perm is set to 'perm' if a page was transferred, 0 otherwise.
// The target environment is marked runnable again, returning 0
// from the paused sys_ipc_recv system call.  (Hint: does the
// sys_ipc_recv function ever actually return?)
//
// If the sender wants to send a page but the receiver isn't asking for one,
// then no page mapping is transferred, but no error occurs.
// The ipc only happens when no errors occur.
//
// Returns 0 on success, < 0 on error.
// Errors are:
//	-E_BAD_ENV if environment envid doesn't currently exist.
//		(No need to check permissions.)
//	-E_IPC_NOT_RECV if envid is not currently blocked in sys_ipc_recv,
//		or another environment managed to send first.
//	-E_INVAL if srcva < UTOP but srcva is not page-aligned.
//	-E_INVAL if srcva < UTOP and perm is inappropriate
//		(see sys_page_alloc).
//	-E_INVAL if srcva < UTOP but srcva is not mapped in the caller's
//		address space.
//	-E_INVAL if (perm & PTE_W), but srcva is read-only in the
//		current environment's address space.
//	-E_NO_MEM if there's not enough memory to map srcva in envid's
//		address space.
static int
sys_ipc_try_send(envid_t envid, uint32_t value, void *srcva, unsigned perm)
{
	// LAB 4: Your code here.
	cprintf("trying to send to id: %d\n",envid);
	struct Env* recenv;
	int r;
	r = envid2env(envid, &recenv, 0);
	if(r < 0){
		return r;
	}
	cprintf("recenv:env_ipc_recving is %d\n",recenv->env_ipc_recving);
	if( (recenv->env_ipc_recving) ){
		
		if(((uint32_t)srcva < UTOP) && ((uint32_t)srcva % PGSIZE != 0)){
			return -E_INVAL;
		}
		//receiver wants page mapping and sender wants to send it
		if((uint32_t)(recenv->env_ipc_dstva) < UTOP && (uint32_t)srcva < UTOP){
			
			//check for permissions as in sys_page_alloc()
			//PTE_P & PTE_U must be present
			//cprintf("1+++++++");
			if(!(perm & PTE_U) || !(perm & PTE_P)){
				return -E_INVAL ; 
			}
			//cprintf("2..........");
			
			//some other bits were set
			if( (perm & (~PTE_AVAIL & ~PTE_W & ~PTE_U & ~PTE_P)) != 0){
				return -E_INVAL;
			}
			//cprintf("3..........");
			
			pte_t* pte_store;
			struct PageInfo* srcPage;
			srcPage = page_lookup(curenv->env_pgdir, srcva, &pte_store);
			if(srcPage == NULL){
				return -E_INVAL;
			}
			//cprintf("4..........");
			
			if((perm & PTE_W) && !((*pte_store) & PTE_W) ){
				return -E_INVAL;
			}
			//cprintf("5..........");
			
			r = page_insert(recenv->env_pgdir, srcPage, recenv->env_ipc_dstva, perm);
			//cprintf("6..........");
			
			if(r < 0){
				return r;
			}
			recenv->env_ipc_perm = perm;	
		}
		//no page transfer needed
		else{
			recenv->env_ipc_perm = 0;
		}
		recenv->env_ipc_recving = 0;
		recenv->env_ipc_from = curenv->env_id;
		recenv->env_ipc_value = value;
		recenv->env_tf.tf_regs.reg_eax = 0;
		recenv->env_status = ENV_RUNNABLE;
	}
	else{
		cprintf("Envi destination was not receiving\n");	
		return -E_IPC_NOT_RECV;
	}
	cprintf("Successful");
	return 0;
	//panic("sys_ipc_try_send not implemented");
}

// Block until a value is ready.  Record that you want to receive
// using the env_ipc_recving and env_ipc_dstva fields of struct Env,
// mark yourself not runnable, and then give up the CPU.
//
// If 'dstva' is < UTOP, then you are willing to receive a page of data.
// 'dstva' is the virtual address at which the sent page should be mapped.
//
// This function only returns on error, but the system call will eventually
// return 0 on success.
// Return < 0 on error.  Errors are:
//	-E_INVAL if dstva < UTOP but dstva is not page-aligned.
static int
sys_ipc_recv(void *dstva)
{
	// LAB 4: Your code here.
	//set that you want to receive and set dstva if valid
	if(((uint32_t)dstva < UTOP) && ((uint32_t)dstva % PGSIZE != 0)){
		return -E_INVAL; 
	}
	curenv->env_ipc_dstva = dstva;
	curenv->env_ipc_recving = 1;
	curenv->env_status = ENV_NOT_RUNNABLE;
	cprintf("Given up at receiver\n");
	//give up on CPU
	
	//sched_yield();
	//panic("sys_ipc_recv not implemented");
	return 0;
}

// Dispatches to the correct kernel function, passing the arguments.
int32_t
syscall(uint32_t syscallno, uint32_t a1, uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5)
{
	// Call the function corresponding to the 'syscallno' parameter.
	// Return any appropriate return value.
	// LAB 3: Your code here.

	//panic("syscall not implemented");
	//cprintf("Arguments are 1: %x\n 2: %x\n 3: %x\n 4: %x\n 5: %x\n",a1,a2,a3,a4,a5);
	//switch the syscallno
	switch (syscallno) {
		//Call the corresponding function respectively...
		//The arguments are as followed in the lib/syscall.c 
		case SYS_cputs : 
			sys_cputs((const char *)a1, (size_t)a2);
			return 0;

		case SYS_cgetc : 
			return sys_cgetc() ;

		case SYS_env_destroy :
			return sys_env_destroy((envid_t) a1);

		case SYS_getenvid : 
			return  sys_getenvid();

		case SYS_yield :
			sys_yield();
			return 0; 

		case SYS_exofork :
			return sys_exofork();
		
		case SYS_env_set_status :
			return sys_env_set_status((envid_t)a1 , (int) a2);
		
		case SYS_page_alloc :
			return sys_page_alloc((envid_t) a1, (void *)a2, (int) a3);
		
		case SYS_page_map :
			return sys_page_map((envid_t) a1, (void *)a2,
	     (envid_t) a3, (void *)a4, (int )a5);
		
		case SYS_page_unmap :
			return sys_page_unmap((envid_t) a1, (void *)a2);

		case SYS_env_set_pgfault_upcall : 
			return sys_env_set_pgfault_upcall( (envid_t)a1  , (void*)a2);

		case SYS_ipc_recv:
			return sys_ipc_recv((void*)a1);

		case SYS_ipc_try_send:
			return sys_ipc_try_send(a1, a2, (void*)a3, a4);


		default:
			return -E_NO_SYS;
	}
}

