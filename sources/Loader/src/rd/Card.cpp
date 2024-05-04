#include <string.h>

#include "scmRTOS/Common/scmRTOS.h"

//#include "../Hardware/CLRC663/hrd_CLRC663.h"

#include "Hardware/hrd_LED.h"

#include "iso14443p3.h"
#include "iso14443p4.h"
#include "MifarePlus.h"
#include "Card.h"
#include "Main/CurrentState.h"

#include "Main/Personalisation.h"

////////////
/*
int ISO14443p3a_WakeUpA( TIso14443p3_Card *card )
{
// int rx_len = ISO14443p3a_REQA( card );
 int rx_len = card->doREQA(  );
 if( rx_len )
//   return ISO14443p3a_Anticollision( card );
   return card->Anticollision(  );
 else
  return 0;  
}  
*/

#define BIT1(p) ((p)&(1<<0))
#define BIT2(p) ((p)&(1<<1))
#define BIT3(p) ((p)&(1<<2))
#define BIT4(p) ((p)&(1<<3))
#define BIT5(p) ((p)&(1<<4))
#define BIT6(p) ((p)&(1<<5))
#define BIT7(p) ((p)&(1<<6))
#define BIT8(p) ((p)&(1<<7)) 

#if 0
int checkCardType( TIso14443p4_Card *card )
{
 int res = 0; 

 
// uint8_t historical_bytes[8];
 
 const static uint8_t mps[7]={0xC1,0x05,0x2F,0x2F,0x00,0x35,0xC7};
 const static uint8_t mpx[7]={0xC1,0x05,0x2F,0x2F,0x01,0xBC,0xD6};
 
 if( card->ISO14443p4_RATS( /*historical_bytes*/ ) >8 )
   {
    if( !memcmp( &card->answer[5], mps,7 ) )
      {
       res = MIFARE_Plus_S_4K_SL1;
      }
     else if( !memcmp( &card->answer[5], mpx,7 ) )  
      {
       res = MIFARE_Plus_X_4K_SL1;
      }
   }    
 return res;
 /*
 uint16_t sak = card_info->sak;// << 1; // в стандарте биты считаются с 1 
 if( ! BIT2(sak)  )
   {
    if( BIT4(sak) ) 
      {
       if( BIT5(sak) ) 
         {
          if( BIT1(sak) ) 
            {
             res = MIFARE_Classic_2K;
            }
          else
            {
             if( BIT6(sak) ) 
               {
                res = SmartMX_MIFARE_4K;
               }
             else
               {
                if( ISO14443p4_RATS( historical_bytes ) )
                  {
                   const uint8_t mps[]={0xC1,0x05,0x2F,0x2F,0x00,0x35,0xC7};
                   const uint8_t mpx[]={0xC1,0x05,0x2F,0x2F,0x01,0xBC,0xD6};
                   if( !memcmp( historical_bytes, mps,7 ) )
                     {
                      res = MIFARE_Plus_S_4K_SL1;
                     }
                   else if( !memcmp( historical_bytes, mpx,7 ) )  
                     {
                      res = MIFARE_Plus_X_4K_SL1;
                     }
                   else
                     res = MIFARE_Classic_4K;   
                  }
                else
                  {
                   res = MIFARE_Classic_4K;
                  }  
               }  
            }  
         }
       else  // !BIT5(sak)
         {
          if( BIT1(sak ))
            {
             res = MIFARE_Classic_Mini;
            }
          else
            {
             if( BIT6( sak ) )
               {
                res = SmartMX_MIFARE_1K;
               } 
             else
               {
                if( ISO14443p4_RATS( historical_bytes ) )
                  {
                   const uint8_t mps[]={0xC1,0x050,0x2F,0x2F,0x00,0x35,0xC7};
                   const uint8_t mpx[]={0xC1,0x050,0x2F,0x2F,0x01,0xBC,0xD6};
                   if( !memcmp( historical_bytes, mps,7 ) )
                     {
                      res = MIFARE_Plus_S_2K_SL1;
                     }
                   else if( !memcmp( historical_bytes, mpx,7 ) )  
                     {
                      res = MIFARE_Plus_X_2K_SL1;
                     }
                   else
                     res = MIFARE_Classic_1K;   
                  }
                else
                  {
                   res = MIFARE_Classic_1K;
                  }  
               }  
            }  
         }    
      }
    else // !BIT4(sak)
      {
       if( BIT5(sak) )
         {
          if( BIT1( sak ) )
            {
             res = MIFARE_Plus_X_4K_SL2;
            }
          else
            {
             res = MIFARE_Plus_X_2K_SL2;
            }
              
         }
       else
         {
          if( BIT6(sak) )
            {
             if( ISO14443p4_RATS( historical_bytes ) )
               {
                if( (historical_bytes[0] == 0xC1) && (historical_bytes[1] == 0x05) )
                  {
                   if( (historical_bytes[4] == 0x00) && (historical_bytes[5] == 0x35) && (historical_bytes[6] == 0xC7) )
                     {//! \todo deselect virtual card
                      res = MIFARE_Plus_S_SL3;
                     }
                   else if( (historical_bytes[4] == 0x01) && (historical_bytes[5] == 0xBC) && (historical_bytes[6] == 0xD6) )  
                     {
                     //! \todo deselect virtual card
                      res = MIFARE_Plus_X_SL3;
                     }
                  }
                
               }
            }
         }  
      }  
   }
 
 
 return res;*/
}
#endif

