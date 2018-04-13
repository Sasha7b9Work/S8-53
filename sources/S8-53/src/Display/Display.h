#pragma once
#include "DisplayTypes.h"
#include "Settings/SettingsTypes.h"
#include "Painter.h"
#include "Tables.h"


class Display
{
public:
    static void Init();

    static void Update(bool endScene = true);

    static void RotateRShift(Channel channel);

    static void RotateTrigLev();

    static void Redraw();

    static void EnableTrigLabel(bool enable);
    /// Функция должна вызываться для очистки поточечной памяти. Вызывается при изменении масштаба по напряжению.
    static void ResetP2Ppoints(bool empty);
    /// Добавляет точки в режиме поточечного вывода и самописца data00, data01 - для первого канала; data10, data11 - для второго канала
    static void AddPoints(uint8 data00, uint8 data01, uint8 data10, uint8 data11);

    static void ShowWarningBad(Warning warning);

    static void ShowWarningGood(Warning warning);

    static void ClearFromWarnings();

    static void SetDrawMode(DrawMode mode, pFuncVV func);

    static void SetAddDrawFunction(pFuncVV func);

    static void RemoveAddDrawFunction();

    static void Clear();

    static void ShiftScreen(int delta);

    static void ChangedRShiftMarkers();

    static void AddStringToIndicating(const char *string);

    static void OneStringUp();

    static void OneStringDown();

    static void SetPauseForConsole(bool pause);
    /// После отрисовки очередного экрана эта функция будет вызвана один раз.
    static void RunAfterDraw(pFuncVV func);
    /// Отключить вспомогательную линию маркера смещения по напряжению первого канала.
    static void DisableShowLevelRShiftA();
    /// Отключить вспомогательную линию маркера смещения по напряжению второго канала.
    static void DisableShowLevelRShiftB();
    
    static void DisableShowLevelTrigLev();

    static void OnRShiftMarkersAutoHide();

private:

    static void ShowWarn(const char *message);
    /// Нарисовать сетку.
    static void DrawGrid(int left, int top, int width, int height);
    /// Нарисовать полную сетку.
    static void DrawFullGrid();

    static void DrawCursorsWindow();

    static void DrawCursorsRShift();
    /// Нарисовать маркеры смещения по напряжению
    static void DrawCursorRShift(Channel chan);
    /// Нарисовать маркер уровня синхронизации.
    static void DrawCursorTrigLevel();
    /// Нарисовать маркер смещения по времени.
    static void DrawCursorTShift();
    /// Написать информацию под сеткой - в нижней части дисплея.
    static void DrawLowPart();

    static void DrawHiPart();

    static void DrawHiRightPart();

    static void DrawMath();

    static void DrawSpectrum();
    /// Вывести текущее время.
    static void DrawTime(int x, int y);
    /// Нарисовать курсоры курсорных измерений.
    static void DrawCursors();
    /// Вывести значения курсоров курсорных измерений.
    static void WriteCursors();
    /// Вывести значения автоматических измерений.
    static void DrawMeasures();
    /// Написать сообщения отладочной консоли.
    static void DrawConsole();
    /// Написать предупреждения.
    static void DrawWarnings();

    static void DrawTimeForFrame(uint timeMS);
    /// Нарисовать горизонтальный курсор курсорных измерений.
    static void DrawHorizontalCursor
                                (int y,             ///< числовое значение курсора.
                                int xTearing        ///< координата места, в котором необходимо сделать разрыв для квадрата пересечения.
                                );
    /// Нарисовать вертикальный курсор курсорных измерений.
    static void DrawVerticalCursor
                                (int x,             ///< числовое значение курсора.
                                int yTearing        ///< координата места, в котором необходимо сделать разрыв для квадрата пересечения.
                                );
    /// Вывести значение уровня синхронизации. 
    static void WriteValueTrigLevel();

};

