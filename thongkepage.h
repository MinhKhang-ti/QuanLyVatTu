#ifndef THONGKEPAGE_H
#define THONGKEPAGE_H

#include <QWidget>
#include "cautrucdulieu.h"

class ThongKePage : public QWidget
{
    Q_OBJECT
public:
    explicit ThongKePage(TreeVT &rootRef, DS_NHANVIEN &dsRef, QWidget *parent = nullptr);

private:
    TreeVT &root;
    DS_NHANVIEN &dsnv;
    // TODO: them UI cho thong ke (g) va (h)
};

#endif // THONGKEPAGE_H