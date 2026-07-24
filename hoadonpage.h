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

    // Gọi lại khi tab hiển thị để cập nhật danh sách nhân viên và vật tư mới nhất
    void napDuLieuCombo();

private slots:
    void onThemCTClicked();
    void onXoaCTClicked();
    void onGhiClicked();
    void onHuyClicked();

private:
    Ui::HoaDonPage *ui;
    TreeVT &root;
    DS_NHANVIEN &dsnv;

    DS_CTHD cthdTam; // Mảng tạm lưu các vật tư đang lập của hóa đơn hiện tại

    void napNVCombo();
    void napVTCombo();
    void capNhatBangCTHD();
    void resetForm();
};

#endif // HOADONPAGE_H