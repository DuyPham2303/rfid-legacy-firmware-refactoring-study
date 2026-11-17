// Giao tiếp với chip đọc thẻ MFRC522.
#include "rc522.h"
#include "RFID_Scan.h"
#include "Systick.h"
#include "AppTypes.h"

#define RFID_FIRST_DATA_BLOCK 2U
#define RFID_BLOCK_COUNTER_RESET 0U
#define RFID_TAG_TYPE_BUFFER_SIZE 2U
#define RFID_UID_BUFFER_SIZE 4U
#define RFID_UID_CHECKSUM_SIZE 1U
#define RFID_UID_FRAME_SIZE (RFID_UID_BUFFER_SIZE + RFID_UID_CHECKSUM_SIZE)
#define RFID_AUX_BUFFER_SIZE 4U
#define RFID_USER_ID_INITIAL_VALUE 0
#define RFID_CARD_SETTLE_DELAY_MS 500U
#define RFID_INITIAL_INDEX 0U
#define RFID_BUFFER_INITIAL_VALUE 0U

// Chỉ số bản ghi người dùng đang được đối chiếu.
uint8_t RfidUserData_Index;

// Block dữ liệu hiện tại trên thẻ.
unsigned char block = RFID_FIRST_DATA_BLOCK;
// Bộ đệm phụ trợ cho dữ liệu RFID.
unsigned char buff[RFID_AUX_BUFFER_SIZE];
// Số byte UID được dùng khi kiểm tra.
uint8_t total_uid = RFID_UID_FRAME_SIZE;
// Bộ đệm lưu loại thẻ trả về từ MFRC522.
uint8_t tagtype[RFID_TAG_TYPE_BUFFER_SIZE];
// Biến lưu mã định danh phụ trợ.
int id = RFID_USER_ID_INITIAL_VALUE;
// UID thẻ được cấu hình sẵn để đối chiếu.
unsigned char list_uid[RFID_UID_FRAME_SIZE] = {0xFA, 0x94, 0x34, 0xE9, 0x0E};
// UID đọc được từ thẻ hiện tại.
unsigned char uid[RFID_UID_BUFFER_SIZE];
// Dữ liệu đọc từ block RFID.
unsigned char block_address[MAX_LEN] = {RFID_BUFFER_INITIAL_VALUE};
// Kích thước dữ liệu trả về khi chọn thẻ.
unsigned char size;
// Bộ đệm phụ trợ cho dữ liệu đọc từ thẻ.
unsigned char read_buff[MAX_LEN];
// Danh sách biển số mẫu; hiện không được dùng trong luồng xác thực.
char *license_list[] = {"none",
						"59F2-2322",
						"89F2-2413",
						"23A1-2129",
						"12K1-2312",
						"59H2-8922"
						"67h3-1291",
						"12h8-1231"};

// Cấu hình block chứa biển số và giới hạn block được đọc trong chu trình.
#define RFID_LICENSE_BLOCK 4U
#define RFID_SCAN_BLOCK_END_EXCLUSIVE 4U
// Chừa thêm một byte để kết thúc chuỗi đọc được.
#define RFID_BLOCK_TEXT_SIZE (MAX_LEN + 1U)

// Khai báo các bước kiểm tra UID và biển số của thẻ.
static bool verify_data(const char *buff);
static bool verify_uid(unsigned char *uid);

// Chờ thẻ và lưu UID được chọn.
static void wait_for_card(void)
{
	// Chờ phát hiện thẻ, đọc UID chống va chạm rồi chọn thẻ.
	while (MFRC522_Request(PICC_REQIDL, tagtype) != MI_OK)
		;

	while (MFRC522_Anticoll(uid) != MI_OK)
		;

	size = MFRC522_SelectTag(uid);
}

