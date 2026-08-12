#pragma once
//
// Toast — lightweight global notification that appears centered at a fixed
// percentage from the top of the screen, then auto-fades away.
//
#include <QWidget>
#include <QTimer>

class QLabel;
class QPropertyAnimation;

class Toast : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(float opacity READ opacity WRITE setOpacity)

public:
    explicit Toast(QWidget *parent = nullptr);

    // Show a toast message. Duration is in milliseconds.
    void show(const QString &text, int durationMs = 2500);

    float opacity() const { return m_opacity; }
    void setOpacity(float o);

protected:
    void paintEvent(QPaintEvent *) override;

private:
    QLabel *m_label = nullptr;
    QTimer m_timer;
    QPropertyAnimation *m_fadeIn = nullptr;
    QPropertyAnimation *m_fadeOut = nullptr;
    float m_opacity = 0.0f;

    void positionOnScreen();
};
