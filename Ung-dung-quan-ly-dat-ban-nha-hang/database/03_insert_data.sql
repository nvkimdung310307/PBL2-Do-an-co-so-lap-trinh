USE RestaurantManagement;
GO

INSERT INTO KhuVuc
(
    MaKhuVuc,
    TenKhuVuc,
    MoTa
)
VALUES
('KV01', N'Tầng 1', N'Khu vực tầng 1'),
('KV02', N'Tầng 2', N'Khu vực tầng 2'),
('KV03', N'Phòng VIP', N'Phòng riêng cao cấp');
GO

INSERT INTO Ban
(
    MaBan,
    TenBan,
    SoCho,
    TrangThai,
    MaKhuVuc
)
VALUES
('B001', N'Bàn 01', 2, N'Trống', 'KV01'),
('B002', N'Bàn 02', 4, N'Trống', 'KV01'),
('B003', N'Bàn 03', 4, N'Trống', 'KV01'),
('B004', N'Bàn 04', 6, N'Trống', 'KV01'),
('B005', N'Bàn 05', 6, N'Trống', 'KV02'),
('B006', N'Bàn 06', 8, N'Trống', 'KV02'),
('B007', N'Bàn 07', 10, N'Trống', 'KV02'),
('B008', N'Bàn VIP 01', 10, N'Trống', 'KV03'),
('B009', N'Bàn VIP 02', 12, N'Trống', 'KV03');
GO

INSERT INTO KhachHang
(
    MaKH,
    HoTen,
    SoDienThoai,
    Email,
    DiaChi
)
VALUES
(
    'KH001',
    N'Nguyễn Văn An',
    '0905000001',
    'an@gmail.com',
    N'Đà Nẵng'
),
(
    'KH002',
    N'Trần Thị Bình',
    '0905000002',
    'binh@gmail.com',
    N'Hội An'
),
(
    'KH003',
    N'Lê Minh Anh',
    '0905000003',
    'anh@gmail.com',
    N'Đà Nẵng'
),
(
    'KH004',
    N'Phạm Văn Nam',
    '0905000004',
    'nam@gmail.com',
    N'Quảng Nam'
);
GO

INSERT INTO NhanVien
(
    MaNV,
    HoTen,
    GioiTinh,
    NgaySinh,
    SoDienThoai,
    Email,
    ChucVu,
    Luong,
    TrangThai
)
VALUES
(
    'NV001',
    N'Nguyễn Văn Quản',
    N'Nam',
    '1995-05-10',
    '0911000001',
    'quan@restaurant.com',
    N'Quản lý',
    15000000,
    N'Đang làm'
),
(
    'NV002',
    N'Trần Thị Lan',
    N'Nữ',
    '1998-08-15',
    '0911000002',
    'lan@restaurant.com',
    N'Nhân viên',
    9000000,
    N'Đang làm'
),
(
    'NV003',
    N'Lê Văn Minh',
    N'Nam',
    '2000-03-20',
    '0911000003',
    'minh@restaurant.com',
    N'Nhân viên',
    8500000,
    N'Đang làm'
);
GO

INSERT INTO TaiKhoan
(
    MaTK,
    TenDangNhap,
    MatKhau,
    VaiTro,
    MaNV,
    TrangThai
)
VALUES
(
    'TK001',
    'admin',
    '123456',
    N'Admin',
    'NV001',
    N'Hoạt động'
),
(
    'TK002',
    'lan',
    '123456',
    N'Nhân viên',
    'NV002',
    N'Hoạt động'
),
(
    'TK003',
    'minh',
    '123456',
    N'Nhân viên',
    'NV003',
    N'Hoạt động'
);
GO

INSERT INTO MonAn
(
    MaMon,
    TenMon,
    LoaiMon,
    DonGia,
    DonViTinh,
    MoTa,
    TrangThai
)
VALUES
(
    'M001',
    N'Cơm chiên Dương Châu',
    N'Món chính',
    60000,
    N'Phần',
    N'Cơm chiên truyền thống',
    N'Đang bán'
),
(
    'M002',
    N'Bò lúc lắc',
    N'Món chính',
    120000,
    N'Phần',
    N'Bò lúc lắc khoai tây',
    N'Đang bán'
),
(
    'M003',
    N'Mì xào hải sản',
    N'Món chính',
    95000,
    N'Phần',
    N'Mì xào với hải sản',
    N'Đang bán'
),
(
    'M004',
    N'Gà nướng',
    N'Món chính',
    180000,
    N'Phần',
    N'Gà nướng nguyên con',
    N'Đang bán'
),
(
    'M005',
    N'Rau củ xào',
    N'Khai vị',
    50000,
    N'Phần',
    N'Rau củ theo mùa',
    N'Đang bán'
),
(
    'M006',
    N'Khoai tây chiên',
    N'Khai vị',
    40000,
    N'Phần',
    N'Khoai tây chiên giòn',
    N'Đang bán'
),
(
    'M007',
    N'Coca Cola',
    N'Đồ uống',
    15000,
    N'Lon',
    N'Nước ngọt Coca Cola',
    N'Đang bán'
),
(
    'M008',
    N'Nước suối',
    N'Đồ uống',
    10000,
    N'Chai',
    N'Nước suối',
    N'Đang bán'
),
(
    'M009',
    N'Chè hạt sen',
    N'Tráng miệng',
    30000,
    N'Ly',
    N'Chè hạt sen',
    N'Đang bán'
);
GO


INSERT INTO KhuyenMai
(
    MaKM,
    TenKM,
    PhanTramGiam,
    NgayBatDau,
    NgayKetThuc,
    DieuKien,
    TrangThai
)
VALUES
(
    'KM001',
    N'Khuyến mãi khai trương',
    10,
    '2026-01-01',
    '2026-12-31',
    300000,
    N'Hoạt động'
),
(
    'KM002',
    N'Khuyến mãi VIP',
    15,
    '2026-01-01',
    '2026-12-31',
    500000,
    N'Hoạt động'
);
GO

INSERT INTO DatBan
(
    MaDatBan,
    MaKH,
    MaBan,
    MaNV,
    NgayDat,
    GioDat,
    SoNguoi,
    TrangThai,
    GhiChu
)
VALUES
(
    'DB001',
    'KH001',
    'B002',
    'NV002',
    '2026-09-25',
    '18:30',
    4,
    N'Đã đặt',
    N'Khách đặt bàn trước'
),
(
    'DB002',
    'KH002',
    'B004',
    'NV002',
    '2026-09-26',
    '19:00',
    5,
    N'Đã đặt',
    N'Khách yêu cầu bàn gần cửa sổ'
);
GO

UPDATE Ban
SET TrangThai = N'Đã đặt'
WHERE MaBan IN ('B002', 'B004');
GO

INSERT INTO HoaDon
(
    MaHD,
    MaDatBan,
    MaNV,
    MaKM,
    TongTien,
    TienGiam,
    ThanhTien,
    PhuongThucThanhToan,
    TrangThai
)
VALUES
(
    'HD001',
    'DB001',
    'NV002',
    'KM001',
    300000,
    30000,
    270000,
    N'Tiền mặt',
    N'Đã thanh toán'
);
GO

INSERT INTO ChiTietHoaDon
(
    MaHD,
    MaMon,
    SoLuong,
    DonGia
)
VALUES
('HD001', 'M001', 2, 60000),
('HD001', 'M002', 1, 120000),
('HD001', 'M007', 4, 15000);
GO


PRINT N'Đã thêm dữ liệu mẫu thành công!';
GO