#if 0
int DiscoveryLoop_DetectActivity( TIso14443p4_Card *card )
{ 
// Clrc663.fieldOn();
 OS::sleep(TICKS_PER_SEC/20);   

// int res = ISO14443p3a_Anticollision( card );
 int res = card->Anticollision( );
 if( res >0 )
   {
 //   currentState.setLastReaded( card->length, card->uid );
//    currentState.lastReadedID.atqa = (((uint16_t)card->atq[1])<<8)|card->atq[0];
//    currentState.lastReadedID.sak = card->sak;
    
 
 // отладочная индикация
 if( card->length == 7 )
   {

    volatile int crd_type = checkCardType( card );
    card->type = crd_type;
   
    hrd_LED_On( 0,200,0);
    OS::sleep( TICKS_PER_SEC/3 );
    hrd_LED_Off();
   } 
 /*else if( card->length == 4 )  
   {
    hrd_LED_On( 0,255,0);
    OS::sleep( TICKS_PER_SEC/3 );
    hrd_LED_Off();
   }      */
 else if(  res < 0  )  
   {
    hrd_LED_On( 255,0,0);
    OS::sleep( TICKS_PER_SEC/3 );
    hrd_LED_Off();
   }      
   }
 return res;  
}

bool DiscoveryLoop()
{
 bool res =false;
 //MifarePlus_Card card;
// while( true )  
   //{
 //OS::sleep(2);   // 5100us
  int dres = DiscoveryLoop_DetectActivity( &currentState.card );
  if( dres > 0 )
    {             
    /*        
     int r = Personalisation( 0x20, &currentState.card );       
     res = true;       
      // static uint8_t switch3_key[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3 };
     //  MifarePlus_SwitchLevel3( switch3_key );
      ///////////      
//       static const uint8_t master_key[16]  = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
     //  static const uint8_t config_key[16]  = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 };
//       int res;
      // res = MifarePlus_FirstAutentication( &card, 0x9000, master_key );
       //static const uint8_t card_uid[7] = {0x04,0x39,0x0A,0x1A,0x4F,0x5F,0x80}; 
       //memcpy( card.uid, card_uid, 7);
       //card.length = 7;
       //res = MifarePlus_Autenticate( &card, 0x9000, master_key ) ;
       //res =MifarePlus_WriteKey( &card, 0x4000, master_key );
//       res = MifarePlus_ReadKey( &card, 0x4000, config_key  );
  
       static const PersonalisationInfo pinfo[] ={
          {0x9000,{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 } }, // master 
          {0x9001,{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2 } }, // config
          {0x9003,{3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3 } }, // Level 3 switch
          {0x4000,{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 } }, // 0000 сектор 0 keyA
          {0x4002,{11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26 } }, // 0004h сектор 1
          {0x4004,{21,22,23,24,25,26,27,28,29,20,31,32,33,34,35,36 } }, // 0008h сектор 2
          {0x4006,{31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46 } }, // 0012h сектор 3
       //   {0x4008,{31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46 } }, // 0014h сектор 4
       };
      // MifarePlus_CardPersonalosation( &card, pinfo, 8 );
       
   //    MifarePlus_UpdateKey( &card, uint16_t KeyBnr, uint16_t sector, const uint8_t akey[16], uint8_t new_key[16] )

      // MifarePlus_Read( &card, 0x9001, pinfo[1].Key, 0xB003, pinfo[1].Key );       
       static uint8_t data[26];       
    //   MifarePlus_Read( &card, 0x4000,  pinfo[3].Key. 0x0000, data );
           
       MifarePlus_ReadEncrypted( &currentState.card, 0x4000,  pinfo[3].Key, 0x0001,data  );       
       static uint8_t wdata[16] = {0x01,0x02,0x03,0x04,0x05,0x44,0x33,0x22,0x11,0x00,0x01,0x02,0x03,0x04,0x05};
//       MifarePlus_WriteEncrypted( &currentState.card, 0x4000, pinfo[3].Key, 0x0001, wdata );
       MifarePlus_ReadPlain( &currentState.card, 0x4000,  pinfo[3].Key, 0x0001,data  );
       
       //    MifarePlus_Read( &card, 0x4000,  pinfo[3].Key, 0x0002, data );
       //       MifarePlus_Read( &card, 0x4000,  pinfo[3].Key,0x0003, data );
       
       static uint8_t def_key[16] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
       static const uint8_t new_key[16]   = {11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26 };
       static const uint8_t new_key_B[16] = {27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42 };
       
       uint16_t block_num = 0x001D;
       uint16_t Ak_bnr = 0x4000 +(block_num/4)*2;   // сектора по 4 блока
              
       if( MifarePlus_ReadPlain( &currentState.card, Ak_bnr, new_key, block_num, data ) > 0 )
         {
          //if( MifarePlus_WritePlain( &card, Ak_bnr, new_key, block_num, wdata ) >0 )       
            {
             MifarePlus_ReadPlain( &currentState.card, Ak_bnr, new_key, block_num, data );
            }
         }               
       
       if( MifarePlus_ReadPlain( &currentState.card, Ak_bnr+1, def_key, block_num, data ) > 0 ) // Key B
         {
          if( MifarePlus_ReadPlain( &currentState.card, Ak_bnr+1, new_key, block_num, data ) > 0)
            {
             MifarePlus_ReadPlain( &currentState.card, Ak_bnr+1, new_key, block_num, data );
            }
         }          
       uint8_t nkey[16];
       memcpy( nkey, new_key, 16 );
       uint16_t Bk_bnr = Ak_bnr+1;
       
       if( MifarePlus_ReadPlain( &currentState.card, Ak_bnr, def_key, 0x001F, data ) >0 )
         {
          MifarePlus_ReadPlain( &currentState.card, Bk_bnr, new_key, 0x001F, data );
         }
       */
       //MifarePlus_WriteEncrypted( &card, Ak_bnr, def_key, Bk_bnr ,  nkey );       
       
  //     if( MifarePlus_ReadPlain( &card, Bk_bnr, new_key_B, block_num, data ) >0 )
   //      {
    //      MifarePlus_ReadPlain( &card, 0x4000, pinfo[3].Key, 0x0001, data ); 
    //     } 
      // static uint8_t data[16];
     //  static uint8_t resb[16];
    //   static const uint8_t keymac[16]={0xD1,0xB7, 0xE8,0x1E, 0x63,0xB0, 0x45,0x80, 0xEB,0x3E, 0x02,0x7C, 0xE0,0x1D, 0x55,0xBF};
       //uint8_t rdc[16] ={0x33,0x00,0x00,0x35,0xE7,0x58,0xC6,0x11,0x00,0x01};
    //   uint8_t rdc[16] ={0x33,0x01,0x00,0x35,0xE7,0x58,0xC6,0x12,0x00,0x01};
      // makePadding( rdc, 10, 16);
       //CalculateMAC( rdc,1, keymac, resb );
//       CalculateMAC( rdc, 10, keymac, resb );
      
  //     static const uint8_t rnda[16]={0xBA,0x0D,0xB8,0x20,0x1E,0x89,0x7F,0x8A,0x5C,0xD2,0xAF,0x32,0x7A,0xD4,0xB5,0xEC};
  //     static const uint8_t rndb[16]={0x08,0x2E,0xC5,0xDB,0x5B,0x11,0xFF,0xEE,0xFB,0x21,0x4F,0x26,0x93,0x66,0x09,0xDE};
      
  //     memcpy( card.RndA,rnda,16);
  //     memcpy( card.RndB,rndb,16);
       
//       static const uint8_t valkey[16]={0xA3,0x81,0x52,0xC9,0x03,0x55, 0xCC,0x63,0xAc,0x6E, 0x97,0xA3,0x99,0x80,0x7B,0x59};
       
   //    generateKmac( &card, valkey, resb );
       
//       OS::sleep(TICKS_PER_SEC/4);   // 5100us
      
  //     CardPersonalosation( &card_info );
      }
   // if( res ==0 )
   //   break;
      
 //  }
 return res;
}
#endif
