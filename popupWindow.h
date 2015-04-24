#ifndef POPUPWINDOW_H
#define POPUPWINDOW_H

#include <QQuickWindow>
#include <QQuickItem>
#include <QMouseEvent>

class popupWindow : public QQuickWindow
{
    Q_OBJECT
    Q_PROPERTY(QQuickItem *parentItem READ parentItem WRITE setParentItem)

public:
    explicit popupWindow(QWindow *parent = 0);
    QQuickItem *parentItem() const { return m_pParentItem; }
    void setParentItem(QQuickItem *item);

private:
    QQuickItem *m_pParentItem;
    bool m_mouseMoved;
    void forwardEventToTransientParent(QMouseEvent *ev);

protected:
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);

public slots:
    void show();

signals:
    void dismissed();
};

#endif // POPUPWINDOW_H
