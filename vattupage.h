#ifndef VATTUPAGE_H
#define VATTUPAGE_H
#include <QWidget>
#include "cautrucdulieu.h"
namespace Ui {
class VatTuPage;
}
class VatTuPage : public QWidget
{
    Q_OBJECT
public:
    explicit VatTuPage(TreeVT &rootRef, QWidget *parent = nullptr);
    ~VatTuPage();
    void lamMoiBang();
private slots:
    void capNhatTrangThaiNut();
    void onThemClicked();
    void onXoaClicked();
    void onSuaClicked();
    void onHuyClicked();
    void onTableSelectionChanged();
    void onTimKiemChanged();
private:
    Ui::VatTuPage *ui;
    TreeVT &root;
    QString maVTDangSua;
    QString tenVTGoc;
    QString dvtGoc;
    int soLuongGoc;
    void resetForm();
};
#endif // VATTUPAGE_H