#include "mainwindow.h"
#include "ui_mainwindow.h"

#define INCREMENT 3 // 温度每升高/降低1°，y轴坐标的增量
#define POINT_RADIUS 3 // 曲线描点的大小
#define TEXT_OFFSET_X 12
#define TEXT_OFFSET_Y 12

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // 1. 设置窗口属性
    setWindowFlag(Qt::FramelessWindowHint);  // 无边框
    setFixedSize(width(), height());         // 固定窗口大小

    // 2. 右键菜单：退出程序
    mExitMenu = new QMenu(this);
    mExitAct = new QAction();
    mExitAct->setText(tr("退出"));
    mExitAct->setIcon(QIcon(":/res/close.png"));
    mExitMenu->addAction(mExitAct);

    connect(mExitAct, &QAction::triggered, this, [=]() { qApp->exit(0); });

    // 3. UI初始化
    //将控件添加到控件数组，方便使用循环进行处理
    //星期和日期
    mWeekList << ui->lblWeek0 << ui->lblWeek1 << ui->lblWeek2 << ui->lblWeek3 << ui->lblWeek4 << ui->lblWeek5;
    mDateList << ui->lblDate0 << ui->lblDate1 << ui->lblDate2 << ui->lblDate3 << ui->lblDate4 << ui->lblDate5;

    //天气和天气图标
    mTypeList << ui->lblType0 << ui->lblType1 << ui->lblType2 << ui->lblType3 << ui->lblType4 << ui->lblType5;
    mTypeIconList << ui->lblTypeIcon0 << ui->lblTypeIcon1 << ui->lblTypeIcon2 << ui->lblTypeIcon3 << ui->lblTypeIcon4 << ui->lblTypeIcon5;

    //天气指数
    mAqiList << ui->lblQuality0 << ui->lblQuality1 << ui->lblQuality2 << ui->lblQuality3 << ui->lblQuality4 << ui->lblQuality5;

    //风向和风力
    mFxList << ui->lblFx0 << ui->lblFx1 << ui->lblFx2 << ui->lblFx3 << ui->lblFx4 << ui->lblFx5;
    mFlList << ui->lblFl0 << ui->lblFl1 << ui->lblFl2 << ui->lblFl3 << ui->lblFl4 << ui->lblFl5;

    mTypeMap.insert("暴雪",":/res/type/BaoXue.png");
    mTypeMap.insert("暴雨",":/res/type/BaoYu.png");
    mTypeMap.insert("暴雨到大暴雨",":/res/type/BaoYuDaoDaBaoYu.png");
    mTypeMap.insert("大暴雨",":/res/type/DaBaoYu.png");
    mTypeMap.insert("大暴雨到特大暴雨",":/res/type/DaBaoYuDaoTeDaBaoYu.png");
    mTypeMap.insert("大到暴雪",":/res/type/DaDaoBaoXue.png");
    mTypeMap.insert("大雪",":/res/type/DaDaoBaoYu.png");
    mTypeMap.insert("大雨",":/res/type/DaYu.png");
    mTypeMap.insert("冻雨",":/res/type/DongYu.png");
    mTypeMap.insert("多云",":/res/type/DuoYun.png");
    mTypeMap.insert("浮沉",":/res/type/FuChen.png");
    mTypeMap.insert("雷阵雨",":/res/type/LeiZhenYu.png");
    mTypeMap.insert("雷阵雨伴有冰雹",":/res/type/LeiZhenYuBanYouBingBao.png");
    mTypeMap.insert("霾",":/res/type/Mai.png");
    mTypeMap.insert("强沙尘暴",":/res/type/QiangShaChenBao.png");
    mTypeMap.insert("晴",":/res/type/Qing.png");
    mTypeMap.insert("沙尘暴",":/res/type/ShaChenBao.png");
    mTypeMap.insert("特大暴雨",":/res/type/TeDaBaoYu.png");
    mTypeMap.insert("undefined",":/res/type/undefined.png");
    mTypeMap.insert("雾",":/res/type/Wu.png");
    mTypeMap.insert("小到中雪",":/res/type/XiaoDaoZhongXue.png");
    mTypeMap.insert("小到中雨",":/res/type/XiaoDaoZhongYu.png");
    mTypeMap.insert("小雪",":/res/type/XiaoXue.png");
    mTypeMap.insert("小雨",":/res/type/XiaoYu.png");
    mTypeMap.insert("雪",":/res/type/Xue.png");
    mTypeMap.insert("扬沙",":/res/type/YangSha.png");
    mTypeMap.insert("阴",":/res/type/Yin.png");
    mTypeMap.insert("雨",":/res/type/Yu.png");
    mTypeMap.insert("雨夹雪",":/res/type/YuJiaXue.png");
    mTypeMap.insert("阵雪",":/res/type/ZhenXue.png");
    mTypeMap.insert("阵雨",":/res/type/ZhenYu.png");
    mTypeMap.insert("中到大雪",":/res/type/ZhongDaoDaXue.png");
    mTypeMap.insert("中到大雨",":/res/type/ZhongDaoDaYu.png");
    mTypeMap.insert("中雪",":/res/type/ZhongXue.png");
    mTypeMap.insert("中雨",":/res/type/ZhongYu.png");

    // 4. 网络请求
    mNetworkAccessManager = new QNetworkAccessManager(this);
    //当mNetworkAccessManager请求数据成功后，自动调用reply函数
    connect(mNetworkAccessManager, &QNetworkAccessManager::finished, this, &MainWindow::onReplied);

    //在构造中请求天气数据
