# Build and Linker Notes

## Compiler Flags

**-mcpu=cortex-m3**	Kiến trúc CPU (STM32F103 là Cortex-M3)
**-mthumb**	        Bật chế độ Thumb (16-bit instruction set)
**-std=c11**	    Chuẩn C11
**-Wall**	        Bật tất cả cảnh báo
**-g**              Gắn debug info cho GDB
**-O0**	            Tắt tối ưu hoá (hữu ích khi debug)
**-Iinclude -Ilib**	Thư mục chứa header
**-DSTM32F10X_MD**	Macro cho phân loại chip (Medium Density)

## Linker Flags

**-T linker.ld**	Dùng file linker do người dùng viết
**-nostartfiles**	Bỏ qua startup mặc định của GCC
**--gc-sections**	Dọn bỏ section không dùng để giảm size
**-lm**	            Link thư viện toán học
**-lc**	            Link thư viện C chuẩn

## Linker Script

**MEMORY**	Khai báo vùng FLASH và RAM của STM32F103C8T6
**.text**	Nơi chứa code chương trình và bảng vector ngắt
**.data**	Biến toàn cục được khởi tạo (từ flash copy vào RAM)
**.bss**	Biến toàn cục chưa khởi tạo (sẽ được zero hóa khi khởi động)
**_estack**	Điểm bắt đầu của stack – thường là cuối RAM
**_sidata, _sdata, _edata, ...** Các biến đặc biệt phục vụ khởi tạo dữ liệu trong startup code
