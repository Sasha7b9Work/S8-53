

#include "defines.h"
#include "WelcomeScreen.h"
#include "Math3D.h"
#include "DisplayTypes.h"
#include "Display/Display.h"
#include "Display/Painter.h"
#include "Hardware/Timer.h"
#include "Settings/Settings.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static Point points[8] =
{
    {-1.0,  1.0, -1.0},
    { 1.0,  1.0, -1.0},
    { 1.0, -1.0, -1.0},
    {-1.0, -1.0, -1.0},
    {-1.0,  1.0,  1.0},
    { 1.0,  1.0,  1.0},
    { 1.0, -1.0,  1.0},
    {-1.0, -1.0,  1.0}
};

static Edge edges[12] =
{
    {0, 1}, {1, 2}, {2, 3}, {3, 0},
    {0, 4}, {1, 5}, {2, 6}, {3, 7},
    {4, 7}, {4, 5}, {5, 6}, {6, 7}
};

static Mesh mesh = {
    12,
    edges,
    8,
    points
};

static float x = 0.0; 
static float y = 0.0;
static float z = 200.0;

static float scaleX = 1.0f;
static float scaleY = 1.0f;
static float scaleZ = 1.0f;

static float angleX = 0.0;
static float angleY = 0.0;
static float angleZ = 0.0;

static uint startTime = 0;

static bool isRun = false;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void WelcomeScreen_Init() {

    if (!set.service.screenWelcomeEnable)
    {
        return;
    }

    for (int i = 0; i < 8; i++) {
        float k = 50.0f;
        points[i].x *= k;
        points[i].y *= k;
        points[i].z *= k;
    }

    startTime = gTimerMS;
    isRun = true;
}


//---------------------------------------------------------------------------------------------------------------------------------------------------
bool WelcomeScreen_Run()
{
    if (!isRun)
    {
        return false;
    }

    static bool isDown = false;

    Painter::BeginScene(COLOR_BLACK);

    Painter::DrawRectangleC(0, 0, 319, 239, COLOR_FILL);

    static float y0 = 0.0f;

    if (isDown)
    {
        if (gTimerMS - startTime > 1000)
        {
            isRun = false;
        }
    }
    else
    {
        float g = 98.0f * 4.0f;
        float time0 = 0.0f;
        y0 = g * time0 * time0 / 2.0f;
        float time = (gTimerMS - startTime) / 1000.0f + time0;
        y0 = -y0 + g * time * time / 2.0f + 10.0f;

        static const float MAX_Y = 130.0f;

        if (y0 > MAX_Y)
        {
            y0 = MAX_Y;
            isDown = true;
            startTime = gTimerMS;
        }
    }

    Painter::DrawBigText(45, y0, 8, "МНИПИ");

    Painter::DrawStringInCenterRect(0, 190, 320, 20, "Для получения помощи нажмите и удерживайте кнопку ПОМОЩЬ");
    Painter::DrawStringInCenterRect(0, 205, 320, 20, "Отдел маркетинга: тел./факс. 8-017-262-57-50");
    Painter::DrawStringInCenterRect(0, 220, 320, 20, "Разработчики: e-mail: mnipi-24(@)tut.by, тел. 8-017-262-57-51");

    Painter::EndScene();

    return isRun;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------
void WelcomeScreen_Update() {
    Painter::BeginScene(COLOR_WHITE);
    for(int num = 0; num < 12; num++) {
        Point p0 = mesh.points[mesh.edges[num].numPoint0];
        Point p1 = mesh.points[mesh.edges[num].numPoint1];
        Point p0out = p0;
        Point p1out = p1;

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Mat4x4 matScale = {0};
        FillMatriceScale(&matScale, scaleX, scaleY, scaleZ);

        TransformPoint(&matScale, &p0, &p0out);
        TransformPoint(&matScale, &p1, &p1out);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Mat4x4 matRotate = {0};
        FillMatriceRotation(&matRotate, angleX, angleY, angleZ);

        Point p0out2 = p0out;
        Point p1out2 = p1out;

        TransformPoint(&matRotate, &p0out, &p0out2);
        TransformPoint(&matRotate, &p1out, &p1out2);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Mat4x4 matMove = {0};
        FillMatriceMov(&matMove, x, y, z);

        Point p0out3 = p0out2;
        Point p1out3 = p1out2;

        TransformPoint(&matMove, &p1out2, &p0out3);
        TransformPoint(&matMove, &p1out2, &p1out3);

        float x0 = 0;
        float x1 = 0;
        float y0 = 0;
        float y1 = 0;
        PresentPointToScreen(&p0out3, &x0, &y0);
        PresentPointToScreen(&p1out3, &x1, &y1);

        Painter::DrawLineC(x0, y0, x1, y1, COLOR_WHITE);
    }
    Painter::EndScene();
}
