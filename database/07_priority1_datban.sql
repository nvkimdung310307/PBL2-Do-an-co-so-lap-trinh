/*
    SUA UU TIEN 1: DAT BAN VA DONG BO TRANG THAI BAN
    Chay sau 02_create_tables.sql va 04_procedure.sql.
    Script chi tao/cap nhat cac procedure dat ban, khong xoa du lieu.
*/
USE RestaurantManagement;
GO
/* Dong bo CHECK voi trang thai khach khong den; an toan khi chay lai. */
IF OBJECT_ID(N'dbo.CK_DatBan_TrangThai', N'C') IS NOT NULL
    ALTER TABLE dbo.DatBan DROP CONSTRAINT CK_DatBan_TrangThai;
GO
ALTER TABLE dbo.DatBan WITH CHECK ADD CONSTRAINT CK_DatBan_TrangThai
CHECK (TrangThai IN (N'Đã đặt', N'Đã nhận bàn', N'Đã hủy', N'Không đến', N'Hoàn thành'));
GO

CREATE OR ALTER PROCEDURE dbo.sp_DatBan_Tao
    @MaDatBan VARCHAR(10),
    @MaKH VARCHAR(10),
    @MaBan VARCHAR(10),
    @MaNV VARCHAR(10) = NULL,
    @NgayDat DATE,
    @GioDat TIME(0),
    @SoNguoi INT,
    @GhiChu NVARCHAR(255) = NULL
AS
BEGIN
    SET NOCOUNT ON;
    SET XACT_ABORT ON;

    IF NULLIF(LTRIM(RTRIM(@MaDatBan)), '') IS NULL
       OR NULLIF(LTRIM(RTRIM(@MaKH)), '') IS NULL
       OR NULLIF(LTRIM(RTRIM(@MaBan)), '') IS NULL
        THROW 50001, N'Mã đặt bàn, khách hàng và bàn không được để trống.', 1;
    IF @NgayDat IS NULL OR @GioDat IS NULL
        THROW 50002, N'Ngày đặt hoặc giờ đặt không hợp lệ.', 1;
    IF @SoNguoi IS NULL OR @SoNguoi <= 0
        THROW 50003, N'Số người phải lớn hơn 0.', 1;
    IF DATEADD(SECOND, DATEDIFF(SECOND, CAST('00:00:00' AS TIME), @GioDat), CAST(@NgayDat AS DATETIME2(0))) < DATEADD(MINUTE, -1, SYSDATETIME())
        THROW 50004, N'Không thể đặt bàn ở thời điểm đã qua.', 1;

    DECLARE @StartAt DATETIME2(0) = DATEADD(SECOND, DATEDIFF(SECOND, CAST('00:00:00' AS TIME), @GioDat), CAST(@NgayDat AS DATETIME2(0)));
    BEGIN TRY
        BEGIN TRANSACTION;

        IF EXISTS (SELECT 1 FROM dbo.DatBan WITH (UPDLOCK, HOLDLOCK) WHERE MaDatBan = @MaDatBan)
            THROW 50005, N'Mã đặt bàn đã tồn tại.', 1;

        DECLARE @SoCho INT, @TrangThaiBan NVARCHAR(30);
        SELECT @SoCho = SoCho, @TrangThaiBan = TrangThai
        FROM dbo.Ban WITH (UPDLOCK, HOLDLOCK)
        WHERE MaBan = @MaBan;

        IF @SoCho IS NULL THROW 50006, N'Không tìm thấy bàn.', 1;
        IF @SoCho < @SoNguoi THROW 50007, N'Bàn không đủ số chỗ.', 1;
        IF @TrangThaiBan IN (N'Bảo trì', N'Đang sử dụng')
            THROW 50008, N'Bàn đang bảo trì hoặc được sử dụng.', 1;

        IF EXISTS (
            SELECT 1
            FROM dbo.DatBan WITH (UPDLOCK, HOLDLOCK)
            CROSS APPLY (SELECT DATEADD(SECOND, DATEDIFF(SECOND, CAST('00:00:00' AS TIME), GioDat), CAST(NgayDat AS DATETIME2(0))) AS StartAt) AS B
            WHERE MaBan = @MaBan
              AND TrangThai IN (N'Đã đặt', N'Đã nhận bàn')
              AND B.StartAt < DATEADD(MINUTE, 120, @StartAt)
              AND @StartAt < DATEADD(MINUTE, 120, B.StartAt)
        ) THROW 50009, N'Bàn đã có lịch đặt trùng thời gian (mỗi lượt 120 phút).', 1;

        INSERT dbo.DatBan (MaDatBan, MaKH, MaBan, MaNV, NgayDat, GioDat, SoNguoi, TrangThai, GhiChu)
        VALUES (@MaDatBan, @MaKH, @MaBan, @MaNV, @NgayDat, @GioDat, @SoNguoi, N'Đã đặt', @GhiChu);

        UPDATE dbo.Ban SET TrangThai = N'Đã đặt'
        WHERE MaBan = @MaBan AND TrangThai = N'Trống';

        COMMIT TRANSACTION;
        SELECT CAST(1 AS BIT) AS ThanhCong, N'Đặt bàn thành công.' AS ThongBao;
    END TRY
    BEGIN CATCH
        IF XACT_STATE() <> 0 ROLLBACK TRANSACTION;
        THROW;
    END CATCH
