#pragma once

#include "Util/FastDelegate.h"

#include <deque>
#include <map>
#include <memory>

struct GLFWwindow;

enum class EventType
{
    MouseMove,
    MouseButton,
    MouseScroll,
    Key,
    Char
};

class Event
{
public:
    Event(EventType e) : Type(e) {}
    virtual ~Event() = default;

    EventType Type;
};
class CharEvent : public Event
{
public:
    CharEvent(EventType e, unsigned int c) : Event(e), Char(c) {}

    unsigned int Char;
};
class MouseXYEvent : public Event
{
public:
    MouseXYEvent(EventType e, double dx, double dy) : Event(e), X(dx), Y(dy) {}
    double X;
    double Y;
};
class ButtonEvent : public Event
{
public:
    ButtonEvent(EventType e, int b, int ac, int m, int sc = 0) : Event(e), Button(b), Action(ac), Mods(m), Scancode(sc)
    {
    }

    int Button;
    int Scancode;
    int Action;
    int Mods;
};

using EventPtr = std::shared_ptr<Event>;

using EventDelegate = fastdelegate::FastDelegate<bool(EventPtr)>;
using EventDelegatePtr = std::shared_ptr<EventDelegate>;

class GLInputManager
{
public:
    static std::shared_ptr<GLInputManager> GetInputManager();
    ~GLInputManager() = default;

    void InstallCallbacks(GLFWwindow* window);
    void UninstallCallbacks(GLFWwindow* window);

    void Update(float deltaT, GLFWwindow* window);

    EventDelegatePtr SubscribeEvent(EventType e, bool pushToFront = false);
    void UnsubscribeEvent(EventType e, EventDelegatePtr d);

    void HandleMouse_cb(GLFWwindow* window, int button, int action, int mods);
    void HandleScroll_cb(GLFWwindow* window, double xoffset, double yoffset);
    void HandleKey_cb(GLFWwindow* window, int key, int scancode, int action, int mods);
    void HandleChar_cb(GLFWwindow* window, unsigned int c);

private:
    GLInputManager() = default;

    void SendEvent(EventPtr event);

private:
    bool m_installedCallbacks = false;
    double m_cursorPos[2]{0.0, 0.0};

    std::map<EventType, std::deque<EventDelegatePtr>> m_subscribers;
};
