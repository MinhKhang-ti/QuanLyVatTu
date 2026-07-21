#ifndef FILEIO_H
#define FILEIO_H

#include "cautrucdulieu.h"
#include <string>

// Duong dan file mac dinh
const std::string FILE_VATTU = "../../data/vattu.txt";
const std::string FILE_NHANVIEN = "../../data/nhanvien.txt";

// Luu toan bo cay vat tu ra file
void luuVatTu(TreeVT root, const std::string& duongDan);

// Doc file, nap du lieu vao cay (root truyen vao phai dang rong/nullptr)
void docVatTu(TreeVT& root, const std::string& duongDan);

// Luu toan bo mang nhan vien (kem theo hoa don + chi tiet hoa don long ben trong) ra file
void luuNhanVien(const DS_NHANVIEN& ds, const std::string& duongDan);

// Doc file, nap du lieu vao mang nhan vien (ds truyen vao phai dang rong, n=0)
void docNhanVien(DS_NHANVIEN& ds, const std::string& duongDan);

#endif // FILEIO_H
