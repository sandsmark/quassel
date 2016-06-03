#ifndef QMLUIAPPLICATION_H_
#define QMLUIAPPLICATION_H_

#include <QGuiApplication>

#include "quassel.h"

class QmlUi;

class QmlUiApplication : public QGuiApplication, public Quassel
{
    Q_OBJECT

public:
    QmlUiApplication(int &, char **);
    ~QmlUiApplication();
    virtual bool init();

protected:
    virtual void quit();
};

#endif
