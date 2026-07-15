#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "vattupage.h"
#include "nhanvienpage.h"
#include "hoadonpage.h"
#include "thongkepage.h"
#include "nhanvienlogic.h"

#include <QStackedWidget>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Quan Ly Nhap Xuat Vat Tu");
    resize(900, 600);

    khoiTaoDSNV(dsnv);

    // Gán con trỏ stack trỏ tới QStackedWidget kéo thả trong file .ui
    stack = ui->stack;

    // Khởi tạo các trang con
    vatTuPage = new VatTuPage(dsvt, this);
    nhanVienPage = new NhanVienPage(dsnv, this);
    hoaDonPage = new HoaDonPage(dsvt, dsnv, this);
    thongKePage = new ThongKePage(dsvt, dsnv, this);

    // Thêm các trang vào stack
    stack->addWidget(vatTuPage);     // Chỉ mục 0
    stack->addWidget(nhanVienPage);  // Chỉ mục 1
    stack->addWidget(hoaDonPage);    // Chỉ mục 2
    stack->addWidget(thongKePage);   // Chỉ mục 3

    // Thiết lập trang mặc định hiển thị ban đầu
    stack->setCurrentIndex(0);

    // Kết nối các sự kiện nút bấm trên sidebar để chuyển trang
    connect(ui->btnVatTu, &QPushButton::clicked, this, [=]() { stack->setCurrentIndex(0); });
    connect(ui->btnNhanVien, &QPushButton::clicked, this, [=]() { stack->setCurrentIndex(1); });
    connect(ui->btnHoaDon, &QPushButton::clicked, this, [=]() { stack->setCurrentIndex(2); });
    connect(ui->btnThongKe, &QPushButton::clicked, this, [=]() { stack->setCurrentIndex(3); });
}

MainWindow::~MainWindow()
{
    delete ui;
}