END;
GO

CREATE OR ALTER PROCEDURE dbo.sp_DatBan_Huy
    @MaDatBan VARCHAR(10)
AS
BEGIN
    SET NOCOUNT ON;
    SET XACT_ABORT ON;
    BEGIN TRY
        BEGIN TRANSACTION;
        DECLARE @MaBan VARCHAR(10);
        SELECT @MaBan = MaBan FROM dbo.DatBan WITH (UPDLOCK, HOLDLOCK)
        WHERE MaDatBan = @MaDatBan AND TrangThai = N'Đã đặt';
        IF @MaBan IS NULL THROW 50011, N'Không tìm thấy lịch đặt đang ở trạng thái Đã đặt.', 1;

        UPDATE dbo.DatBan SET TrangThai = N'Đã hủy' WHERE MaDatBan = @MaDatBan;
        IF EXISTS (SELECT 1 FROM dbo.DatBan WHERE MaBan=@MaBan AND TrangThai=N'Đã nhận bàn')
            UPDATE dbo.Ban SET TrangThai=N'Đang sử dụng' WHERE MaBan=@MaBan AND TrangThai<>N'Bảo trì';
        ELSE IF EXISTS (SELECT 1 FROM dbo.DatBan WHERE MaBan=@MaBan AND TrangThai=N'Đã đặt')
            UPDATE dbo.Ban SET TrangThai=N'Đã đặt' WHERE MaBan=@MaBan AND TrangThai<>N'Bảo trì';
        ELSE
            UPDATE dbo.Ban SET TrangThai=N'Trống' WHERE MaBan=@MaBan AND TrangThai<>N'Bảo trì';
        COMMIT TRANSACTION;
    END TRY
    BEGIN CATCH
        IF XACT_STATE() <> 0 ROLLBACK TRANSACTION;
        THROW;
    END CATCH
END;
GO

CREATE OR ALTER PROCEDURE dbo.sp_DatBan_XacNhanDen
    @MaDatBan VARCHAR(10)
AS
BEGIN
    SET NOCOUNT ON;
    SET XACT_ABORT ON;
    BEGIN TRY
        BEGIN TRANSACTION;
        DECLARE @MaBan VARCHAR(10);
        SELECT @MaBan = MaBan FROM dbo.DatBan WITH (UPDLOCK, HOLDLOCK)
        WHERE MaDatBan = @MaDatBan AND TrangThai = N'Đã đặt';
        IF @MaBan IS NULL THROW 50012, N'Chỉ xác nhận khách đến với lịch đang ở trạng thái Đã đặt.', 1;

        UPDATE dbo.DatBan SET TrangThai = N'Đã nhận bàn' WHERE MaDatBan = @MaDatBan;
        UPDATE dbo.Ban SET TrangThai = N'Đang sử dụng' WHERE MaBan = @MaBan AND TrangThai <> N'Bảo trì';
        COMMIT TRANSACTION;
    END TRY
    BEGIN CATCH
        IF XACT_STATE() <> 0 ROLLBACK TRANSACTION;
        THROW;
    END CATCH
END;
GO

CREATE OR ALTER PROCEDURE dbo.sp_DatBan_KhongDen
    @MaDatBan VARCHAR(10)
