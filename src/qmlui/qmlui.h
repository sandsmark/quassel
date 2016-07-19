#ifndef QMLUI_H
#define QMLUI_H

#include "abstractui.h"

#ifdef USE_UBUNTU
class MainWin;
#endif
class MessageModel;
class QmlUiBufferModel;

//! This class encapsulates Quassel's Qt-based GUI.
/** This is basically a wrapper around QQuickView, which is necessary because we cannot derive MainWin
 *  from both QQuickView and AbstractUi (because of multiple inheritance of QObject).
 */
class QmlUi : public AbstractUi
{
    Q_OBJECT

public:
    ~QmlUi();

    MessageModel *createMessageModel(QObject *parent);
    AbstractMessageProcessor *createMessageProcessor(QObject *parent);

    static QmlUi *instance();

public slots:
    virtual void init();

signals:
    void coreConnected();
    void coreDisconnected();

protected slots:
    void connectedToCore();
    void disconnectedFromCore();

protected:

private slots:
    void addBufferView(int bufferViewConfigId);
    void removeBufferView(int);
    void loadLayout();

private:
    QmlUi();

    QmlUiBufferModel *m_bufferModel;

#ifdef USE_UBUNTU
    MainWin *_mainWin;
#endif
};

#endif
