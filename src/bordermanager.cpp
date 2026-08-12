#include "bordermanager.h"
#include "borderoverlay.h"

#include <QTimer>

#include <windows.h>

// Singleton pointer for the static hook callback.
static BorderManager *s_borderManagerInstance = nullptr;

BorderManager::BorderManager(QObject *parent)
    : QObject(parent)
{
    s_borderManagerInstance = this;

    // Throttle timer: fires once per frame to batch rapid geometry updates.
    m_updateTimer = new QTimer(this);
    m_updateTimer->setSingleShot(true);
    m_updateTimer->setInterval(16); // ~60 fps
    connect(m_updateTimer, &QTimer::timeout, this, [this]() {
        for (auto it = m_pendingUpdates.cbegin(); it != m_pendingUpdates.cend(); ++it) {
            if (m_overlays.contains(it.key()))
                m_overlays[it.key()]->updateGeometry();
        }
        m_pendingUpdates.clear();
    });

    // Install global WinEvent hook to track window movement/resize/lifecycle.
    m_hook = SetWinEventHook(
        EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE,
        nullptr, winEventProc, 0, 0,
        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

    // Also track minimize/restore/show/hide/destroy.
    SetWinEventHook(
        EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZEEND,
        nullptr, winEventProc, 0, 0,
        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

    SetWinEventHook(
        EVENT_OBJECT_SHOW, EVENT_OBJECT_HIDE,
        nullptr, winEventProc, 0, 0,
        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

    SetWinEventHook(
        EVENT_OBJECT_DESTROY, EVENT_OBJECT_DESTROY,
        nullptr, winEventProc, 0, 0,
        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
}

BorderManager::~BorderManager()
{
    detachAll();
    if (m_hook) {
        UnhookWinEvent(m_hook);
        m_hook = nullptr;
    }
    s_borderManagerInstance = nullptr;
}

void BorderManager::attach(intptr_t hwnd)
{
    if (m_overlays.contains(hwnd))
        return;

    auto *overlay = new BorderOverlay;
    overlay->attach(hwnd);

    if (!overlay->isAttached()) {
        delete overlay;
        return;
    }

    m_overlays.insert(hwnd, overlay);
}

void BorderManager::detach(intptr_t hwnd)
{
    if (BorderOverlay *overlay = m_overlays.take(hwnd)) {
        overlay->detach();
        overlay->deleteLater();
    }
    m_pendingUpdates.remove(hwnd);
}

void BorderManager::detachAll()
{
    for (auto it = m_overlays.cbegin(); it != m_overlays.cend(); ++it) {
        it.value()->detach();
        it.value()->deleteLater();
    }
    m_overlays.clear();
    m_pendingUpdates.clear();
}

void BorderManager::updateGeometry(intptr_t hwnd)
{
    if (BorderOverlay *overlay = m_overlays.value(hwnd)) {
        if (!overlay->updateGeometry())
            detach(hwnd);
    }
}

void CALLBACK BorderManager::winEventProc(HWINEVENTHOOK, DWORD event,
                                           HWND hwnd, LONG idObject, LONG,
                                           DWORD, DWORD)
{
    if (!s_borderManagerInstance || !hwnd)
        return;
    s_borderManagerInstance->handleWinEvent(event, hwnd, idObject);
}

void BorderManager::handleWinEvent(DWORD event, HWND hwnd, LONG idObject)
{
    // Only care about the window itself, not child objects.
    if (idObject != OBJID_WINDOW)
        return;

    intptr_t key = reinterpret_cast<intptr_t>(hwnd);
    if (!m_overlays.contains(key))
        return;

    switch (event) {
    case EVENT_OBJECT_LOCATIONCHANGE: {
        // Throttle: mark as pending, actual update happens on timer tick.
        m_pendingUpdates.insert(key, true);
        if (!m_updateTimer->isActive())
            m_updateTimer->start();
        break;
    }
    case EVENT_SYSTEM_MINIMIZESTART:
        if (BorderOverlay *o = m_overlays.value(key))
            o->hide();
        break;

    case EVENT_SYSTEM_MINIMIZEEND:
        if (BorderOverlay *o = m_overlays.value(key)) {
            o->updateGeometry();
            o->show();
        }
        break;

    case EVENT_OBJECT_SHOW:
        if (BorderOverlay *o = m_overlays.value(key)) {
            o->updateGeometry();
            o->show();
        }
        break;

    case EVENT_OBJECT_HIDE:
        if (BorderOverlay *o = m_overlays.value(key))
            o->hide();
        break;

    case EVENT_OBJECT_DESTROY:
        detach(key);
        break;
    }
}
