#ifndef NHANVIENPAGE_H
#define NHANVIENPAGE_H

#include <QWidget>
#include "cautrucdulieu.h"

namespace Ui {
class NhanVienPage;
}

// Cấu trúc Node ngăn xếp lưu lịch sử Undo/Redo
struct HistoryNode {
    DS_NHANVIEN* state;
    HistoryNode* next;
};

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
    void onCellDoubleClicked(int row, int column);
    void onTimKiemChanged();

private:
    Ui::NhanVienPage *ui;
    DS_NHANVIEN &dsnv; // tham chieu toi mang dung chung

    // Quản lý đỉnh ngăn xếp lưu lịch sử
    HistoryNode* undoStackTop;
    HistoryNode* redoStackTop;

    void saveState();
    void updateUndoRedoButtons();
    void lamMoiBang();

    // Các hàm thao tác trên ngăn xếp lưu lịch sử
    void pushState(HistoryNode*& top, DS_NHANVIEN* state);
    DS_NHANVIEN* popState(HistoryNode*& top);
    void clearStack(HistoryNode*& top);

    // Các hàm hỗ trợ deep copy để quản lý bộ nhớ
    DS_NHANVIEN* cloneState(const DS_NHANVIEN &source);
    void freeState(DS_NHANVIEN* state);
};

#endif // NHANVIENPAGE_H