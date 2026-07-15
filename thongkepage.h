#ifndef THONGKEPAGE_H
#define THONGKEPAGE_H

#include <QWidget>
#include "cautrucdulieu.h"

namespace Ui {
class ThongKePage;
}

class ThongKePage : public QWidget
{
    Q_OBJECT
public:
    explicit ThongKePage(TreeVT &rootRef, DS_NHANVIEN &dsRef, QWidget *parent = nullptr);
    ~ThongKePage();

private:
    Ui::ThongKePage *ui;
    TreeVT &root;
    DS_NHANVIEN &dsnv;
};

#endif // THONGKEPAGE_H