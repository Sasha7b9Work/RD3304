// 2022/7/3 11:16:16 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modules/CLRC66303HN/CLRC66303HN.h"
#include "Modules/CLRC66303HN/CommandsCLRC663.h"
#include "Modules/CLRC66303HN/RegistersCLRC663.h"
#include "Device/Device.h"
#include "Reader/Card.h"
#include "Reader/Task/Task.h"
#include "Nodes/Communicator.h"
#include "Settings/Settings.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "Reader/Events.h"
#include "Reader/Messages.h"
#include "Modules/Indicator/Indicator.h"
#include "Reader/Signals.h"
#include "Utils/StringUtils.h"
#include "Settings/AccessCards.h"
#include "system.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>


namespace CLRC66303HN
{
    // Подача питания на чип
    namespace Power
    {
        static void Off()
        {
            pinENN.ToHi();
        }

        static void On()
        {
            pinENN.ToLow();
        }

        static void Init()
        {
            pinENN.Init();
            Off();
        }
    }

    // Включение электромагнитного поля
    namespace RF
    {
        static void On()
        {
            Command::Idle();

            uint8 reg = Register(MFRC630_REG_DRVMOD).Read();
            _SET_BIT(reg, 3);
            Register(MFRC630_REG_DRVMOD).Write(reg);
        }

        static void Off()
        {
            uint8 reg = Register(MFRC630_REG_DRVMOD).Read();
            _CLEAR_BIT(reg, 3);
            Register(MFRC630_REG_DRVMOD).Write(reg);
        }
    }

    // number_successful_attempts - число попыток
    // mode_and - если true, то возвращаем true, если все попытки успешные, иначе - если хотя бы одна
    static bool DetectCard(int number_successful_attempts, bool mode_and);

    // Возвращает false, если в процессе обработки произошла ошибка чтения/записи
    static bool ProcessMasterCard();

    // Возвращает false, если в процессе обработки произошла ошибка чтения/записи
    static bool ProcessUserCard();
}


void CLRC66303HN::Init()
{
    pinIRQ_TRX.Init();

    Power::Init();

    Power::On();

    RF::Off();

    HAL_FLASH::LoadAntennaConfiguration106();

    HAL_FLASH::LoadProtocol();

    fifo.Init();

    Register(MFRC630_REG_IRQ1EN).Write(0xC0);      // IRQ1En        // Включаем пин прерывание в Push-Pull

    while ((irq0.GetValue() & IRQ0::IdleIRQ) == 0)          // Ждём, пока отработают внутренние схемы
    {
    }

    Register(MFRC630_REG_IRQ0EN).Write(0x84);      // IRQ0En        // Включаем "железное прерываниие" IRQ на чтение данных. Инвертируем

    while ((irq0.GetValue() & IRQ0::IdleIRQ) == 0)          // Ждём, пока отработают внутренние схемы
    {
    }
}


bool CLRC66303HN::DetectCard(int number_successful_attempts, bool mode_and)
{
    bool detected = false;

    for (int i = 0; i < number_successful_attempts; i++)
    {
        if (i != 0)
        {
            Init();

            RF::On();
        }

        Card::uid.Clear();

        Command::Idle();

        fifo.Clear();

        TimeMeterUS meterUS;

        meterUS.WaitFor(5100);         // \todo здесь нужно отмерять 5100 мкс

        irq0.Clear();                                   // Очищаем флаги

        Register(MFRC630_REG_TXCRCPRESET).Write(0x18);  // Switches the CRC extention OFF in tx direction //-V525
                                                        // TxCRCEn == 0 (CRC is not appended to the data stream)
                                                        // TXPresetVal == 0001 (6363h for CRC16)
                                                        // TxCRCtype == 10 (CRC16)

        Register(MFRC630_REG_RXCRCCON).Write(0x18);     // Switches the CRC extention OFF in rx direction
                                                        // RxCRCEn == 0 (If set, the CRC is checked and in case of a wrong CRC an error flag is
                                                        // set.Otherwise the CRC is calculated but the error flag is not modified)

        Register(MFRC630_REG_TXDATANUM).Write(0x0F);    // Only the 7 last bits will be sent via NFC
                                                        // TxLastBits == 111
                                                        // DataEn == 1 (data is sent)

        Command::Send(0x26);                            // REQA

        BitSet16 data;

        while (meterUS.ElapsedUS() < 8000)                // Запрос REQA //-V654
        {
            if (!pinIRQ_TRX.IsHi())                     // данные получены
            {
                uint8 reg_0x06 = irq0.GetValue();

                if (reg_0x06 & IRQ0::ErrIRQ)            // ошибка данных
                {
                    break;
                }
                else                                    // данные верны
                {
                    data.byte[0] = fifo.Pop();
                    data.byte[1] = fifo.Pop();

                    break;
                }
            }
        }

        if (data.half_word != 0)
        {
            if (Command::AnticollisionCL(1, &Card::uid))
            {
                if (Command::SelectCL(1, &Card::uid))
                {
                    if (!Card::uid.Calcualted())
                    {
                        if (Command::AnticollisionCL(2, &Card::uid))
                        {
                            Command::SelectCL(2, &Card::uid);
                        }
                    }
                }
            }
        }

        detected = Card::uid.Calcualted();

        if (mode_and)
        {
            if (!detected)
            {
                break;
            }
        }
        else
        {
            if (detected)
            {
                break;
            }
        }
    }

    static bool prev_detected = false;

    if (prev_detected != detected)
    {
        if (detected)
        {
//            LOG_WRITE("---Card detected---");

            Event::CardDetected();

            while (Indicator::IsRunning())
            {
                Device::UpdateTasks();
            }

            if (ModeSignals::IsExternal())
            {
                Indicator::FireInputs(pinLG.IsLow(), pinLR.IsLow());
            }
            else
            {
                Indicator::TurnOn(gset.ColorRed(), TIME_RISE_LEDS, false);
            }
        }
    }

    prev_detected = detected;

    return detected;
}


