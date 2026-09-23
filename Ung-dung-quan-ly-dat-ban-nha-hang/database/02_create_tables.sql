USE RestaurantManagement;
GO
  
CREATE TABLE KhuVuc
(
    MaKhuVuc VARCHAR(10) PRIMARY KEY,
    TenKhuVuc NVARCHAR(100) NOT NULL,
    MoTa NVARCHAR(255)
);
GO
  
CREATE TABLE Ban
(
    MaBan VARCHAR(10) PRIMARY KEY,
    TenBan NVARCHAR(50) NOT NULL,
    SoCho INT NOT NULL,
    TrangThai NVARCHAR(30) NOT NULL DEFAULT N'Trống',
    MaKhuVuc VARCHAR(10) NOT NULL,

    CONSTRAINT CK_Ban_SoCho
        CHECK (SoCho > 0),

    CONSTRAINT CK_Ban_TrangThai
        CHECK (TrangThai IN
        (
            N'Trống',
            N'Đã đặt',
            N'Đang sử dụng',
            N'Bảo trì'
        )),

    CONSTRAINT FK_Ban_KhuVuc
        FOREIGN KEY (MaKhuVuc)
        REFERENCES KhuVuc(MaKhuVuc)
);
GO

CREATE TABLE KhachHang
(
    MaKH VARCHAR(10) PRIMARY KEY,
    HoTen NVARCHAR(100) NOT NULL,
    SoDienThoai VARCHAR(15) NOT NULL UNIQUE,
    Email VARCHAR(100),
    DiaChi NVARCHAR(255),
    NgayTao DATETIME NOT NULL DEFAULT GETDATE()
);
GO

CREATE TABLE NhanVien
(
    MaNV VARCHAR(10) PRIMARY KEY,
    HoTen NVARCHAR(100) NOT NULL,
    GioiTinh NVARCHAR(10),
    NgaySinh DATE,
    SoDienThoai VARCHAR(15) UNIQUE,
    Email VARCHAR(100),
    ChucVu NVARCHAR(50),
    Luong DECIMAL(18,2) DEFAULT 0,
    TrangThai NVARCHAR(30) DEFAULT N'Đang làm'
);
GO

CREATE TABLE TaiKhoan
(
    MaTK VARCHAR(10) PRIMARY KEY,
    TenDangNhap VARCHAR(50) NOT NULL UNIQUE,
    MatKhau VARCHAR(255) NOT NULL,
    VaiTro NVARCHAR(30) NOT NULL,
    MaNV VARCHAR(10) NOT NULL UNIQUE,
    TrangThai NVARCHAR(30) DEFAULT N'Hoạt động',

    CONSTRAINT CK_TaiKhoan_VaiTro
        CHECK (VaiTro IN
        (
            N'Admin',
            N'Quản lý',
            N'Nhân viên'
        )),

    CONSTRAINT FK_TaiKhoan_NhanVien
        FOREIGN KEY (MaNV)
        REFERENCES NhanVien(MaNV)
);
GO


-- =============================================
-- 6. BẢNG ĐẶT BÀN
-- =============================================

CREATE TABLE DatBan
(
    MaDatBan VARCHAR(10) PRIMARY KEY,
    MaKH VARCHAR(10) NOT NULL,
    MaBan VARCHAR(10) NOT NULL,
    MaNV VARCHAR(10),
    NgayDat DATE NOT NULL,
    GioDat TIME NOT NULL,
    SoNguoi INT NOT NULL,
    TrangThai NVARCHAR(30) NOT NULL DEFAULT N'Đã đặt',
    GhiChu NVARCHAR(255),

    CONSTRAINT CK_DatBan_SoNguoi
        CHECK (SoNguoi > 0),

    CONSTRAINT CK_DatBan_TrangThai
        CHECK (TrangThai IN
        (
            N'Đã đặt',
            N'Đã nhận bàn',
            N'Đã hủy',
            N'Hoàn thành'
        )),

    CONSTRAINT FK_DatBan_KhachHang
        FOREIGN KEY (MaKH)
        REFERENCES KhachHang(MaKH),

    CONSTRAINT FK_DatBan_Ban
        FOREIGN KEY (MaBan)
        REFERENCES Ban(MaBan),

    CONSTRAINT FK_DatBan_NhanVien
        FOREIGN KEY (MaNV)
        REFERENCES NhanVien(MaNV)
);
GO


-- =============================================
-- 7. BẢNG MÓN ĂN
-- =============================================