AS
BEGIN
    SET NOCOUNT ON;
    SET XACT_ABORT ON;
    BEGIN TRY
        BEGIN TRANSACTION;
        DECLARE @MaBan VARCHAR(10);
        SELECT @MaBan = MaBan FROM dbo.DatBan WITH (UPDLOCK, HOLDLOCK)
        WHERE MaDatBan = @MaDatBan AND TrangThai = N'Đã đặt';
        IF @MaBan IS NULL THROW 50013, N'Chỉ đánh dấu không đến với lịch đang ở trạng thái Đã đặt.', 1;

        /* Luu rieng trang thai khach khong den de thong ke khong nham voi huy dat. */
        UPDATE dbo.DatBan SET TrangThai = N'Không đến' WHERE MaDatBan = @MaDatBan;
        IF EXISTS (SELECT 1 FROM dbo.DatBan WHERE MaBan=@MaBan AND TrangThai=N'Đã nhận bàn')
            UPDATE dbo.Ban SET TrangThai=N'Đang sử dụng' WHERE MaBan=@MaBan AND TrangThai<>N'Bảo trì';
        ELSE IF EXISTS (SELECT 1 FROM dbo.DatBan WHERE MaBan=@MaBan AND TrangThai=N'Đã đặt')
            UPDATE dbo.Ban SET TrangThai=N'Đã đặt' WHERE MaBan=@MaBan AND TrangThai<>N'Bảo trì';
        ELSE
            UPDATE dbo.Ban SET TrangThai=N'Trống' WHERE MaBan=@MaBan AND TrangThai<>N'Bảo trì';
        COMMIT TRANSACTION;
    END TRY
    BEGIN CATCH
        IF XACT_STATE() <> 0 ROLLBACK TRANSACTION;
        THROW;
    END CATCH
END;
GO

CREATE OR ALTER PROCEDURE dbo.sp_DatBan_Sua
    @MaDatBan VARCHAR(10),
    @MaKH VARCHAR(10),
    @MaBanMoi VARCHAR(10),
    @MaNV VARCHAR(10) = NULL,
    @NgayDat DATE,
    @GioDat TIME(0),
    @SoNguoi INT,
    @TrangThai NVARCHAR(30),
    @GhiChu NVARCHAR(255) = NULL