bool CLRC66303HN::UpdateExtendedMode(const TypeAuth &type_auth, bool new_auth)
{
    Init();

    RF::On();

    bool result = false;

    if (DetectCard(3, !Card::IsInserted()))
    {
        bool auth_ok = false;

        if (type_auth.enabled)
        {
            auth_ok = Command::PasswordAuth(type_auth.password);

            result = auth_ok;
        }
        else
        {
            auth_ok = Card::RAW::ReadDataWitouthAuth();

            result = auth_ok;
        }

        uint64 number = 0;

        if (!Card::RAW::ReadNumber(&number))
        {
            auth_ok = false;
        }

        Card::InsertExtendedMode(type_auth, auth_ok, new_auth, number);

        Task::Run();
    }
    else
    {
        Card::Eject();
    }

    RF::Off();

    return result;
}


bool CLRC66303HN::UpdateNormalMode()
{
    Init();

    RF::On();

    bool result = false;

    if (DetectCard(3, !Card::IsInserted()))
    {
        if (!Card::IsInserted())
        {
            if (ModeWG::IsNormal())             // Это выполняется также и в том случае, если OSDP и автономный режим
            {
                uint64 number = 0;
                char message_fail[128];
                std::sprintf(message_fail, "CARD %s*%s AUTHENTICATION FAILED", Card::uid.ToString(true).c_str(), Card::uid.ToString(false).c_str());

                if (Command::PasswordAuth(SettingsReader::PSWD::Get()) && Card::RAW::ReadNumber(&number))
                {
                    result = true;

                    ProtectionBruteForce::Reset();

                    if ((uint)number == (uint)-1)      // В четвёртом секторе FF FF FF FF - мастер-карта
                    {
                        if (!ProcessMasterCard())
                        {
                            Message::Send(message_fail);
                        }
                    }
                    else                        // Пользовательская карта
                    {
                        if (!ProcessUserCard())
                        {
                            Message::Send(message_fail);
                        }
                    }
                }
                else
                {
                    if (!Card::IsInserted())
                    {
                        Message::Send(message_fail);

                        ProtectionBruteForce::FailAttempt();
                    }

                    Card::InsertNormalModeUser(0, false);
                }
            }
            else                                                                                        // Нужно передавать только GUID карты
            {
                Card::InsertNormalModeUser(0, false);
            }
        }
    }
    else
    {
        Card::Eject();
    }

    RF::Off();

    return result;
}


bool CLRC66303HN::ProcessMasterCard()
{
    SettingsReader settings;

    bool result = false;

    if (Card::RAW::ReadDataFromblocks(4, (uint8 *)&settings, settings.Size()))
    {
        if (settings.CRC32IsMatches())
        {
            if (settings.OldPassword() == (uint64)-1)                           // Меняем пароль
            {
                SettingsReader::PSWD::Set(settings.Password());

                result = true;
            }
            else if (settings.OldPassword() == SettingsReader::PSWD::Get())     // Меняем всю конфигурацию
            {
                gset = settings;
                gset.Save();

                char access_card[500];

                if (Card::RAW::ReadBitmapCards(access_card))
                {
                    AccessCards::Set(access_card);

                    result = true;
                }
            }
        }
    }

    bool need_led = !Card::IsInserted();

    Card::InsertNormalModeMaster(result);

    if (need_led)
    {
        if (result)
        {
            Communicator::WriteConfitToUSART();

            Signals::GoodMasterCard();
        }
        else
        {
            Signals::BadMasterCard();
        }
    }

    return result;
}


bool CLRC66303HN::ProcessUserCard()
{
    uint64 number = 0;

    bool result = false;

    if (Card::RAW::ReadNumber(&number))
    {
        result = true;
    }

    Card::InsertNormalModeUser(number, result);

    return result;
}
