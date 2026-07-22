#ifndef VATTULOGIC_H
#define VATTULOGIC_H

#include "cautrucdulieu.h"
#include <string>

nodeVT* taoNodeVT(const char* mavt, const char* tenvt, const char* dvt, int soLuongTon);
nodeVT* timVT(TreeVT root, const char* mavt);
bool themVT(TreeVT& root, const char* mavt, const char* tenvt, const char* dvt,
            int soLuongTon, std::string& loi);
bool suaVT(TreeVT root, const char* mavt, const char* tenvtMoi, const char* dvtMoi,
           std::string& loi);
bool xoaVT(TreeVT& root, const char* mavt, std::string& loi);

// Dem so vat tu hien co trong cay
int demSoVT(TreeVT root);

// Cap phat mang dong (new[]), duyet inorder roi sap xep theo TENVT tang dan.
// Ghi so luong phan tu vao "soLuong". NGUOI GOI PHAI tu delete[] mang sau khi dung xong.
VATTU* duyetTheoTen(TreeVT root, int& soLuong);

void huyCayVT(TreeVT& root);

// Nhan ban sau toan bo cay (deep clone) - dung de luu trang thai cho Undo/Redo
nodeVT* cloneCayVT(nodeVT* root);

#endif // VATTULOGIC_H