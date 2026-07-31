#ifndef VATTUPAGE_H
#define VATTUPAGE_H
#include <QWidget>
#include "cautrucdulieu.h"

namespace Ui {
class VatTuPage;
}

enum LoaiThaoTac { THEM, SUA, XOA };

// Ghi lai 1 hanh dong da xay ra, du de "dao nguoc" ma khong can chup ca cay
struct HistoryEntryVT {
    LoaiThaoTac loai;
    VATTU truoc; // du lieu TRUOC khi thao tac (dung cho SUA, XOA)
    VATTU sau;   // du lieu SAU khi thao tac (dung cho THEM, SUA)
};

struct HistoryNodeVT {
    HistoryEntryVT entry;
    HistoryNodeVT* next;
};

class VatTuPage : public QWidget
{
    Q_OBJECT
public:
    explicit VatTuPage(TreeVT &rootRef, QWidget *parent = nullptr);
    ~VatTuPage();

    // MỚI - chuyển từ private sang public, để MainWindow gọi được
    // moi khi chuyen sang trang Vat tu, nham lam moi lai bang du lieu
    void lamMoiBang();

private slots:
    void capNhatTrangThaiNut();
    void onThemClicked();
    void onXoaClicked();
    void onSuaClicked();
    void onHuyClicked();
    void onTableSelectionChanged();
    void onUndoClicked();
    void onRedoClicked();
    void onTimKiemChanged();

private:
    Ui::VatTuPage *ui;
    TreeVT &root;
    QString maVTDangSua;
    QString tenVTGoc;
    QString dvtGoc;
    int soLuongGoc;
    bool dangKhoa = false;   // MỚI - true neu vat tu dang chon da xuat hien trong hoa don da ghi

    HistoryNodeVT* undoStackTop;
    HistoryNodeVT* redoStackTop;

    void resetForm();
    void updateUndoRedoButtons();
    void pushEntry(HistoryNodeVT*& top, HistoryEntryVT entry);
    HistoryEntryVT popEntry(HistoryNodeVT*& top);
    void clearStack(HistoryNodeVT*& top);
};
#endif // VATTUPAGE_H