/* Các syscall tối thiểu cho newlib.
 * Cung cấp `_sbrk` và các hàm thay thế cần thiết cho thư viện C.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stddef.h>
#include <fcntl.h>

/* Cấp phát thêm vùng heap cho malloc. */
caddr_t _sbrk(int incr)
{
    extern char _end; /* Điểm bắt đầu heap do linker script cung cấp. */
    static char *heap_end;
    char *prev_heap_end;

    /* Khởi tạo cuối heap tại điểm bắt đầu nếu chưa được thiết lập. */
    if (heap_end == NULL)
    {
        heap_end = &_end;
    }

    /* Lưu vị trí hiện tại rồi mở rộng heap theo yêu cầu. */
    prev_heap_end = heap_end;
    heap_end += incr;

    return (caddr_t)prev_heap_end;
}

/* Các syscall thay thế tối thiểu để liên kết với newlib. */
int _open(const char *name, int flags, int mode)
{
    /* Đánh dấu tham số chưa hỗ trợ và trả về lỗi hệ thống. */
    (void)name;
    (void)flags;
    (void)mode;
    errno = ENOSYS;
    return -1;
}

int _close(int file)
{
    /* Đánh dấu tham số chưa được sử dụng. */
    (void)file;
    return -1;
}

int _fstat(int file, struct stat *st)
{
    /* Báo thiết bị dạng ký tự cho thư viện C. */
    (void)file;
    st->st_mode = S_IFCHR;
    return 0;
}

off_t _lseek(int file, off_t offset, int whence)
{
    /* Stub không hỗ trợ dịch chuyển vị trí trong tệp. */
    (void)file;
    (void)offset;
    (void)whence;
    return 0;
}

int _read(int file, char *ptr, int len)
{
    /* Stub không đọc dữ liệu từ thiết bị. */
    (void)file;
    (void)ptr;
    (void)len;
    return 0;
}

int _write(int file, const char *ptr, int len)
{
    /* Giả lập ghi thành công bằng cách trả về số byte yêu cầu. */
    (void)file;
    (void)ptr;
    return len;
}

int _isatty(int file)
{
    /* Cho biết luồng được xem như thiết bị đầu cuối. */
    (void)file;
    return 1;
}

void _exit(int status)
{
    /* Giữ CPU trong vòng lặp khi chương trình kết thúc. */
    (void)status;
    while (1)
    {
        /* Không thoát khỏi vòng lặp trên hệ thống nhúng. */
    }
}

pid_t _getpid(void)
{
    /* Trả mã tiến trình cố định cho môi trường đơn tiến trình. */
    return 1;
}

int _kill(int pid, int sig)
{
    /* Báo thao tác gửi tín hiệu không được hỗ trợ. */
    (void)pid;
    (void)sig;
    errno = EINVAL;
    return -1;
}

/* Cung cấp _init/_fini rỗng dạng weak cho tương thích với libc. */
void _init(void) __attribute__((weak));
void _init(void)
{
}

void _fini(void) __attribute__((weak));
void _fini(void)
{
}