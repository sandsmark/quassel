#ifndef QMLUI_H
#define QMLUI_H

#include "abstractui.h"

class MainWin;
class MessageModel;

//! This class encapsulates Quassel's Qt-based GUI.
/** This is basically a wrapper around QQuickView, which is necessary because we cannot derive MainWin
 *  from both QQuickView and AbstractUi (because of multiple inheritance of QObject).
 */
class QmlUi : public AbstractUi
{
    Q_OBJECT

public:
    QmlUi();
    ~QmlUi();

    MessageModel *createMessageModel(QObject *parent);
    AbstractMessageProcessor *createMessageProcessor(QObject *parent);

    inline static QmlUi *instance();
    inline static MainWin *mainWindow();

public slots:
    virtual void init();

protected slots:
    void connectedToCore();
    void disconnectedFromCore();

protected:

private slots:

private:
    static QmlUi *_instance;
    static MainWin *_mainWin;
};


QmlUi *QmlUi::instance() { return _instance ? _instance : new QmlUi(); }
MainWin *QmlUi::mainWindow() { return _mainWin; }

#endif
