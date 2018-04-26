#pragma once 
#include "defines.h"


class Panel
{
public:

    static void Init();

    static void EnableLEDRegSet(bool enable);
    /// Включить/выключить светодиод КАНАЛ 1.
    static void EnableLEDChannel0(bool enable);
    /// Включить/выключить светодиод КАНАЛ 2.
    static void EnableLEDChannel1(bool enable);
    /// Включить/выключить светодиод СИНХР.
    static void EnableLEDTrig(bool enable);
    /// Передать даннные в мк панели управления.
    static void TransmitData(uint16 data);
    /// В отлюченном режиме панель лишь обновляет состояние переменной pressedButton, не выполняя больше никаких действий.
    static void Disable();

    static void Enable();
    /// Ожидать нажатие клавиши.
    static PanelButton WaitPressingButton();

    static bool ProcessingCommandFromPIC(uint16 command);

    static uint16 NextData();
};