AS
BEGIN
    SET NOCOUNT ON;
    SET XACT_ABORT ON;
    /* Sua thong tin khong duoc dung de nhay qua cac buoc nghiep vu.
       Xac nhan den, huy, khong den va hoan thanh co procedure rieng. */
    IF @TrangThai IS NULL OR @TrangThai <> N'Đã đặt'
        THROW 50014, N'Chỉ được sửa thông tin lịch đang ở trạng thái Đã đặt; hãy dùng chức năng nghiệp vụ tương ứng để đổi trạng thái.', 1;
    IF @SoNguoi IS NULL OR @SoNguoi <= 0 OR @NgayDat IS NULL OR @GioDat IS NULL
        THROW 50015, N'Thông tin ngày, giờ hoặc số người không hợp lệ.', 1;

    DECLARE @StartAt DATETIME2(0) = DATEADD(SECOND, DATEDIFF(SECOND, CAST('00:00:00' AS TIME), @GioDat), CAST(@NgayDat AS DATETIME2(0)));
    BEGIN TRY
        BEGIN TRANSACTION;
        DECLARE @MaBanCu VARCHAR(10), @TrangThaiCu NVARCHAR(30), @SoCho INT, @TrangThaiBan NVARCHAR(30);
        SELECT @MaBanCu = MaBan, @TrangThaiCu = TrangThai
        FROM dbo.DatBan WITH (UPDLOCK, HOLDLOCK) WHERE MaDatBan = @MaDatBan;
        IF @MaBanCu IS NULL THROW 50016, N'Không tìm thấy lịch đặt bàn.', 1;
        IF @TrangThaiCu <> N'Đã đặt' THROW 50017, N'Chỉ được sửa lịch đang ở trạng thái Đã đặt.', 1;

        SELECT @SoCho = SoCho, @TrangThaiBan = TrangThai
        FROM dbo.Ban WITH (UPDLOCK, HOLDLOCK) WHERE MaBan = @MaBanMoi;
        IF @SoCho IS NULL THROW 50018, N'Không tìm thấy bàn mới.', 1;
        IF @SoCho < @SoNguoi THROW 50019, N'Bàn không đủ số chỗ.', 1;
        IF @TrangThaiBan IN (N'Bảo trì', N'Đang sử dụng') THROW 50020, N'Bàn mới đang bảo trì hoặc được sử dụng.', 1;

        IF EXISTS (
            SELECT 1 FROM dbo.DatBan WITH (UPDLOCK, HOLDLOCK)
            CROSS APPLY (SELECT DATEADD(SECOND, DATEDIFF(SECOND, CAST('00:00:00' AS TIME), GioDat), CAST(NgayDat AS DATETIME2(0))) AS StartAt) AS B
            WHERE MaBan = @MaBanMoi AND MaDatBan <> @MaDatBan
              AND TrangThai IN (N'Đã đặt', N'Đã nhận bàn')
              AND B.StartAt < DATEADD(MINUTE, 120, @StartAt)
              AND @StartAt < DATEADD(MINUTE, 120, B.StartAt)
        ) THROW 50021, N'Bàn mới bị trùng lịch đặt trong 120 phút.', 1;

        UPDATE dbo.DatBan SET MaKH=@MaKH, MaBan=@MaBanMoi, MaNV=@MaNV,
            NgayDat=@NgayDat, GioDat=@GioDat, SoNguoi=@SoNguoi,
            TrangThai=N'Đã đặt', GhiChu=@GhiChu
        WHERE MaDatBan=@MaDatBan;

        /* Recalculate both table states from all remaining active bookings. */
        IF EXISTS (SELECT 1 FROM dbo.DatBan WHERE MaBan=@MaBanCu AND TrangThai=N'Đã nhận bàn')
            UPDATE dbo.Ban SET TrangThai=N'Đang sử dụng' WHERE MaBan=@MaBanCu AND TrangThai<>N'Bảo trì';
        ELSE IF EXISTS (SELECT 1 FROM dbo.DatBan WHERE MaBan=@MaBanCu AND TrangThai=N'Đã đặt')
            UPDATE dbo.Ban SET TrangThai=N'Đã đặt' WHERE MaBan=@MaBanCu AND TrangThai<>N'Bảo trì';
        ELSE
            UPDATE dbo.Ban SET TrangThai=N'Trống' WHERE MaBan=@MaBanCu AND TrangThai<>N'Bảo trì';

        IF @MaBanMoi<>@MaBanCu
        BEGIN
            IF EXISTS (SELECT 1 FROM dbo.DatBan WHERE MaBan=@MaBanMoi AND TrangThai=N'Đã nhận bàn')
                UPDATE dbo.Ban SET TrangThai=N'Đang sử dụng' WHERE MaBan=@MaBanMoi AND TrangThai<>N'Bảo trì';
            ELSE IF EXISTS (SELECT 1 FROM dbo.DatBan WHERE MaBan=@MaBanMoi AND TrangThai=N'Đã đặt')
                UPDATE dbo.Ban SET TrangThai=N'Đã đặt' WHERE MaBan=@MaBanMoi AND TrangThai<>N'Bảo trì';
            ELSE
                UPDATE dbo.Ban SET TrangThai=N'Trống' WHERE MaBan=@MaBanMoi AND TrangThai<>N'Bảo trì';
        END;

        COMMIT TRANSACTION;
    END TRY
    BEGIN CATCH
        IF XACT_STATE() <> 0 ROLLBACK TRANSACTION;
        THROW;
    END CATCH
END;
GO

CREATE OR ALTER PROCEDURE dbo.sp_DatBan_Xoa
    @MaDatBan VARCHAR(10)
