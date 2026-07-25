#include "vattupage.h"
#include "ui_vattupage.h"
#include "vattulogic.h"
#include "fileio.h"
#include <QRegularExpressionValidator>
#include <QHeaderView>
#include <QSignalBlocker>
#include <QMessageBox>
#include <cstring>

VatTuPage::VatTuPage(TreeVT &rootRef, QWidget *parent)
    : QWidget(parent), ui(new Ui::VatTuPage), root(rootRef)
{
    ui->setupUi(this);

    undoStackTop = nullptr;
    redoStackTop = nullptr;

    ui->maVTEdit->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[A-Za-z0-9]{0,10}"), this));
    ui->soLuongEdit->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[0-9]{0,6}"), this));

    ui->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->table->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->errorLabel->setVisible(false);
    ui->themButton->setEnabled(false);
    ui->suaButton->setEnabled(false);
    ui->huyButton->setEnabled(false);

    updateUndoRedoButtons();

    connect(ui->maVTEdit, &QLineEdit::textChanged, this, &VatTuPage::validateForm);
    connect(ui->tenVTEdit, &QLineEdit::textChanged, this, &VatTuPage::validateForm);
    connect(ui->dvtEdit, &QLineEdit::textChanged, this, &VatTuPage::validateForm);
    connect(ui->soLuongEdit, &QLineEdit::textChanged, this, &VatTuPage::validateForm);
    connect(ui->themButton, &QPushButton::clicked, this, &VatTuPage::onThemClicked);
    connect(ui->xoaButton, &QPushButton::clicked, this, &VatTuPage::onXoaClicked);
    connect(ui->suaButton, &QPushButton::clicked, this, &VatTuPage::onSuaClicked);
    connect(ui->huyButton, &QPushButton::clicked, this, &VatTuPage::onHuyClicked);
    connect(ui->table, &QTableWidget::itemSelectionChanged, this, &VatTuPage::onTableSelectionChanged);
    connect(ui->undoButton, &QPushButton::clicked, this, &VatTuPage::onUndoClicked);
    connect(ui->redoButton, &QPushButton::clicked, this, &VatTuPage::onRedoClicked);
    connect(ui->timKiemEdit, &QLineEdit::textChanged, this, &VatTuPage::onTimKiemChanged);

    docVatTu(root, FILE_VATTU);
    lamMoiBang();
}

VatTuPage::~VatTuPage() {
    clearStack(undoStackTop);
    clearStack(redoStackTop);
    delete ui;
}

void VatTuPage::validateForm() {
    QString loi;
    if (ui->maVTEdit->text().trimmed().isEmpty()) loi = "Mã vật tư không được để trống";
    else if (ui->tenVTEdit->text().trimmed().isEmpty()) loi = "Tên vật tư không được để trống";
    else if (ui->dvtEdit->text().trimmed().isEmpty()) loi = "Đơn vị tính không được để trống";
    else if (ui->soLuongEdit->text().trimmed().isEmpty()) loi = "Số lượng tồn không được để trống";

    if (!loi.isEmpty()) {
        ui->errorLabel->setText(loi);
        ui->errorLabel->setVisible(true);
        ui->themButton->setEnabled(false);
    } else {
        ui->errorLabel->setVisible(false);
        ui->themButton->setEnabled(true);
    }
}

void VatTuPage::onThemClicked() {
    HistoryEntryVT entry{};
    entry.loai = THEM;

    QString mavt = ui->maVTEdit->text().toUpper();
    QString tenvt = ui->tenVTEdit->text();
    QString dvt = ui->dvtEdit->text();
    int soLuong = ui->soLuongEdit->text().toInt();

    strncpy(entry.sau.MAVT, mavt.toStdString().c_str(), 10); entry.sau.MAVT[10] = '\0';
    strncpy(entry.sau.TENVT, tenvt.toStdString().c_str(), 50); entry.sau.TENVT[50] = '\0';
    strncpy(entry.sau.DVT, dvt.toStdString().c_str(), 10); entry.sau.DVT[10] = '\0';
    entry.sau.SoLuongTon = soLuong;

    std::string loi;
    bool ok = themVT(root, entry.sau.MAVT, entry.sau.TENVT, entry.sau.DVT, entry.sau.SoLuongTon, loi);
    if (!ok) {
        ui->errorLabel->setText(QString::fromStdString(loi));
        ui->errorLabel->setVisible(true);
        return;
    }

    pushEntry(undoStackTop, entry);
    clearStack(redoStackTop);
    updateUndoRedoButtons();

    resetForm();
    luuVatTu(root, FILE_VATTU);
    lamMoiBang();
}

