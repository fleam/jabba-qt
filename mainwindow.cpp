#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QHBoxLayout>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), networkManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
    setWindowTitle("JDK 版本下载工具");

    // 初始化表格
    ui->tableWidget->setColumnCount(6);
    ui->tableWidget->setHorizontalHeaderLabels({"系统", "架构", "发行商", "版本", "下载链接", "操作"});
    ui->tableWidget->setColumnWidth(4, 400); // 调整链接列宽度
    ui->tableWidget->setColumnWidth(5, 150); // 调整操作列宽度

    loadJson(); // 加载JSON数据
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::loadJson() {
    QFile jsonFile("D:\\Documents\\jabba\\index.json");
    if (!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法打开 JSON 文件");
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(jsonFile.readAll());
    jsonFile.close();

    if (doc.isNull() || !doc.isObject()) {
        QMessageBox::warning(this, "错误", "JSON 格式无效");
        return;
    }

    QJsonObject root = doc.object();
    const QStringList osList = {"windows", "linux", "darwin"};

    foreach (const QString &os, osList) {
        if (!root.contains(os)) continue;

        QJsonObject osObj = root[os].toObject();
        QJsonObject::iterator archIt = osObj.begin();

        while (archIt != osObj.end()) {
            QString arch = archIt.key();
            QJsonObject archObj = archIt.value().toObject();
            QJsonObject::iterator vendorIt = archObj.begin();

            while (vendorIt != archObj.end()) {
                QString vendorKey = vendorIt.key();
                QString vendor = vendorKey.split("@").last(); // 提取发行商名称（如 zulu/openjdk）
                QJsonObject versionObj = vendorIt.value().toObject();
                QJsonObject::iterator versionIt = versionObj.begin();

                while (versionIt != versionObj.end()) {
                    QString version = versionIt.key();
                    QString downloadUrl = versionIt.value().toString().split("+").last(); // 提取URL

                    // 添加表格行
                    int row = ui->tableWidget->rowCount();
                    ui->tableWidget->insertRow(row);

                    // 填充单元格
                    ui->tableWidget->setItem(row, 0, new QTableWidgetItem(os));
                    ui->tableWidget->setItem(row, 1, new QTableWidgetItem(arch));
                    ui->tableWidget->setItem(row, 2, new QTableWidgetItem(vendor));
                    ui->tableWidget->setItem(row, 3, new QTableWidgetItem(version));
                    ui->tableWidget->setItem(row, 4, new QTableWidgetItem(downloadUrl));

                    // // 添加下载按钮
                    // QPushButton *downloadBtn = new QPushButton("下载");
                    // connect(downloadBtn, &QPushButton::clicked, [this, downloadUrl]() {
                    //     startDownload(downloadUrl);
                    // });
                    // // ui->tableWidget->setCellWidget(row, 5, downloadBtn);

                    // 添加操作容器
                    QWidget *actionTable = new QWidget();
                    QHBoxLayout *actionLayout = new QHBoxLayout(actionTable);
                    actionLayout->setContentsMargins(0, 0, 0, 0); // 去除边距
                    actionTable->setLayout(actionLayout);

                    // 添加下载按钮
                    QPushButton *downloadBtn = new QPushButton("下载");
                    connect(downloadBtn, &QPushButton::clicked, [this, downloadUrl]() {
                        startDownload(downloadUrl);
                    });
                    actionLayout->addWidget(downloadBtn); // 添加到容器布局

                    // 添加切换按钮
                    QPushButton *useBtn = new QPushButton("切换");
                    connect(useBtn, &QPushButton::clicked, [this, version]() {
                        useJdk(version); // 假设 version 是当前行的版本号
                    });
                    actionLayout->addWidget(useBtn); // 添加到容器布局

                    // 将容器设置为单元格部件
                    ui->tableWidget->setCellWidget(row, 5,actionTable);

                    versionIt++;
                }
                vendorIt++;
            }
            archIt++;
        }
    }
}

void MainWindow::startDownload(const QString &url) {
    QString fileName = QFileDialog::getSaveFileName(this, "保存文件", QUrl(url).fileName(), "所有文件 (*.*)");
    if (fileName.isEmpty()) return;

    file.setFileName(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "错误", "无法保存文件");
        return;
    }

    // QNetworkRequest request(QUrl(url));
    QNetworkRequest request;
    QUrl newUrl(url);
    request.setUrl(newUrl);
    QNetworkReply *reply = networkManager->get(request);
    connect(reply, &QNetworkReply::downloadProgress, this, &MainWindow::updateProgress);
    connect(reply, &QNetworkReply::finished, this, &MainWindow::handleDownloadFinished);
}

void MainWindow::useJdk(const QString &version){
    qDebug() << version;
}


void MainWindow::updateProgress(qint64 bytesReceived, qint64 bytesTotal) {
    // 可添加进度条显示（示例中用控制台输出）
    qDebug() << QString("下载进度：%1%").arg(bytesReceived * 100 / bytesTotal);
}

void MainWindow::handleDownloadFinished() { // 修改槽函数参数
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) {
        QMessageBox::critical(this, "错误", "下载请求已失效");
        return;
    }

    if (reply->error()) {
        QMessageBox::critical(this, "错误", QString("下载失败：%1").arg(reply->errorString()));
    } else {
        QByteArray data = reply->readAll();
        if (file.write(data) == data.size()) {
            QMessageBox::information(this, "完成", "文件下载成功！");
        } else {
            QMessageBox::warning(this, "警告", "文件写入不完整");
        }
    }
    reply->deleteLater();
    file.close();
}



