USE RestaurantManagement;
GO

CREATE OR ALTER PROCEDURE sp_DangNhap
    @TenDangNhap VARCHAR(50),
    @MatKhau VARCHAR(255)
AS
BEGIN
    SET NOCOUNT ON;

    SELECT
        TK.MaTK,
        TK.TenDangNhap,
        TK.VaiTro,
        TK.TrangThai,
        NV.MaNV,
        NV.HoTen
    FROM TaiKhoan TK
    INNER JOIN NhanVien NV
        ON TK.MaNV = NV.MaNV
    WHERE TK.TenDangNhap = @TenDangNhap
      AND TK.MatKhau = @MatKhau
      AND TK.TrangThai = N'Hoạt động';
END;
GO

CREATE OR ALTER PROCEDURE sp_GetDanhSachBan
AS
BEGIN
    SET NOCOUNT ON;

    SELECT
        B.MaBan,
        B.TenBan,
        B.SoCho,
        B.TrangThai,
        KV.MaKhuVuc,
        KV.TenKhuVuc
    FROM Ban B
    INNER JOIN KhuVuc KV
        ON B.MaKhuVuc = KV.MaKhuVuc
    ORDER BY B.MaBan;
END;
GO
  

CREATE OR ALTER PROCEDURE sp_TimBanPhuHop
    @SoNguoi INT
AS
BEGIN
    SET NOCOUNT ON;

    SELECT
        B.MaBan,
        B.TenBan,
        B.SoCho,
        B.TrangThai,
        KV.TenKhuVuc
    FROM Ban B
    INNER JOIN KhuVuc KV
        ON B.MaKhuVuc = KV.MaKhuVuc
    WHERE B.SoCho >= @SoNguoi
      AND B.TrangThai = N'Trống'
    ORDER BY B.SoCho ASC;
END;
GO

CREATE OR ALTER PROCEDURE sp_TimKhachHang
    @TuKhoa NVARCHAR(100)
AS
BEGIN
    SET NOCOUNT ON;

    SELECT
        MaKH,
        HoTen,
        SoDienThoai,
        Email,
        DiaChi,
        NgayTao
    FROM KhachHang
    WHERE HoTen LIKE N'%' + @TuKhoa + N'%'
       OR SoDienThoai LIKE '%' + @TuKhoa + '%'
       OR Email LIKE '%' + @TuKhoa + '%'
    ORDER BY HoTen;
END;
GO

CREATE OR ALTER PROCEDURE sp_GetDanhSachDatBan
AS
BEGIN
    SET NOCOUNT ON;

    SELECT
        DB.MaDatBan,
        KH.HoTen AS TenKhachHang,
        KH.SoDienThoai,
        B.TenBan,
        DB.NgayDat,
        DB.GioDat,
        DB.SoNguoi,
        NV.HoTen AS NhanVien,
        DB.TrangThai,
        DB.GhiChu
    FROM DatBan DB

    INNER JOIN KhachHang KH
        ON DB.MaKH = KH.MaKH

    INNER JOIN Ban B
        ON DB.MaBan = B.MaBan

    LEFT JOIN NhanVien NV
        ON DB.MaNV = NV.MaNV

    ORDER BY DB.NgayDat DESC, DB.GioDat DESC;
END;
GO


-- =============================================
-- 6. KIỂM TRA BÀN ĐÃ ĐƯỢC ĐẶT
-- =============================================

CREATE OR ALTER PROCEDURE sp_KiemTraDatBan
    @MaBan VARCHAR(10),
    @NgayDat DATE,
    @GioDat TIME
AS
BEGIN
    SET NOCOUNT ON;

    IF EXISTS
    (
        SELECT 1
        FROM DatBan
        WHERE MaBan = @MaBan
          AND NgayDat = @NgayDat
          AND GioDat = @GioDat
          AND TrangThai IN
          (
              N'Đã đặt',
              N'Đã nhận bàn'
          )
    )
    BEGIN
        SELECT
            CAST(1 AS BIT) AS DaTonTai,
            N'Bàn đã được đặt trong thời gian này!' AS ThongBao;
    END
    ELSE
    BEGIN
        SELECT
            CAST(0 AS BIT) AS DaTonTai,
            N'Bàn còn trống!' AS ThongBao;
    END
END;
GO


-- =============================================
-- 7. TẠO ĐẶT BÀN
-- =============================================

