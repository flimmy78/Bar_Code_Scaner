/**
 * @file dlg.h
 * @brief This file generate automatically from dialog.xml by machine.
 *
 * @version parser version V1.0.2
 * @author netcom
 * @date Feb 21 2013
 * @note add multi editor for T1Gen <20110326>
 */
// string list 
const unsigned char *fengefu_str[] = {
  " ",  // english
  "--------------------------------",  // s chinese
  " ",  // t chinese
};

// dialog ID define 
#define DLG_logo	1
#define DLG_scan_func	(DLG_logo+1)
#define DLG_data_transfer_func	(DLG_scan_func+1)
#define DLG_terminal_setting	(DLG_data_transfer_func+1)
#define DLG_system_info	(DLG_terminal_setting+1)
#define DLG_about_copyright	(DLG_system_info+1)
#define DLG_scan_option_menu	(DLG_about_copyright+1)
#define DLG_scan_mode_menu	(DLG_scan_option_menu+1)
#define DLG_quantity_enter_menu	(DLG_scan_mode_menu+1)
#define DLG_add_time_option	(DLG_quantity_enter_menu+1)
#define DLG_add_date_option	(DLG_add_time_option+1)
#define DLG_input_barcode	(DLG_add_date_option+1)
#define DLG_input_send_num	(DLG_input_barcode+1)
#define DLG_seperator_setting	(DLG_input_send_num+1)
#define DLG_modify_num_ack	(DLG_seperator_setting+1)
#define DLG_database_query_option	(DLG_modify_num_ack+1)
#define DLG_avoid_dunplication_option	(DLG_database_query_option+1)
#define DLG_barcode_statistic_option	(DLG_avoid_dunplication_option+1)
#define DLG_transfer_confirm_option	(DLG_barcode_statistic_option+1)
#define DLG_language_option	(DLG_transfer_confirm_option+1)
#define DLG_beeper_vol_option	(DLG_language_option+1)
#define DLG_timeout_option	(DLG_beeper_vol_option+1)
#define DLG_transfer_bluetooth_option	(DLG_timeout_option+1)
#define DLG_transfer_u_disk_option	(DLG_transfer_bluetooth_option+1)
#define DLG_transfer_batch_option	(DLG_transfer_u_disk_option+1)
#define DLG_import_database	(DLG_transfer_batch_option+1)
#define DLG_barcode_option	(DLG_import_database+1)
#define DLG_time_date_setting	(DLG_barcode_option+1)
#define DLG_default_setting	(DLG_time_date_setting+1)
#define DLG_set_system_password	(DLG_default_setting+1)
#define DLG_decoder_setting	(DLG_set_system_password+1)
#define DLG_invalid_offset_setting	(DLG_decoder_setting+1)
#define DLG_batch_data_send	(DLG_invalid_offset_setting+1)
#define DLG_batch_transfer_mode	(DLG_batch_data_send+1)
#define DLG_batch_data_clear	(DLG_batch_transfer_mode+1)
#define DLG_clear_scan_record	(DLG_batch_data_clear+1)
#define DLG_view_memery	(DLG_clear_scan_record+1)
#define DLG_bluetooth_info	(DLG_view_memery+1)
#define DLG_firmware_info	(DLG_bluetooth_info+1)
#define DLG_create_new_file	(DLG_firmware_info+1)
#define DLG_file_list	(DLG_create_new_file+1)
#define DLG_u_disk_format	(DLG_file_list+1)
#define DLG_file_options_menu	(DLG_u_disk_format+1)
#define DLG_clear_file_ack	(DLG_file_options_menu+1)
#define DLG_delete_file_ack	(DLG_clear_file_ack+1)
#define DLG_test_menu	(DLG_delete_file_ack+1)
#define DLG_age_test_menu	(DLG_test_menu+1)
#define DLG_single_test_menu	(DLG_age_test_menu+1)
#define DLG_test_lcd_display	(DLG_single_test_menu+1)
#define DLG_test_keypad	(DLG_test_lcd_display+1)
#define DLG_test_scaner_init	(DLG_test_keypad+1)
#define DLG_test_usb	(DLG_test_scaner_init+1)
#define DLG_test_rtc	(DLG_test_usb+1)
#define DLG_test_AD	(DLG_test_rtc+1)
#define DLG_test_bluetooth	(DLG_test_AD+1)
#define DLG_test_complete	(DLG_test_bluetooth+1)
#define DLG_todo	(DLG_test_complete+1)
#define DLG_MAX	57



