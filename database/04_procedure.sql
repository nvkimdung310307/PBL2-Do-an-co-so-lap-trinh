
USE RestaurantManagement;
GO

/* =====================================================
   1. DANG NHAP
   ===================================================== */
IF OBJECT_ID('sp_DangNhap', 'P') IS NOT NULL
    DROP PROCEDURE sp_DangNhap;
GO

CREATE PROCEDURE sp_DangNhap
    @TenDangNhap VARCHAR(50),
    @MatKhau VARCHAR(255)
AS
BEGIN
    SET NOCOUNT ON;
    THROW 51001, 'Use DangNhapService for PBKDF2 password verification.', 1;
END;
GO


/* =====================================================
   2. DANH SACH BAN
   ===================================================== */
IF OBJECT_ID('sp_GetDanhSachBan', 'P') IS NOT NULL
    DROP PROCEDURE sp_GetDanhSachBan;
GO

CREATE PROCEDURE sp_GetDanhSachBan
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
    INNER JOIN KhuVuc KV ON B.MaKhuVuc = KV.MaKhuVuc
    ORDER BY B.MaBan;
END;
GO


/* =====================================================
   3. TIM BAN PHU HOP
   ===================================================== */
IF OBJECT_ID('sp_TimBanPhuHop', 'P') IS NOT NULL
    DROP PROCEDURE sp_TimBanPhuHop;
GO

CREATE PROCEDURE sp_TimBanPhuHop
    @SoNguoi INT
AS
BEGIN
    SET NOCOUNT ON;

    IF @SoNguoi IS NULL OR @SoNguoi <= 0
    BEGIN
        RAISERROR(N'Số người phải lớn hơn 0!', 16, 1);
        RETURN;
    END;

    SELECT
        B.MaBan,
        B.TenBan,
        B.SoCho,
        B.TrangThai,
        KV.TenKhuVuc
    FROM Ban B
    INNER JOIN KhuVuc KV ON B.MaKhuVuc = KV.MaKhuVuc
    WHERE B.SoCho >= @SoNguoi
      AND B.TrangThai = N'Trống'
    ORDER BY B.SoCho ASC, B.MaBan;
END;
GO


/* =====================================================
   4. TIM KHACH HANG
   ===================================================== */
IF OBJECT_ID('sp_TimKhachHang', 'P') IS NOT NULL
    DROP PROCEDURE sp_TimKhachHang;
GO

CREATE PROCEDURE sp_TimKhachHang
    @TuKhoa NVARCHAR(100)
AS
BEGIN
    SET NOCOUNT ON;

    SET @TuKhoa = LTRIM(RTRIM(ISNULL(@TuKhoa, N'')));

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


/* =====================================================
   5. DANH SACH DAT BAN
   ===================================================== */
IF OBJECT_ID('sp_GetDanhSachDatBan', 'P') IS NOT NULL
    DROP PROCEDURE sp_GetDanhSachDatBan;
GO

CREATE PROCEDURE sp_GetDanhSachDatBan
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
    INNER JOIN KhachHang KH ON DB.MaKH = KH.MaKH
    INNER JOIN Ban B ON DB.MaBan = B.MaBan
    LEFT JOIN NhanVien NV ON DB.MaNV = NV.MaNV
    ORDER BY DB.NgayDat DESC, DB.GioDat DESC;
END;
GO


/* =====================================================
   6. KIEM TRA DAT BAN
   ===================================================== */
IF OBJECT_ID('sp_KiemTraDatBan', 'P') IS NOT NULL
    DROP PROCEDURE sp_KiemTraDatBan;
GO

CREATE PROCEDURE sp_KiemTraDatBan
    @MaBan VARCHAR(10),
    @NgayDat DATE,
    @GioDat TIME
