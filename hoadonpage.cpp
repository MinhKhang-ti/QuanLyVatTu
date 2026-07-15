#include "hoadonpage.h"
#include "ui_hoadonpage.h"

HoaDonPage::HoaDonPage(TreeVT &rootRef, DS_NHANVIEN &dsRef, QWidget *parent)
    : QWidget(parent), ui(new Ui::HoaDonPage), root(rootRef), dsnv(dsRef)
{
    ui->setupUi(this);
}

HoaDonPage::~HoaDonPage()
{
    delete ui;
}