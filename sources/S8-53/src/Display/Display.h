#pragma once
#include "DisplayTypes.h"
#include "Settings/SettingsTypes.h"
#include "Painter.h"
#include "Tables.h"


class Display
{
public:
    void Init();

    void Update();

    void RotateRShift(Channel channel);

    void RotateTrigLev();

    void Redraw();

    void EnableTrigLabel(bool enable);
    /// Функция должна вызываться для очистки поточечной памяти. Вызывается при изменении масштаба по напряжению.
    void ResetP2Ppoints(bool empty);
    /// Добавляет точки в режиме поточечного вывода и самописца data00, data01 - для первого канала; data10, data11 - для второго канала
    void AddPoints(uint8 data00, uint8 data01, uint8 data10, uint8 data11);

    void ShowWarningBad(Warning warning);

    void ShowWarningGood(Warning warning);

    void ClearFromWarnings();

    void SetDrawMode(DrawMode mode, pFuncVV func);

    void SetAddDrawFunction(pFuncVV func);

    void RemoveAddDrawFunction();

    void Clear();

    void ShiftScreen(int delta);

    void ChangedRShiftMarkers();

    void AddStringToIndicating(const char *string);

    void OneStringUp();

    void OneStringDown();

    void SetPauseForConsole(bool pause);
    /// После отрисовки очередного экрана эта функция будет вызвана один раз.
    void RunAfterDraw(pFuncVV func);
    /// Отключить вспомогательную линию маркера смещения по напряжению первого канала.
    void DisableShowLevelRShiftA();
    /// Отключить вспомогательную линию маркера смещения по напряжению второго канала.
    void DisableShowLevelRShiftB();
    
    void DisableShowLevelTrigLev();

    void OnRShiftMarkersAutoHide();

private:

    void ShowWarn(const char *message);
    /// Нарисовать сетку.
    void DrawGrid(int left, int top, int width, int height);
    /// Нарисовать полную сетку.
    void DrawFullGrid();

    void DrawCursorsWindow();

    void DrawCursorsRShift();
    /// Нарисовать маркеры смещения по напряжению
    void DrawCursorRShift(Channel chan);
    /// Нарисовать маркер уровня синхронизации.
    void DrawCursorTrigLevel();
    /// Нарисовать маркер смещения по времени.
    void DrawCursorTShift();
    /// Написать информацию под сеткой - в нижней части дисплея.
    void DrawLowPart();

    void DrawHiPart();

    void DrawHiRightPart();

    void DrawMath();

    void DrawSpectrum();
    /// Вывести текущее время.
    void DrawTime(int x, int y);
    /// Нарисовать курсоры курсорных измерений.
    void DrawCursors();
    /// Вывести значения курсоров курсорных измерений.
    void WriteCursors();
    /// Вывести значения автоматических измерений.
    void DrawMeasures();
    /// Написать сообщения отладочной консоли.
    void DrawConsole();
    /// Написать предупреждения.
    void DrawWarnings();

    void DrawTimeForFrame(uint timeMS);
    /// Нарисовать горизонтальный курсор курсорных измерений.
    void DrawHorizontalCursor
                                (int y,             ///< числовое значение курсора.
                                int xTearing        ///< координата места, в котором необходимо сделать разрыв для квадрата пересечения.
                                );
    /// Нарисовать вертикальный курсор курсорных измерений.
    void DrawVerticalCursor
                                (int x,             ///< числовое значение курсора.
                                int yTearing        ///< координата места, в котором необходимо сделать разрыв для квадрата пересечения.
                                );
    /// Вывести значение уровня синхронизации. 
    void WriteValueTrigLevel();

};


extern Display display;
