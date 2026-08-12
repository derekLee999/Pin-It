#include "borderoverlay.h"

#include <QPainter>
#include <QPainterPath>
#include <QScreen>
#include <QGuiApplication>

#include <windows.h>
#include <dwmapi.h>

BorderOverlay::BorderOverlay(QWidget *parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint
                   | Qt::WindowDoesNotAcceptFocus);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setAttribute(Qt::WA_NoSystemBackground);

    // After attaching, the DWM may return stale bounds for a few frames.
    // This timer re-checks geometry several times to converge on the correct rect.
    m_settleTimer.setSingleShot(true);
    connect(&m_settleTimer, &QTimer::timeout, this, [this]() {
        if (m_target && m_settleCount > 0) {
            m_lastRect = QRect(); // force update
            updateGeometry();
            --m_settleCount;
            if (m_settleCount > 0)
                m_settleTimer.start(50);
        }
    });
}

void BorderOverlay::attach(intptr_t targetHwnd)
{
    if (m_target == targetHwnd)
        return;

    m_target = targetHwnd;
    m_lastRect = QRect();

    if (!updateGeometry()) {
        m_target = 0;
        return;
    }

    // Apply Win32 styles AFTER the window handle is created.
    HWND self = reinterpret_cast<HWND>(winId());
    LONG_PTR exStyle = GetWindowLongPtrW(self, GWL_EXSTYLE);
    exStyle |= WS_EX_TRANSPARENT | WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW;
    SetWindowLongPtrW(self, GWL_EXSTYLE, exStyle);

    show();

    // Ensure overlay sits just above the target in Z-order.
    HWND target = reinterpret_cast<HWND>(m_target);
    SetWindowPos(self, target, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOSENDCHANGING);

    scheduleSettleUpdates();
}

void BorderOverlay::detach()
{
    m_settleTimer.stop();
    m_target = 0;
    m_lastRect = QRect();
    hide();
}

void BorderOverlay::scheduleSettleUpdates()
{
    // Re-check geometry a few times after attach to account for DWM lag.
    m_settleCount = 4;
    m_settleTimer.start(50);
}

bool BorderOverlay::updateGeometry()
{
    if (!m_target)
        return false;

    HWND target = reinterpret_cast<HWND>(m_target);

    if (!IsWindow(target))
        return false;

    // Prefer DwmGetWindowAttribute for accurate visible bounds.
    RECT frame{};
    HRESULT hr = DwmGetWindowAttribute(target, DWMWA_EXTENDED_FRAME_BOUNDS,
                                        &frame, sizeof(frame));
    if (FAILED(hr)) {
        if (!GetWindowRect(target, &frame))
            return false;
    }

    const int t = kBorderThickness;
    QRect rect(frame.left - t, frame.top - t,
               (frame.right - frame.left) + t * 2,
               (frame.bottom - frame.top) + t * 2);

    // Skip if nothing changed.
    if (rect == m_lastRect)
        return true;

    m_lastRect = rect;

    // Use Win32 API directly for precise positioning.
    HWND self = reinterpret_cast<HWND>(winId());
    SetWindowPos(self, nullptr, rect.x(), rect.y(), rect.width(), rect.height(),
                 SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING);

    // Re-apply Z-order.
    SetWindowPos(self, target, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOSENDCHANGING);

    return true;
}

void BorderOverlay::paintEvent(QPaintEvent *)
{
    if (!m_target)
        return;

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    // Clear with fully transparent background.
    p.setCompositionMode(QPainter::CompositionMode_Clear);
    p.fillRect(rect(), Qt::transparent);
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);

    const int t = kBorderThickness;
    const QColor color(kBorderColor);

    // Draw outer filled rect, then cut out the center to leave just the border.
    QPainterPath outer;
    outer.addRoundedRect(QRectF(0, 0, width(), height()), 8, 8);

    QPainterPath inner;
    inner.addRoundedRect(QRectF(t, t, width() - t * 2, height() - t * 2), 6, 6);

    p.setPen(Qt::NoPen);
    p.setBrush(color);
    p.drawPath(outer - inner);
}

bool BorderOverlay::nativeEvent(const QByteArray &eventType, void *message,
                                 qintptr *result)
{
    Q_UNUSED(eventType);
    MSG *msg = static_cast<MSG *>(message);
    if (msg->message == WM_NCHITTEST) {
        // Click-through: let all mouse input pass to the window beneath.
        if (result)
            *result = HTTRANSPARENT;
        return true;
    }
    return false;
}