void VatTuPage::onXoaClicked() {
    int row = ui->table->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Cảnh báo", "Vui lòng chọn vật tư muốn xóa từ bảng danh sách!");
        return;
    }
    QString mavt = ui->table->item(row, 0)->text();
    QString tenvt = ui->table->item(row, 1)->text();

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Xác nhận xóa",
        QString("Bạn có chắc chắn muốn xóa vật tư %1 (Mã: %2)?").arg(tenvt).arg(mavt),
        QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    HistoryEntryVT entry{};
    entry.loai = XOA;

    // Doc thang tu bang - bang dang hien dung du lieu hien tai cua node nay
    // (da co san tu buoc lamMoiBang() truoc do), khong can goi timVT() them lan nua
    QString dvt = ui->table->item(row, 2)->text();
    int soLuong = ui->table->item(row, 3)->text().toInt();

    strncpy(entry.truoc.MAVT, mavt.toStdString().c_str(), 10); entry.truoc.MAVT[10] = '\0';
    strncpy(entry.truoc.TENVT, tenvt.toStdString().c_str(), 50); entry.truoc.TENVT[50] = '\0';
    strncpy(entry.truoc.DVT, dvt.toStdString().c_str(), 10); entry.truoc.DVT[10] = '\0';
    entry.truoc.SoLuongTon = soLuong;

    std::string loi;
    bool ok = xoaVT(root, mavt.toStdString().c_str(), loi);
    if (!ok) {
        QMessageBox::critical(this, "Lỗi xóa vật tư", QString::fromStdString(loi));
        return;
    }

    pushEntry(undoStackTop, entry);
    clearStack(redoStackTop);
    updateUndoRedoButtons();

    resetForm();
    luuVatTu(root, FILE_VATTU);
    lamMoiBang();
}

void VatTuPage::onSuaClicked() {
    if (maVTDangSua.isEmpty()) return;

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Xác nhận hiệu chỉnh",
        QString("Bạn có chắc chắn muốn sửa thông tin vật tư có mã %1?").arg(maVTDangSua),
        QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    HistoryEntryVT entry{};
    entry.loai = SUA;

    // Dung snapshot da luu tu luc chon dong - KHONG can goi timVT() de hoi lai cay
    strncpy(entry.truoc.MAVT, maVTDangSua.toStdString().c_str(), 10); entry.truoc.MAVT[10] = '\0';
    strncpy(entry.truoc.TENVT, tenVTGoc.toStdString().c_str(), 50); entry.truoc.TENVT[50] = '\0';
    strncpy(entry.truoc.DVT, dvtGoc.toStdString().c_str(), 10); entry.truoc.DVT[10] = '\0';
    entry.truoc.SoLuongTon = soLuongGoc;

    entry.sau = entry.truoc; // sao chep, roi cap nhat 2 field se doi
    QString tenMoi = ui->tenVTEdit->text();
    QString dvtMoi = ui->dvtEdit->text();
    strncpy(entry.sau.TENVT, tenMoi.toStdString().c_str(), 50); entry.sau.TENVT[50] = '\0';
    strncpy(entry.sau.DVT, dvtMoi.toStdString().c_str(), 10); entry.sau.DVT[10] = '\0';

    std::string loi;
    bool ok = suaVT(root, maVTDangSua.toStdString().c_str(), entry.sau.TENVT, entry.sau.DVT, loi);
    if (!ok) {
        ui->errorLabel->setText(QString::fromStdString(loi));
        ui->errorLabel->setVisible(true);
        return;
    }

    pushEntry(undoStackTop, entry);
    clearStack(redoStackTop);
    updateUndoRedoButtons();

    resetForm();
    luuVatTu(root, FILE_VATTU);
    lamMoiBang();
}

void VatTuPage::onHuyClicked() {
    ui->table->clearSelection();
    ui->table->setCurrentCell(-1, -1);
    resetForm();
}

void VatTuPage::onTableSelectionChanged() {
    int row = ui->table->currentRow();
    if (row < 0) {
        ui->suaButton->setEnabled(false);
        ui->huyButton->setEnabled(false);
        return;
    }
    maVTDangSua = ui->table->item(row, 0)->text();
    ui->maVTEdit->setText(maVTDangSua);
    ui->tenVTEdit->setText(ui->table->item(row, 1)->text());
    ui->dvtEdit->setText(ui->table->item(row, 2)->text());
    ui->soLuongEdit->setText(ui->table->item(row, 3)->text());

    // Luu lai snapshot du lieu GOC ngay tai thoi diem chon - tan dung du lieu
    // da co san trong bang (tu buoc lamMoiBang() truoc do), khong can hoi lai cay
    tenVTGoc = ui->table->item(row, 1)->text();
    dvtGoc = ui->table->item(row, 2)->text();
    soLuongGoc = ui->table->item(row, 3)->text().toInt();

    ui->maVTEdit->setReadOnly(true);
    ui->soLuongEdit->setReadOnly(true);
    ui->themButton->setEnabled(false);
    ui->suaButton->setEnabled(true);
    ui->huyButton->setEnabled(true);
}

