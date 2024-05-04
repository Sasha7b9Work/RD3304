// 2023/09/18 09:16:54 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Reader/Events.h"
#include "Reader/Reader.h"
#include "Hardware/HAL/HAL.h"
#include "Reader/Messages.h"
#include "Modules/Indicator/Indicator.h"
#include "Modules/Player/Player.h"
#include "Nodes/OSDP/OSDP.h"


namespace Event
{
    static UID last_readed_uid;
}


void Event::WriteList()
{
    Message::Send("\"CARD DETECTED\" - card detected in the reader's radio field");

    Message::Send("\"CARD READ\"     - event after card authentication attempt");
    Message::Send(1, "\"CARD READ <UID> NUMBER <NUMBER> AUTH IS OK\" - successful authentication, card ready to use");
    Message::Send(1, "\"CARD READ <UID> AUTH IS FAIL\"               - authentication failed");

    Message::Send("\"CARD REMOVE\"   - card remove from reader's radio field");
}


void Event::CardDetected()
{
    if (ModeReader::IsExtended())
    {
        Message::Send("CARD DETECTED");
    }
    else
    {
        Indicator::Blink(Color(0x0000FF, 1.0f), Color(0, 0.0f), 50, true);

        if (OSDP::IsEnabled())
        {
            Indicator::TaskOSDP::FirePermanentColor();
        }
    }
}


void Event::CardReadOK(const UID &uid, uint number, pchar password_string)
{
    last_readed_uid = uid;

    if (ModeReader::IsExtended())
    {
        Message::SendFormat("CARD READ %s*%s NUMBER %u AUTH %s OK",
            uid.ToString(true).c_str(),
            uid.ToString(false).c_str(),
            number,
            password_string
        );
    }
}


void Event::CardReadFAIL(const UID &uid, pchar password_string)
{
    last_readed_uid = uid;

    if (ModeReader::IsExtended())
    {
        Message::SendFormat("CARD READ %s*%s AUTH %s FAIL",
            uid.ToString(true).c_str(),
            uid.ToString(false).c_str(),
            password_string
            );
    }
}


void Event::CardRemove()
{
    if (ModeReader::IsExtended())
    {
        Message::SendFormat("CARD REMOVE %s*%s", last_readed_uid.ToString(true).c_str(), last_readed_uid.ToString(false).c_str());
    }
}
