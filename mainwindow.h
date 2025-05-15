#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPushButton>
#include <QMessageBox>
#include <QFileDialog>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void loadJson();
    void startDownload(const QString &url);
    void useJdk(const QString &version);
    void handleDownloadFinished();
    void updateProgress(qint64 bytesReceived, qint64 bytesTotal);


private:
    Ui::MainWindow *ui;
    QFile file;
    QNetworkAccessManager *networkManager;
};
#endif // MAINWINDOW_H