AS
BEGIN
    SET NOCOUNT ON;

    IF NOT EXISTS (SELECT 1 FROM Ban WHERE MaBan = @MaBan)
    BEGIN
        RAISERROR(N'Bàn không tồn tại!', 16, 1);
        RETURN;
    END;

    IF @NgayDat IS NULL OR @GioDat IS NULL
    BEGIN
        RAISERROR(N'Ngày đặt và giờ đặt không hợp lệ!', 16, 1);
        RETURN;
    END;

    DECLARE @BatDauMoi DATETIME2(0) =
        DATEADD(SECOND, DATEDIFF(SECOND, CAST('00:00:00' AS TIME), @GioDat), CAST(@NgayDat AS DATETIME2(0)));
    DECLARE @KetThucMoi DATETIME2(0) = DATEADD(MINUTE, 120, @BatDauMoi);

    IF EXISTS (
        SELECT 1
        FROM DatBan AS DB
        WHERE DB.MaBan = @MaBan
          AND DB.TrangThai IN (N'Đã đặt', N'Đã nhận bàn')
          AND DATEADD(SECOND, DATEDIFF(SECOND, CAST('00:00:00' AS TIME), DB.GioDat), CAST(DB.NgayDat AS DATETIME2(0))) < @KetThucMoi
          AND @BatDauMoi < DATEADD(MINUTE, 120,
              DATEADD(SECOND, DATEDIFF(SECOND, CAST('00:00:00' AS TIME), DB.GioDat), CAST(DB.NgayDat AS DATETIME2(0))))
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
    END;
END;
GO


/* =====================================================
   7. TAO DAT BAN
   ===================================================== */
IF OBJECT_ID('sp_TaoDatBan', 'P') IS NOT NULL
    DROP PROCEDURE sp_TaoDatBan;
GO

CREATE PROCEDURE sp_TaoDatBan
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
    SET XACT_ABORT ON;

    IF @MaDatBan IS NULL OR @MaKH IS NULL OR @MaBan IS NULL
    BEGIN
        RAISERROR(N'Mã đặt bàn, mã khách hàng và mã bàn không được để trống!', 16, 1);
        RETURN;
    END;

    IF @NgayDat IS NULL OR @GioDat IS NULL
    BEGIN
        RAISERROR(N'Ngày đặt và giờ đặt không hợp lệ!', 16, 1);
        RETURN;
    END;

    IF @SoNguoi IS NULL OR @SoNguoi <= 0
    BEGIN
        RAISERROR(N'Số người phải lớn hơn 0!', 16, 1);
        RETURN;
    END;

    BEGIN TRY
        BEGIN TRANSACTION;

        IF EXISTS (
            SELECT 1 FROM DatBan WHERE MaDatBan = @MaDatBan
        )
        BEGIN
            RAISERROR(N'Mã đặt bàn đã tồn tại!', 16, 1);
        END
        ELSE IF NOT EXISTS (
            SELECT 1 FROM KhachHang WHERE MaKH = @MaKH
        )
        BEGIN
            RAISERROR(N'Khách hàng không tồn tại!', 16, 1);
        END
        ELSE IF NOT EXISTS (
            SELECT 1 FROM Ban WHERE MaBan = @MaBan
        )
        BEGIN
            RAISERROR(N'Bàn không tồn tại!', 16, 1);
        END
        ELSE IF EXISTS (
            SELECT 1
            FROM Ban
            WHERE MaBan = @MaBan
              AND SoCho < @SoNguoi
        )
        BEGIN
            RAISERROR(N'Bàn không đủ số chỗ!', 16, 1);
        END
        ELSE IF EXISTS (
            SELECT 1
            FROM Ban
            WHERE MaBan = @MaBan
              AND TrangThai IN (N'Bảo trì', N'Đang sử dụng')
        )
        BEGIN
            RAISERROR(N'Bàn hiện không thể nhận đặt bàn!', 16, 1);
        END
        ELSE IF EXISTS (
            SELECT 1
            FROM DatBan WITH (UPDLOCK, HOLDLOCK)
            WHERE MaBan = @MaBan
              AND TrangThai IN (N'Đã đặt', N'Đã nhận bàn')
              AND DATEADD(SECOND, DATEDIFF(SECOND, CAST('00:00:00' AS TIME), GioDat), CAST(NgayDat AS DATETIME2(0)))
                    < DATEADD(MINUTE, 120, DATEADD(SECOND, DATEDIFF(SECOND, CAST('00:00:00' AS TIME), @GioDat), CAST(@NgayDat AS DATETIME2(0))))
              AND DATEADD(SECOND, DATEDIFF(SECOND, CAST('00:00:00' AS TIME), @GioDat), CAST(@NgayDat AS DATETIME2(0)))
                    < DATEADD(MINUTE, 120, DATEADD(SECOND, DATEDIFF(SECOND, CAST('00:00:00' AS TIME), GioDat), CAST(NgayDat AS DATETIME2(0))))
        )
        BEGIN
            RAISERROR(N'Bàn đã được đặt trùng khung giờ phục vụ 2 giờ!', 16, 1);
        END
        ELSE
        BEGIN
            INSERT INTO DatBan (
                MaDatBan, MaKH, MaBan, MaNV,
                NgayDat, GioDat, SoNguoi, TrangThai, GhiChu
            )
            VALUES (
                @MaDatBan, @MaKH, @MaBan, @MaNV,
                @NgayDat, @GioDat, @SoNguoi, N'Đã đặt', @GhiChu
            );

            COMMIT TRANSACTION;

            SELECT
                1 AS ThanhCong,
                N'Đặt bàn thành công!' AS ThongBao;

            RETURN;
        END;

        IF @@TRANCOUNT > 0
            ROLLBACK TRANSACTION;

    END TRY
    BEGIN CATCH
        IF @@TRANCOUNT > 0
            ROLLBACK TRANSACTION;

        DECLARE @Loi NVARCHAR(4000);
        SET @Loi = ERROR_MESSAGE();

        RAISERROR(N'%s', 16, 1, @Loi);
        RETURN;
    END CATCH;
END;
GO


/* =====================================================
   8. HUY DAT BAN
   ===================================================== */
IF OBJECT_ID('sp_HuyDatBan', 'P') IS NOT NULL
    DROP PROCEDURE sp_HuyDatBan;
GO

CREATE PROCEDURE sp_HuyDatBan
    @MaDatBan VARCHAR(10)
AS
BEGIN
    SET NOCOUNT ON;
    SET XACT_ABORT ON;

    BEGIN TRY
        BEGIN TRANSACTION;

        DECLARE @MaBan VARCHAR(10);
        DECLARE @TrangThai NVARCHAR(30);

        SELECT
            @MaBan = MaBan,
            @TrangThai = TrangThai
        FROM DatBan
        WHERE MaDatBan = @MaDatBan;

        IF @MaBan IS NULL
        BEGIN
            RAISERROR(N'Không tìm thấy mã đặt bàn!', 16, 1);
        END
        ELSE IF @TrangThai <> N'Đã đặt'
        BEGIN
            RAISERROR(N'Chỉ được hủy lịch đang ở trạng thái Đã đặt!', 16, 1);
        END
        ELSE
        BEGIN
            UPDATE DatBan
            SET TrangThai = N'Đã hủy'
            WHERE MaDatBan = @MaDatBan;

            IF NOT EXISTS (
                SELECT 1
                FROM DatBan
                WHERE MaBan = @MaBan
                  AND TrangThai IN (N'Đã đặt', N'Đã nhận bàn')
            )
            BEGIN
                UPDATE Ban
                SET TrangThai = N'Trống'
                WHERE MaBan = @MaBan
                  AND TrangThai = N'Đã đặt';
            END;

            COMMIT TRANSACTION;

            SELECT
                1 AS ThanhCong,
                N'Hủy đặt bàn thành công!' AS ThongBao;

            RETURN;
        END;

        IF @@TRANCOUNT > 0
            ROLLBACK TRANSACTION;

    END TRY
    BEGIN CATCH
        IF @@TRANCOUNT > 0
            ROLLBACK TRANSACTION;

        DECLARE @Loi NVARCHAR(4000);
        SET @Loi = ERROR_MESSAGE();

        RAISERROR(N'%s', 16, 1, @Loi);
        RETURN;
    END CATCH;
END;
GO


/* =====================================================
   9. TINH TONG HOA DON
   ===================================================== */
IF OBJECT_ID('sp_TinhTongHoaDon', 'P') IS NOT NULL
    DROP PROCEDURE sp_TinhTongHoaDon;
GO

CREATE PROCEDURE sp_TinhTongHoaDon
    @MaHD VARCHAR(10)
AS
BEGIN
    SET NOCOUNT ON;

    IF NOT EXISTS (SELECT 1 FROM HoaDon WHERE MaHD = @MaHD)
    BEGIN
        RAISERROR(N'Không tìm thấy hóa đơn!', 16, 1);
        RETURN;
    END;

    DECLARE @TongTien DECIMAL(18,2);
    DECLARE @TienGiam DECIMAL(18,2);

    SELECT @TongTien = ISNULL(SUM(SoLuong * DonGia), 0)
    FROM ChiTietHoaDon
    WHERE MaHD = @MaHD;

    SELECT @TienGiam = ISNULL(TienGiam, 0)
    FROM HoaDon
    WHERE MaHD = @MaHD;

    UPDATE HoaDon
    SET
        TongTien = @TongTien,
        ThanhTien =
            CASE
                WHEN @TongTien - @TienGiam < 0 THEN 0
                ELSE @TongTien - @TienGiam
            END
    WHERE MaHD = @MaHD
      AND TrangThai = N'Chưa thanh toán';

    SELECT MaHD, TongTien, TienGiam, ThanhTien, TrangThai
    FROM HoaDon
    WHERE MaHD = @MaHD;
END;
GO


/* =====================================================
   10. THANH TOAN HOA DON
   ===================================================== */
IF OBJECT_ID('sp_ThanhToanHoaDon', 'P') IS NOT NULL
    DROP PROCEDURE sp_ThanhToanHoaDon;
GO

CREATE PROCEDURE sp_ThanhToanHoaDon
    @MaHD VARCHAR(10),
    @PhuongThucThanhToan NVARCHAR(30)
AS
BEGIN
    SET NOCOUNT ON;
    SET XACT_ABORT ON;

    BEGIN TRY
        BEGIN TRANSACTION;

        DECLARE @MaDatBan VARCHAR(10);
        DECLARE @MaBan VARCHAR(10);
        DECLARE @TrangThaiHD NVARCHAR(30);

        SELECT
            @MaDatBan = MaDatBan,
            @TrangThaiHD = TrangThai
        FROM HoaDon
        WHERE MaHD = @MaHD;

        IF @MaDatBan IS NULL
        BEGIN
            RAISERROR(N'Không tìm thấy hóa đơn hoặc mã đặt bàn bị NULL!', 16, 1);
        END
        ELSE IF @TrangThaiHD <> N'Chưa thanh toán'
        BEGIN
            RAISERROR(N'Hóa đơn không ở trạng thái chờ thanh toán!', 16, 1);
        END
        ELSE IF @PhuongThucThanhToan IS NULL
             OR LTRIM(RTRIM(@PhuongThucThanhToan)) = N''
        BEGIN
            RAISERROR(N'Vui lòng chọn phương thức thanh toán!', 16, 1);
        END
        ELSE
        BEGIN
            SELECT @MaBan = MaBan
            FROM DatBan
            WHERE MaDatBan = @MaDatBan;

            IF @MaBan IS NULL
            BEGIN
                RAISERROR(N'Không tìm thấy lịch đặt bàn của hóa đơn!', 16, 1);
            END
            ELSE
            BEGIN
                EXEC sp_TinhTongHoaDon @MaHD;

                UPDATE HoaDon
                SET
                    TrangThai = N'Đã thanh toán',
                    PhuongThucThanhToan = @PhuongThucThanhToan
                WHERE MaHD = @MaHD
                  AND TrangThai = N'Chưa thanh toán';

                IF @@ROWCOUNT <> 1
                BEGIN
                    RAISERROR(N'Không thể cập nhật trạng thái hóa đơn!', 16, 1);
                END
                ELSE
                BEGIN
                    UPDATE DatBan
                    SET TrangThai = N'Hoàn thành'
                    WHERE MaDatBan = @MaDatBan
                      AND TrangThai IN (N'Đã đặt', N'Đã nhận bàn');

                    IF NOT EXISTS (
                        SELECT 1
                        FROM DatBan
                        WHERE MaBan = @MaBan
                          AND TrangThai IN (N'Đã đặt', N'Đã nhận bàn')
                    )
                    BEGIN
                        UPDATE Ban
                        SET TrangThai = N'Trống'
                        WHERE MaBan = @MaBan
                          AND TrangThai IN (N'Đã đặt', N'Đang sử dụng');
                    END;

                    COMMIT TRANSACTION;

                    SELECT
                        1 AS ThanhCong,
                        N'Thanh toán thành công!' AS ThongBao;

                    RETURN;
                END;
            END;
        END;

        IF @@TRANCOUNT > 0
            ROLLBACK TRANSACTION;

    END TRY
    BEGIN CATCH
        IF @@TRANCOUNT > 0
            ROLLBACK TRANSACTION;

        DECLARE @Loi NVARCHAR(4000);
        SET @Loi = ERROR_MESSAGE();

        RAISERROR(N'%s', 16, 1, @Loi);
        RETURN;
    END CATCH;
END;
GO


/* =====================================================
   11. DOANH THU THEO NGAY
   ===================================================== */
IF OBJECT_ID('sp_DoanhThuTheoNgay', 'P') IS NOT NULL
    DROP PROCEDURE sp_DoanhThuTheoNgay;
GO

CREATE PROCEDURE sp_DoanhThuTheoNgay
    @Ngay DATE
AS
BEGIN
    SET NOCOUNT ON;

    IF @Ngay IS NULL
    BEGIN
        RAISERROR(N'Ngày thống kê không được để trống!', 16, 1);
        RETURN;
    END;

    SELECT
        @Ngay AS Ngay,
        COUNT(MaHD) AS SoHoaDon,
        ISNULL(SUM(ThanhTien), 0) AS DoanhThu
    FROM HoaDon
    WHERE NgayLap >= @Ngay
      AND NgayLap < DATEADD(DAY, 1, @Ngay)
      AND TrangThai = N'Đã thanh toán';
END;
GO


/* =====================================================
   12. DOANH THU THEO THANG
   ===================================================== */
IF OBJECT_ID('sp_DoanhThuTheoThang', 'P') IS NOT NULL
    DROP PROCEDURE sp_DoanhThuTheoThang;
GO

CREATE PROCEDURE sp_DoanhThuTheoThang
    @Nam INT,
    @Thang INT
AS
BEGIN
    SET NOCOUNT ON;

    IF @Nam < 1900 OR @Nam > 9999
    BEGIN
        RAISERROR(N'Năm thống kê không hợp lệ!', 16, 1);
        RETURN;
    END;

    IF @Thang < 1 OR @Thang > 12
    BEGIN
        RAISERROR(N'Tháng phải từ 1 đến 12!', 16, 1);
        RETURN;
    END;

    DECLARE @TuNgay DATE;
    DECLARE @DenNgay DATE;

    SET @TuNgay = CAST(
        CAST(@Nam AS VARCHAR(4)) + '-' +
        RIGHT('0' + CAST(@Thang AS VARCHAR(2)), 2) + '-01'
        AS DATE
    );

    SET @DenNgay = DATEADD(MONTH, 1, @TuNgay);

    SELECT
        @Nam AS Nam,
        @Thang AS Thang,
        COUNT(MaHD) AS SoHoaDon,
        ISNULL(SUM(ThanhTien), 0) AS DoanhThu
    FROM HoaDon
    WHERE NgayLap >= @TuNgay
      AND NgayLap < @DenNgay
      AND TrangThai = N'Đã thanh toán';
END;
GO


/* =====================================================
   13. TOP MON AN BAN CHAY
   ===================================================== */
IF OBJECT_ID('sp_TopMonAnBanChay', 'P') IS NOT NULL
    DROP PROCEDURE sp_TopMonAnBanChay;
GO

CREATE PROCEDURE sp_TopMonAnBanChay
    @SoLuong INT = 5
AS
BEGIN
    SET NOCOUNT ON;

    IF @SoLuong IS NULL OR @SoLuong <= 0
    BEGIN
        RAISERROR(N'Số lượng phải lớn hơn 0!', 16, 1);
        RETURN;
    END;

    SELECT TOP (@SoLuong)
        M.MaMon,
        M.TenMon,
        SUM(CT.SoLuong) AS SoLuongBan,
        SUM(CT.SoLuong * CT.DonGia) AS DoanhThu
    FROM ChiTietHoaDon CT
    INNER JOIN MonAn M ON CT.MaMon = M.MaMon
    INNER JOIN HoaDon HD ON CT.MaHD = HD.MaHD
    WHERE HD.TrangThai = N'Đã thanh toán'
    GROUP BY M.MaMon, M.TenMon
    ORDER BY SoLuongBan DESC, M.TenMon ASC;
END;
GO


/* =====================================================
   14. TOP BAN DUOC DAT
   ===================================================== */
IF OBJECT_ID('sp_TopBanDuocDat', 'P') IS NOT NULL
    DROP PROCEDURE sp_TopBanDuocDat;
GO

CREATE PROCEDURE sp_TopBanDuocDat
    @SoLuong INT = 5
AS
BEGIN
    SET NOCOUNT ON;

    IF @SoLuong IS NULL OR @SoLuong <= 0
    BEGIN
        RAISERROR(N'Số lượng phải lớn hơn 0!', 16, 1);
        RETURN;
    END;

    SELECT TOP (@SoLuong)
        B.MaBan,
        B.TenBan,
        COUNT(DB.MaDatBan) AS SoLanDat
    FROM Ban B
    LEFT JOIN DatBan DB
        ON B.MaBan = DB.MaBan
       AND DB.TrangThai <> N'Đã hủy'
    GROUP BY B.MaBan, B.TenBan
    ORDER BY SoLanDat DESC, B.MaBan ASC;
END;
GO

PRINT N'Đã tạo xong 14 Stored Procedure!';
GO

USE RestaurantManagement;
GO

/* Da loai bo khoi thu tuc bi lap o cuoi file. */
