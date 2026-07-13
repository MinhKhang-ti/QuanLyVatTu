#include "hoadonpage.h"
#include <QVBoxLayout>
#include <QLabel>

HoaDonPage::HoaDonPage(TreeVT &rootRef, DS_NHANVIEN &dsRef, QWidget *parent)
    : QWidget(parent), root(rootRef), dsnv(dsRef)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *label = new QLabel("Chức năng lập hóa đơn");
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
}