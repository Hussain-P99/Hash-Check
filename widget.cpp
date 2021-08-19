#include "widget.h"
#include "ui_widget.h"
#include <QFile>
#include <QFileDialog>
#include <QCryptographicHash>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>
#include <QCloseEvent>
#include <QMessageBox>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    connect(&watcher,&QFutureWatcher<void>::finished,this,&Widget::caclCompleted);

    ui->hashCombo->addItem("MD5");
    ui->hashCombo->addItem("SHA1");
    ui->hashCombo->addItem("SHA256");

    ui->calculateHash->setDisabled(true);
    ui->inputHash->setDisabled(true);
    ui->checkButton->setDisabled(true);
    setFixedSize(600,400);
}

Widget::~Widget()
{
    delete ui;
}


void Widget::closeEvent(QCloseEvent *event){
    if(watcher.isRunning()){
        int ret = QMessageBox::warning(this,"Close Application",
                                       "Do you want to cancel operation and exit ?",
                                       QMessageBox::Yes | QMessageBox::No);
        if(ret == QMessageBox::Yes){
            watcher.cancel();
            event->accept();
        } else if(ret == QMessageBox::No){
            event->ignore();
        }
    }
}

void Widget::getFilename(){
    filename = QFileDialog::getOpenFileName(this,"Open a file");
    if(!filename.isNull()){
        ui->calculateHash->setDisabled(false);
        QFileInfo finfo(filename);
        size = finfo.size();
        ui->filename->setText(finfo.completeBaseName());
        ui->fileSize->setText(QString("%1").arg(size));
        ui->fileHash->setText("-");
    } else {
        QMessageBox::warning(this,"Warning","No File Selected.");
    }
}

void Widget::getHash(){
    if(filename.isNull()){
        return;
    }

    QFile ifile(filename);
    if(!ifile.open(QIODevice::ReadOnly)){
        return;
    }

    QCryptographicHash hashit(hashfunc);

    int chunksize(1024);
    while(!ifile.atEnd()){
        hashit.addData(ifile.read(chunksize));
    }

    dataHash = hashit.result().toHex();
}

void Widget::selectHashFunction(int index){
    if(index < 0)
        index = ui->hashCombo->currentIndex();
    QString t = ui->hashCombo->itemText(index);

    if(t == "MD5"){
        hashfunc = QCryptographicHash::Md5;
    } else if(t == "SHA1"){
        hashfunc = QCryptographicHash::Sha1;
    } else if(t == "SHA256"){
        hashfunc = QCryptographicHash::Sha256;
    } else {
        hashfunc = QCryptographicHash::Md5;
    }
}

void Widget::on_sFileButton_clicked()
{
    getFilename();
}

void Widget::on_calculateHash_clicked()
{
    QFuture<void> hashrun = QtConcurrent::run(this,&Widget::getHash);
    watcher.setFuture(hashrun);
    ui->fileHash->setText("Calculating Hash...");
    ui->calculateHash->setDisabled(true);
    ui->inputHash->clear();
    ui->inputHash->setDisabled(true);
    ui->hashMatchStatus->clear();
}

void Widget::caclCompleted(){
    ui->fileHash->setText(dataHash);
    ui->inputHash->setDisabled(false);
    ui->calculateHash->setDisabled(false);
    ui->checkButton->setDisabled(false);
}

void Widget::on_hashCombo_currentIndexChanged(int index)
{
    selectHashFunction(index);
}

bool Widget::verifyHash(QString inputHash){
    if(dataHash == inputHash){
        return true;
    } else {
        return false;
    }
}

void Widget::on_checkButton_clicked()
{
    if(verifyHash(ui->inputHash->text().trimmed())){
        ui->hashMatchStatus->setPixmap(QPixmap(":/icons/check.png"));
    } else {
        ui->hashMatchStatus->setPixmap(QPixmap(":/icons/cross.png"));
    }
}

void Widget::on_inputHash_returnPressed()
{
    on_checkButton_clicked();
}
