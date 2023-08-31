#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QMainWindow>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QDebug>
#include <QMenu>
#include <QMessageBox>
#include <QPainter>
#include <QTimer>
#include <QMessageBox>
#include <QPainter>

#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>

#include "weatherdata.h"
#include "WeatherTool.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

   protected:

    // 重写父类的方法
    void contextMenuEvent(QContextMenuEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

    // 获取天气数据、解析JSON、更新UI
    void getWeatherInfo(QString cityName);
    void PreseJSON(QByteArray& byteArray);
    void UpdataUI();

    // 重写父类的eventfilter方法
    bool eventFilter(QObject* watched, QEvent* event);

    // 绘制高低温曲线
    void paintHighCurve();
    void paintLowCurve();

private slots:
    void on_btnSearch_clicked();

private:
    void onReplied(QNetworkReply* reply);     //网络接收的槽函数
    Ui::MainWindow* ui;

    QMenu* mExitMenu;   // 右键退出的菜单
    QAction* mExitAct;  // 退出的行为
    QPoint mOffset; //窗口移动时，鼠标与窗口左上角的偏移

    QNetworkAccessManager* mNetworkAccessManager;  //发起网络请求的对象
    Today mToday;
    Day mDay[6];

    //星期和日期
    QList<QLabel*> mWeekList;
    QList<QLabel*> mDateList;

    //天气和天气图标
    QList<QLabel*> mTypeList;
    QList<QLabel*> mTypeIconList;

    //天气污染指数
    QList<QLabel*> mAqiList;

    //风力和风向
    QList<QLabel*> mFxList;
    QList<QLabel*> mFlList;

    QMap<QString, QString> mTypeMap;
};
#endif  // MAINWINDOW_H
