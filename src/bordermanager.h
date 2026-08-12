#pragma once
//
// BorderManager — manages a BorderOverlay per pinned window. Uses
// SetWinEventHook to track window movement/resize/lifecycle so overlays
// follow their targets without polling.
//
#include <QObject>
#include <QHash>
#include <cstdint>

#include <windows.h>

class BorderOverlay;
class QTimer;

class BorderManager : public QObject
{
    Q_OBJECT
public:
    explicit BorderManager(QObject *parent = nullptr);
    ~BorderManager() override;

    // Attach a red border overlay to the given window.
    void attach(intptr_t hwnd);

    // Detach and destroy the overlay for the given window.
    void detach(intptr_t hwnd);

    // Detach all overlays.
    void detachAll();

    // Update geometry for a specific window (used by the event hook).
    void updateGeometry(intptr_t hwnd);

private:
    // Win32 event hook callback.
    static void CALLBACK winEventProc(HWINEVENTHOOK hHook, DWORD event,
                                       HWND hwnd, LONG idObject, LONG idChild,
                                       DWORD dwEventThread, DWORD dwmsEventTime);

    // Instance method to process events.
    void handleWinEvent(DWORD event, HWND hwnd, LONG idObject);

    QHash<intptr_t, BorderOverlay *> m_overlays;
    HWINEVENTHOOK m_hook = nullptr;

    // Throttle timer to coalesce rapid LOCATIONCHANGE events.
    QTimer *m_updateTimer = nullptr;
    QHash<intptr_t, bool> m_pendingUpdates;
};
