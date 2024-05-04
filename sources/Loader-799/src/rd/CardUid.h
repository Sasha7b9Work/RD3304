#ifndef CARD_UID_H
#define CARD_UID_H

#include <stdint.h>


/*!
 * UID �����
 */
class TUid
{
 public:
   uint8_t length;  //!< ����������� ������ UID, 7 ��� 4, ���� ����� ������ ��������, �� UID �� ������������
   uint8_t uid[7];  //!< ��� UID
};

#endif
