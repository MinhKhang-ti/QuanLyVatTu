#ifndef INHOADONDIALOG_H
#define INHOADONDIALOG_H
#include <QDialog>
#include "cautrucdulieu.h"

class QLineEdit;
class QPushButton;
class QLabel;
class QTableWidget;

class InHoaDonDialog : public QDialog {
    Q_OBJECT
public:
    explicit InHoaDonDialog(TreeVT& rootRef, DS_NHANVIEN& dsRef, QWidget* parent = nullptr);
private slots:
    void onTimClicked();
    void onChonHDTrongDanhSach(int row, int column);   // MỚI
private:
    TreeVT& root;
    DS_NHANVIEN& dsnv;

    QTableWidget* danhSachTable;   // MỚI — bảng liệt kê toàn bộ hóa đơn
    QLineEdit* soHDEdit;
    QPushButton* timButton;
    QLabel* thongTinLabel;
    QTableWidget* table;
    QLabel* tongTienLabel;
    QLabel* tienChuLabel;
    QLabel* errorLabel;

    void napDanhSachHD();          // MỚI — đổ toàn bộ hóa đơn vào danhSachTable
    void hienThiHoaDon(nodeHD* hd, int idxNV);
    void xoaBang();
};
#endif // INHOADONDIALOG_H