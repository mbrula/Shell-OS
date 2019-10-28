#include <stdint.h>
#include <strings.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <idtLoader.h>
#include <defs.h>
#include <videoDriver.h>
#include <console.h>
#include <moduleAddresses.h>
//#include <time.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;


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

	loadModules(&endOfKernelBinary, moduleAddresses);

	clearBSS(&bss, &endOfKernel - &bss);

	init_video_driver();
  	init_console();
	
	load_idt();

	return getStackBase();
}

int main() {
    // ncClear();
    goToUserland();
	// ncNewline();

	// int i = 0;
	// char car;
    // while (i < 10) {
    //     if ((car = read_character()) != 0){
	// 		ncPrintChar(car);
	// 		i++;
	// 	}
    // }
	return 0;
}