#ifndef _ISO14443P4_H
#define _ISO14443P4_H

#include "iso14443p3.h"

class TIso14443p4_Card: public TIso14443p3_Card
{
 public:
   TIso14443p4_Card( TCardAnalofInterface* maf )
     :TIso14443p3_Card( maf )
     {
     }
   void init_T_CL() const
     {
      blockNumber =0;
     } 
   
   int ISO14443p4_RATS(  ) const;
   //uint8_t  defineType() const; 
 protected:
    mutable uint8_t blockNumber; // счетчик, нужен для формирования младшего бита в байте PCB протокола T=CL
    int transceiveTCL( uint8_t *tx_buf, int tx_byte_num ) const;
};


#endif
