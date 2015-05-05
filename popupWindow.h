#ifndef POPUPWINDOW_H
#define POPUPWINDOW_H

#include <QQuickWindow>
#include <QQuickItem>
#include <QMouseEvent>

class popupWindow : public QQuickWindow
{
    Q_OBJECT
    Q_PROPERTY(QQuickItem *parentItem READ parentItem WRITE setParentItem)
    Q_PROPERTY(int yOffset READ yOffset WRITE setYOffset NOTIFY yOffsetChanged)
    Q_PROPERTY(int xOffset READ xOffset WRITE setXOffset NOTIFY xOffsetChanged)

public:
    explicit popupWindow();
    QQuickItem *parentItem() const { return m_pParentItem; }
    virtual void setParentItem(QQuickItem *item);
    void setYOffset(int offset);
    void setXOffset(int offset);
    int yOffset() const { return m_yOffset; }
    int xOffset() const { return m_xOffset; }

private:
    QQuickItem *m_pParentItem;
    int m_yOffset;
    int m_xOffset;
    bool m_dismissed;
    bool m_mouseMoved;
    bool m_needsActivatedEvent;
    void forwardEventToTransientParent(QMouseEvent *ev);

protected:
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);

protected slots:
    void updateSize();
    void applicationStateChanged(Qt::ApplicationState state);

public slots:
    virtual void show();
    void dismissPopup();

signals:
    void dismissed();
    void geometryChanged();
    void yOffsetChanged();
    void xOffsetChanged();
    void popupDismissed();
};

#endif // POPUPWINDOW_H
