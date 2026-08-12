#pragma once
//
// BorderOverlay — a single transparent Win32 overlay window that draws a red
// border around a pinned target window. The overlay is click-through,
// focus-neutral, and hidden from Alt+Tab / taskbar.
//
#include <QWidget>
#include <QTimer>
#include <cstdint>

class BorderOverlay : public QWidget
{
    Q_OBJECT
public:
    explicit BorderOverlay(QWidget *parent = nullptr);

    // Attach to a target window and show the border.
    void attach(intptr_t targetHwnd);

    // Update overlay position/size to match the target window.
    // Returns false if the target window is gone.
    bool updateGeometry();

    // Detach and hide the overlay.
    void detach();

    intptr_t targetHwnd() const { return m_target; }
    bool isAttached() const { return m_target != 0; }

    // Border appearance.
    static constexpr int kBorderThickness = 3;
    static constexpr QRgb kBorderColor = 0xFFE53935; // red

protected:
    void paintEvent(QPaintEvent *) override;
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;

private:
    void scheduleSettleUpdates();

    intptr_t m_target = 0;
    QRect m_lastRect;
    QTimer m_settleTimer;
    int m_settleCount = 0;
};
