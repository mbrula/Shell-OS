// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <lib.h>

#include <moduleLoader.h>

static void * loadModule(uint8_t ** module, void * targetModuleAddress);
static uint32_t readUint32(uint8_t ** address);

/* Loads all the modules given in target on payloadStart */
void * loadModules(void * payloadStart, void ** targetModuleAddress) {
	uint8_t * currentModule = (uint8_t*)payloadStart;
	uint32_t moduleCount = readUint32(&currentModule);
	void * endOfModules;

	for (uint64_t i = 0; i < moduleCount; i++)
		endOfModules = loadModule(&currentModule, targetModuleAddress[i]);

	return endOfModules;
}

/* Loads one module */
static void * loadModule(uint8_t ** module, void * targetModuleAddress) {
	uint32_t moduleSize = readUint32(module);
	memcpy(targetModuleAddress, *module, moduleSize);
	*module += moduleSize;
	return (void *)((uint8_t*) targetModuleAddress + moduleSize);
}

/* Gets the module size */
static uint32_t readUint32(uint8_t ** address) {
	uint32_t result = *(uint32_t*)(*address);
	*address += sizeof(uint32_t);
	return result;
}
