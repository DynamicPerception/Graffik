#include "popupWindow.h"
#include <QDebug>
#include <QFocusEvent>
#include <QGuiApplication>
#include <QQuickRenderControl>
//#include <qpa/qwindowsysteminterface.h>

popupWindow::popupWindow() :
    QQuickWindow()
{
    setFlags(Qt::Tool | Qt::Window | Qt::FramelessWindowHint);
    //setFlags(Qt::Popup);
    m_dismissed = false;
    m_mouseMoved = false;
    m_needsActivatedEvent = true;

    m_xOffset = 0;
    m_yOffset = 0;

    connect(qApp, SIGNAL(applicationStateChanged(Qt::ApplicationState)),
            this, SLOT(applicationStateChanged(Qt::ApplicationState)));
}

void popupWindow::setParentItem(QQuickItem *item) {
    m_pParentItem = item;
    if(m_pParentItem)
        setTransientParent(item->window());
}

void popupWindow::applicationStateChanged(Qt::ApplicationState state) {
    if (state != Qt::ApplicationActive)
        dismissPopup();
}

void popupWindow::setYOffset(int offset) {
    if(m_yOffset != offset) {
        m_yOffset = offset;
        updateSize();
        emit yOffsetChanged();
    }
}

void popupWindow::setXOffset(int offset)  {
    if(m_xOffset != offset) {
        m_yOffset = offset;
        updateSize();
        emit xOffsetChanged();
    }
}

void popupWindow::dismissPopup() {
    m_dismissed = true;
    emit popupDismissed();
    close();
}

void popupWindow::show() {
    qreal posx = x();
    qreal posy = y();
    // transientParent may not be a QQuickWindow when embedding into widgets
    if (QWindow *tp = transientParent()) {
        if (m_pParentItem) {
            QPointF pos = m_pParentItem->mapToItem(m_pParentItem->window()->contentItem(), QPointF());
            posx = pos.x();
            posy = pos.y();
        }

        QPoint tlwOffset = tp->mapToGlobal(QPoint());
        posx += tlwOffset.x();
        posy += tlwOffset.y();
    } else if (m_pParentItem && m_pParentItem->window()) {
        QPoint offset;
        QQuickWindow *quickWindow = m_pParentItem->window();
        QWindow *renderWindow = QQuickRenderControl::renderWindowFor(quickWindow, &offset);

        QPointF pos = m_pParentItem->mapToItem(quickWindow->contentItem(), QPointF(posx, posy));
        posx = pos.x();
        posy = pos.y();

        QPoint parentWindowOffset = (renderWindow ? renderWindow : quickWindow)->mapToGlobal(QPoint());
        posx += offset.x() + parentWindowOffset.x();
        posy += offset.y() + parentWindowOffset.y();
    }

    setGeometry(posx + m_xOffset, posy + m_yOffset, width(), height());
    emit geometryChanged();

    m_mouseMoved = false;
    QQuickWindow::show();
    setMouseGrabEnabled(true); // Needs to be done after calling show()
    //setKeyboardGrabEnabled(true);
}

void popupWindow::forwardEventToTransientParent(QMouseEvent *e) {
    if (!qobject_cast<popupWindow*>(transientParent())
        && ((m_mouseMoved && e->type() == QEvent::MouseButtonRelease)
        || e->type() == QEvent::MouseButtonPress))
    {
        // Clicked outside any popup
        dismissPopup();
    }
    if (transientParent()) {
        QPoint parentPos = transientParent()->mapFromGlobal(mapToGlobal(e->pos()));
        QMouseEvent pe = QMouseEvent(e->type(), parentPos, e->button(), e->buttons(), e->modifiers());
        QGuiApplication::sendEvent(transientParent(), &pe);
    }
}

void popupWindow::mouseMoveEvent(QMouseEvent *e) {
    QRect rect = QRect(QPoint(), size());
    m_mouseMoved = true;
    if (rect.contains(e->pos())) {
        QQuickWindow::mouseMoveEvent(e);
    } else {
        forwardEventToTransientParent(e);
    }
}

void popupWindow::mousePressEvent(QMouseEvent *e) {
    QRect rect = QRect(QPoint(), size());
    if (rect.contains(e->pos())) {
        QQuickWindow::mousePressEvent(e);
    } else {
        forwardEventToTransientParent(e);
    }
}

void popupWindow::mouseReleaseEvent(QMouseEvent *e) {
    QRect rect = QRect(QPoint(), size());
    if (rect.contains(e->pos())) {
        if(m_mouseMoved) {
            QMouseEvent pe = QMouseEvent(QEvent::MouseButtonPress, e->pos(), e->button(),
                                         e->buttons(), e->modifiers());
            QQuickWindow::mousePressEvent(&pe);
            QQuickWindow::mouseReleaseEvent(e);
        }
        m_mouseMoved = true; // Initial mouse release counts as move.
    } else {
        forwardEventToTransientParent(e);
    }
}

void popupWindow::updateSize() {
    setGeometry(x(), y(), width(), height());
    emit geometryChanged();
}
