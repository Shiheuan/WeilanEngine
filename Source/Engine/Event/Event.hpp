#pragma once

struct WindowSizeChange
{
    bool state;
    int width;
    int height;
};

struct WindowClose
{
    bool state;
};

struct SwapchainRecreated
{
    bool state;
};

class Event
{
public:
    void Poll();
    void Reset();

    const WindowSizeChange& GetWindowSizeChanged()
    {
        return windowSizeChange;
    }
    const WindowClose& GetWindowClose()
    {
        return windowClose;
    }
    const SwapchainRecreated& GetSwapchainRecreated()
    {
        return swapchainRecreated;
    }

private:
    WindowSizeChange windowSizeChange;
    WindowClose windowClose;
    SwapchainRecreated swapchainRecreated;

    friend class WeilanEngine;
};
