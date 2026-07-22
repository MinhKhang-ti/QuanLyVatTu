#ifndef VATTUPAGE_H
#define VATTUPAGE_H

#include <QWidget>
#include "cautrucdulieu.h"

namespace Ui {
class VatTuPage;
}

struct HistoryNodeVT {
    TreeVT state;
    HistoryNodeVT* next;
};

class VatTuPage : public QWidget
{
    Q_OBJECT
public:
    explicit VatTuPage(TreeVT &rootRef, QWidget *parent = nullptr);
    ~VatTuPage();

private slots:
    void validateForm();
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

    HistoryNodeVT* undoStackTop;
    HistoryNodeVT* redoStackTop;

    void lamMoiBang();
    void resetForm();
    void saveState();
    void updateUndoRedoButtons();

    void pushState(HistoryNodeVT*& top, TreeVT state);
    TreeVT popState(HistoryNodeVT*& top);
    void clearStack(HistoryNodeVT*& top);
};

#endif // VATTUPAGE_H