#pragma once 
#include "defines.h"


class Panel
{
public:

    void Init();

    void EnableLEDRegSet(bool enable);
    /// Включить/выключить светодиод КАНАЛ 1.
    void EnableLEDChannel0(bool enable);
    /// Включить/выключить светодиод КАНАЛ 2.
    void EnableLEDChannel1(bool enable);
    /// Включить/выключить светодиод СИНХР.
    void EnableLEDTrig(bool enable);
    /// Передать даннные в мк панели управления.
    void TransmitData(uint16 data);
    /// В отлюченном режиме панель лишь обновляет состояние переменной pressedButton, не выполняя больше никаких действий.
    void Disable();

    void Enable();
    /// Ожидать нажатие клавиши.
    PanelButton WaitPressingButton();

    bool ProcessingCommandFromPIC(uint16 command);

    uint16 NextData();
};


extern Panel panel;
