#ifndef CNOTIFY_H
#define CNOTIFY_H


#include <QtWidgets/QDialog>
#include <QtWidgets/QWidget>
#include <QString>
#include <QStringList>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QStatusBar>
#include <QTimer>

namespace Ui {
class CNotify;
}

class CNotify : public QDialog
{
    Q_OBJECT
    
public:
    explicit CNotify(QWidget *parent = 0);
    ~CNotify();

    void execWarning(QString);
    void execInfo(QString);
    void execError(QString);
    void execLinkError(int number);
    void execNotConnected(void);
    int execPrompt(QString);

    static void setUpdateObjects(QStatusBar * statusBar, QProgressBar * progressBar);
    static void updateStatus(QString status);
    static void updateProgress(int progress, int max, bool showprogress = true);

private slots:
    void on_okButton_clicked();
    void on_cancelButton_clicked();
    static void msgTimeout();

private:
    void setHeight(QString msg);
    Ui::CNotify *ui;
    static QStatusBar * statusBar;
    static QProgressBar * progressBar;
    static QStringList messages;
    static QTimer * msgTimer;
    static CNotify * notifyPtr;
};

#endif // CNOTIFY_H
