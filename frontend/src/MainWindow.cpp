#include "MainWindow.hpp"
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSslSocket>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent),
    sslSocket_(new QSslSocket(this))
{
    setupUi();


    connect(sslSocket_, &QSslSocket::encrypted,
            this, &MainWindow::onSslConnected);
    connect(sslSocket_, &QSslSocket::readyRead,
            this, &MainWindow::onReadyRead);
    connect(sslSocket_, &QSslSocket::errorOccurred,
            this, &MainWindow::onErrorOccurred);


    connect(cmdCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onCommandChanged);
    connect(sendBtn_, &QPushButton::clicked,
            this, &MainWindow::onSendClicked);

    onCommandChanged(cmdCombo_->currentIndex());
}

void MainWindow::setupUi() {
    auto *central = new QWidget(this);
    setCentralWidget(central);

    auto *layout = new QVBoxLayout(central);


    auto *hp = new QHBoxLayout;
    hp->addWidget(new QLabel("Host:"));
    hostEdit_ = new QLineEdit("localhost");
    hp->addWidget(hostEdit_);
    hp->addWidget(new QLabel("Port:"));
    portEdit_ = new QLineEdit("5500");
    hp->addWidget(portEdit_);
    layout->addLayout(hp);

    auto *cmdLay = new QHBoxLayout;
    cmdLay->addWidget(new QLabel("Command:"));
    cmdCombo_ = new QComboBox;
    cmdCombo_->addItems({"CREATE","DEPOSIT","WITHDRAW","TRANSFER","BALANCE"});
    cmdLay->addWidget(cmdCombo_);
    layout->addLayout(cmdLay);

    auto *fLay = new QHBoxLayout;
    fLay->addWidget(new QLabel("ID:"));
    idEdit_ = new QLineEdit;
    fLay->addWidget(idEdit_);
    fLay->addWidget(new QLabel("To ID:"));
    toIdEdit_ = new QLineEdit;
    fLay->addWidget(toIdEdit_);
    fLay->addWidget(new QLabel("Amount:"));
    amountEdit_ = new QLineEdit;
    fLay->addWidget(amountEdit_);
    layout->addLayout(fLay);

    sendBtn_ = new QPushButton("Send");
    layout->addWidget(sendBtn_);

    logEdit_ = new QTextEdit;
    logEdit_->setReadOnly(true);
    layout->addWidget(logEdit_);

    setWindowTitle("Banking Client (Qt TLS)");
    resize(500, 400);
}

void MainWindow::onCommandChanged(int idx) {
    bool isTransfer = (idx == cmdCombo_->findText("TRANSFER"));
    bool isBalance  = (idx == cmdCombo_->findText("BALANCE"));
    toIdEdit_->setEnabled(isTransfer);
    amountEdit_->setEnabled(!isBalance);
}

void MainWindow::onSendClicked() {
    logEdit_->append("Connecting...");
    if (sslSocket_->isOpen()) sslSocket_->disconnectFromHost();

    QString host = hostEdit_->text();
    int port     = portEdit_->text().toInt();
    sslSocket_->setPeerVerifyMode(QSslSocket::VerifyNone);
    sslSocket_->connectToHostEncrypted(host, port);
}

void MainWindow::onSslConnected() {
    logEdit_->append("Encrypted connection established.");

    QString cmd = cmdCombo_->currentText();
    QString id  = idEdit_->text();
    QString to  = toIdEdit_->text();
    QString amt = amountEdit_->text();

    QString full;
    if (cmd == "TRANSFER")
        full = QString("%1 %2 %3 %4\n").arg(cmd, id, to, amt);
    else if (cmd == "BALANCE")
        full = QString("%1 %2\n").arg(cmd, id);
    else
        full = QString("%1 %2 %3\n").arg(cmd, id, amt);

    logEdit_->append(">> " + full.trimmed());
    sslSocket_->write(full.toUtf8());
}

void MainWindow::onReadyRead() {
    QByteArray resp = sslSocket_->readAll();
    logEdit_->append("<< " + QString::fromUtf8(resp).trimmed());
    sslSocket_->disconnectFromHost();
}

void MainWindow::onErrorOccurred(QAbstractSocket::SocketError err) {
    Q_UNUSED(err);
    logEdit_->append("Error: " + sslSocket_->errorString());
}