void VatTuPage::onUndoClicked() {
    if (!undoStackTop) return;
    HistoryEntryVT entry = popEntry(undoStackTop);
    std::string loi;

    switch (entry.loai) {
    case THEM:
        xoaVT(root, entry.sau.MAVT, loi); // dao nguoc THEM = XOA lai
        break;
    case XOA:
        themVT(root, entry.truoc.MAVT, entry.truoc.TENVT, entry.truoc.DVT,
               entry.truoc.SoLuongTon, loi); // dao nguoc XOA = THEM LAI dung du lieu cu
        break;
    case SUA:
        suaVT(root, entry.truoc.MAVT, entry.truoc.TENVT, entry.truoc.DVT, loi); // tra ve gia tri cu
        break;
    }

    pushEntry(redoStackTop, entry);
    resetForm();
    lamMoiBang();
    updateUndoRedoButtons();
    luuVatTu(root, FILE_VATTU);
}

void VatTuPage::onRedoClicked() {
    if (!redoStackTop) return;
    HistoryEntryVT entry = popEntry(redoStackTop);
    std::string loi;

    switch (entry.loai) {
    case THEM:
        themVT(root, entry.sau.MAVT, entry.sau.TENVT, entry.sau.DVT,
               entry.sau.SoLuongTon, loi); // lam lai THEM
        break;
    case XOA:
        xoaVT(root, entry.truoc.MAVT, loi); // lam lai XOA
        break;
    case SUA:
        suaVT(root, entry.truoc.MAVT, entry.sau.TENVT, entry.sau.DVT, loi); // ap dung lai gia tri moi
        break;
    }

    pushEntry(undoStackTop, entry);
    resetForm();
    lamMoiBang();
    updateUndoRedoButtons();
    luuVatTu(root, FILE_VATTU);
}

void VatTuPage::onTimKiemChanged() {
    lamMoiBang();
}

void VatTuPage::updateUndoRedoButtons() {
    ui->undoButton->setEnabled(undoStackTop != nullptr);
    ui->redoButton->setEnabled(redoStackTop != nullptr);
}

void VatTuPage::pushEntry(HistoryNodeVT*& top, HistoryEntryVT entry) {
    HistoryNodeVT* node = new HistoryNodeVT();
    node->entry = entry;
    node->next = top;
    top = node;
}

HistoryEntryVT VatTuPage::popEntry(HistoryNodeVT*& top) {
    HistoryNodeVT* temp = top;
    HistoryEntryVT entry = temp->entry;
    top = top->next;
    delete temp;
    return entry;
}

void VatTuPage::clearStack(HistoryNodeVT*& top) {
    while (top) {
        HistoryNodeVT* temp = top;
        top = top->next;
        delete temp;
    }
}

void VatTuPage::lamMoiBang() {
    int soLuong = 0;
    nodeVT** ds = duyetTheoTen(root, soLuong);

    QString keyword = ui->timKiemEdit->text().trimmed().toUpper();

    int soKhop = 0;
    for (int i = 0; i < soLuong; i++) {
        QString mavt = QString::fromUtf8(ds[i]->vt.MAVT).toUpper();
        QString tenvt = QString::fromUtf8(ds[i]->vt.TENVT).toUpper();
        QString dvt = QString::fromUtf8(ds[i]->vt.DVT).toUpper();
        if (keyword.isEmpty() || mavt.contains(keyword) || tenvt.contains(keyword) || dvt.contains(keyword)) {
            soKhop++;
        }
    }

    ui->table->setRowCount(soKhop);
    int row = 0;
    for (int i = 0; i < soLuong; i++) {
        QString mavt = QString::fromUtf8(ds[i]->vt.MAVT).toUpper();
        QString tenvt = QString::fromUtf8(ds[i]->vt.TENVT).toUpper();
        QString dvt = QString::fromUtf8(ds[i]->vt.DVT).toUpper();
        if (keyword.isEmpty() || mavt.contains(keyword) || tenvt.contains(keyword) || dvt.contains(keyword)) {
            ui->table->setItem(row, 0, new QTableWidgetItem(ds[i]->vt.MAVT));
            ui->table->setItem(row, 1, new QTableWidgetItem(ds[i]->vt.TENVT));
            ui->table->setItem(row, 2, new QTableWidgetItem(ds[i]->vt.DVT));
            ui->table->setItem(row, 3, new QTableWidgetItem(QString::number(ds[i]->vt.SoLuongTon)));
            row++;
        }
    }

    delete[] ds;
}

void VatTuPage::resetForm() {
    QSignalBlocker b1(ui->maVTEdit);
    QSignalBlocker b2(ui->tenVTEdit);
    QSignalBlocker b3(ui->dvtEdit);
    QSignalBlocker b4(ui->soLuongEdit);

    ui->maVTEdit->clear();
    ui->tenVTEdit->clear();
    ui->dvtEdit->clear();
    ui->soLuongEdit->clear();
    ui->maVTEdit->setReadOnly(false);
    ui->soLuongEdit->setReadOnly(false);
    ui->suaButton->setEnabled(false);
    ui->huyButton->setEnabled(false);
    maVTDangSua.clear();

    ui->errorLabel->setVisible(false);
    ui->themButton->setEnabled(false);
}