//    getWeatherInfo("101010100"); //"101010100"为北京城市编码
//    getWeatherInfo("101280101"); //"101280101"为广州城市编码
    getWeatherInfo("大连"); //"101070201"为大连城市编码，发送请求。

    // 5. 给标签添加事件过滤器
    // 参数指定为this，也就是当前窗口对象 MainWindow
    ui->lblHighCurve->installEventFilter(this);
    ui->lblLowCurve->installEventFilter(this);
}

MainWindow::~MainWindow() { delete ui; }

//重写父类的虚函数
//父类中默认是忽略右键菜单事件
//重写之后，就可以处理右键菜单
void MainWindow::contextMenuEvent(QContextMenuEvent* event) {
    mExitMenu->exec(QCursor::pos()); //弹出右键菜单,菜单在某个位置弹出
    event->accept(); //事件已处理，无须传递
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    mOffset = event->globalPos() - this->pos();  //mOffset：鼠标相对于窗口左上角的偏移位置、event->globalPos()：当前点的全局位置、this->pos()：当前窗口的全局位置
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() - mOffset);   //当前窗口左上角的全局位置
}

void MainWindow::getWeatherInfo(QString cityName)
{
    QString cityCode = WeatherTool::getCityCode(cityName);
    if(cityCode.isEmpty()){
        QMessageBox::warning(this, "天气", "请检查输入是否正确!", QMessageBox::Ok);
        return;
    }
    QUrl url("http://t.weather.itboy.net/api/weather/city/" + cityCode);
    mNetworkAccessManager->get(QNetworkRequest(url));   //发起GET网络请求
}

void MainWindow::PreseJSON(QByteArray &byteArray)
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(byteArray, &err);   //解析json对象
    if(err.error != QJsonParseError::NoError){
        return;
    }
    QJsonObject rootObj = doc.object();     //获取根json对象
//    qDebug() << rootObj.value("message").toString();

    // 1. 解析日期和城市
    mToday.date = rootObj.value("date").toString();
    mToday.city = rootObj.value("cityInfo").toObject().value("city").toString();

    // 2. 解析yesterday
    QJsonObject objData =  rootObj.value("data").toObject();
    QJsonObject objYesterday = objData.value("yesterday").toObject();

    mDay[0].week = objYesterday.value("week").toString();
    mDay[0].date = objYesterday.value("ymd").toString();
    mDay[0].type = objYesterday.value("type").toString();

    //高温 和 低温
    QString s;
    s = objYesterday.value("high").toString().split(" ").at(1);
    mDay[0].high =  s.left(s.length()-1).toInt();

    s = objYesterday.value("low").toString().split(" ").at(1);
    mDay[0].low = s.left(s.length()-1).toInt();

    //方向风力
    mDay[0].fx = objYesterday.value("fx").toString();
    mDay[0].fl = objYesterday.value("fl").toString();
    //污染指数
    mDay[0].aqi = objYesterday.value("aqi").toDouble();

    // 3. 解析forecast中5天的数据
    QJsonArray forecastArr = objData.value("forecast").toArray();

    for(int i=0; i<5; i++){
        QJsonObject objForecast = forecastArr[i].toObject();

        mDay[i+1].week = objForecast.value("week").toString();
        mDay[i+1].date = objForecast.value("ymd").toString();
        mDay[i+1].type = objForecast.value("type").toString();
        //高温 和 低温
        QString s1;
        s1 = objForecast.value("high").toString().split(" ").at(1);
        mDay[i+1].high = s1.left(s1.length()-1).toInt();

        s1 = objForecast.value("low").toString().split(" ").at(1);
        mDay[i+1].low = s1.left(s1.length()-1).toInt();

        //方向风力
        mDay[i+1].fx = objForecast.value("fx").toString();
        mDay[i+1].fl = objForecast.value("fl").toString();
        //污染指数
        mDay[i+1].aqi = objForecast.value("aqi").toDouble();
    }
    // 4. 解析今天的数据
    mToday.ganmao = objData.value("ganmao").toString();
    mToday.wendu = objData.value("wendu").toString().toInt();
    mToday.shidu = objData.value("shidu").toString();
    mToday.pm25 = objData.value("pm25").toDouble();
    mToday.quality = objData.value("quality").toString();

    // 5. forecast 中第一个数组元素，也就是今天的数据
    mToday.type = mDay[1].type;
    mToday.fx = mDay[1].fx;
    mToday.fl = mDay[1].fl;
    mToday.high = mDay[1].high;
    mToday.low = mDay[1].low;

    // 6.更新UI

    // 6.1 显示文本和图标
    UpdataUI();

    // 6.2 绘制温度曲线
    ui->lblHighCurve->update();
    ui->lblLowCurve->update();
}