// Xác thực thẻ và đọc dữ liệu biển số vào bộ đệm.
static void read_card_payload(unsigned char *payload)
{
	// Khóa mặc định dùng khi xác thực block dữ liệu.
	static unsigned char sectorKey[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	// Dữ liệu mẫu hiện được ghi vào block trước khi đọc biển số.
	static unsigned char data[] = "Pham Cao Duy";

	// Xác thực block cần truy cập trước khi đọc và ghi dữ liệu.
	while (MFRC522_Read(block, block_address) != MI_OK)
		;

	while (MFRC522_Auth(PICC_AUTHENT1A, block, sectorKey, uid) != MI_OK)
		;

	while (MFRC522_Read(block, block_address) != MI_OK)
		;

	while (MFRC522_Write(block, data) != MI_OK)
		;

	// Đọc các block tiếp theo đến giới hạn rồi đưa thẻ về trạng thái dừng.
	do
	{
		while (MFRC522_Read(block, block_address) != MI_OK)
			;
		block++;
	} while (block < RFID_SCAN_BLOCK_END_EXCLUSIVE);

	block = RFID_BLOCK_COUNTER_RESET;
	MFRC522_Halt();
	delay_ms(RFID_CARD_SETTLE_DELAY_MS);

	// Đọc block chứa biển số và kết thúc dữ liệu bằng ký tự NUL.
	while (MFRC522_Read(RFID_LICENSE_BLOCK, payload) != MI_OK)
		;
	payload[MAX_LEN] = '\0';
}

// Cập nhật chế độ ứng dụng theo UID và biển số.
static void update_mode_from_card(const char *payload)
{
	// Nếu UID hợp lệ, tiếp tục kiểm tra dữ liệu thẻ.
	if (verify_uid(uid))
	{
		// Dữ liệu khớp cơ sở dữ liệu thì cho phép vận hành.
		if (verify_data(payload))
		{
			ModeStatusCfg.apptype = RUN_MODE;
		}
		// UID hợp lệ nhưng dữ liệu không có trong cơ sở dữ liệu.
		else
		{
			ModeStatusCfg.apptype = ERROR_MODE;
			ModeStatusCfg.errortype = RFID_SCAN_EMPTY;
		}
		// Yêu cầu LCD hiển thị trạng thái mới.
		ModeStatusCfg.IsLcdModeUpdate = true;
	}
	// UID không hợp lệ thì báo lỗi không tìm thấy thẻ.
	else
	{
		ModeStatusCfg.apptype = ERROR_MODE;
		ModeStatusCfg.errortype = UID_NOTFOUND;
	}
}

// Tìm biển số đã đọc trong danh sách người dùng từ thẻ SD.
static bool verify_data(const char *buff)
{
	for (RfidUserData_Index = RFID_INITIAL_INDEX;
		 RfidUserData_Index < count;
		 RfidUserData_Index++)
	{
		// Trả về đúng khi tìm thấy biển số trùng khớp.
		if (strcmp(buff, list_RfidUserData[RfidUserData_Index].license_plate) == 0)
		{
			return true;
		}
	}
	// Không có bản ghi nào khớp.
	return false;
}

// So sánh từng byte UID; hiện trả về đúng ngay khi gặp một byte trùng.
static bool verify_uid(unsigned char *uid)
{
	while (total_uid)
	{
		// Giữ vị trí byte đang so sánh giữa các lần gọi.
		static int index = RFID_INITIAL_INDEX;
		// Trả về đúng ngay khi byte UID tại vị trí hiện tại trùng khớp.
		if (uid[index] == list_uid[index])
		{
			return true;
		}
		index++;
		total_uid--;
	}
	// Không tìm thấy byte UID trùng khớp trong các lượt kiểm tra.
	return false;
}

// Đọc thông tin thẻ, kiểm tra quyền truy cập và cập nhật chế độ hệ thống.
void RFID_Scan()
{
	// Bộ đệm nhận nội dung block, có thêm chỗ cho ký tự kết thúc chuỗi.
	unsigned char payload[RFID_BLOCK_TEXT_SIZE] = {RFID_BUFFER_INITIAL_VALUE};
	// Thực hiện lần lượt nhận thẻ, đọc dữ liệu và cập nhật trạng thái.
	wait_for_card();
	read_card_payload(payload);
	update_mode_from_card((const char *)payload);
}

// Khởi tạo chip đọc thẻ MFRC522.
void RFID_ModuleInit()
{
	// Khởi tạo phần cứng đọc RFID trước khi bắt đầu quét.
	MFRC522_Init();
}
