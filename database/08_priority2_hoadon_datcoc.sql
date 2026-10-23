/*
  Uu tien 2: hoa don, chi tiet hoa don, thanh toan va tien coc.
  Chay tren database RestaurantManagement sau khi backup.
  Script chi ALTER procedure, khong xoa bang hay du lieu.
*/
USE RestaurantManagement;
GO


CREATE OR ALTER PROCEDURE dbo.sp_ThemDatCoc
    @MaDatCoc VARCHAR(10), @MaDatBan VARCHAR(10),
    @TienCoc DECIMAL(18,2), @NgayDatCoc DATE = NULL
AS
BEGIN
    SET NOCOUNT ON;
    SET XACT_ABORT ON;
    BEGIN TRY
        BEGIN TRANSACTION;
        IF NULLIF(LTRIM(RTRIM(@MaDatCoc)),'') IS NULL THROW 50221,N'Mã đặt cọc không được trống.',1;
        IF @TienCoc IS NULL OR @TienCoc<=0 THROW 50222,N'Tiền cọc phải lớn hơn 0.',1;
        IF NOT EXISTS(SELECT 1 FROM dbo.DatBan WITH (UPDLOCK,HOLDLOCK) WHERE MaDatBan=@MaDatBan) THROW 50223,N'Mã đặt bàn không tồn tại.',1;
        IF EXISTS(SELECT 1 FROM dbo.DatCoc WITH (UPDLOCK,HOLDLOCK) WHERE MaDatCoc=@MaDatCoc) THROW 50224,N'Mã đặt cọc đã tồn tại.',1;
        IF EXISTS(SELECT 1 FROM dbo.DatCoc WITH (UPDLOCK,HOLDLOCK) WHERE MaDatBan=@MaDatBan AND TrangThai='DaCoc') THROW 50225,N'Đặt bàn này đã có tiền cọc chưa sử dụng.',1;
        INSERT dbo.DatCoc(MaDatCoc,MaDatBan,TienCoc,NgayDatCoc,TrangThai)
        VALUES(@MaDatCoc,@MaDatBan,@TienCoc,COALESCE(@NgayDatCoc,CONVERT(date,GETDATE())),'DaCoc');

        EXEC dbo.sp_LamMoiTienHoaDonTheoDatBan @MaDatBan=@MaDatBan;
        COMMIT TRANSACTION;
    END TRY
    BEGIN CATCH
        IF @@TRANCOUNT>0 ROLLBACK TRANSACTION;
        THROW;
    END CATCH
END;
GO

CREATE OR ALTER PROCEDURE dbo.sp_TinhTongHoaDon
    @MaHD VARCHAR(10),
    @TienGiamMoi DECIMAL(18,2) = NULL
AS
BEGIN
    SET NOCOUNT ON;
    SET XACT_ABORT ON;
    BEGIN TRY
        BEGIN TRANSACTION;
        DECLARE @MaDatBan VARCHAR(10), @TrangThai NVARCHAR(30);
        SELECT @MaDatBan=MaDatBan, @TrangThai=TrangThai
        FROM dbo.HoaDon WITH (UPDLOCK,HOLDLOCK) WHERE MaHD=@MaHD;
        IF @MaDatBan IS NULL THROW 50201, N'Không tìm thấy hóa đơn.', 1;
        IF @TrangThai<>N'Chưa thanh toán' THROW 50202, N'Chỉ được tính lại hóa đơn chưa thanh toán.', 1;

        IF @TienGiamMoi IS NOT NULL
        BEGIN
            IF @TienGiamMoi<0 THROW 50203, N'Tiền giảm không hợp lệ.', 1;
            UPDATE dbo.HoaDon SET TienGiam=@TienGiamMoi
            WHERE MaHD=@MaHD AND TrangThai=N'Chưa thanh toán';
        END

        EXEC dbo.sp_LamMoiTienHoaDonTheoDatBan @MaDatBan=@MaDatBan;
        COMMIT TRANSACTION;
        SELECT MaHD,TongTien,TienGiam,ThanhTien,TrangThai
        FROM dbo.HoaDon WHERE MaHD=@MaHD;
    END TRY
    BEGIN CATCH
        IF @@TRANCOUNT>0 ROLLBACK TRANSACTION;
        THROW;
    END CATCH
END;
GO

CREATE OR ALTER PROCEDURE dbo.sp_ThanhToanHoaDon
    @MaHD VARCHAR(10),
    @PhuongThucThanhToan NVARCHAR(30)
