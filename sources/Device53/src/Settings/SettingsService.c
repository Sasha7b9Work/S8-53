#include "Settings.h"
#include "SettingsService.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool sService_MathEnabled()
{
    return !DISABLED_DRAW_MATH || ENABLED_FFT;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
float sService_MathGetFFTmaxDBabs()
{
    static const float db[] = {-40.0f, -60.0f, -80.0f};
    return db[FFT_MAX_DB];
}
