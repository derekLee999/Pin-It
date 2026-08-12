#include "toast.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QScreen>
#include <QGuiApplication>
#include <QPainter>
#include <QPainterPath>

Toast::Toast(QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint
                       | Qt::Tool | Qt::WindowDoesNotAcceptFocus)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setAttribute(Qt::WA_DeleteOnClose, false);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 10, 20, 10);

    m_label = new QLabel(this);
    m_label->setAlignment(Qt::AlignCenter);
    m_label->setStyleSheet(QStringLiteral(
        "color: #ffffff;"
        "font-size: 20px;"
        "font-weight: 600;"
        "font-family: 'Segoe UI';"
        "padding: 0;"));
    layout->addWidget(m_label);

    setStyleSheet(QStringLiteral(
        "Toast {"
        "  background: transparent;"
        "}"));

    // Fade-in animation.
    m_fadeIn = new QPropertyAnimation(this, "opacity", this);
    m_fadeIn->setDuration(150);
    m_fadeIn->setStartValue(0.0f);
    m_fadeIn->setEndValue(1.0f);

    // Fade-out animation.
    m_fadeOut = new QPropertyAnimation(this, "opacity", this);
    m_fadeOut->setDuration(300);
    m_fadeOut->setStartValue(1.0f);
    m_fadeOut->setEndValue(0.0f);
    connect(m_fadeOut, &QPropertyAnimation::finished, this, &QWidget::hide);

    // Auto-hide timer.
    m_timer.setSingleShot(true);
    connect(&m_timer, &QTimer::timeout, this, [this]() {
        m_fadeOut->start();
    });
}

void Toast::setOpacity(float o)
{
    m_opacity = o;
    setWindowOpacity(static_cast<qreal>(o));
}

void Toast::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(QColor(255, 255, 255, 20), 1));
    p.setBrush(QColor(45, 113, 251, 230));
    p.drawRoundedRect(rect().adjusted(0, 0, -1, -1), 10, 10);
}

void Toast::show(const QString &text, int durationMs)
{
    m_label->setText(text);

    // Resize to fit content.
    adjustSize();
    // Enforce a minimum width so very short messages look balanced.
    if (width() < 180)
        resize(180, height());

    positionOnScreen();

    // Reset animations.
    m_timer.stop();
    m_fadeOut->stop();

    QWidget::show();
    m_fadeIn->start();
    m_timer.start(durationMs);
}

void Toast::positionOnScreen()
{
    if (QScreen *screen = QGuiApplication::primaryScreen()) {
        const QRect geo = screen->availableGeometry();
        const int x = geo.x() + (geo.width() - width()) / 2;
        const int y = geo.y() + static_cast<int>(geo.height() * 0.30) - height() / 2;
        move(x, y);
    }
}
