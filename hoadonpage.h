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
    void onChonVTClicked();
    void onThemCTClicked();
    void onXoaCTClicked();
    void onGhiClicked();
    void onHuyClicked();
    void onInHoaDonClicked();

private:
    Ui::HoaDonPage *ui;
    TreeVT &root;
    DS_NHANVIEN &dsnv;

    DS_CTHD cthdTam; // Mảng tạm lưu các vật tư đang lập của hóa đơn hiện tại
    QString selectedMAVT; // Mã vật tư đang được chọn từ Dialog

    void napNVCombo();
    void capNhatBangCTHD();
    void resetForm();
};

#endif // HOADONPAGE_H