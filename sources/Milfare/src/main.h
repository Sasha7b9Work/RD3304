#pragma once

#define MAIN_DEBUG 1


#if MAIN_DEBUG

//message has to include \n or call fflush()
#define MAIN_DEBUG_TRACE(module, message)  \
	if(MAIN_DEBUG_##module <= MAIN_DEBUG_LEVEL_TRACE) { printf message; }

//message has to include \n or call fflush()
#define MAIN_DEBUG_ERR(module, message)  \
	if(MAIN_DEBUG_##module <= MAIN_DEBUG_LEVEL_ERR) { printf message; }

//redirect to:
#define MAIN_DEBUG_TO_UART6			0
#define MAIN_DEBUG_TO_USB			1
//debug level
#define MAIN_DEBUG_LEVEL_ERR		2
#define MAIN_DEBUG_LEVEL_TRACE		1

#define MAIN_DEBUG_RFID				MAIN_DEBUG_LEVEL_ERR
#define MAIN_DEBUG_MFRC				MAIN_DEBUG_LEVEL_ERR

#else

#define MAIN_DEBUG_TRACE(module, message)
#define MAIN_DEBUG_ERR(module, message)

#endif
