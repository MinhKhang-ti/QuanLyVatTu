#include "nhanvienlogic.h"
#include <cstring>

void khoiTaoDSNV(DS_NHANVIEN& ds) {
    ds.n = 0;
}

int timViTriNV(const DS_NHANVIEN& ds, const char* manv) {
    for (int i = 0; i < ds.n; i++) {
        if (strcmp(ds.nodes[i]->MANV, manv) == 0) return i;
    }
    return -1;
}

static int timViTriChen(const DS_NHANVIEN& ds, const char* ten) {
    int i = 0;
    while (i < ds.n && strcmp(ds.nodes[i]->TEN, ten) <= 0) i++;
    return i;
}

bool themNV(DS_NHANVIEN& ds, const char* manv, const char* ho, const char* ten,
            const char* phai, std::string& loi) {
    if (ds.n >= MAX_NV) { loi = "Đã đủ tối đa " + std::to_string(MAX_NV) + " nhân viên"; return false; }
    if (strlen(ho) == 0 || strlen(ten) == 0) { loi = "Họ và tên không được rỗng"; return false; }
    if (timViTriNV(ds, manv) != -1) { loi = "Mã nhân viên đã tồn tại"; return false; }

    NHANVIEN* nv = new NHANVIEN();
    strncpy(nv->MANV, manv, 10); nv->MANV[10] = '\0';
    strncpy(nv->HO, ho, 30); nv->HO[30] = '\0';
    strncpy(nv->TEN, ten, 20); nv->TEN[20] = '\0';
    strncpy(nv->PHAI, phai, 3); nv->PHAI[3] = '\0';
    nv->dshd = nullptr;

    int viTri = timViTriChen(ds, ten);
    for (int i = ds.n; i > viTri; i--) ds.nodes[i] = ds.nodes[i - 1];
    ds.nodes[viTri] = nv;
    ds.n++;
    return true;
}

bool suaNV(DS_NHANVIEN& ds, const char* manv, const char* hoMoi, const char* tenMoi,
           const char* phaiMoi, std::string& loi) {
    int viTri = timViTriNV(ds, manv);
    if (viTri == -1) { loi = "Không tìm thấy nhân viên"; return false; }
    if (strlen(hoMoi) == 0 || strlen(tenMoi) == 0) { loi = "Họ và tên không được rỗng"; return false; }

    NHANVIEN* nv = ds.nodes[viTri];
    bool doiTen = (strcmp(nv->TEN, tenMoi) != 0);
    strncpy(nv->HO, hoMoi, 30); nv->HO[30] = '\0';
    strncpy(nv->PHAI, phaiMoi, 3); nv->PHAI[3] = '\0';

    if (doiTen) {
        for (int i = viTri; i < ds.n - 1; i++) ds.nodes[i] = ds.nodes[i + 1];
        ds.n--;
        strncpy(nv->TEN, tenMoi, 20); nv->TEN[20] = '\0';
        int viTriMoi = timViTriChen(ds, tenMoi);
        for (int i = ds.n; i > viTriMoi; i--) ds.nodes[i] = ds.nodes[i - 1];
        ds.nodes[viTriMoi] = nv;
        ds.n++;
    } else {
        strncpy(nv->TEN, tenMoi, 20); nv->TEN[20] = '\0';
    }
    return true;
}

bool xoaNV(DS_NHANVIEN& ds, const char* manv, std::string& loi) {
    int viTri = timViTriNV(ds, manv);
    if (viTri == -1) { loi = "Không tìm thấy nhân viên"; return false; }
    delete ds.nodes[viTri];
    for (int i = viTri; i < ds.n - 1; i++) ds.nodes[i] = ds.nodes[i + 1];
    ds.n--;
    return true;
}

void huyDSNV(DS_NHANVIEN& ds) {
    for (int i = 0; i < ds.n; i++) delete ds.nodes[i];
    ds.n = 0;
}