AS
BEGIN
    SET NOCOUNT ON;
    SET XACT_ABORT ON;
    BEGIN TRY
        BEGIN TRANSACTION;
        DECLARE @MaDatBan VARCHAR(10), @MaBan VARCHAR(10), @TrangThai NVARCHAR(30);
        SELECT @MaDatBan=h.MaDatBan,@TrangThai=h.TrangThai
        FROM dbo.HoaDon h WITH (UPDLOCK,HOLDLOCK) WHERE h.MaHD=@MaHD;
        IF @MaDatBan IS NULL THROW 50211, N'Không tìm thấy hóa đơn.', 1;
        IF @TrangThai<>N'Chưa thanh toán' THROW 50212, N'Hóa đơn không ở trạng thái chờ thanh toán.', 1;
        IF @PhuongThucThanhToan NOT IN (N'Tiền mặt',N'Chuyển khoản',N'Ví điện tử',N'QR') THROW 50213, N'Phương thức thanh toán không hợp lệ.', 1;
        IF NOT EXISTS(SELECT 1 FROM dbo.ChiTietHoaDon WHERE MaHD=@MaHD) THROW 50214, N'Hóa đơn chưa có món ăn.', 1;

        DECLARE @TrangThaiDatBan NVARCHAR(30);
        SELECT @MaBan=MaBan, @TrangThaiDatBan=TrangThai
        FROM dbo.DatBan WITH (UPDLOCK,HOLDLOCK) WHERE MaDatBan=@MaDatBan;
        IF @MaBan IS NULL THROW 50215, N'Không tìm thấy lịch đặt bàn của hóa đơn.', 1;
        IF @TrangThaiDatBan <> N'Đã nhận bàn'
            THROW 50218, N'Chỉ được thanh toán sau khi xác nhận khách đã đến nhận bàn.', 1;

        /* Tinh lai tien truoc khi thanh toan; tien coc chi duoc phan bo cho mot hoa don. */
        EXEC dbo.sp_LamMoiTienHoaDonTheoDatBan @MaDatBan=@MaDatBan;

        DECLARE @TongTien DECIMAL(18,2), @TienGiam DECIMAL(18,2), @ThanhTien DECIMAL(18,2);
        SELECT @TongTien=TongTien, @TienGiam=TienGiam, @ThanhTien=ThanhTien
        FROM dbo.HoaDon WITH (UPDLOCK,HOLDLOCK) WHERE MaHD=@MaHD;
        IF @TienGiam IS NULL OR @TienGiam<0 OR @TienGiam>@TongTien
            THROW 50216, N'Tiền giảm không hợp lệ so với tổng tiền.', 1;
        IF @ThanhTien IS NULL OR @ThanhTien<0
            THROW 50219, N'Số tiền còn phải trả không hợp lệ.', 1;

        UPDATE dbo.HoaDon
        SET TrangThai=N'Đã thanh toán',PhuongThucThanhToan=@PhuongThucThanhToan
        WHERE MaHD=@MaHD AND TrangThai=N'Chưa thanh toán';
        IF @@ROWCOUNT<>1 THROW 50217, N'Không thể cập nhật trạng thái hóa đơn.', 1;

        UPDATE dbo.DatBan SET TrangThai=N'Hoàn thành'
        WHERE MaDatBan=@MaDatBan AND TrangThai=N'Đã nhận bàn';

        /* Chi danh dau DaTru neu tien coc da duoc tru vao chinh hoa don nay. */
        IF @ThanhTien < @TongTien-@TienGiam
            UPDATE dbo.DatCoc SET TrangThai='DaTru'
            WHERE MaDatBan=@MaDatBan AND TrangThai='DaCoc';

        IF EXISTS(SELECT 1 FROM dbo.DatBan WHERE MaBan=@MaBan AND TrangThai=N'Đã nhận bàn')
            UPDATE dbo.Ban SET TrangThai=N'Đang sử dụng' WHERE MaBan=@MaBan AND TrangThai<>N'Bảo trì';
        ELSE IF EXISTS(SELECT 1 FROM dbo.DatBan WHERE MaBan=@MaBan AND TrangThai=N'Đã đặt')
            UPDATE dbo.Ban SET TrangThai=N'Đã đặt' WHERE MaBan=@MaBan AND TrangThai<>N'Bảo trì';
        ELSE
            UPDATE dbo.Ban SET TrangThai=N'Trống' WHERE MaBan=@MaBan AND TrangThai<>N'Bảo trì';
        COMMIT TRANSACTION;
        SELECT CAST(1 AS BIT) AS ThanhCong,N'Thanh toán thành công.' AS ThongBao;
    END TRY
    BEGIN CATCH
        IF @@TRANCOUNT>0 ROLLBACK TRANSACTION;
        THROW;
    END CATCH
END;
GO
PRINT N'Da cap nhat thu tuc hoa don va thanh toan.';
GO
