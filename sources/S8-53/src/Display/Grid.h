#pragma once


class Grid
{
public:
    int Left();
    int Right();
    int Width();
    float DeltaY();
    float DeltaX();

    int WidthInCells();

    int FullBottom();
    int FullHeight();
    int FullCenterHeight();

    int ChannelBottom();
    int ChannelHeight();
    int ChannelCenterHeight();

    int MathHeight();
    int MathBottom();
    int MathTop();

    int BottomMessages();

private:
    int SignalWidth();
};


extern Grid grid;
