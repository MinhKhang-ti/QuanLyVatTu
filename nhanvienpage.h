#ifndef NHANVIENPAGE_H
#define NHANVIENPAGE_H

#include <QWidget>
#include "cautrucdulieu.h"

namespace Ui {
class NhanVienPage;
}

class NhanVienPage : public QWidget
{
    Q_OBJECT
public:
    explicit NhanVienPage(DS_NHANVIEN &dsRef, QWidget *parent = nullptr);
    ~NhanVienPage();

private slots:
    void validateForm();
    void onThemClicked();
    void onXoaClicked();

private:
    Ui::NhanVienPage *ui;
    DS_NHANVIEN &dsnv; // tham chieu toi mang dung chung

    void lamMoiBang();
};

#endif // NHANVIENPAGE_H