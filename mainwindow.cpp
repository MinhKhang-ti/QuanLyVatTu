#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "vattupage.h"
#include "nhanvienpage.h"
#include "hoadonpage.h"
#include "thongkepage.h"
#include "nhanvienlogic.h"

#include <QStackedWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Quan Ly Nhap Xuat Vat Tu");
    resize(900, 600);

    khoiTaoDSNV(dsnv);

    QWidget *central = ui->centralwidget;
    QHBoxLayout *mainLayout = new QHBoxLayout(central);

    QVBoxLayout *sidebar = new QVBoxLayout;
    QPushButton *btnVatTu = new QPushButton("Vat tu");
    QPushButton *btnNhanVien = new QPushButton("Nhan vien");
    QPushButton *btnHoaDon = new QPushButton("Lap hoa don");
    QPushButton *btnThongKe = new QPushButton("Thong ke");

    sidebar->addWidget(btnVatTu);
    sidebar->addWidget(btnNhanVien);
    sidebar->addWidget(btnHoaDon);
    sidebar->addWidget(btnThongKe);
    sidebar->addStretch();

    QWidget *sidebarWidget = new QWidget;
    sidebarWidget->setLayout(sidebar);
    sidebarWidget->setFixedWidth(180);

    stack = new QStackedWidget;
    vatTuPage = new VatTuPage(dsvt);
    nhanVienPage = new NhanVienPage(dsnv);
    hoaDonPage = new HoaDonPage(dsvt, dsnv);
    thongKePage = new ThongKePage(dsvt, dsnv);

    stack->addWidget(vatTuPage);     // 0
    stack->addWidget(nhanVienPage);  // 1
    stack->addWidget(hoaDonPage);    // 2
    stack->addWidget(thongKePage);   // 3

    mainLayout->addWidget(sidebarWidget);
    mainLayout->addWidget(stack);

    connect(btnVatTu, &QPushButton::clicked, this, [=]() { stack->setCurrentIndex(0); });
    connect(btnNhanVien, &QPushButton::clicked, this, [=]() { stack->setCurrentIndex(1); });
    connect(btnHoaDon, &QPushButton::clicked, this, [=]() { stack->setCurrentIndex(2); });
    connect(btnThongKe, &QPushButton::clicked, this, [=]() { stack->setCurrentIndex(3); });
}

MainWindow::~MainWindow()
{
    delete ui;
}