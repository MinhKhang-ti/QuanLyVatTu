#ifndef HOADONPAGE_H
#define HOADONPAGE_H

#include <QWidget>
#include "cautrucdulieu.h"

namespace Ui {
class HoaDonPage;
}

class HoaDonPage : public QWidget
{
    Q_OBJECT
public:
    explicit HoaDonPage(TreeVT &rootRef, DS_NHANVIEN &dsRef, QWidget *parent = nullptr);
    ~HoaDonPage();

private:
    Ui::HoaDonPage *ui;
    TreeVT &root;
    DS_NHANVIEN &dsnv;
};

#endif // HOADONPAGE_H