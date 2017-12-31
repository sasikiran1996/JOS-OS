// implement fork from user space

#include <inc/string.h>
#include <inc/lib.h>

// PTE_COW marks copy-on-write page table entries.
// It is one of the bits explicitly allocated to user processes (PTE_AVAIL).
#define PTE_COW		0x800

// Assembly language pgfault entrypoint defined in lib/pfentry.S.
//extern void _pgfault_upcall(void);

//
// Custom page fault handler - if faulting page is copy-on-write,
// map in our own private writable copy.
//
static void
pgfault(struct UTrapframe *utf)
{
	void *addr = (void *) utf->utf_fault_va;
	uint32_t err = utf->utf_err;
	int r;

	// Check that the faulting access was (1) a write, and (2) to a
	// copy-on-write page.  If not, panic.
	// Hint:
	//   Use the read-only page table mappings at uvpt
	//   (see <inc/memlayout.h>).

	// LAB 4: Your code here.

	//doing a page lookup
	
	pde_t pde;
	pde = uvpd[PDX((uint32_t)addr)];
	
	if( !(pde & PTE_P) ){
		panic("Page directory entry was not present for address %x\n",(uint32_t)pde);
	}
	
	pte_t pte;
	pte = uvpt[PGNUM((uint32_t)addr)];
	if( !(pte & PTE_P) ){
		panic("Page table entry was not present\n");
	}
	



	// Allocate a new page, map it at a temporary location (PFTEMP),
	// copy the data from the old page to the new page, then move the new
	// page to the old page's address.
	// Hint:
	//   You should make three system calls.

	// LAB 4: Your code here.
	if( (err & PTE_W) && (pte & PTE_COW) ){
		
		addr = ROUNDDOWN(addr,PGSIZE);
		//Allocate a new page for the process.If error then panic
		if( sys_page_alloc(sys_getenvid(), (void*)PFTEMP, PTE_P|PTE_W|PTE_U) < 0 ) {
			panic("Error in sys_page_alloc called in pgfault\n");
		}
		
		// Copy the old page contents to the new page just created.
		memcpy((void*)PFTEMP , addr , PGSIZE);

		// Re-Map the created page to the old va .
		sys_page_map(sys_getenvid(), (void *)PFTEMP, sys_getenvid(), addr,  (PTE_P|PTE_W|PTE_U));

		//unmap old mapping
		sys_page_unmap(sys_getenvid(), (void *)PFTEMP);
		
	}
	else{
		panic("Either not write error or not cow page\n");
	}

	//panic("pgfault not implemented");
}

//
// Map our virtual page pn (address pn*PGSIZE) into the target envid
// at the same virtual address.  If the page is writable or copy-on-write,
// the new mapping must be created copy-on-write, and then our mapping must be
// marked copy-on-write as well.  (Exercise: Why do we need to mark ours
// copy-on-write again if it was already copy-on-write at the beginning of
// this function?)
//
// Returns: 0 on success, < 0 on error.
// It is also OK to panic on error.
//
static int
duppage(envid_t envid, unsigned pn)
{
	int r;
	// LAB 4: Your code here.
	//doing a page lookup
	
	pte_t pte;
	pte = uvpt[pn];
	void *addr = (void*) (pn*PGSIZE);
	if( (pte & PTE_W) || (pte & PTE_COW) ){
		
		//new mapping must be made COW
		r = sys_page_map(sys_getenvid(), addr, envid, addr, PTE_COW|PTE_P|PTE_U);
		if ( r < 0 ){
			return r; 
		}
		//make our own copy COW
		r = sys_page_map(sys_getenvid(), addr, sys_getenvid(), addr, PTE_COW|PTE_P|PTE_U);
		if ( r < 0 ){
			return r; 
		}
		
	}
	else{
		r = sys_page_map(sys_getenvid(), addr, envid, addr, PTE_P|PTE_U);
		if ( r < 0 ){
			return r; 
		}	
	}
	

	//panic("duppage not implemented");
	return 0;
}

//
// User-level fork with copy-on-write.
// Set up our page fault handler appropriately.
// Create a child.
// Copy our address space and page fault handler setup to the child.
// Then mark the child as runnable and return.
//
// Returns: child's envid to the parent, 0 to the child, < 0 on error.
// It is also OK to panic on error.
//
// Hint:
//   Use uvpd, uvpt, and duppage.
//   Remember to fix "thisenv" in the child process.
//   Neither user exception stack should ever be marked copy-on-write,
//   so you must allocate a new page for the child's user exception stack.
//
envid_t
fork(void)
{
	// LAB 4: Your code here.
	set_pgfault_handler(pgfault);
	envid_t childID = sys_exofork();
	cprintf("Came out \n");
	if (childID == 0) {
		cprintf("Just going in\n");
		thisenv = &envs[ENVX(sys_getenvid())];
		cprintf("Completed\n");
		return 0;
	}
	int i = 0 ; 
	int r;

	for(i=0 ; i<(USTACKTOP/PGSIZE ) ; ++i){

		//doing a page lookup
		pde_t pde;
		pde = uvpd[PDX((uint32_t)(i*PGSIZE))];
		
		//the page directory entry was not present
		if( !(pde & PTE_P) ){
			continue;
		}
		
		pte_t pte;
		pte = uvpt[PGNUM((uint32_t)(i*PGSIZE))];
		
		if( (pte & PTE_W) || (pte & PTE_COW) ){
			//cprintf("Dupping cow Page\n");
			//duppage for writable or cow pages
			duppage(childID , i );
			//cprintf("Dupped COW Page\n");
		}
		else{
			//if both present and user
			if((pte & PTE_P) && (pte & PTE_U)){
				//cprintf("Dupping non cow Page\n");
				duppage(childID, i);
				//cprintf("Dupped non-COW Page\n");			
			}
		}
	}
	//cprintf("fjfklsjdfakl\n");
	if (sys_page_alloc(childID, (void*)(UTOP - PGSIZE), PTE_P|PTE_U|PTE_W) < 0 ){
		//cprintf("tadaaa\n");
		return -1 ;
	}
	//cprintf("allocated UXSTACKTOP");
	extern void _pgfault_upcall();
	sys_env_set_pgfault_upcall(childID, (void*)_pgfault_upcall);

	//set child status to runnable
	r = sys_env_set_status(childID, ENV_RUNNABLE);
	cprintf("Made runnable\n");
	if(r < 0){
		return r;
	}
	return childID;
	//panic("fork not implemented");
}

// Challenge!
int
sfork(void)
{
	panic("sfork not implemented");
	return -E_INVAL;
}