void MainWindow::UpdataUI()
{
    // 1. 更新日期和城市
    ui->lblDate->setText(QDateTime::fromString(mToday.date,"yyyyMMdd").toString("yyyy/MM/dd") + " " + mDay[1].week);
    ui->lblCity->setText(mToday.city);

    // 2. 更新今天
    ui->lblTypeIcon->setPixmap(mTypeMap[mToday.type]);
    ui->lblTemp->setText(QString::number(mToday.wendu));
//    ui->lblTemp->setText(mToday.type);
    ui->lblLowHigh->setText(QString::number(mToday.low) + "~" + QString::number(mToday.high) + "°C");
    ui->lblGanMao->setText("感冒指数" + mToday.ganmao);
    ui->lblWindFx->setText(mToday.fx);
    ui->lblWindFl->setText(mToday.fl);
    ui->lblPM25->setText(QString::number(mToday.pm25));
    ui->lblShiDu->setText(mToday.shidu);
    ui->lblQuality->setText(mToday.quality);

    // 3. 更新6天
    for(int k=0; k<6; k++)
    {
        // 3.1 更新日期和时间
        mWeekList[k]->setText("周" + mDay[k].week.right(1));
        ui->lblWeek0->setText("昨天");
        ui->lblWeek1->setText("今天");
        ui->lblWeek2->setText("明天");

        QStringList ymdList = mDay[k].date.split("-");
        mDateList[k]->setText(ymdList[1] + "/" + ymdList[2]);

        // 3.2 更新天气类型
        mTypeList[k]->setText(mDay[k].type);
        mTypeIconList[k]->setPixmap(mTypeMap[mDay[k].type]);

        // 3.3 更新空气质量
        if(mDay[k].aqi >= 0 && mDay[k].aqi <= 50){
            mAqiList[k]->setText("优");
            mAqiList[k]->setStyleSheet("background-color: rgb(121, 184, 0)");
        }else if(mDay[k].aqi > 50 && mDay[k].aqi <= 100){
            mAqiList[k]->setText("良");
            mAqiList[k]->setStyleSheet("background-color: rgb(255, 187, 23)");
        }else if(mDay[k].aqi > 100 && mDay[k].aqi <= 150){
            mAqiList[k]->setText("轻度");
            mAqiList[k]->setStyleSheet("background-color: rgb(255, 87, 97)");
        }else if(mDay[k].aqi > 150 && mDay[k].aqi <= 200){
            mAqiList[k]->setText("中度");
            mAqiList[k]->setStyleSheet("background-color: rgb(235, 17, 27)");
        }else if(mDay[k].aqi > 200 && mDay[k].aqi <= 250){
            mAqiList[k]->setText("重度");
            mAqiList[k]->setStyleSheet("background-color: rgb(170, 0, 0)");
        }else{
            mAqiList[k]->setText("严重");
            mAqiList[k]->setStyleSheet("background-color: rgb(110, 0, 0)");
        }

        // 3.4 更新风力和风向
        mFxList[k]->setText(mDay[k].fx);
        mFlList[k]->setText(mDay[k].fl);
    }

}

//事件过滤器
bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->lblHighCurve && event->type() == QEvent::Paint){
        paintHighCurve();
    }
    if(watched == ui->lblLowCurve && event->type() == QEvent::Paint){
        paintLowCurve();
    }
    return QWidget::eventFilter(watched, event);
}

