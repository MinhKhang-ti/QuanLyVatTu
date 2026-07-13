#ifndef HOADONPAGE_H
#define HOADONPAGE_H

#include <QWidget>
#include "cautrucdulieu.h"

class HoaDonPage : public QWidget
{
    Q_OBJECT
public:
    explicit HoaDonPage(TreeVT &rootRef, DS_NHANVIEN &dsRef, QWidget *parent = nullptr);

private:
    TreeVT &root;
    DS_NHANVIEN &dsnv;
    // TODO: them cac QWidget cho form lap hoa don
};

#endif // HOADONPAGE_H