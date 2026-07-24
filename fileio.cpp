#include "fileio.h"
#include "vattulogic.h"
#include "nhanvienlogic.h"
#include <fstream>
#include <sstream>
#include <cstring>
#include <QDir>
#include <QFileInfo>

using namespace std;

// Helper de duyet cay VatTu va ghi ra file theo Pre-order
static void luuVatTuHelper(nodeVT* root, ofstream& outFile) {
    if (root == nullptr) return;
    outFile << root->vt.MAVT << "|"
            << root->vt.TENVT << "|"
            << root->vt.DVT << "|"
            << root->vt.SoLuongTon << "\n";
    luuVatTuHelper(root->left, outFile);
    luuVatTuHelper(root->right, outFile);
}

// Luu toan bo cay vat tu ra file
void luuVatTu(TreeVT root, const string& duongDan) {
    // Tu dong tao thu muc cha neu chua co
    QFileInfo fileInfo(QString::fromStdString(duongDan));
    QDir dir = fileInfo.dir();
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    ofstream outFile(duongDan);
    if (!outFile.is_open()) return;
    luuVatTuHelper(root, outFile);
    outFile.close();
}

// Doc file, nap du lieu vao cay (root truyen vao phai dang rong/nullptr)
void docVatTu(TreeVT& root, const string& duongDan) {
    ifstream inFile(duongDan);
    if (!inFile.is_open()) return;
    
    // Giai phong cay cu neu co du lieu
    huyCayVT(root);
    
    string line;
    while (getline(inFile, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string mavt, tenvt, dvt, slTonStr;
        if (getline(ss, mavt, '|') &&
            getline(ss, tenvt, '|') &&
            getline(ss, dvt, '|') &&
            getline(ss, slTonStr, '|')) {
            int slTon = stoi(slTonStr);
            string loi;
            themVT(root, mavt.c_str(), tenvt.c_str(), dvt.c_str(), slTon, loi);
        }
    }
    inFile.close();
}

// Luu toan bo mang nhan vien (kem theo hoa don + chi tiet hoa don) ra file
void luuNhanVien(const DS_NHANVIEN& ds, const string& duongDan) {
    // Tu dong tao thu muc cha neu chua co
    QFileInfo fileInfo(QString::fromStdString(duongDan));
    QDir dir = fileInfo.dir();
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    ofstream outFile(duongDan);
    if (!outFile.is_open()) return;

    for (int i = 0; i < ds.n; i++) {
        NHANVIEN* nv = ds.nodes[i];
        outFile << "NV|" << nv->MANV << "|"
                << nv->HO << "|"
                << nv->TEN << "|"
                << nv->PHAI << "\n";
        
        nodeHD* currentHD = nv->dshd;
        while (currentHD != nullptr) {
            HOADON& hd = currentHD->hd;
            outFile << "HD|" << hd.SoHD << "|"
                    << hd.NgayLap.ngay << "|"
                    << hd.NgayLap.thang << "|"
                    << hd.NgayLap.nam << "|"
                    << hd.Loai << "\n";
            
            for (int j = 0; j < hd.dscthd.n; j++) {
                CT_HOADON& ct = hd.dscthd.nodes[j];
                outFile << "CT|" << ct.MAVT << "|"
                        << ct.SoLuong << "|"
                        << ct.DonGia << "|"
                        << ct.VAT << "\n";
            }
            currentHD = currentHD->next;
        }
    }
    outFile.close();
}

// Doc file, nap du lieu vao mang nhan vien
void docNhanVien(DS_NHANVIEN& ds, const string& duongDan) {
    ifstream inFile(duongDan);
    if (!inFile.is_open()) return;

    // Giai phong danh sach cu neu co du lieu
    huyDSNV(ds);

    string line;
    NHANVIEN* currentNV = nullptr;
    nodeHD* currentHD = nullptr;

    while (getline(inFile, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string type;
        if (!getline(ss, type, '|')) continue;

        if (type == "NV") {
            string manv, ho, ten, phai;
            if (getline(ss, manv, '|') &&
                getline(ss, ho, '|') &&
                getline(ss, ten, '|') &&
                getline(ss, phai, '|')) {
                
                if (ds.n >= MAX_NV) continue;
                NHANVIEN* nv = new NHANVIEN();
                strncpy(nv->MANV, manv.c_str(), 10); nv->MANV[10] = '\0';
                strncpy(nv->HO, ho.c_str(), 30); nv->HO[30] = '\0';
                strncpy(nv->TEN, ten.c_str(), 20); nv->TEN[20] = '\0';
                strncpy(nv->PHAI, phai.c_str(), 3); nv->PHAI[3] = '\0';
                nv->CoHD = false;
                nv->dshd = nullptr;

                ds.nodes[ds.n++] = nv;
                currentNV = nv;
                currentHD = nullptr;
            }
        } 
        else if (type == "HD") {
            if (currentNV == nullptr) continue;
            currentNV->CoHD = true;
            string soHD, ngayStr, thangStr, namStr, loaiStr;
            if (getline(ss, soHD, '|') &&
                getline(ss, ngayStr, '|') &&
                getline(ss, thangStr, '|') &&
                getline(ss, namStr, '|') &&
                getline(ss, loaiStr, '|')) {
                
                Date ngay;
                ngay.ngay = stoi(ngayStr);
                ngay.thang = stoi(thangStr);
                ngay.nam = stoi(namStr);
                char loai = loaiStr.empty() ? 'N' : loaiStr[0];

                nodeHD* hdMoi = new nodeHD();
                strncpy(hdMoi->hd.SoHD, soHD.c_str(), 20); hdMoi->hd.SoHD[20] = '\0';
                hdMoi->hd.NgayLap = ngay;
                hdMoi->hd.Loai = loai;
                hdMoi->hd.dscthd.n = 0;
                hdMoi->next = nullptr;

                // Them vao cuoi danh sach lien ket de giu dung thu tu doc tu file
                if (currentNV->dshd == nullptr) {
                    currentNV->dshd = hdMoi;
                } else {
                    nodeHD* temp = currentNV->dshd;
                    while (temp->next != nullptr) {
                        temp = temp->next;
                    }
                    temp->next = hdMoi;
                }
                currentHD = hdMoi;
            }
        } 
        else if (type == "CT") {
            if (currentHD == nullptr) continue;
            string mavt, soLuongStr, donGiaStr, vatStr;
            if (getline(ss, mavt, '|') &&
                getline(ss, soLuongStr, '|') &&
                getline(ss, donGiaStr, '|') &&
                getline(ss, vatStr, '|')) {
                
                if (currentHD->hd.dscthd.n >= SO_VT_TOIDA_MOI_HD) continue;
                CT_HOADON& ct = currentHD->hd.dscthd.nodes[currentHD->hd.dscthd.n++];
                strncpy(ct.MAVT, mavt.c_str(), 10); ct.MAVT[10] = '\0';
                ct.SoLuong = stoi(soLuongStr);
                ct.DonGia = stof(donGiaStr);
                ct.VAT = stof(vatStr);
            }
        }
    }
    inFile.close();
}