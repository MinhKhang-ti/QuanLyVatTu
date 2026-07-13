#include "hoadonlogic.h"
#include <cstring>

nodeHD* taoHoaDon(const char* soHD, Date ngay, char loai) {
    nodeHD* p = new nodeHD();
    strncpy(p->hd.SoHD, soHD, 20); p->hd.SoHD[20] = '\0';
    p->hd.NgayLap = ngay;
    p->hd.Loai = loai;
    p->hd.dscthd.n = 0;
    p->next = nullptr;
    return p;
}

void themHoaDonVaoDS(PTRHD& dshd, nodeHD* hdMoi) {
    hdMoi->next = dshd;
    dshd = hdMoi;
}

nodeHD* timHoaDon(PTRHD dshd, const char* soHD) {
    nodeHD* p = dshd;
    while (p) {
        if (strcmp(p->hd.SoHD, soHD) == 0) return p;
        p = p->next;
    }
    return nullptr;
}

bool themCTHD(DS_CTHD& ds, const char* mavt, int soLuong, float donGia, float vat, std::string& loi) {
    if (ds.n >= SO_VT_TOIDA_MOI_HD) {
        loi = "Hóa đơn đã đủ tối đa" + std::to_string(SO_VT_TOIDA_MOI_HD) + " vật tư";
        return false;
    }
    for (int i = 0; i < ds.n; i++) {
        if (strcmp(ds.nodes[i].MAVT, mavt) == 0) {
            loi = "Vật tư này đã có trong hóa đơn";
            return false;
        }
    }
    CT_HOADON& ct = ds.nodes[ds.n];
    strncpy(ct.MAVT, mavt, 10); ct.MAVT[10] = '\0';
    ct.SoLuong = soLuong;
    ct.DonGia = donGia;
    ct.VAT = vat;
    ds.n++;
    return true;
}

bool xoaCTHD(DS_CTHD& ds, const char* mavt, std::string& loi) {
    for (int i = 0; i < ds.n; i++) {
        if (strcmp(ds.nodes[i].MAVT, mavt) == 0) {
            for (int j = i; j < ds.n - 1; j++) ds.nodes[j] = ds.nodes[j + 1];
            ds.n--;
            return true;
        }
    }
    loi = "Không tìm thấy vật tư trong hóa đơn ";
    return false;
}

double tinhTriGiaDong(const CT_HOADON& ct) {
    double thanhTien = ct.SoLuong * ct.DonGia;
    return thanhTien + thanhTien * (ct.VAT / 100.0);
}

double tinhTongTriGiaHD(const DS_CTHD& ds) {
    double tong = 0;
    for (int i = 0; i < ds.n; i++) tong += tinhTriGiaDong(ds.nodes[i]);
    return tong;
}

void huyDSHoaDon(PTRHD& dshd) {
    while (dshd) {
        nodeHD* t = dshd;
        dshd = dshd->next;
        delete t;
    }
}