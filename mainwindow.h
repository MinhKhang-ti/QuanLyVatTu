#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "cautrucdulieu.h"

class QStackedWidget;
class VatTuPage;
class NhanVienPage;
class HoaDonPage;
class ThongKePage;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;
    QStackedWidget *stack;

    // Du lieu dung chung toan bo ung dung - chi co 1 ban duy nhat
    TreeVT dsvt = nullptr;
    DS_NHANVIEN dsnv;

    VatTuPage *vatTuPage;
    NhanVienPage *nhanVienPage;
    HoaDonPage *hoaDonPage;
    ThongKePage *thongKePage;
};
#endif // MAINWINDOW_H