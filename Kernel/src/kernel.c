// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <lib.h>
#include <moduleLoader.h>
#include <moduleAddresses.h>
#include <idtLoader.h>
#include <defs.h>
#include <videoDriver.h>
#include <console.h>
#include <memoryManager.h>
//#include <timelib.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;
/* Total amount of memory 512MB */
static const uint64_t totalBytes = 0x20000000;

typedef int (*EntryPoint)();

void goToUserland() {
	((EntryPoint)shellModuleAddress)();
}

void clearBSS(void * bssAddress, uint64_t bssSize) {
	memset(bssAddress, 0, bssSize);
}

void * getStackBase() {
	return (void*) (
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary() {
	char buffer[10];

	cpu_vendor(buffer);

	/* Fills with addresses where copies the modules */
	void * moduleAddresses[] = {
		shellModuleAddress,
		processAModuleAddress,
		processBModuleAddress,
		idleModuleAddress,
		sleepModuleAddress,
		loopModuleAddress,
		catModuleAddress,
		wcModuleAddress,
		filterModuleAddress,
		phyloModuleAddress
	};

	/* Gets modules final address */
	void * endOfModules = loadModules(&endOfKernelBinary, moduleAddresses);
	clearBSS(&bss, &endOfKernel - &bss);
	void * startOfMem = (void *)(((uint8_t *) endOfModules + PageSize - (uint64_t) endOfModules % PageSize));

	/* Creates memory manager at the end of the modules loaded */
	create_manager(startOfMem, totalBytes);

	init_video_driver();
  	init_console();
	
	load_idt();

	return getStackBase();
}

int main() {
	mm_print_status();
    // goToUserland();
	return 0;
}