CREATE OR ALTER PROCEDURE sp_TaoDatBan
    @MaDatBan VARCHAR(10),
    @MaKH VARCHAR(10),
    @MaBan VARCHAR(10),
    @MaNV VARCHAR(10),
    @NgayDat DATE,
    @GioDat TIME,
    @SoNguoi INT,
    @GhiChu NVARCHAR(255)
AS
BEGIN
    SET NOCOUNT ON;

    BEGIN TRY

        BEGIN TRANSACTION;

        -- Kiểm tra khách hàng
        IF NOT EXISTS
        (
            SELECT 1
            FROM KhachHang
            WHERE MaKH = @MaKH
        )
        BEGIN
            THROW 50001, N'Khách hàng không tồn tại!', 1;
        END;


        -- Kiểm tra bàn
        IF NOT EXISTS
        (
            SELECT 1
            FROM Ban
            WHERE MaBan = @MaBan
        )
        BEGIN
            THROW 50002, N'Bàn không tồn tại!', 1;
        END;


        -- Kiểm tra số chỗ
        IF EXISTS
        (
            SELECT 1
            FROM Ban
            WHERE MaBan = @MaBan
              AND SoCho < @SoNguoi
        )
        BEGIN
            THROW 50003, N'Bàn không đủ số chỗ!', 1;
        END;


        -- Kiểm tra trùng giờ
        IF EXISTS
        (
            SELECT 1
            FROM DatBan
            WHERE MaBan = @MaBan
              AND NgayDat = @NgayDat
              AND GioDat = @GioDat
              AND TrangThai IN
              (
                  N'Đã đặt',
                  N'Đã nhận bàn'
              )
        )
        BEGIN
            THROW 50004, N'Bàn đã được đặt trong thời gian này!', 1;
        END;


        -- Thêm đặt bàn
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
            @MaDatBan,
            @MaKH,
            @MaBan,
            @MaNV,
            @NgayDat,
            @GioDat,
            @SoNguoi,
            N'Đã đặt',
            @GhiChu
        );


        -- Cập nhật trạng thái bàn
        UPDATE Ban
        SET TrangThai = N'Đã đặt'
        WHERE MaBan = @MaBan;


        COMMIT TRANSACTION;

        SELECT
            1 AS ThanhCong,
            N'Đặt bàn thành công!' AS ThongBao;

    END TRY

    BEGIN CATCH

        IF @@TRANCOUNT > 0
            ROLLBACK TRANSACTION;

        THROW;

    END CATCH
END;
GO
  

CREATE OR ALTER PROCEDURE sp_HuyDatBan
    @MaDatBan VARCHAR(10)
AS
BEGIN
    SET NOCOUNT ON;

    BEGIN TRY

        BEGIN TRANSACTION;

        DECLARE @MaBan VARCHAR(10);

        SELECT @MaBan = MaBan
        FROM DatBan
        WHERE MaDatBan = @MaDatBan;

        IF @MaBan IS NULL
        BEGIN
            THROW 50005, N'Không tìm thấy mã đặt bàn!', 1;
        END;


        UPDATE DatBan
        SET TrangThai = N'Đã hủy'
        WHERE MaDatBan = @MaDatBan;


        UPDATE Ban
        SET TrangThai = N'Trống'
        WHERE MaBan = @MaBan;


        COMMIT TRANSACTION;

        SELECT
            1 AS ThanhCong,
            N'Hủy đặt bàn thành công!' AS ThongBao;

    END TRY

    BEGIN CATCH

        IF @@TRANCOUNT > 0
            ROLLBACK TRANSACTION;

        THROW;

    END CATCH
END;
GO


-- =============================================
-- 9. TÍNH TỔNG TIỀN HÓA ĐƠN
-- =============================================

CREATE OR ALTER PROCEDURE sp_TinhTongHoaDon
    @MaHD VARCHAR(10)
AS
BEGIN
    SET NOCOUNT ON;

    DECLARE @TongTien DECIMAL(18,2);

    SELECT
        @TongTien = ISNULL(SUM(SoLuong * DonGia), 0)
    FROM ChiTietHoaDon
    WHERE MaHD = @MaHD;


    UPDATE HoaDon
    SET
        TongTien = @TongTien,
        ThanhTien =
            CASE
                WHEN @TongTien - TienGiam < 0 THEN 0
                ELSE @TongTien - TienGiam
            END
    WHERE MaHD = @MaHD;


    SELECT
        MaHD,
        TongTien,
        TienGiam,
        ThanhTien
    FROM HoaDon
    WHERE MaHD = @MaHD;