AS
BEGIN
    SET NOCOUNT ON;
    SET XACT_ABORT ON;
    BEGIN TRY
        BEGIN TRANSACTION;
        DECLARE @MaBan VARCHAR(10), @TrangThai NVARCHAR(30);
        SELECT @MaBan=MaBan, @TrangThai=TrangThai
        FROM dbo.DatBan WITH (UPDLOCK, HOLDLOCK) WHERE MaDatBan=@MaDatBan;
        IF @MaBan IS NULL THROW 50022, N'Không tìm thấy lịch đặt bàn.', 1;
        IF @TrangThai NOT IN (N'Đã hủy', N'Hoàn thành')
            THROW 50023, N'Chỉ xóa lịch đã hủy hoặc hoàn thành.', 1;
        DELETE dbo.DatBan WHERE MaDatBan=@MaDatBan;
        IF EXISTS (SELECT 1 FROM dbo.DatBan WHERE MaBan=@MaBan AND TrangThai=N'Đã nhận bàn')
            UPDATE dbo.Ban SET TrangThai=N'Đang sử dụng' WHERE MaBan=@MaBan AND TrangThai<>N'Bảo trì';
        ELSE IF EXISTS (SELECT 1 FROM dbo.DatBan WHERE MaBan=@MaBan AND TrangThai=N'Đã đặt')
            UPDATE dbo.Ban SET TrangThai=N'Đã đặt' WHERE MaBan=@MaBan AND TrangThai<>N'Bảo trì';
        ELSE
            UPDATE dbo.Ban SET TrangThai=N'Trống' WHERE MaBan=@MaBan AND TrangThai<>N'Bảo trì';
        COMMIT TRANSACTION;
    END TRY
    BEGIN CATCH
        IF XACT_STATE() <> 0 ROLLBACK TRANSACTION;
        THROW;
    END CATCH
END;
GO

CREATE OR ALTER PROCEDURE dbo.sp_DatBan_HoanThanh
    @MaDatBan VARCHAR(10)
AS
BEGIN
    SET NOCOUNT ON;
    SET XACT_ABORT ON;
    BEGIN TRY
        BEGIN TRANSACTION;
        DECLARE @MaBan VARCHAR(10);
        SELECT @MaBan=MaBan FROM dbo.DatBan WITH (UPDLOCK, HOLDLOCK)
        WHERE MaDatBan=@MaDatBan AND TrangThai=N'Đã nhận bàn';
        IF @MaBan IS NULL THROW 50024, N'Chỉ hoàn thành lịch đã nhận bàn.', 1;

        UPDATE dbo.DatBan SET TrangThai=N'Hoàn thành' WHERE MaDatBan=@MaDatBan;
        IF EXISTS (SELECT 1 FROM dbo.DatBan WHERE MaBan=@MaBan AND TrangThai=N'Đã nhận bàn')
            UPDATE dbo.Ban SET TrangThai=N'Đang sử dụng' WHERE MaBan=@MaBan AND TrangThai<>N'Bảo trì';
        ELSE IF EXISTS (SELECT 1 FROM dbo.DatBan WHERE MaBan=@MaBan AND TrangThai=N'Đã đặt')
            UPDATE dbo.Ban SET TrangThai=N'Đã đặt' WHERE MaBan=@MaBan AND TrangThai<>N'Bảo trì';
        ELSE
            UPDATE dbo.Ban SET TrangThai=N'Trống' WHERE MaBan=@MaBan AND TrangThai<>N'Bảo trì';
        COMMIT TRANSACTION;
    END TRY
    BEGIN CATCH
        IF XACT_STATE() <> 0 ROLLBACK TRANSACTION;
        THROW;
    END CATCH
END;
GO

CREATE OR ALTER PROCEDURE dbo.sp_DatBan_ChuyenBan
    @MaDatBan VARCHAR(10), @MaBanMoi VARCHAR(10)
