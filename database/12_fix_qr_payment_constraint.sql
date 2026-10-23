USE RestaurantManagement;
GO

/*
    Sua rang buoc phuong thuc thanh toan hoa don de dong bo voi
    HoaDonService.cpp va dbo.sp_ThanhToanHoaDon.
    Chay script nay tren database dang su dung; khong can tao lai bang.
*/
SET XACT_ABORT ON;
GO

BEGIN TRY
    BEGIN TRANSACTION;

    IF OBJECT_ID(N'dbo.HoaDon', N'U') IS NULL
        THROW 51201, N'Khong tim thay bang dbo.HoaDon. Hay chay script tao bang truoc.', 1;

    IF EXISTS (
        SELECT 1 FROM sys.check_constraints
        WHERE parent_object_id = OBJECT_ID(N'dbo.HoaDon')
          AND name = N'CK_HoaDon_PhuongThuc'
    )
    BEGIN
        ALTER TABLE dbo.HoaDon DROP CONSTRAINT CK_HoaDon_PhuongThuc;
    END;

    ALTER TABLE dbo.HoaDon WITH CHECK
    ADD CONSTRAINT CK_HoaDon_PhuongThuc
    CHECK (
        PhuongThucThanhToan IS NULL
        OR PhuongThucThanhToan IN (N'Tiền mặt', N'Chuyển khoản', N'Ví điện tử', N'QR')
    );

    ALTER TABLE dbo.HoaDon CHECK CONSTRAINT CK_HoaDon_PhuongThuc;

    COMMIT TRANSACTION;
END TRY
BEGIN CATCH
    IF @@TRANCOUNT > 0 ROLLBACK TRANSACTION;
    THROW;
END CATCH;
GO

/* Xac nhan rang buoc sau khi cap nhat. */
SELECT name, definition, is_disabled, is_not_trusted
FROM sys.check_constraints
WHERE parent_object_id = OBJECT_ID(N'dbo.HoaDon')
  AND name = N'CK_HoaDon_PhuongThuc';
GO