CREATE TABLE MonAn
(
    MaMon VARCHAR(10) PRIMARY KEY,
    TenMon NVARCHAR(100) NOT NULL,
    LoaiMon NVARCHAR(50),
    DonGia DECIMAL(18,2) NOT NULL,
    DonViTinh NVARCHAR(30) DEFAULT N'Phần',
    MoTa NVARCHAR(255),
    HinhAnh VARCHAR(255),
    TrangThai NVARCHAR(30) DEFAULT N'Đang bán',

    CONSTRAINT CK_MonAn_DonGia
        CHECK (DonGia >= 0),

    CONSTRAINT CK_MonAn_TrangThai
        CHECK (TrangThai IN
        (
            N'Đang bán',
            N'Hết món',
            N'Ngừng bán'
        ))
);
GO

CREATE TABLE KhuyenMai
(
    MaKM VARCHAR(10) PRIMARY KEY,
    TenKM NVARCHAR(100) NOT NULL,
    PhanTramGiam DECIMAL(5,2) NOT NULL,
    NgayBatDau DATE NOT NULL,
    NgayKetThuc DATE NOT NULL,
    DieuKien DECIMAL(18,2) DEFAULT 0,
    TrangThai NVARCHAR(30) DEFAULT N'Hoạt động',

    CONSTRAINT CK_KhuyenMai_PhanTram
        CHECK (PhanTramGiam >= 0 AND PhanTramGiam <= 100),

    CONSTRAINT CK_KhuyenMai_Ngay
        CHECK (NgayKetThuc >= NgayBatDau)
);
GO

CREATE TABLE HoaDon
(
    MaHD VARCHAR(10) PRIMARY KEY,
    MaDatBan VARCHAR(10) NOT NULL,
    MaNV VARCHAR(10),
    MaKM VARCHAR(10),
    NgayLap DATETIME NOT NULL DEFAULT GETDATE(),
    TongTien DECIMAL(18,2) NOT NULL DEFAULT 0,
    TienGiam DECIMAL(18,2) NOT NULL DEFAULT 0,
    ThanhTien DECIMAL(18,2) NOT NULL DEFAULT 0,
    PhuongThucThanhToan NVARCHAR(30),
    TrangThai NVARCHAR(30) DEFAULT N'Chưa thanh toán',

    CONSTRAINT CK_HoaDon_TongTien
        CHECK (TongTien >= 0),

    CONSTRAINT CK_HoaDon_TienGiam
        CHECK (TienGiam >= 0),

    CONSTRAINT CK_HoaDon_ThanhTien
        CHECK (ThanhTien >= 0),

    CONSTRAINT CK_HoaDon_PhuongThuc
        CHECK
        (
            PhuongThucThanhToan IS NULL
            OR PhuongThucThanhToan IN
            (
                N'Tiền mặt',
                N'Chuyển khoản',
                N'Ví điện tử'
            )
        ),

    CONSTRAINT CK_HoaDon_TrangThai
        CHECK
        (
            TrangThai IN
            (
                N'Chưa thanh toán',
                N'Đã thanh toán',
                N'Đã hủy'
            )
        ),

    CONSTRAINT FK_HoaDon_DatBan
        FOREIGN KEY (MaDatBan)
        REFERENCES DatBan(MaDatBan),

    CONSTRAINT FK_HoaDon_NhanVien
        FOREIGN KEY (MaNV)
        REFERENCES NhanVien(MaNV),

    CONSTRAINT FK_HoaDon_KhuyenMai
        FOREIGN KEY (MaKM)
        REFERENCES KhuyenMai(MaKM)
);
GO

CREATE TABLE ChiTietHoaDon
(
    MaHD VARCHAR(10) NOT NULL,
    MaMon VARCHAR(10) NOT NULL,
    SoLuong INT NOT NULL,
    DonGia DECIMAL(18,2) NOT NULL,

    ThanhTien AS (SoLuong * DonGia) PERSISTED,

    CONSTRAINT PK_ChiTietHoaDon
        PRIMARY KEY (MaHD, MaMon),

    CONSTRAINT CK_ChiTietHoaDon_SoLuong
        CHECK (SoLuong > 0),

    CONSTRAINT CK_ChiTietHoaDon_DonGia
        CHECK (DonGia >= 0),

    CONSTRAINT FK_ChiTietHoaDon_HoaDon
        FOREIGN KEY (MaHD)
        REFERENCES HoaDon(MaHD),

    CONSTRAINT FK_ChiTietHoaDon_MonAn
        FOREIGN KEY (MaMon)
        REFERENCES MonAn(MaMon)
);
GO


PRINT N'Đã tạo toàn bộ bảng thành công!';
GO
