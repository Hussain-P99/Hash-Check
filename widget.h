#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QCryptographicHash>
#include <QFutureWatcher>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void getFilename();
    void getHash();
    void selectHashFunction(int index = -1);
    bool verifyHash(QString inputHash);

private slots:
    void on_sFileButton_clicked();

    void on_calculateHash_clicked();

    void caclCompleted();

    void on_hashCombo_currentIndexChanged(int index);

    void on_checkButton_clicked();

    void on_inputHash_returnPressed();

protected:
    void closeEvent(QCloseEvent *event) override;
private:
    Ui::Widget *ui;
    QString filename;
    QByteArray dataHash;
    unsigned long size;
    QCryptographicHash::Algorithm hashfunc;
    QFutureWatcher<void> watcher;
};
#endif // WIDGET_H
