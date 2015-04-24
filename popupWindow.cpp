#include "popupWindow.h"
#include <QDebug>
#include <QFocusEvent>
#include <QGuiApplication>

popupWindow::popupWindow(QWindow *parent) :
    QQuickWindow(parent)
{
    setFlags(Qt::Popup);
    setModality(Qt::WindowModal);
    m_mouseMoved = false;
}

void popupWindow::setParentItem(QQuickItem *item) {
    m_pParentItem = item;
    setTransientParent(item->window());
}

void popupWindow::show() {
    QQuickWindow::show();
    setMouseGrabEnabled(true); // Needs to be done after calling show()
}

void popupWindow::forwardEventToTransientParent(QMouseEvent *e) {
    if(((m_mouseMoved && e->type() == QEvent::MouseButtonRelease) ||
         e->type() == QEvent::MouseButtonPress))
    {
        //close window
        hide();
        emit dismissed();
    }

    if(transientParent()) {
        QPoint parentPos = transientParent()->mapFromGlobal(mapToGlobal(e->pos()));
        QMouseEvent pe = QMouseEvent(e->type(), parentPos, e->button(), e->buttons(), e->modifiers());
        QGuiApplication::sendEvent(transientParent(), &pe);
    }
}

void popupWindow::mousePressEvent(QMouseEvent *e) {
    QRect rect = QRect(QPoint(), size());
    if(!rect.contains(e->pos()))
        forwardEventToTransientParent(e);
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

void popupWindow::mouseMoveEvent(QMouseEvent *e) {
    QRect rect = QRect(QPoint(), size());
    m_mouseMoved = true;
    if(rect.contains(e->pos()))
        QQuickWindow::mouseMoveEvent(e);
    else
        forwardEventToTransientParent(e);
}

