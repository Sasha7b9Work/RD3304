#ifndef FMW_H
#define FMW_H

#include <stdint.h>

extern uint8_t fileHeader[ 32 ];

uint32_t countProgramCRC( void );


uint8_t CheckMainProgControlSum( void );

int check_file( void );

void read_header( void );

int check_header( void );

uint32_t get_file_program_version( void );

uint32_t get_file_program_sig( void );

uint32_t get_file_coded_len( void );

#endif