// 绘制最高温曲线
void MainWindow::paintHighCurve()
{
    QPainter painter(ui->lblHighCurve);

    // 抗锯齿
    painter.setRenderHint(QPainter::Antialiasing, true);

    // 1. 获取x坐标
    int pointX[6] = {0};
    for(int i=0; i<6; i++){
        pointX[i] = mWeekList[i]->pos().x() + mWeekList[i]->width()/2;
    }

    // 2. 获取y坐标
    int tempSum = 0;
    int tempAverage = 0;
    for(int i=0; i<6; i++){
        tempSum += mDay[i].high;
    }
    tempAverage = tempSum / 6; //这6天最高温的平均值

    // 计算y轴坐标
    int pointY[6] = {0};
    int yCenter = ui->lblHighCurve->height()/2;
    for(int i=0; i<6; i++){
        pointY[i] = yCenter - ((mDay[i].high - tempAverage) * INCREMENT);
    }

    // 3. 开始绘制
    // 3.1 初始化画笔相关
    QPen pen = painter.pen();
    pen.setWidth(1);                    //设置画笔的宽度
    pen.setColor(QColor(255, 170, 0));  //设置画笔的颜色
    painter.setPen(pen);
    painter.setBrush(QColor(255, 170, 0)); //设置画刷内部填充的颜色

    // 3.2 画点、写文本
    for(int i=0; i<6; i++){

        //显示点
        painter.drawEllipse(QPoint(pointX[i], pointY[i]), POINT_RADIUS, POINT_RADIUS);

        //显示温度文本
        painter.drawText(pointX[i]-TEXT_OFFSET_X, pointY[i]-TEXT_OFFSET_Y, QString::number(mDay[i].high) + "°");
    }

    // 3.3 绘制曲线
    for(int i=0; i<5; i++){
        if(i==0){
            pen.setStyle(Qt::DotLine); //虚线
            painter.setPen(pen);
        }else{
            pen.setStyle(Qt::SolidLine); //实线
            painter.setPen(pen);
        }
        painter.drawLine(pointX[i], pointY[i], pointX[i+1], pointY[i+1]);
    }
}

// 绘制最低温曲线
void MainWindow::paintLowCurve()
{
    QPainter painter(ui->lblLowCurve);

    // 抗锯齿
    painter.setRenderHint(QPainter::Antialiasing, true);

    // 1. 获取x坐标
    int pointX[6] = {0};
    for(int i=0; i<6; i++){
        pointX[i] = mWeekList[i]->pos().x() + mWeekList[i]->width()/2;
    }

    // 2. 获取y坐标
    int tempSum = 0;
    int tempAverage = 0;
    for(int i=0; i<6; i++){
        tempSum += mDay[i].low;
    }
    tempAverage = tempSum / 6; //这6天最高温的平均值

    // 计算y轴坐标
    int pointY[6] = {0};
    int yCenter = ui->lblLowCurve->height()/2;
    for(int i=0; i<6; i++){
        pointY[i] = yCenter - ((mDay[i].low - tempAverage) * INCREMENT);
    }


    // 3. 开始绘制
    // 3.1 初始化画笔相关
    QPen pen = painter.pen();
    pen.setWidth(1);                    //设置画笔的宽度
    pen.setColor(QColor(0, 255, 255));  //设置画笔的颜色
    painter.setPen(pen);
    painter.setBrush(QColor(0, 255, 255)); //设置画刷内部填充的颜色

    // 3.2 画点、写文本

    for(int i=0; i<6; i++){
        //显示点
        painter.drawEllipse(QPoint(pointX[i], pointY[i]), POINT_RADIUS, POINT_RADIUS);

        //显示温度文本
        painter.drawText(pointX[i]-TEXT_OFFSET_X, pointY[i]-TEXT_OFFSET_Y, QString::number(mDay[i].low) + "°");
    }

    // 3.3 绘制曲线
    for(int i=0; i<5; i++){
        if(i==0){
            pen.setStyle(Qt::DotLine); //虚线
            painter.setPen(pen);
        }else{
            pen.setStyle(Qt::SolidLine); //实线
            painter.setPen(pen);
        }
        painter.drawLine(pointX[i], pointY[i], pointX[i+1], pointY[i+1]);
    }
}

void MainWindow::onReplied(QNetworkReply *reply)
{
    //响应状态码为200，表示成功
    int status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if(reply->error() != QNetworkReply::NoError || status_code != 200){
        qDebug() << reply->errorString().toLatin1().data();
        QMessageBox::warning(this, "天气", "请求数据失败", QMessageBox::Ok);
    } else{
        QByteArray byteArray = reply->readAll();
//        qDebug() << "read all: " << byteArray.data();
        PreseJSON(byteArray);
    }
    reply->deleteLater();
}

void MainWindow::on_btnSearch_clicked()
{
    QString cityName = ui->leCity->text();
    getWeatherInfo(cityName);
}
