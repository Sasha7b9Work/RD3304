#ifndef CARD_UID_H
#define CARD_UID_H

#include <stdint.h>


/*!
 * UID карты
 */
class TUid
{
 public:
   uint8_t length;  //!< фактический размер UID, 7 или 4, если любое другое значение, то UID не действителен
   uint8_t uid[7];  //!< сам UID
};

#endif
