#ifndef _MODULE_LOADER_H_
#define _MODULE_LOADER_H_

/* Loads all the modules given in target on payloadStart */
void * loadModules(void * payloadStart, void ** moduleTargetAddress);

#endif /* _MODULE_LOADER_H_ */