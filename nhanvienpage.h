#ifndef NHANVIENPAGE_H
#define NHANVIENPAGE_H

#include <QWidget>
#include <vector>
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
    void onSuaClicked();
    void onHuyClicked();
    void onUndoClicked();
    void onRedoClicked();
    void onSelectionChanged();
    void onTimKiemChanged();

private:
    Ui::NhanVienPage *ui;
    DS_NHANVIEN &dsnv; // tham chieu toi mang dung chung

    // Ngăn xếp lưu trạng thái phục vụ Undo/Redo
    std::vector<DS_NHANVIEN*> undoStack;
    std::vector<DS_NHANVIEN*> redoStack;

    void saveState();
    void updateUndoRedoButtons();
    void lamMoiBang();

    // Các hàm hỗ trợ deep copy để quản lý bộ nhớ
    DS_NHANVIEN* cloneState(const DS_NHANVIEN &source);
    void freeState(DS_NHANVIEN* state);
};

#endif // NHANVIENPAGE_H