AS
BEGIN
    SET NOCOUNT ON; SET XACT_ABORT ON;
    BEGIN TRY
        BEGIN TRANSACTION;
        DECLARE @MaBanCu VARCHAR(10), @TrangThai NVARCHAR(30);
        SELECT @MaBanCu=MaBan, @TrangThai=TrangThai
        FROM dbo.DatBan WITH (UPDLOCK,HOLDLOCK) WHERE MaDatBan=@MaDatBan;
        IF @MaBanCu IS NULL THROW 51201, N'Khong tim thay dat ban.', 1;
        IF @MaBanCu=@MaBanMoi THROW 51202, N'Ban moi trung voi ban hien tai.', 1;
        IF @TrangThai NOT IN (N'Đã đặt',N'Da dat',N'Đã nhận bàn',N'Da nhan ban')
            THROW 51203, N'Chi duoc chuyen ban cho dat ban dang hoat dong.', 1;
        IF NOT EXISTS(SELECT 1 FROM dbo.Ban WITH (UPDLOCK,HOLDLOCK) WHERE MaBan=@MaBanMoi)
            THROW 51204, N'Ban moi khong ton tai.', 1;
        IF EXISTS(SELECT 1 FROM dbo.Ban WHERE MaBan=@MaBanMoi AND TrangThai NOT IN (N'Trống',N'Trong',N'Đã đặt',N'Da dat'))
            THROW 51205, N'Ban moi dang duoc su dung.', 1;
        DECLARE @NgayDat DATE, @GioDat TIME(0), @SoCho INT, @SoNguoi INT;
        SELECT @NgayDat=NgayDat, @GioDat=GioDat, @SoNguoi=SoNguoi
        FROM dbo.DatBan WITH (UPDLOCK,HOLDLOCK) WHERE MaDatBan=@MaDatBan;

        SELECT @SoCho=SoCho FROM dbo.Ban WITH (UPDLOCK,HOLDLOCK) WHERE MaBan=@MaBanMoi;
        IF @SoCho IS NULL THROW 51207, N'Ban moi khong ton tai.', 1;
        IF @SoCho < @SoNguoi THROW 51208, N'Ban moi khong du so cho.', 1;

        DECLARE @StartAt DATETIME2(0) = DATEADD(SECOND, DATEDIFF(SECOND, CAST('00:00:00' AS TIME), @GioDat), CAST(@NgayDat AS DATETIME2(0)));
        IF EXISTS (
            SELECT 1
            FROM dbo.DatBan WITH (UPDLOCK,HOLDLOCK)
            CROSS APPLY (SELECT DATEADD(SECOND, DATEDIFF(SECOND, CAST('00:00:00' AS TIME), GioDat), CAST(NgayDat AS DATETIME2(0))) AS StartAt) AS B
            WHERE MaBan=@MaBanMoi AND MaDatBan<>@MaDatBan
              AND TrangThai IN (N'Đã đặt',N'Da dat',N'Đã nhận bàn',N'Da nhan ban')
              AND B.StartAt < DATEADD(MINUTE,120,@StartAt)
              AND @StartAt < DATEADD(MINUTE,120,B.StartAt)
        ) THROW 51206, N'Ban moi da co dat ban trung khoang thoi gian 120 phut.', 1;
        UPDATE dbo.DatBan SET MaBan=@MaBanMoi WHERE MaDatBan=@MaDatBan;

        /* Tinh lai trang thai ban cu dua tren cac lich con hoat dong. */
        IF EXISTS (SELECT 1 FROM dbo.DatBan WHERE MaBan=@MaBanCu AND TrangThai=N'Đã nhận bàn')
            UPDATE dbo.Ban SET TrangThai=N'Đang sử dụng' WHERE MaBan=@MaBanCu AND TrangThai<>N'Bảo trì';
        ELSE IF EXISTS (SELECT 1 FROM dbo.DatBan WHERE MaBan=@MaBanCu AND TrangThai=N'Đã đặt')
            UPDATE dbo.Ban SET TrangThai=N'Đã đặt' WHERE MaBan=@MaBanCu AND TrangThai<>N'Bảo trì';
        ELSE
            UPDATE dbo.Ban SET TrangThai=N'Trống' WHERE MaBan=@MaBanCu AND TrangThai<>N'Bảo trì';

        /* Tinh lai trang thai ban moi sau khi nhan lich chuyen sang. */
        IF EXISTS (SELECT 1 FROM dbo.DatBan WHERE MaBan=@MaBanMoi AND TrangThai=N'Đã nhận bàn')
            UPDATE dbo.Ban SET TrangThai=N'Đang sử dụng' WHERE MaBan=@MaBanMoi AND TrangThai<>N'Bảo trì';
        ELSE IF EXISTS (SELECT 1 FROM dbo.DatBan WHERE MaBan=@MaBanMoi AND TrangThai=N'Đã đặt')
            UPDATE dbo.Ban SET TrangThai=N'Đã đặt' WHERE MaBan=@MaBanMoi AND TrangThai<>N'Bảo trì';
        ELSE
            UPDATE dbo.Ban SET TrangThai=N'Trống' WHERE MaBan=@MaBanMoi AND TrangThai<>N'Bảo trì';

        COMMIT TRANSACTION;
    END TRY
    BEGIN CATCH IF XACT_STATE()<>0 ROLLBACK TRANSACTION; THROW; END CATCH
END;
GO
