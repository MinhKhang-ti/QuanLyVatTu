#include "thongkepage.h"
#include "ui_thongkepage.h"

ThongKePage::ThongKePage(TreeVT &rootRef, DS_NHANVIEN &dsRef, QWidget *parent)
    : QWidget(parent), ui(new Ui::ThongKePage), root(rootRef), dsnv(dsRef)
{
    ui->setupUi(this);
}

ThongKePage::~ThongKePage()
{
    delete ui;
}