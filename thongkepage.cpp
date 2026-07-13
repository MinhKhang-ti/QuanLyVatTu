#include "thongkepage.h"
#include <QVBoxLayout>
#include <QLabel>

ThongKePage::ThongKePage(TreeVT &rootRef, DS_NHANVIEN &dsRef, QWidget *parent)
    : QWidget(parent), root(rootRef), dsnv(dsRef)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *label = new QLabel("Chức năng thống kê");
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
}