#pragma once

#include <QMainWindow>
#include <QSslSocket>

class QLineEdit;
class QComboBox;
class QPushButton;
class QTextEdit;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void onCommandChanged(int index);
    void onSendClicked();
    void onSslConnected();
    void onReadyRead();
    void onErrorOccurred(QAbstractSocket::SocketError);

private:
    void setupUi();

    QSslSocket *sslSocket_;
    QLineEdit   *hostEdit_;
    QLineEdit   *portEdit_;
    QComboBox   *cmdCombo_;
    QLineEdit   *idEdit_;
    QLineEdit   *toIdEdit_;
    QLineEdit   *amountEdit_;
    QPushButton *sendBtn_;
    QTextEdit   *logEdit_;
};