// function prototypes
unsigned char dlgproc_logo(unsigned char type,unsigned char *key);
unsigned char dlgproc_scan_func(unsigned char type,unsigned char *key);
unsigned char dlgproc_data_transfer_func(unsigned char type,unsigned char *key);
unsigned char dlgproc_terminal_setting(unsigned char type,unsigned char *key);
unsigned char dlgproc_system_info(unsigned char type,unsigned char *key);
unsigned char dlgproc_about_copyright(unsigned char type,unsigned char *key);
unsigned char dlgproc_scan_option_menu(unsigned char type,unsigned char *key);
unsigned char dlgproc_scan_mode_menu(unsigned char type,unsigned char *key);
unsigned char dlgproc_quantity_enter_menu(unsigned char type,unsigned char *key);
unsigned char dlgproc_add_time_option(unsigned char type,unsigned char *key);
unsigned char dlgproc_add_date_option(unsigned char type,unsigned char *key);
unsigned char dlgproc_input_barcode(unsigned char type,unsigned char *key);
unsigned char dlgproc_input_send_num(unsigned char type,unsigned char *key);
unsigned char dlgproc_seperator_setting(unsigned char type,unsigned char *key);
unsigned char dlgproc_modify_num_ack(unsigned char type,unsigned char *key);
unsigned char dlgproc_database_query_option(unsigned char type,unsigned char *key);
unsigned char dlgproc_avoid_dunplication_option(unsigned char type,unsigned char *key);
unsigned char dlgproc_barcode_statistic_option(unsigned char type,unsigned char *key);
unsigned char dlgproc_transfer_confirm_option(unsigned char type,unsigned char *key);
unsigned char dlgproc_language_option(unsigned char type,unsigned char *key);
unsigned char dlgproc_beeper_vol_option(unsigned char type,unsigned char *key);
unsigned char dlgproc_timeout_option(unsigned char type,unsigned char *key);
unsigned char dlgproc_transfer_bluetooth_option(unsigned char type,unsigned char *key);
unsigned char dlgproc_transfer_u_disk_option(unsigned char type,unsigned char *key);
unsigned char dlgproc_transfer_batch_option(unsigned char type,unsigned char *key);
unsigned char dlgproc_import_database(unsigned char type,unsigned char *key);
unsigned char dlgproc_barcode_option(unsigned char type,unsigned char *key);
unsigned char dlgproc_time_date_setting(unsigned char type,unsigned char *key);
unsigned char dlgproc_default_setting(unsigned char type,unsigned char *key);
unsigned char dlgproc_set_system_password(unsigned char type,unsigned char *key);
unsigned char dlgproc_decoder_setting(unsigned char type,unsigned char *key);
unsigned char dlgproc_invalid_offset_setting(unsigned char type,unsigned char *key);
unsigned char dlgproc_batch_data_send(unsigned char type,unsigned char *key);
unsigned char dlgproc_batch_transfer_mode(unsigned char type,unsigned char *key);
unsigned char dlgproc_batch_data_clear(unsigned char type,unsigned char *key);
unsigned char dlgproc_clear_scan_record(unsigned char type,unsigned char *key);
unsigned char dlgproc_view_memery(unsigned char type,unsigned char *key);
unsigned char dlgproc_bluetooth_info(unsigned char type,unsigned char *key);
unsigned char dlgproc_firmware_info(unsigned char type,unsigned char *key);
unsigned char dlgproc_create_new_file(unsigned char type,unsigned char *key);
unsigned char dlgproc_file_list(unsigned char type,unsigned char *key);
unsigned char dlgproc_u_disk_format(unsigned char type,unsigned char *key);
unsigned char dlgproc_file_options_menu(unsigned char type,unsigned char *key);
unsigned char dlgproc_clear_file_ack(unsigned char type,unsigned char *key);
unsigned char dlgproc_delete_file_ack(unsigned char type,unsigned char *key);
unsigned char dlgproc_test_menu(unsigned char type,unsigned char *key);
unsigned char dlgproc_age_test_menu(unsigned char type,unsigned char *key);
unsigned char dlgproc_single_test_menu(unsigned char type,unsigned char *key);
unsigned char dlgproc_test_lcd_display(unsigned char type,unsigned char *key);
unsigned char dlgproc_test_keypad(unsigned char type,unsigned char *key);
unsigned char dlgproc_test_scaner_init(unsigned char type,unsigned char *key);
unsigned char dlgproc_test_usb(unsigned char type,unsigned char *key);
unsigned char dlgproc_test_rtc(unsigned char type,unsigned char *key);
unsigned char dlgproc_test_AD(unsigned char type,unsigned char *key);
unsigned char dlgproc_test_bluetooth(unsigned char type,unsigned char *key);
unsigned char dlgproc_test_complete(unsigned char type,unsigned char *key);
// ---------------------------------------------------------------------------------------------
// 主界面 dialog content
const unsigned char *content_logo_1[] = { // English
	"Home", // english
	"Options", // english
	(unsigned char*)0,
};
const unsigned char *content_logo_2[] = { // Simplified Chinese
	" ", // S chinese
	"选项", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_logo_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 主界面 dialog struct
const TDialogRes dlgres_logo = {
	DLG_logo, // self id 
	STYLE_ICON | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_logo_1, // text content english
	(unsigned char**)content_logo_2, // text content chinese S 
	(unsigned char**)content_logo_3, // text content chinese T 
};
// logo jump list
const TJumpList jump_logo[] = {
	{KEY_ENTER,	DLG_logo	},
	{KEY_EXT_1,	DLG_scan_func	},
	{KEY_EXT_2,	DLG_data_transfer_func	},
	{KEY_EXT_3,	DLG_terminal_setting	},
	{KEY_EXT_4,	DLG_system_info	},
	{KEY_EXT_9,	DLG_test_menu	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 扫描功能 dialog content
const unsigned char *content_scan_func_1[] = { // English
	" ", // english
	"Options", // english
	(unsigned char*)0,
};
const unsigned char *content_scan_func_2[] = { // Simplified Chinese
	"  ", // S chinese
	"选项", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_scan_func_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 扫描功能 dialog struct
const TDialogRes dlgres_scan_func = {
	DLG_scan_func, // self id 
	STYLE_ICON | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_scan_func_1, // text content english
	(unsigned char**)content_scan_func_2, // text content chinese S 
	(unsigned char**)content_scan_func_3, // text content chinese T 
};
// scan_func jump list
const TJumpList jump_scan_func[] = {
	{KEY_ESC,	DLG_logo	},
	{KEY_LEFT_SHOTCUT,	DLG_scan_option_menu	},
	{KEY_EXT_9,	DLG_input_send_num	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 数据传输 dialog content
const unsigned char *content_data_transfer_func_1[] = { // English
	"Transfer Mode", // english
	"         Select   Esc", // english
	"1 BlueTooth", // english
	"2 USB Keyboard", // english
	"3 USB Virtual COM", // english
	"4 U Disk Mode", // english
	"5 Batch Mode", // english
	"6 Import Database", // english
	"7 Seperator", // english
	(unsigned char*)0,
};
const unsigned char *content_data_transfer_func_2[] = { // Simplified Chinese
	"数据传输 ", // S chinese
	"        选择     退出", // S chinese
	"1 蓝牙", // S chinese
	"2 USB键盘", // S chinese
	"3 USB虚拟串口", // S chinese
	"4 U盘模式", // S chinese
	"5 脱机存储", // S chinese
	"6 导入商品信息", // S chinese
	"7 分隔符设置", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_data_transfer_func_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 数据传输 dialog struct
const TDialogRes dlgres_data_transfer_func = {
	DLG_data_transfer_func, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_data_transfer_func_1, // text content english
	(unsigned char**)content_data_transfer_func_2, // text content chinese S 
	(unsigned char**)content_data_transfer_func_3, // text content chinese T 
};
// data_transfer_func jump list
const TJumpList jump_data_transfer_func[] = {
	{KEY_ESC,	DLG_logo	},
	{KEY_EXT_1,	DLG_transfer_bluetooth_option	},
	{KEY_EXT_4,	DLG_transfer_u_disk_option	},
	{KEY_EXT_5,	DLG_transfer_batch_option	},
	{KEY_EXT_6,	DLG_import_database	},
	{KEY_EXT_7,	DLG_seperator_setting	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 系统设置 dialog content
const unsigned char *content_terminal_setting_1[] = { // English
	"Terminal Setting", // english
	"       Select     Esc", // english
	"1 BlueTooth", // english
	"2 Language", // english
	"3 Key Volume", // english
	"4 Timeout Setting", // english
	"5 DateTime", // english
	"6 Default setting", // english
	(unsigned char*)0,
};
const unsigned char *content_terminal_setting_2[] = { // Simplified Chinese
	"系统设置 ", // S chinese
	"        选择     退出", // S chinese
	"1 条码", // S chinese
	"2 语言", // S chinese
	"3 按键音", // S chinese
	"4 超时设置", // S chinese
	"5 时间和日期", // S chinese
	"6 恢复默认设置", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_terminal_setting_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 系统设置 dialog struct
const TDialogRes dlgres_terminal_setting = {
	DLG_terminal_setting, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_terminal_setting_1, // text content english
	(unsigned char**)content_terminal_setting_2, // text content chinese S 
	(unsigned char**)content_terminal_setting_3, // text content chinese T 
};
// terminal_setting jump list
const TJumpList jump_terminal_setting[] = {
	{KEY_ESC,	DLG_logo	},
	{KEY_EXT_1,	DLG_barcode_option	},
	{KEY_EXT_2,	DLG_language_option	},
	{KEY_EXT_3,	DLG_beeper_vol_option	},
	{KEY_EXT_4,	DLG_timeout_option	},
	{KEY_EXT_5,	DLG_time_date_setting	},
	{KEY_EXT_6,	DLG_default_setting	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 系统信息 dialog content
const unsigned char *content_system_info_1[] = { // English
	"Info", // english
	"     Select       Esc", // english
	"1 Memery Info", // english
	"2 BlueTooth Info", // english
	"3 Firmware Version", // english
	(unsigned char*)0,
};
const unsigned char *content_system_info_2[] = { // Simplified Chinese
	"系统信息 ", // S chinese
	"        选择     退出", // S chinese
	"1 查看内存", // S chinese
	"2 蓝牙信息", // S chinese
	"3 版本信息", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_system_info_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 系统信息 dialog struct
const TDialogRes dlgres_system_info = {
	DLG_system_info, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_system_info_1, // text content english
	(unsigned char**)content_system_info_2, // text content chinese S 
	(unsigned char**)content_system_info_3, // text content chinese T 
};
// system_info jump list
const TJumpList jump_system_info[] = {
	{KEY_ESC,	DLG_logo	},
	{KEY_EXT_1,	DLG_view_memery	},
	{KEY_EXT_2,	DLG_bluetooth_info	},
	{KEY_EXT_3,	DLG_firmware_info	},
	{KEY_EXT_9,	DLG_about_copyright	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 系统信息 dialog content
const unsigned char *content_about_copyright_1[] = { // English
	(unsigned char*)0,
};
const unsigned char *content_about_copyright_2[] = { // Simplified Chinese
	(unsigned char*)0,
};
const unsigned char *content_about_copyright_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 系统信息 dialog struct
const TDialogRes dlgres_about_copyright = {
	DLG_about_copyright, // self id 
	STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_about_copyright_1, // text content english
	(unsigned char**)content_about_copyright_2, // text content chinese S 
	(unsigned char**)content_about_copyright_3, // text content chinese T 
};
// about_copyright jump list
const TJumpList jump_about_copyright[] = {
	{KEY_ESC,	DLG_system_info	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 扫描功能的选项菜单 dialog content
const unsigned char *content_scan_option_menu_1[] = { // English
	" ", // english
	"      Select      Esc", // english
	"1 Quantity Enter ", // english
	"2 Add Time", // english
	"3 Add  Date", // english
	"4 Input Barcode", // english
	"5 Database Query", // english
	"6 Avoid Dunplication", // english
	"7 Barcode Statistic", // english
	"8 Transfer Confirm", // english
	"9 Delete Scan Record", // english
	(unsigned char*)0,
};
const unsigned char *content_scan_option_menu_2[] = { // Simplified Chinese
	"  ", // S chinese
	"        选择     退出", // S chinese
	"1 数量录入", // S chinese
	"2 添加扫描时间", // S chinese
	"3 添加扫描日期", // S chinese
	"4 输入条码", // S chinese
	"5 数据库查询", // S chinese
	"6 防止重复条码", // S chinese
	"7 条码统计", // S chinese
	"8 传输确认", // S chinese
	"9 清除扫描记录", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_scan_option_menu_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 扫描功能的选项菜单 dialog struct
const TDialogRes dlgres_scan_option_menu = {
	DLG_scan_option_menu, // self id 
	STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_scan_option_menu_1, // text content english
	(unsigned char**)content_scan_option_menu_2, // text content chinese S 
	(unsigned char**)content_scan_option_menu_3, // text content chinese T 
};
// scan_option_menu jump list
const TJumpList jump_scan_option_menu[] = {
	{KEY_ESC,	DLG_scan_func	},
	{KEY_EXT_1,	DLG_quantity_enter_menu	},
	{KEY_EXT_2,	DLG_add_time_option	},
	{KEY_EXT_3,	DLG_add_date_option	},
	{KEY_EXT_4,	DLG_input_barcode	},
	{KEY_EXT_5,	DLG_database_query_option	},
	{KEY_EXT_6,	DLG_avoid_dunplication_option	},
	{KEY_EXT_7,	DLG_barcode_statistic_option	},
	{KEY_EXT_8,	DLG_transfer_confirm_option	},
	{KEY_EXT_9,	DLG_clear_scan_record	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 扫描模式菜单 dialog content
const unsigned char *content_scan_mode_menu_1[] = { // English
	"Scan Mode", // english
	"      Select      Esc", // english
	"1 Good-read Off", // english
	"2 Momentary", // english
	"3 Continuous", // english
	(unsigned char*)0,
};
const unsigned char *content_scan_mode_menu_2[] = { // Simplified Chinese
	"扫描模式 ", // S chinese
	"        选择     退出", // S chinese
	"1 单次按键触发", // S chinese
	"2 按键保持", // S chinese
	"3 连续扫描", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_scan_mode_menu_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 扫描模式菜单 dialog struct
const TDialogRes dlgres_scan_mode_menu = {
	DLG_scan_mode_menu, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_scan_mode_menu_1, // text content english
	(unsigned char**)content_scan_mode_menu_2, // text content chinese S 
	(unsigned char**)content_scan_mode_menu_3, // text content chinese T 
};
// scan_mode_menu jump list
const TJumpList jump_scan_mode_menu[] = {
	{KEY_ESC,	DLG_scan_option_menu	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 数量录入模式菜单 dialog content
const unsigned char *content_quantity_enter_menu_1[] = { // English
	"Quantity Enter", // english
	"      Select      Esc", // english
	"1 Disable", // english
	"2 Repeat Output ", // english
	"3 Output Quantity", // english
	(unsigned char*)0,
};
const unsigned char *content_quantity_enter_menu_2[] = { // Simplified Chinese
	"数量录入", // S chinese
	"         选择    退出", // S chinese
	"1 关闭", // S chinese
	"2 重复输出条码", // S chinese
	"3 输出条码数量", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_quantity_enter_menu_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 数量录入模式菜单 dialog struct
const TDialogRes dlgres_quantity_enter_menu = {
	DLG_quantity_enter_menu, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_quantity_enter_menu_1, // text content english
	(unsigned char**)content_quantity_enter_menu_2, // text content chinese S 
	(unsigned char**)content_quantity_enter_menu_3, // text content chinese T 
};
// quantity_enter_menu jump list
const TJumpList jump_quantity_enter_menu[] = {
	{KEY_ESC,	DLG_scan_option_menu	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 手动输入条码的处理界面 dialog content
const unsigned char *content_add_time_option_1[] = { // English
	"Add Time", // english
	"       Select     Esc", // english
	"1 Disbale", // english
	"2 Enable ", // english
	(unsigned char*)0,
};
const unsigned char *content_add_time_option_2[] = { // Simplified Chinese
	"添加时间", // S chinese
	"        选择     退出", // S chinese
	"1 关闭", // S chinese
	"2 开启", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_add_time_option_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 手动输入条码的处理界面 dialog struct
const TDialogRes dlgres_add_time_option = {
	DLG_add_time_option, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_add_time_option_1, // text content english
	(unsigned char**)content_add_time_option_2, // text content chinese S 
	(unsigned char**)content_add_time_option_3, // text content chinese T 
};
// add_time_option jump list
const TJumpList jump_add_time_option[] = {
	{KEY_ESC,	DLG_scan_option_menu	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 是否需要添加日期 dialog content
const unsigned char *content_add_date_option_1[] = { // English
	"Add Date", // english
	"       Select     Esc", // english
	"1 Disbale", // english
	"2 Enable ", // english
	(unsigned char*)0,
};
const unsigned char *content_add_date_option_2[] = { // Simplified Chinese
	"添加日期", // S chinese
	"        选择     退出", // S chinese
	"1 关闭", // S chinese
	"2 开启", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_add_date_option_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 是否需要添加日期 dialog struct
const TDialogRes dlgres_add_date_option = {
	DLG_add_date_option, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_add_date_option_1, // text content english
	(unsigned char**)content_add_date_option_2, // text content chinese S 
	(unsigned char**)content_add_date_option_3, // text content chinese T 
};
// add_date_option jump list
const TJumpList jump_add_date_option[] = {
	{KEY_ESC,	DLG_scan_option_menu	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 手动输入条码的处理界面 dialog content
const unsigned char *content_input_barcode_1[] = { // English
	(unsigned char*)0,
};
const unsigned char *content_input_barcode_2[] = { // Simplified Chinese
	(unsigned char*)0,
};
const unsigned char *content_input_barcode_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 手动输入条码的处理界面 editor
const TEditRes editres_input_barcode = {
	0, // x
	0, // y
	EDIT_ALPHA, // attrib
	20, // display max length
	30, // input max length
	0,
	(TEditRes*)0, // no next edit
};
// 手动输入条码的处理界面 dialog struct
const TDialogRes dlgres_input_barcode = {
	DLG_input_barcode, // self id 
	STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)&editres_input_barcode, // editor for this dialog
	(unsigned char**)content_input_barcode_1, // text content english
	(unsigned char**)content_input_barcode_2, // text content chinese S 
	(unsigned char**)content_input_barcode_3, // text content chinese T 
};
// input_barcode jump list
const TJumpList jump_input_barcode[] = {
	{KEY_ESC,	DLG_scan_option_menu	},
	{KEY_EXT_1,	DLG_scan_func	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 手动输入发送数量的处理界面 dialog content
const unsigned char *content_input_send_num_1[] = { // English
	(unsigned char*)0,
};
const unsigned char *content_input_send_num_2[] = { // Simplified Chinese
	(unsigned char*)0,
};
const unsigned char *content_input_send_num_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 手动输入发送数量的处理界面 editor
const TEditRes editres_input_send_num = {
	0, // x
	0, // y
	EDIT_NUM, // attrib
	6, // display max length
	6, // input max length
	0,
	(TEditRes*)0, // no next edit
};
// 手动输入发送数量的处理界面 dialog struct
const TDialogRes dlgres_input_send_num = {
	DLG_input_send_num, // self id 
	STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)&editres_input_send_num, // editor for this dialog
	(unsigned char**)content_input_send_num_1, // text content english
	(unsigned char**)content_input_send_num_2, // text content chinese S 
	(unsigned char**)content_input_send_num_3, // text content chinese T 
};
// input_send_num jump list
const TJumpList jump_input_send_num[] = {
	{KEY_ESC,	DLG_scan_func	},
	{KEY_EXT_1,	DLG_modify_num_ack	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 设置分隔符的处理界面 dialog content
const unsigned char *content_seperator_setting_1[] = { // English
	(unsigned char*)0,
};
const unsigned char *content_seperator_setting_2[] = { // Simplified Chinese
	(unsigned char*)0,
};
const unsigned char *content_seperator_setting_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 设置分隔符的处理界面 editor
const TEditRes editres_seperator_setting = {
	0, // x
	0, // y
	EDIT_ALPHA, // attrib
	2, // display max length
	2, // input max length
	0,
	(TEditRes*)0, // no next edit
};
// 设置分隔符的处理界面 dialog struct
const TDialogRes dlgres_seperator_setting = {
	DLG_seperator_setting, // self id 
	STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)&editres_seperator_setting, // editor for this dialog
	(unsigned char**)content_seperator_setting_1, // text content english
	(unsigned char**)content_seperator_setting_2, // text content chinese S 
	(unsigned char**)content_seperator_setting_3, // text content chinese T 
};
// seperator_setting jump list
const TJumpList jump_seperator_setting[] = {
	{KEY_ESC,	DLG_data_transfer_func	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 确认修改发送数量的处理界面 dialog content
const unsigned char *content_modify_num_ack_1[] = { // English
	" ", // english
	"Yes               No ", // english
	(unsigned char*)0,
};
const unsigned char *content_modify_num_ack_2[] = { // Simplified Chinese
	"  ", // S chinese
	"是                否", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_modify_num_ack_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 确认修改发送数量的处理界面 dialog struct
const TDialogRes dlgres_modify_num_ack = {
	DLG_modify_num_ack, // self id 
	STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_modify_num_ack_1, // text content english
	(unsigned char**)content_modify_num_ack_2, // text content chinese S 
	(unsigned char**)content_modify_num_ack_3, // text content chinese T 
};
// modify_num_ack jump list
const TJumpList jump_modify_num_ack[] = {
	{KEY_ESC,	DLG_scan_func	},
	{KEY_ENTER,	DLG_scan_func	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 是否需要开启数据库查询模式菜单 dialog content
const unsigned char *content_database_query_option_1[] = { // English
	"Database Query", // english
	"     Select      Esc", // english
	"1 Disbale", // english
	"2 Enable ", // english
	(unsigned char*)0,
};
const unsigned char *content_database_query_option_2[] = { // Simplified Chinese
	"数据库查询", // S chinese
	"        选择    退出", // S chinese
	"1 关闭", // S chinese
	"2 开启", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_database_query_option_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 是否需要开启数据库查询模式菜单 dialog struct
const TDialogRes dlgres_database_query_option = {
	DLG_database_query_option, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_database_query_option_1, // text content english
	(unsigned char**)content_database_query_option_2, // text content chinese S 
	(unsigned char**)content_database_query_option_3, // text content chinese T 
};
// database_query_option jump list
const TJumpList jump_database_query_option[] = {
	{KEY_ESC,	DLG_scan_option_menu	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 是否需要开启防重码功能模式菜单 dialog content
const unsigned char *content_avoid_dunplication_option_1[] = { // English
	"Avoid Dunplicaton", // english
	"     Select      Esc", // english
	"1 Disbale", // english
	"2 Enable ", // english
	(unsigned char*)0,
};
const unsigned char *content_avoid_dunplication_option_2[] = { // Simplified Chinese
	"防止重复条码", // S chinese
	"        选择    退出", // S chinese
	"1 关闭", // S chinese
	"2 开启", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_avoid_dunplication_option_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 是否需要开启防重码功能模式菜单 dialog struct
const TDialogRes dlgres_avoid_dunplication_option = {
	DLG_avoid_dunplication_option, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_avoid_dunplication_option_1, // text content english
	(unsigned char**)content_avoid_dunplication_option_2, // text content chinese S 
	(unsigned char**)content_avoid_dunplication_option_3, // text content chinese T 
};
// avoid_dunplication_option jump list
const TJumpList jump_avoid_dunplication_option[] = {
	{KEY_ESC,	DLG_scan_option_menu	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 是否需要开启条码统计功能模式菜单 dialog content
const unsigned char *content_barcode_statistic_option_1[] = { // English
	"Barcode Statistic", // english
	"     Select      Esc", // english
	"1 Disbale", // english
	"2 Enable ", // english
	(unsigned char*)0,
};
const unsigned char *content_barcode_statistic_option_2[] = { // Simplified Chinese
	"条码统计", // S chinese
	"        选择    退出", // S chinese
	"1 关闭", // S chinese
	"2 开启", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_barcode_statistic_option_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 是否需要开启条码统计功能模式菜单 dialog struct
const TDialogRes dlgres_barcode_statistic_option = {
	DLG_barcode_statistic_option, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_barcode_statistic_option_1, // text content english
	(unsigned char**)content_barcode_statistic_option_2, // text content chinese S 
	(unsigned char**)content_barcode_statistic_option_3, // text content chinese T 
};
// barcode_statistic_option jump list
const TJumpList jump_barcode_statistic_option[] = {
	{KEY_ESC,	DLG_scan_option_menu	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 是否需要传输确认模式菜单 dialog content
const unsigned char *content_transfer_confirm_option_1[] = { // English
	"Transfer Confirm", // english
	"     Select      Esc", // english
	"1 Disbale", // english
	"2 Enable ", // english
	(unsigned char*)0,
};
const unsigned char *content_transfer_confirm_option_2[] = { // Simplified Chinese
	"传输确认", // S chinese
	"        选择    退出", // S chinese
	"1 关闭", // S chinese
	"2 开启", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_transfer_confirm_option_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 是否需要传输确认模式菜单 dialog struct
const TDialogRes dlgres_transfer_confirm_option = {
	DLG_transfer_confirm_option, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_transfer_confirm_option_1, // text content english
	(unsigned char**)content_transfer_confirm_option_2, // text content chinese S 
	(unsigned char**)content_transfer_confirm_option_3, // text content chinese T 
};
// transfer_confirm_option jump list
const TJumpList jump_transfer_confirm_option[] = {
	{KEY_ESC,	DLG_scan_option_menu	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 语言设置菜单 dialog content
const unsigned char *content_language_option_1[] = { // English
	"Language", // english
	"      Select      Esc", // english
	"1 English", // english
	"2 Simply Chinese", // english
	(unsigned char*)0,
};
const unsigned char *content_language_option_2[] = { // Simplified Chinese
	"语言", // S chinese
	"        选择     退出", // S chinese
	"1 英语", // S chinese
	"2 简体中文", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_language_option_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 语言设置菜单 dialog struct
const TDialogRes dlgres_language_option = {
	DLG_language_option, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_language_option_1, // text content english
	(unsigned char**)content_language_option_2, // text content chinese S 
	(unsigned char**)content_language_option_3, // text content chinese T 
};
// language_option jump list
const TJumpList jump_language_option[] = {
	{KEY_ESC,	DLG_terminal_setting	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 按键音设置菜单 dialog content
const unsigned char *content_beeper_vol_option_1[] = { // English
	"Key Beeper", // english
	"      Select      Esc", // english
	"1 Disable", // english
	"2 Enable", // english
	(unsigned char*)0,
};
const unsigned char *content_beeper_vol_option_2[] = { // Simplified Chinese
	"按键音", // S chinese
	"        选择     退出", // S chinese
	"1 关闭", // S chinese
	"2 开启", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_beeper_vol_option_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 按键音设置菜单 dialog struct
const TDialogRes dlgres_beeper_vol_option = {
	DLG_beeper_vol_option, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_beeper_vol_option_1, // text content english
	(unsigned char**)content_beeper_vol_option_2, // text content chinese S 
	(unsigned char**)content_beeper_vol_option_3, // text content chinese T 
};
// beeper_vol_option jump list
const TJumpList jump_beeper_vol_option[] = {
	{KEY_ESC,	DLG_terminal_setting	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 超时设置菜单 dialog content
const unsigned char *content_timeout_option_1[] = { // English
	"Timeout", // english
	"      Select      Esc", // english
	"1 Disbale", // english
	"2 30s", // english
	"3 60s", // english
	"4 2 minutes", // english
	"5 5 minutes", // english
	(unsigned char*)0,
};
const unsigned char *content_timeout_option_2[] = { // Simplified Chinese
	"超时设置", // S chinese
	"        选择     退出", // S chinese
	"1 关闭", // S chinese
	"2 30秒", // S chinese
	"3 60秒", // S chinese
	"4 2分钟", // S chinese
	"5 5分钟", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_timeout_option_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 超时设置菜单 dialog struct
const TDialogRes dlgres_timeout_option = {
	DLG_timeout_option, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_timeout_option_1, // text content english
	(unsigned char**)content_timeout_option_2, // text content chinese S 
	(unsigned char**)content_timeout_option_3, // text content chinese T 
};
// timeout_option jump list
const TJumpList jump_timeout_option[] = {
	{KEY_ESC,	DLG_terminal_setting	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 蓝牙传输模式选项 dialog content
const unsigned char *content_transfer_bluetooth_option_1[] = { // English
	"BlueTooth", // english
	"      Select      Esc", // english
	"1 Offline Disable ", // english
	"2 Offline Enable ", // english
	(unsigned char*)0,
};
const unsigned char *content_transfer_bluetooth_option_2[] = { // Simplified Chinese
	"蓝牙模式", // S chinese
	"        选择     退出", // S chinese
	"1 脱机存储关闭", // S chinese
	"2 脱机存储开启", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_transfer_bluetooth_option_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 蓝牙传输模式选项 dialog struct
const TDialogRes dlgres_transfer_bluetooth_option = {
	DLG_transfer_bluetooth_option, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_transfer_bluetooth_option_1, // text content english
	(unsigned char**)content_transfer_bluetooth_option_2, // text content chinese S 
	(unsigned char**)content_transfer_bluetooth_option_3, // text content chinese T 
};
// transfer_bluetooth_option jump list
const TJumpList jump_transfer_bluetooth_option[] = {
	{KEY_ESC,	DLG_data_transfer_func	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// U盘传输模式选项 dialog content
const unsigned char *content_transfer_u_disk_option_1[] = { // English
	"U Disk", // english
	"      Select      Esc", // english
	"1 Create new file", // english
	"2 File list", // english
	"3 Format", // english
	(unsigned char*)0,
};
const unsigned char *content_transfer_u_disk_option_2[] = { // Simplified Chinese
	"U盘", // S chinese
	"        选择     退出", // S chinese
	"1 创建新文件", // S chinese
	"2 文件列表", // S chinese
	"3 格式化", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_transfer_u_disk_option_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// U盘传输模式选项 dialog struct
const TDialogRes dlgres_transfer_u_disk_option = {
	DLG_transfer_u_disk_option, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_transfer_u_disk_option_1, // text content english
	(unsigned char**)content_transfer_u_disk_option_2, // text content chinese S 
	(unsigned char**)content_transfer_u_disk_option_3, // text content chinese T 
};
// transfer_u_disk_option jump list
const TJumpList jump_transfer_u_disk_option[] = {
	{KEY_ESC,	DLG_data_transfer_func	},
	{KEY_EXT_1,	DLG_create_new_file	},
	{KEY_EXT_2,	DLG_file_list	},
	{KEY_EXT_3,	DLG_u_disk_format	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 脱机存储传输模式选项 dialog content
const unsigned char *content_transfer_batch_option_1[] = { // English
	"Offline Data", // english
	"      Select      Esc", // english
	"1 Send offline data", // english
	"2 Interface", // english
	"3 Batch Data Delete", // english
	(unsigned char*)0,
};
const unsigned char *content_transfer_batch_option_2[] = { // Simplified Chinese
	"脱机数据处理", // S chinese
	"        选择     退出", // S chinese
	"1 发送脱机数据", // S chinese
	"2 通信接口", // S chinese
	"3 清除脱机数据", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_transfer_batch_option_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 脱机存储传输模式选项 dialog struct
const TDialogRes dlgres_transfer_batch_option = {
	DLG_transfer_batch_option, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_transfer_batch_option_1, // text content english
	(unsigned char**)content_transfer_batch_option_2, // text content chinese S 
	(unsigned char**)content_transfer_batch_option_3, // text content chinese T 
};
// transfer_batch_option jump list
const TJumpList jump_transfer_batch_option[] = {
	{KEY_ESC,	DLG_data_transfer_func	},
	{KEY_EXT_1,	DLG_batch_data_send	},
	{KEY_EXT_2,	DLG_batch_transfer_mode	},
	{KEY_EXT_3,	DLG_batch_data_clear	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 导入商品信息 dialog content
const unsigned char *content_import_database_1[] = { // English
	"Import Database", // english
	"                  Esc", // english
	(unsigned char*)0,
};
const unsigned char *content_import_database_2[] = { // Simplified Chinese
	"导入数据库", // S chinese
	"                 退出", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_import_database_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 导入商品信息 dialog struct
const TDialogRes dlgres_import_database = {
	DLG_import_database, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_import_database_1, // text content english
	(unsigned char**)content_import_database_2, // text content chinese S 
	(unsigned char**)content_import_database_3, // text content chinese T 
};
// import_database jump list
const TJumpList jump_import_database[] = {
	{KEY_ESC,	DLG_data_transfer_func	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 条码设置 dialog content
const unsigned char *content_barcode_option_1[] = { // English
	"Barcode Setting", // english
	"       Select     Esc", // english
	"1 Decoder", // english
	"2 Invalid offset", // english
	(unsigned char*)0,
};
const unsigned char *content_barcode_option_2[] = { // Simplified Chinese
	"条码设置", // S chinese
	"       选择      退出", // S chinese
	"1 解码器", // S chinese
	"2 有效位设置", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_barcode_option_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 条码设置 dialog struct
const TDialogRes dlgres_barcode_option = {
	DLG_barcode_option, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_barcode_option_1, // text content english
	(unsigned char**)content_barcode_option_2, // text content chinese S 
	(unsigned char**)content_barcode_option_3, // text content chinese T 
};
// barcode_option jump list
const TJumpList jump_barcode_option[] = {
	{KEY_ESC,	DLG_terminal_setting	},
	{KEY_EXT_1,	DLG_decoder_setting	},
	{KEY_EXT_2,	DLG_invalid_offset_setting	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 时间设置 dialog content
const unsigned char *content_time_date_setting_1[] = { // English
	"Time/Date", // english
	"Input             Esc", // english
	"Set Date/Time:", // english
	" ", // english
	"(YYYYMMDDHHMMSS)", // english
	(unsigned char*)0,
};
const unsigned char *content_time_date_setting_2[] = { // Simplified Chinese
	"时间日期", // S chinese
	"输入             退出", // S chinese
	"设置日期/时间:", // S chinese
	" ", // S chinese
	"(YYYYMMDDHHMMSS)", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_time_date_setting_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 时间设置 editor
const TEditRes editres_time_date_setting = {
	0, // x
	26, // y
	EDIT_NUM | EDIT_UNDERLINE, // attrib
	14, // display max length
	14, // input max length
	0,
	(TEditRes*)0, // no next edit
};
// 时间设置 dialog struct
const TDialogRes dlgres_time_date_setting = {
	DLG_time_date_setting, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)&editres_time_date_setting, // editor for this dialog
	(unsigned char**)content_time_date_setting_1, // text content english
	(unsigned char**)content_time_date_setting_2, // text content chinese S 
	(unsigned char**)content_time_date_setting_3, // text content chinese T 
};
// time_date_setting jump list
const TJumpList jump_time_date_setting[] = {
	{KEY_ESC,	DLG_terminal_setting	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 恢复默认设置 dialog content
const unsigned char *content_default_setting_1[] = { // English
	"Default Setting", // english
	"Cancle          Enter", // english
	(unsigned char*)0,
};
const unsigned char *content_default_setting_2[] = { // Simplified Chinese
	"恢复默认设置", // S chinese
	"取消             确定", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_default_setting_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 恢复默认设置 dialog struct
const TDialogRes dlgres_default_setting = {
	DLG_default_setting, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_default_setting_1, // text content english
	(unsigned char**)content_default_setting_2, // text content chinese S 
	(unsigned char**)content_default_setting_3, // text content chinese T 
};
// default_setting jump list
const TJumpList jump_default_setting[] = {
	{KEY_ESC,	DLG_terminal_setting	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 设置系统密码 dialog content
const unsigned char *content_set_system_password_1[] = { // English
	"System Password", // english
	"                  Esc", // english
	(unsigned char*)0,
};
const unsigned char *content_set_system_password_2[] = { // Simplified Chinese
	"设置系统密码", // S chinese
	"                 退出", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_set_system_password_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 设置系统密码 dialog struct
const TDialogRes dlgres_set_system_password = {
	DLG_set_system_password, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_set_system_password_1, // text content english
	(unsigned char**)content_set_system_password_2, // text content chinese S 
	(unsigned char**)content_set_system_password_3, // text content chinese T 
};
// set_system_password jump list
const TJumpList jump_set_system_password[] = {
	{KEY_ESC,	DLG_terminal_setting	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 解码器设置 dialog content
const unsigned char *content_decoder_setting_1[] = { // English
	"Decoder Setting", // english
	"      On/Off     Esc", // english
	"EAN8/13/128", // english
	"UPCA/C/E/E1", // english
	"Code39", // english
	"Code93", // english
	"Code11", // english
	"Code128", // english
	"Interleaved 2/5", // english
	"Matrix 2/5", // english
	"Industry 2/5", // english
	"MSI", // english
	"RSS14/Limited/Ex", // english
	"Chinapost", // english
	(unsigned char*)0,
};
const unsigned char *content_decoder_setting_2[] = { // Simplified Chinese
	"解码器设置", // S chinese
	"       开/关     退出", // S chinese
	"EAN8/13/128", // S chinese
	"UPCA/E/E1", // S chinese
	"Code39", // S chinese
	"Code93", // S chinese
	"Code11", // S chinese
	"Code128", // S chinese
	"Interleaved 2/5", // S chinese
	"Matrix 2/5", // S chinese
	"Industry 2/5", // S chinese
	"MSI", // S chinese
	"RSS14/Limited/Ex", // S chinese
	"Chinapost", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_decoder_setting_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 解码器设置 dialog struct
const TDialogRes dlgres_decoder_setting = {
	DLG_decoder_setting, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_decoder_setting_1, // text content english
	(unsigned char**)content_decoder_setting_2, // text content chinese S 
	(unsigned char**)content_decoder_setting_3, // text content chinese T 
};
// decoder_setting jump list
const TJumpList jump_decoder_setting[] = {
	{KEY_ESC,	DLG_barcode_option	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 有效位数设置 dialog content
const unsigned char *content_invalid_offset_setting_1[] = { // English
	"Invalid Offset", // english
	"Input            Esc", // english
	"Input BeginOff:", // english
	"Input End Off:", // english
	(unsigned char*)0,
};
const unsigned char *content_invalid_offset_setting_2[] = { // Simplified Chinese
	"有效位数", // S chinese
	"输入             退出", // S chinese
	"输入起始有效位:", // S chinese
	"输入结束有效位:", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_invalid_offset_setting_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 有效位数设置 editor1
const TEditRes editres_invalid_offset_setting1 = {
	90, // x
	26, // y
	EDIT_NUM, // attrib
	2, // display max length
	2, // input max length
	0,
	(TEditRes*)0, // no next edit
};
// 有效位数设置 editor
const TEditRes editres_invalid_offset_setting = {
	90, // x
	13, // y
	EDIT_NUM, // attrib
	2, // display max length
	2, // input max length
	0,
	(TEditRes*)&editres_invalid_offset_setting1, // next is edit1
};
// 有效位数设置 dialog struct
const TDialogRes dlgres_invalid_offset_setting = {
	DLG_invalid_offset_setting, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)&editres_invalid_offset_setting, // editor for this dialog
	(unsigned char**)content_invalid_offset_setting_1, // text content english
	(unsigned char**)content_invalid_offset_setting_2, // text content chinese S 
	(unsigned char**)content_invalid_offset_setting_3, // text content chinese T 
};
// invalid_offset_setting jump list
const TJumpList jump_invalid_offset_setting[] = {
	{KEY_ESC,	DLG_barcode_option	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 脱机数据传输 dialog content
const unsigned char *content_batch_data_send_1[] = { // English
	"Batch Trans", // english
	"                  Esc", // english
	(unsigned char*)0,
};
const unsigned char *content_batch_data_send_2[] = { // Simplified Chinese
	"脱机数据传输", // S chinese
	"                 返回", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_batch_data_send_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 脱机数据传输 dialog struct
const TDialogRes dlgres_batch_data_send = {
	DLG_batch_data_send, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_batch_data_send_1, // text content english
	(unsigned char**)content_batch_data_send_2, // text content chinese S 
	(unsigned char**)content_batch_data_send_3, // text content chinese T 
};
// batch_data_send jump list
const TJumpList jump_batch_data_send[] = {
	{KEY_ESC,	DLG_transfer_batch_option	},
	{KEY_RIGHT_SHOTCUT,	DLG_transfer_batch_option	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 数据传输 dialog content
const unsigned char *content_batch_transfer_mode_1[] = { // English
	"Batch Trans Mode", // english
	"         Select   Esc", // english
	"1 BlueTooth", // english
	"2 USB Keyboard", // english
	"3 USB Virtual COM", // english
	(unsigned char*)0,
};
const unsigned char *content_batch_transfer_mode_2[] = { // Simplified Chinese
	"脱机数据通信接口 ", // S chinese
	"        选择     退出", // S chinese
	"1 蓝牙", // S chinese
	"2 USB键盘", // S chinese
	"3 USB虚拟串口", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_batch_transfer_mode_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 数据传输 dialog struct
const TDialogRes dlgres_batch_transfer_mode = {
	DLG_batch_transfer_mode, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_batch_transfer_mode_1, // text content english
	(unsigned char**)content_batch_transfer_mode_2, // text content chinese S 
	(unsigned char**)content_batch_transfer_mode_3, // text content chinese T 
};
// batch_transfer_mode jump list
const TJumpList jump_batch_transfer_mode[] = {
	{KEY_ESC,	DLG_transfer_batch_option	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 脱机数据清除 dialog content
const unsigned char *content_batch_data_clear_1[] = { // English
	"Batch clear", // english
	"Cancle          Enter", // english
	(unsigned char*)0,
};
const unsigned char *content_batch_data_clear_2[] = { // Simplified Chinese
	"记录清除", // S chinese
	"取消             确定", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_batch_data_clear_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 脱机数据清除 dialog struct
const TDialogRes dlgres_batch_data_clear = {
	DLG_batch_data_clear, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_batch_data_clear_1, // text content english
	(unsigned char**)content_batch_data_clear_2, // text content chinese S 
	(unsigned char**)content_batch_data_clear_3, // text content chinese T 
};
// batch_data_clear jump list
const TJumpList jump_batch_data_clear[] = {
	{KEY_ESC,	DLG_transfer_batch_option	},
	{KEY_LEFT_SHOTCUT,	DLG_transfer_batch_option	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 清除扫描数据 dialog content
const unsigned char *content_clear_scan_record_1[] = { // English
	"Record clear", // english
	"Cancle          Enter", // english
	(unsigned char*)0,
};
const unsigned char *content_clear_scan_record_2[] = { // Simplified Chinese
	"记录清除", // S chinese
	"取消             确定", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_clear_scan_record_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 清除扫描数据 dialog struct
const TDialogRes dlgres_clear_scan_record = {
	DLG_clear_scan_record, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_clear_scan_record_1, // text content english
	(unsigned char**)content_clear_scan_record_2, // text content chinese S 
	(unsigned char**)content_clear_scan_record_3, // text content chinese T 
};
// clear_scan_record jump list
const TJumpList jump_clear_scan_record[] = {
	{KEY_LEFT_SHOTCUT,	DLG_scan_option_menu	},
	{KEY_ESC,	DLG_scan_option_menu	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 查看内存 dialog content
const unsigned char *content_view_memery_1[] = { // English
	"Memory", // english
	"                  Esc", // english
	(unsigned char*)0,
};
const unsigned char *content_view_memery_2[] = { // Simplified Chinese
	"内存信息", // S chinese
	"                 返回", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_view_memery_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 查看内存 dialog struct
const TDialogRes dlgres_view_memery = {
	DLG_view_memery, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_view_memery_1, // text content english
	(unsigned char**)content_view_memery_2, // text content chinese S 
	(unsigned char**)content_view_memery_3, // text content chinese T 
};
// view_memery jump list
const TJumpList jump_view_memery[] = {
	{KEY_ESC,	DLG_system_info	},
	{KEY_RIGHT_SHOTCUT,	DLG_system_info	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 蓝牙版本信息 dialog content
const unsigned char *content_bluetooth_info_1[] = { // English
	"About Bluetooth", // english
	"                  Esc", // english
	(unsigned char*)0,
};
const unsigned char *content_bluetooth_info_2[] = { // Simplified Chinese
	"关于蓝牙", // S chinese
	"                 返回", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_bluetooth_info_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 蓝牙版本信息 dialog struct
const TDialogRes dlgres_bluetooth_info = {
	DLG_bluetooth_info, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_bluetooth_info_1, // text content english
	(unsigned char**)content_bluetooth_info_2, // text content chinese S 
	(unsigned char**)content_bluetooth_info_3, // text content chinese T 
};
// bluetooth_info jump list
const TJumpList jump_bluetooth_info[] = {
	{KEY_ESC,	DLG_system_info	},
	{KEY_RIGHT_SHOTCUT,	DLG_system_info	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 固件版本 dialog content
const unsigned char *content_firmware_info_1[] = { // English
	"Version", // english
	"                  Esc", // english
	(unsigned char*)0,
};
const unsigned char *content_firmware_info_2[] = { // Simplified Chinese
	"版本信息 ", // S chinese
	"                 返回", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_firmware_info_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 固件版本 dialog struct
const TDialogRes dlgres_firmware_info = {
	DLG_firmware_info, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_firmware_info_1, // text content english
	(unsigned char**)content_firmware_info_2, // text content chinese S 
	(unsigned char**)content_firmware_info_3, // text content chinese T 
};
// firmware_info jump list
const TJumpList jump_firmware_info[] = {
	{KEY_ESC,	DLG_system_info	},
	{KEY_RIGHT_SHOTCUT,	DLG_system_info	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 创建新文件 dialog content
const unsigned char *content_create_new_file_1[] = { // English
	"New File", // english
	"Input  Creat   Cancle", // english
	"Input file name:", // english
	" ", // english
	"Only Creat Txt File", // english
	(unsigned char*)0,
};
const unsigned char *content_create_new_file_2[] = { // Simplified Chinese
	"创建文件", // S chinese
	"输入    确定     取消", // S chinese
	"请输入文件名:", // S chinese
	" ", // S chinese
	"默认建立txt文件", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_create_new_file_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 创建新文件 editor
const TEditRes editres_create_new_file = {
	0, // x
	26, // y
	EDIT_ALPHA, // attrib
	8, // display max length
	8, // input max length
	0,
	(TEditRes*)0, // no next edit
};
// 创建新文件 dialog struct
const TDialogRes dlgres_create_new_file = {
	DLG_create_new_file, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)&editres_create_new_file, // editor for this dialog
	(unsigned char**)content_create_new_file_1, // text content english
	(unsigned char**)content_create_new_file_2, // text content chinese S 
	(unsigned char**)content_create_new_file_3, // text content chinese T 
};
// create_new_file jump list
const TJumpList jump_create_new_file[] = {
	{KEY_ENTER,	DLG_transfer_u_disk_option	},
	{KEY_ESC,	DLG_transfer_u_disk_option	},
	{KEY_EXT_1,	DLG_create_new_file	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 文件列表 dialog content
const unsigned char *content_file_list_1[] = { // English
	"File List", // english
	"Options  Select  Esc ", // english
	(unsigned char*)0,
};
const unsigned char *content_file_list_2[] = { // Simplified Chinese
	"文件列表", // S chinese
	"选项    选择     返回", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_file_list_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 文件列表 dialog struct
const TDialogRes dlgres_file_list = {
	DLG_file_list, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_file_list_1, // text content english
	(unsigned char**)content_file_list_2, // text content chinese S 
	(unsigned char**)content_file_list_3, // text content chinese T 
};
// file_list jump list
const TJumpList jump_file_list[] = {
	{KEY_ENTER,	DLG_transfer_u_disk_option	},
	{KEY_ESC,	DLG_transfer_u_disk_option	},
	{KEY_LEFT_SHOTCUT,	DLG_file_options_menu	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// u盘格式化 dialog content
const unsigned char *content_u_disk_format_1[] = { // English
	"Format", // english
	"Yes                No", // english
	(unsigned char*)0,
};
const unsigned char *content_u_disk_format_2[] = { // Simplified Chinese
	"格式化", // S chinese
	"是                否", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_u_disk_format_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// u盘格式化 dialog struct
const TDialogRes dlgres_u_disk_format = {
	DLG_u_disk_format, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_u_disk_format_1, // text content english
	(unsigned char**)content_u_disk_format_2, // text content chinese S 
	(unsigned char**)content_u_disk_format_3, // text content chinese T 
};
// u_disk_format jump list
const TJumpList jump_u_disk_format[] = {
	{KEY_ENTER,	DLG_transfer_u_disk_option	},
	{KEY_ESC,	DLG_transfer_u_disk_option	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 文件操作选项 dialog content
const unsigned char *content_file_options_menu_1[] = { // English
	"Options", // english
	"                 Esc", // english
	"1 Clear file", // english
	"2 Delete file", // english
	(unsigned char*)0,
};
const unsigned char *content_file_options_menu_2[] = { // Simplified Chinese
	"选项", // S chinese
	"                返回", // S chinese
	"1 清空文件 ", // S chinese
	"2 删除文件", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_file_options_menu_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 文件操作选项 dialog struct
const TDialogRes dlgres_file_options_menu = {
	DLG_file_options_menu, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_file_options_menu_1, // text content english
	(unsigned char**)content_file_options_menu_2, // text content chinese S 
	(unsigned char**)content_file_options_menu_3, // text content chinese T 
};
// file_options_menu jump list
const TJumpList jump_file_options_menu[] = {
	{KEY_EXT_1,	DLG_clear_file_ack	},
	{KEY_EXT_2,	DLG_delete_file_ack	},
	{KEY_ESC,	DLG_transfer_u_disk_option	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 清空文件 dialog content
const unsigned char *content_clear_file_ack_1[] = { // English
	"Clear file", // english
	"Yes               No ", // english
	(unsigned char*)0,
};
const unsigned char *content_clear_file_ack_2[] = { // Simplified Chinese
	"清空文件", // S chinese
	"是                否", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_clear_file_ack_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 清空文件 dialog struct
const TDialogRes dlgres_clear_file_ack = {
	DLG_clear_file_ack, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_clear_file_ack_1, // text content english
	(unsigned char**)content_clear_file_ack_2, // text content chinese S 
	(unsigned char**)content_clear_file_ack_3, // text content chinese T 
};
// clear_file_ack jump list
const TJumpList jump_clear_file_ack[] = {
	{KEY_ENTER,	DLG_file_options_menu	},
	{KEY_ESC,	DLG_file_options_menu	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 删除文件 dialog content
const unsigned char *content_delete_file_ack_1[] = { // English
	"Delete file", // english
	"Yes               No ", // english
	(unsigned char*)0,
};
const unsigned char *content_delete_file_ack_2[] = { // Simplified Chinese
	"删除文件", // S chinese
	"是                否", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_delete_file_ack_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 删除文件 dialog struct
const TDialogRes dlgres_delete_file_ack = {
	DLG_delete_file_ack, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_delete_file_ack_1, // text content english
	(unsigned char**)content_delete_file_ack_2, // text content chinese S 
	(unsigned char**)content_delete_file_ack_3, // text content chinese T 
};
// delete_file_ack jump list
const TJumpList jump_delete_file_ack[] = {
	{KEY_ENTER,	DLG_file_options_menu	},
	{KEY_ESC,	DLG_file_options_menu	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 测试菜单 dialog content
const unsigned char *content_test_menu_1[] = { // English
	(unsigned char*)0,
};
const unsigned char *content_test_menu_2[] = { // Simplified Chinese
	" ", // S chinese
	"返回", // S chinese
	"1.工厂测试", // S chinese
	"2.老化测试", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_test_menu_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 测试菜单 dialog struct
const TDialogRes dlgres_test_menu = {
	DLG_test_menu, // self id 
	STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_test_menu_1, // text content english
	(unsigned char**)content_test_menu_2, // text content chinese S 
	(unsigned char**)content_test_menu_3, // text content chinese T 
};
// test_menu jump list
const TJumpList jump_test_menu[] = {
	{KEY_ESC,	DLG_logo	},
	{KEY_EXT_1,	DLG_test_lcd_display	},
	{KEY_EXT_2,	DLG_age_test_menu	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 老化测试菜单 dialog content
const unsigned char *content_age_test_menu_1[] = { // English
	(unsigned char*)0,
};
const unsigned char *content_age_test_menu_2[] = { // Simplified Chinese
	" ", // S chinese
	"返回", // S chinese
	"1.扫描老化测试", // S chinese
	"2.蓝牙老化测试", // S chinese
	"3.扫描传输测试", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_age_test_menu_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 老化测试菜单 dialog struct
const TDialogRes dlgres_age_test_menu = {
	DLG_age_test_menu, // self id 
	STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_age_test_menu_1, // text content english
	(unsigned char**)content_age_test_menu_2, // text content chinese S 
	(unsigned char**)content_age_test_menu_3, // text content chinese T 
};
// age_test_menu jump list
const TJumpList jump_age_test_menu[] = {
	{KEY_ESC,	DLG_logo	},
	{KEY_EXT_1,	DLG_age_test_menu	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 单项测试菜单 dialog content
const unsigned char *content_single_test_menu_1[] = { // English
	(unsigned char*)0,
};
const unsigned char *content_single_test_menu_2[] = { // Simplified Chinese
	" ", // S chinese
	"返回", // S chinese
	"1.显示测试", // S chinese
	"2.键盘测试", // S chinese
	"3.扫描测试", // S chinese
	"4.蓝牙测试", // S chinese
	"5.USB测试", // S chinese
	"6.RTC测试", // S chinese
	"7.电池电压检测", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_single_test_menu_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 单项测试菜单 dialog struct
const TDialogRes dlgres_single_test_menu = {
	DLG_single_test_menu, // self id 
	STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_single_test_menu_1, // text content english
	(unsigned char**)content_single_test_menu_2, // text content chinese S 
	(unsigned char**)content_single_test_menu_3, // text content chinese T 
};
// single_test_menu jump list
const TJumpList jump_single_test_menu[] = {
	{KEY_ESC,	DLG_logo	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 背光控制测试 dialog content
const unsigned char *content_test_lcd_display_1[] = { // English
	(unsigned char*)0,
};
const unsigned char *content_test_lcd_display_2[] = { // Simplified Chinese
	" ", // S chinese
	"                确认", // S chinese
	"按下SCAN键之后将开始", // S chinese
	"     做显示检测", // S chinese
	"                    ", // S chinese
	"再次按下SCAN继续测试", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_test_lcd_display_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 背光控制测试 dialog struct
const TDialogRes dlgres_test_lcd_display = {
	DLG_test_lcd_display, // self id 
	STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_test_lcd_display_1, // text content english
	(unsigned char**)content_test_lcd_display_2, // text content chinese S 
	(unsigned char**)content_test_lcd_display_3, // text content chinese T 
};
// test_lcd_display jump list
const TJumpList jump_test_lcd_display[] = {
	{KEY_ENTER,	DLG_test_keypad	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 按键测试 dialog content
const unsigned char *content_test_keypad_1[] = { // English
	(unsigned char*)0,
};
const unsigned char *content_test_keypad_2[] = { // Simplified Chinese
	" ", // S chinese
	"                确认", // S chinese
	"按下相应的键，相应的", // S chinese
	"键名就会在屏幕上消失", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_test_keypad_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 按键测试 dialog struct
const TDialogRes dlgres_test_keypad = {
	DLG_test_keypad, // self id 
	STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_test_keypad_1, // text content english
	(unsigned char**)content_test_keypad_2, // text content chinese S 
	(unsigned char**)content_test_keypad_3, // text content chinese T 
};
// test_keypad jump list
const TJumpList jump_test_keypad[] = {
	{KEY_ENTER,	DLG_test_scaner_init	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 扫描头测试 dialog content
const unsigned char *content_test_scaner_init_1[] = { // English
	(unsigned char*)0,
};
const unsigned char *content_test_scaner_init_2[] = { // Simplified Chinese
	" ", // S chinese
	"                确认", // S chinese
	"按下SCAN键测试扫描头", // S chinese
	" ", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_test_scaner_init_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 扫描头测试 dialog struct
const TDialogRes dlgres_test_scaner_init = {
	DLG_test_scaner_init, // self id 
	STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_test_scaner_init_1, // text content english
	(unsigned char**)content_test_scaner_init_2, // text content chinese S 
	(unsigned char**)content_test_scaner_init_3, // text content chinese T 
};
// test_scaner_init jump list
const TJumpList jump_test_scaner_init[] = {
	{KEY_ENTER,	DLG_test_usb	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// usb连接测试 dialog content
const unsigned char *content_test_usb_1[] = { // English
	(unsigned char*)0,
};
const unsigned char *content_test_usb_2[] = { // Simplified Chinese
	" ", // S chinese
	"                确认", // S chinese
	"按下SCAN键测试USB", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_test_usb_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// usb连接测试 dialog struct
const TDialogRes dlgres_test_usb = {
	DLG_test_usb, // self id 
	STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_test_usb_1, // text content english
	(unsigned char**)content_test_usb_2, // text content chinese S 
	(unsigned char**)content_test_usb_3, // text content chinese T 
};
// test_usb jump list
const TJumpList jump_test_usb[] = {
	{KEY_ENTER,	DLG_test_rtc	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 测试RTC界面 dialog content
const unsigned char *content_test_rtc_1[] = { // English
	(unsigned char*)0,
};
const unsigned char *content_test_rtc_2[] = { // Simplified Chinese
	(unsigned char*)0,
};
const unsigned char *content_test_rtc_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 测试RTC界面 dialog struct
const TDialogRes dlgres_test_rtc = {
	DLG_test_rtc, // self id 
	STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_test_rtc_1, // text content english
	(unsigned char**)content_test_rtc_2, // text content chinese S 
	(unsigned char**)content_test_rtc_3, // text content chinese T 
};
// test_rtc jump list
const TJumpList jump_test_rtc[] = {
	{KEY_ENTER,	DLG_test_AD	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 测试AD采样电路界面 dialog content
const unsigned char *content_test_AD_1[] = { // English
	(unsigned char*)0,
};
const unsigned char *content_test_AD_2[] = { // Simplified Chinese
	(unsigned char*)0,
};
const unsigned char *content_test_AD_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 测试AD采样电路界面 dialog struct
const TDialogRes dlgres_test_AD = {
	DLG_test_AD, // self id 
	STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_test_AD_1, // text content english
	(unsigned char**)content_test_AD_2, // text content chinese S 
	(unsigned char**)content_test_AD_3, // text content chinese T 
};
// test_AD jump list
const TJumpList jump_test_AD[] = {
	{KEY_ENTER,	DLG_test_bluetooth	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 蓝牙模块测试 dialog content
const unsigned char *content_test_bluetooth_1[] = { // English
	(unsigned char*)0,
};
const unsigned char *content_test_bluetooth_2[] = { // Simplified Chinese
	" ", // S chinese
	"                确认", // S chinese
	"按下SCAN键测试蓝牙", // S chinese
	"   ", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_test_bluetooth_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 蓝牙模块测试 dialog struct
const TDialogRes dlgres_test_bluetooth = {
	DLG_test_bluetooth, // self id 
	STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_test_bluetooth_1, // text content english
	(unsigned char**)content_test_bluetooth_2, // text content chinese S 
	(unsigned char**)content_test_bluetooth_3, // text content chinese T 
};
// test_bluetooth jump list
const TJumpList jump_test_bluetooth[] = {
	{KEY_ENTER,	DLG_test_complete	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 测试完成 dialog content
const unsigned char *content_test_complete_1[] = { // English
	(unsigned char*)0,
};
const unsigned char *content_test_complete_2[] = { // Simplified Chinese
	(unsigned char*)0,
};
const unsigned char *content_test_complete_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 测试完成 dialog struct
const TDialogRes dlgres_test_complete = {
	DLG_test_complete, // self id 
	STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_test_complete_1, // text content english
	(unsigned char**)content_test_complete_2, // text content chinese S 
	(unsigned char**)content_test_complete_3, // text content chinese T 
};
// test_complete jump list
const TJumpList jump_test_complete[] = {
	{KEY_ENTER,	DLG_test_menu	},
	{KEY_ESC,	DLG_test_menu	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// 待实现界面 dialog content
const unsigned char *content_todo_1[] = { // English
	"Todo", // english
	"Esc", // english
	(unsigned char*)0,
};
const unsigned char *content_todo_2[] = { // Simplified Chinese
	"待实现 ", // S chinese
	"返回", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_todo_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// 待实现界面 dialog struct
const TDialogRes dlgres_todo = {
	DLG_todo, // self id 
	STYLE_ICON | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_todo_1, // text content english
	(unsigned char**)content_todo_2, // text content chinese S 
	(unsigned char**)content_todo_3, // text content chinese T 
};
// todo jump list
const TJumpList jump_todo[] = {
	{KEY_ENTER,	DLG_logo	},
	{0, 0},
};
// --------------------------------------------------
// dialog action list
const TActionList dlg_actionlist[] ={
	// 主界面
	{ DLG_logo, (TDialogRes*)&dlgres_logo, (DialogCallback)dlgproc_logo, (TJumpList*)&jump_logo[0], "logo" }, // "主界面"
	// 扫描功能
	{ DLG_scan_func, (TDialogRes*)&dlgres_scan_func, (DialogCallback)dlgproc_scan_func, (TJumpList*)&jump_scan_func[0], "scan_func" }, // "扫描功能"
	// 数据传输
	{ DLG_data_transfer_func, (TDialogRes*)&dlgres_data_transfer_func, (DialogCallback)dlgproc_data_transfer_func, (TJumpList*)&jump_data_transfer_func[0], "data_transfer_func" }, // "数据传输"
	// 系统设置
	{ DLG_terminal_setting, (TDialogRes*)&dlgres_terminal_setting, (DialogCallback)dlgproc_terminal_setting, (TJumpList*)&jump_terminal_setting[0], "terminal_setting" }, // "系统设置"
	// 系统信息
	{ DLG_system_info, (TDialogRes*)&dlgres_system_info, (DialogCallback)dlgproc_system_info, (TJumpList*)&jump_system_info[0], "system_info" }, // "系统信息"
	// 系统信息
	{ DLG_about_copyright, (TDialogRes*)&dlgres_about_copyright, (DialogCallback)dlgproc_about_copyright, (TJumpList*)&jump_about_copyright[0], "about_copyright" }, // "系统信息"
	// 扫描功能的选项菜单
	{ DLG_scan_option_menu, (TDialogRes*)&dlgres_scan_option_menu, (DialogCallback)dlgproc_scan_option_menu, (TJumpList*)&jump_scan_option_menu[0], "scan_option_menu" }, // "扫描功能的选项菜单"
	// 扫描模式菜单
	{ DLG_scan_mode_menu, (TDialogRes*)&dlgres_scan_mode_menu, (DialogCallback)dlgproc_scan_mode_menu, (TJumpList*)&jump_scan_mode_menu[0], "scan_mode_menu" }, // "扫描模式菜单"
	// 数量录入模式菜单
	{ DLG_quantity_enter_menu, (TDialogRes*)&dlgres_quantity_enter_menu, (DialogCallback)dlgproc_quantity_enter_menu, (TJumpList*)&jump_quantity_enter_menu[0], "quantity_enter_menu" }, // "数量录入模式菜单"
	// 手动输入条码的处理界面
	{ DLG_add_time_option, (TDialogRes*)&dlgres_add_time_option, (DialogCallback)dlgproc_add_time_option, (TJumpList*)&jump_add_time_option[0], "add_time_option" }, // "手动输入条码的处理界面"
	// 是否需要添加日期
	{ DLG_add_date_option, (TDialogRes*)&dlgres_add_date_option, (DialogCallback)dlgproc_add_date_option, (TJumpList*)&jump_add_date_option[0], "add_date_option" }, // "是否需要添加日期"
	// 手动输入条码的处理界面
	{ DLG_input_barcode, (TDialogRes*)&dlgres_input_barcode, (DialogCallback)dlgproc_input_barcode, (TJumpList*)&jump_input_barcode[0], "input_barcode" }, // "手动输入条码的处理界面"
	// 手动输入发送数量的处理界面
	{ DLG_input_send_num, (TDialogRes*)&dlgres_input_send_num, (DialogCallback)dlgproc_input_send_num, (TJumpList*)&jump_input_send_num[0], "input_send_num" }, // "手动输入发送数量的处理界面"
	// 设置分隔符的处理界面
	{ DLG_seperator_setting, (TDialogRes*)&dlgres_seperator_setting, (DialogCallback)dlgproc_seperator_setting, (TJumpList*)&jump_seperator_setting[0], "seperator_setting" }, // "设置分隔符的处理界面"
	// 确认修改发送数量的处理界面
	{ DLG_modify_num_ack, (TDialogRes*)&dlgres_modify_num_ack, (DialogCallback)dlgproc_modify_num_ack, (TJumpList*)&jump_modify_num_ack[0], "modify_num_ack" }, // "确认修改发送数量的处理界面"
	// 是否需要开启数据库查询模式菜单
	{ DLG_database_query_option, (TDialogRes*)&dlgres_database_query_option, (DialogCallback)dlgproc_database_query_option, (TJumpList*)&jump_database_query_option[0], "database_query_option" }, // "是否需要开启数据库查询模式菜单"
	// 是否需要开启防重码功能模式菜单
	{ DLG_avoid_dunplication_option, (TDialogRes*)&dlgres_avoid_dunplication_option, (DialogCallback)dlgproc_avoid_dunplication_option, (TJumpList*)&jump_avoid_dunplication_option[0], "avoid_dunplication_option" }, // "是否需要开启防重码功能模式菜单"
	// 是否需要开启条码统计功能模式菜单
	{ DLG_barcode_statistic_option, (TDialogRes*)&dlgres_barcode_statistic_option, (DialogCallback)dlgproc_barcode_statistic_option, (TJumpList*)&jump_barcode_statistic_option[0], "barcode_statistic_option" }, // "是否需要开启条码统计功能模式菜单"
	// 是否需要传输确认模式菜单
	{ DLG_transfer_confirm_option, (TDialogRes*)&dlgres_transfer_confirm_option, (DialogCallback)dlgproc_transfer_confirm_option, (TJumpList*)&jump_transfer_confirm_option[0], "transfer_confirm_option" }, // "是否需要传输确认模式菜单"
	// 语言设置菜单
	{ DLG_language_option, (TDialogRes*)&dlgres_language_option, (DialogCallback)dlgproc_language_option, (TJumpList*)&jump_language_option[0], "language_option" }, // "语言设置菜单"
	// 按键音设置菜单
	{ DLG_beeper_vol_option, (TDialogRes*)&dlgres_beeper_vol_option, (DialogCallback)dlgproc_beeper_vol_option, (TJumpList*)&jump_beeper_vol_option[0], "beeper_vol_option" }, // "按键音设置菜单"
	// 超时设置菜单
	{ DLG_timeout_option, (TDialogRes*)&dlgres_timeout_option, (DialogCallback)dlgproc_timeout_option, (TJumpList*)&jump_timeout_option[0], "timeout_option" }, // "超时设置菜单"
	// 蓝牙传输模式选项
	{ DLG_transfer_bluetooth_option, (TDialogRes*)&dlgres_transfer_bluetooth_option, (DialogCallback)dlgproc_transfer_bluetooth_option, (TJumpList*)&jump_transfer_bluetooth_option[0], "transfer_bluetooth_option" }, // "蓝牙传输模式选项"
	// U盘传输模式选项
	{ DLG_transfer_u_disk_option, (TDialogRes*)&dlgres_transfer_u_disk_option, (DialogCallback)dlgproc_transfer_u_disk_option, (TJumpList*)&jump_transfer_u_disk_option[0], "transfer_u_disk_option" }, // "U盘传输模式选项"
	// 脱机存储传输模式选项
	{ DLG_transfer_batch_option, (TDialogRes*)&dlgres_transfer_batch_option, (DialogCallback)dlgproc_transfer_batch_option, (TJumpList*)&jump_transfer_batch_option[0], "transfer_batch_option" }, // "脱机存储传输模式选项"
	// 导入商品信息
	{ DLG_import_database, (TDialogRes*)&dlgres_import_database, (DialogCallback)dlgproc_import_database, (TJumpList*)&jump_import_database[0], "import_database" }, // "导入商品信息"
	// 条码设置
	{ DLG_barcode_option, (TDialogRes*)&dlgres_barcode_option, (DialogCallback)dlgproc_barcode_option, (TJumpList*)&jump_barcode_option[0], "barcode_option" }, // "条码设置"
	// 时间设置
	{ DLG_time_date_setting, (TDialogRes*)&dlgres_time_date_setting, (DialogCallback)dlgproc_time_date_setting, (TJumpList*)&jump_time_date_setting[0], "time_date_setting" }, // "时间设置"
	// 恢复默认设置
	{ DLG_default_setting, (TDialogRes*)&dlgres_default_setting, (DialogCallback)dlgproc_default_setting, (TJumpList*)&jump_default_setting[0], "default_setting" }, // "恢复默认设置"
	// 设置系统密码
	{ DLG_set_system_password, (TDialogRes*)&dlgres_set_system_password, (DialogCallback)dlgproc_set_system_password, (TJumpList*)&jump_set_system_password[0], "set_system_password" }, // "设置系统密码"
	// 解码器设置
	{ DLG_decoder_setting, (TDialogRes*)&dlgres_decoder_setting, (DialogCallback)dlgproc_decoder_setting, (TJumpList*)&jump_decoder_setting[0], "decoder_setting" }, // "解码器设置"
	// 有效位数设置
	{ DLG_invalid_offset_setting, (TDialogRes*)&dlgres_invalid_offset_setting, (DialogCallback)dlgproc_invalid_offset_setting, (TJumpList*)&jump_invalid_offset_setting[0], "invalid_offset_setting" }, // "有效位数设置"
	// 脱机数据传输
	{ DLG_batch_data_send, (TDialogRes*)&dlgres_batch_data_send, (DialogCallback)dlgproc_batch_data_send, (TJumpList*)&jump_batch_data_send[0], "batch_data_send" }, // "脱机数据传输"
	// 数据传输
	{ DLG_batch_transfer_mode, (TDialogRes*)&dlgres_batch_transfer_mode, (DialogCallback)dlgproc_batch_transfer_mode, (TJumpList*)&jump_batch_transfer_mode[0], "batch_transfer_mode" }, // "数据传输"
	// 脱机数据清除
	{ DLG_batch_data_clear, (TDialogRes*)&dlgres_batch_data_clear, (DialogCallback)dlgproc_batch_data_clear, (TJumpList*)&jump_batch_data_clear[0], "batch_data_clear" }, // "脱机数据清除"
	// 清除扫描数据
	{ DLG_clear_scan_record, (TDialogRes*)&dlgres_clear_scan_record, (DialogCallback)dlgproc_clear_scan_record, (TJumpList*)&jump_clear_scan_record[0], "clear_scan_record" }, // "清除扫描数据"
	// 查看内存
	{ DLG_view_memery, (TDialogRes*)&dlgres_view_memery, (DialogCallback)dlgproc_view_memery, (TJumpList*)&jump_view_memery[0], "view_memery" }, // "查看内存"
	// 蓝牙版本信息
	{ DLG_bluetooth_info, (TDialogRes*)&dlgres_bluetooth_info, (DialogCallback)dlgproc_bluetooth_info, (TJumpList*)&jump_bluetooth_info[0], "bluetooth_info" }, // "蓝牙版本信息"
	// 固件版本
	{ DLG_firmware_info, (TDialogRes*)&dlgres_firmware_info, (DialogCallback)dlgproc_firmware_info, (TJumpList*)&jump_firmware_info[0], "firmware_info" }, // "固件版本"
	// 创建新文件
	{ DLG_create_new_file, (TDialogRes*)&dlgres_create_new_file, (DialogCallback)dlgproc_create_new_file, (TJumpList*)&jump_create_new_file[0], "create_new_file" }, // "创建新文件"
	// 文件列表
	{ DLG_file_list, (TDialogRes*)&dlgres_file_list, (DialogCallback)dlgproc_file_list, (TJumpList*)&jump_file_list[0], "file_list" }, // "文件列表"
	// u盘格式化
	{ DLG_u_disk_format, (TDialogRes*)&dlgres_u_disk_format, (DialogCallback)dlgproc_u_disk_format, (TJumpList*)&jump_u_disk_format[0], "u_disk_format" }, // "u盘格式化"
	// 文件操作选项
	{ DLG_file_options_menu, (TDialogRes*)&dlgres_file_options_menu, (DialogCallback)dlgproc_file_options_menu, (TJumpList*)&jump_file_options_menu[0], "file_options_menu" }, // "文件操作选项"
	// 清空文件
	{ DLG_clear_file_ack, (TDialogRes*)&dlgres_clear_file_ack, (DialogCallback)dlgproc_clear_file_ack, (TJumpList*)&jump_clear_file_ack[0], "clear_file_ack" }, // "清空文件"
	// 删除文件
	{ DLG_delete_file_ack, (TDialogRes*)&dlgres_delete_file_ack, (DialogCallback)dlgproc_delete_file_ack, (TJumpList*)&jump_delete_file_ack[0], "delete_file_ack" }, // "删除文件"
	// 测试菜单
	{ DLG_test_menu, (TDialogRes*)&dlgres_test_menu, (DialogCallback)dlgproc_test_menu, (TJumpList*)&jump_test_menu[0], "test_menu" }, // "测试菜单"
	// 老化测试菜单
	{ DLG_age_test_menu, (TDialogRes*)&dlgres_age_test_menu, (DialogCallback)dlgproc_age_test_menu, (TJumpList*)&jump_age_test_menu[0], "age_test_menu" }, // "老化测试菜单"
	// 单项测试菜单
	{ DLG_single_test_menu, (TDialogRes*)&dlgres_single_test_menu, (DialogCallback)dlgproc_single_test_menu, (TJumpList*)&jump_single_test_menu[0], "single_test_menu" }, // "单项测试菜单"
	// 背光控制测试
	{ DLG_test_lcd_display, (TDialogRes*)&dlgres_test_lcd_display, (DialogCallback)dlgproc_test_lcd_display, (TJumpList*)&jump_test_lcd_display[0], "test_lcd_display" }, // "背光控制测试"
	// 按键测试
	{ DLG_test_keypad, (TDialogRes*)&dlgres_test_keypad, (DialogCallback)dlgproc_test_keypad, (TJumpList*)&jump_test_keypad[0], "test_keypad" }, // "按键测试"
	// 扫描头测试
	{ DLG_test_scaner_init, (TDialogRes*)&dlgres_test_scaner_init, (DialogCallback)dlgproc_test_scaner_init, (TJumpList*)&jump_test_scaner_init[0], "test_scaner_init" }, // "扫描头测试"
	// usb连接测试
	{ DLG_test_usb, (TDialogRes*)&dlgres_test_usb, (DialogCallback)dlgproc_test_usb, (TJumpList*)&jump_test_usb[0], "test_usb" }, // "usb连接测试"
	// 测试RTC界面
	{ DLG_test_rtc, (TDialogRes*)&dlgres_test_rtc, (DialogCallback)dlgproc_test_rtc, (TJumpList*)&jump_test_rtc[0], "test_rtc" }, // "测试RTC界面"
	// 测试AD采样电路界面
	{ DLG_test_AD, (TDialogRes*)&dlgres_test_AD, (DialogCallback)dlgproc_test_AD, (TJumpList*)&jump_test_AD[0], "test_AD" }, // "测试AD采样电路界面"
	// 蓝牙模块测试
	{ DLG_test_bluetooth, (TDialogRes*)&dlgres_test_bluetooth, (DialogCallback)dlgproc_test_bluetooth, (TJumpList*)&jump_test_bluetooth[0], "test_bluetooth" }, // "蓝牙模块测试"
	// 测试完成
	{ DLG_test_complete, (TDialogRes*)&dlgres_test_complete, (DialogCallback)dlgproc_test_complete, (TJumpList*)&jump_test_complete[0], "test_complete" }, // "测试完成"
	// 待实现界面
	{ DLG_todo, (TDialogRes*)&dlgres_todo, (DialogCallback)0, (TJumpList*)&jump_todo[0], "todo" }, // "待实现界面"
};
