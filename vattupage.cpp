#include "vattupage.h"
#include "ui_vattupage.h"
#include "vattulogic.h"
#include "fileio.h"
#include <QIntValidator>
#include <QRegularExpressionValidator>
#include <QHeaderView>
#include <QSignalBlocker>
#include <QMessageBox>

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
    saveState();

    std::string loi;
    bool ok = themVT(root,
                     ui->maVTEdit->text().toUpper().toStdString().c_str(),
                     ui->tenVTEdit->text().toStdString().c_str(),
                     ui->dvtEdit->text().toStdString().c_str(),
                     ui->soLuongEdit->text().toInt(),
                     loi);
    if (!ok) {
        TreeVT bo = popState(undoStackTop);
        huyCayVT(bo);
        updateUndoRedoButtons();
        ui->errorLabel->setText(QString::fromStdString(loi));
        ui->errorLabel->setVisible(true);
        return;
    }
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

    saveState();

    std::string loi;
    bool ok = xoaVT(root, mavt.toStdString().c_str(), loi);
    if (!ok) {
        TreeVT bo = popState(undoStackTop);
        huyCayVT(bo);
        updateUndoRedoButtons();
        QMessageBox::critical(this, "Lỗi xóa vật tư", QString::fromStdString(loi));
        return;
    }
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

    saveState();

    std::string loi;
    bool ok = suaVT(root,
                    maVTDangSua.toStdString().c_str(),
                    ui->tenVTEdit->text().toStdString().c_str(),
                    ui->dvtEdit->text().toStdString().c_str(),
                    loi);
    if (!ok) {
        TreeVT bo = popState(undoStackTop);
        huyCayVT(bo);
        updateUndoRedoButtons();
        ui->errorLabel->setText(QString::fromStdString(loi));
        ui->errorLabel->setVisible(true);
        return;
    }

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

    ui->maVTEdit->setReadOnly(true);
    ui->soLuongEdit->setReadOnly(true);
    ui->themButton->setEnabled(false);
    ui->suaButton->setEnabled(true);
    ui->huyButton->setEnabled(true);
}

void VatTuPage::onUndoClicked() {
    if (!undoStackTop) return;

    TreeVT previous = popState(undoStackTop);
    TreeVT hienTai = cloneCayVT(root);
    pushState(redoStackTop, hienTai);

    huyCayVT(root);
    root = previous;

    resetForm();
    lamMoiBang();
    updateUndoRedoButtons();
    luuVatTu(root, FILE_VATTU);
}

void VatTuPage::onRedoClicked() {
    if (!redoStackTop) return;

    TreeVT tiepTheo = popState(redoStackTop);
    TreeVT hienTai = cloneCayVT(root);
    pushState(undoStackTop, hienTai);

    huyCayVT(root);
    root = tiepTheo;

    resetForm();
    lamMoiBang();
    updateUndoRedoButtons();
    luuVatTu(root, FILE_VATTU);
}

void VatTuPage::onTimKiemChanged() {
    lamMoiBang();
}

void VatTuPage::saveState() {
    TreeVT snapshot = cloneCayVT(root);
    pushState(undoStackTop, snapshot);
    clearStack(redoStackTop);
    updateUndoRedoButtons();
}

void VatTuPage::updateUndoRedoButtons() {
    ui->undoButton->setEnabled(undoStackTop != nullptr);
    ui->redoButton->setEnabled(redoStackTop != nullptr);
}

void VatTuPage::pushState(HistoryNodeVT*& top, TreeVT state) {
    HistoryNodeVT* node = new HistoryNodeVT();
    node->state = state;
    node->next = top;
    top = node;
}

TreeVT VatTuPage::popState(HistoryNodeVT*& top) {
    if (!top) return nullptr;
    HistoryNodeVT* temp = top;
    TreeVT state = temp->state;
    top = top->next;
    delete temp;
    return state;
}

void VatTuPage::clearStack(HistoryNodeVT*& top) {
    while (top) {
        HistoryNodeVT* temp = top;
        top = top->next;
        huyCayVT(temp->state);
        delete temp;
    }
}

void VatTuPage::lamMoiBang() {
    int soLuong = 0;
    VATTU* ds = duyetTheoTen(root, soLuong);

    QString keyword = ui->timKiemEdit->text().trimmed().toUpper();

    int soKhop = 0;
    for (int i = 0; i < soLuong; i++) {
        QString mavt = QString::fromUtf8(ds[i].MAVT).toUpper();
        QString tenvt = QString::fromUtf8(ds[i].TENVT).toUpper();
        QString dvt = QString::fromUtf8(ds[i].DVT).toUpper();
        if (keyword.isEmpty() || mavt.contains(keyword) || tenvt.contains(keyword) || dvt.contains(keyword)) {
            soKhop++;
        }
    }

    ui->table->setRowCount(soKhop);
    int row = 0;
    for (int i = 0; i < soLuong; i++) {
        QString mavt = QString::fromUtf8(ds[i].MAVT).toUpper();
        QString tenvt = QString::fromUtf8(ds[i].TENVT).toUpper();
        QString dvt = QString::fromUtf8(ds[i].DVT).toUpper();
        if (keyword.isEmpty() || mavt.contains(keyword) || tenvt.contains(keyword) || dvt.contains(keyword)) {
            ui->table->setItem(row, 0, new QTableWidgetItem(ds[i].MAVT));
            ui->table->setItem(row, 1, new QTableWidgetItem(ds[i].TENVT));
            ui->table->setItem(row, 2, new QTableWidgetItem(ds[i].DVT));
            ui->table->setItem(row, 3, new QTableWidgetItem(QString::number(ds[i].SoLuongTon)));
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