END;
GO


-- =============================================
-- 10. THANH TOÁN HÓA ĐƠN
-- =============================================

CREATE OR ALTER PROCEDURE sp_ThanhToanHoaDon
    @MaHD VARCHAR(10),
    @PhuongThucThanhToan NVARCHAR(30)
AS
BEGIN
    SET NOCOUNT ON;

    BEGIN TRY

        BEGIN TRANSACTION;

        DECLARE @MaDatBan VARCHAR(10);
        DECLARE @MaBan VARCHAR(10);

        SELECT
            @MaDatBan = MaDatBan
        FROM HoaDon
        WHERE MaHD = @MaHD;


        IF @MaDatBan IS NULL
        BEGIN
            THROW 50006, N'Không tìm thấy hóa đơn!', 1;
        END;


        SELECT
            @MaBan = MaBan
        FROM DatBan
        WHERE MaDatBan = @MaDatBan;


        -- Tính tiền trước khi thanh toán
        EXEC sp_TinhTongHoaDon @MaHD;


        UPDATE HoaDon
        SET
            TrangThai = N'Đã thanh toán',
            PhuongThucThanhToan = @PhuongThucThanhToan
        WHERE MaHD = @MaHD;


        -- Hoàn thành đặt bàn
        UPDATE DatBan
        SET TrangThai = N'Hoàn thành'
        WHERE MaDatBan = @MaDatBan;


        -- Trả bàn về trạng thái trống
        UPDATE Ban
        SET TrangThai = N'Trống'
        WHERE MaBan = @MaBan;


        COMMIT TRANSACTION;

        SELECT
            1 AS ThanhCong,
            N'Thanh toán thành công!' AS ThongBao;

    END TRY

    BEGIN CATCH

        IF @@TRANCOUNT > 0
            ROLLBACK TRANSACTION;

        THROW;

    END CATCH
END;
GO


CREATE OR ALTER PROCEDURE sp_DoanhThuTheoNgay
    @Ngay DATE
AS
BEGIN
    SET NOCOUNT ON;

    SELECT
        @Ngay AS Ngay,
        COUNT(MaHD) AS SoHoaDon,
        ISNULL(SUM(ThanhTien), 0) AS DoanhThu
    FROM HoaDon
    WHERE CAST(NgayLap AS DATE) = @Ngay
      AND TrangThai = N'Đã thanh toán';
END;
GO
  

CREATE OR ALTER PROCEDURE sp_DoanhThuTheoThang
    @Nam INT,
    @Thang INT
AS
BEGIN
    SET NOCOUNT ON;

    SELECT
        @Nam AS Nam,
        @Thang AS Thang,
        COUNT(MaHD) AS SoHoaDon,
        ISNULL(SUM(ThanhTien), 0) AS DoanhThu
    FROM HoaDon
    WHERE YEAR(NgayLap) = @Nam
      AND MONTH(NgayLap) = @Thang
      AND TrangThai = N'Đã thanh toán';
END;
GO

  
CREATE OR ALTER PROCEDURE sp_TopMonAnBanChay
    @SoLuong INT = 5
AS
BEGIN
    SET NOCOUNT ON;

    SELECT TOP (@SoLuong)
        M.MaMon,
        M.TenMon,
        SUM(CT.SoLuong) AS SoLuongBan,
        SUM(CT.SoLuong * CT.DonGia) AS DoanhThu
    FROM ChiTietHoaDon CT
    INNER JOIN MonAn M
        ON CT.MaMon = M.MaMon
    INNER JOIN HoaDon HD
        ON CT.MaHD = HD.MaHD
    WHERE HD.TrangThai = N'Đã thanh toán'
    GROUP BY
        M.MaMon,
        M.TenMon
    ORDER BY
        SoLuongBan DESC;
END;
GO
  

CREATE OR ALTER PROCEDURE sp_TopBanDuocDat
    @SoLuong INT = 5
AS
BEGIN
    SET NOCOUNT ON;

    SELECT TOP (@SoLuong)
        B.MaBan,
        B.TenBan,
        COUNT(DB.MaDatBan) AS SoLanDat
    FROM Ban B
    LEFT JOIN DatBan DB
        ON B.MaBan = DB.MaBan
       AND DB.TrangThai <> N'Đã hủy'
    GROUP BY
        B.MaBan,
        B.TenBan
    ORDER BY
        SoLanDat DESC;
END;
GO


PRINT N'Đã tạo toàn bộ Stored Procedure thành công!';
GO
