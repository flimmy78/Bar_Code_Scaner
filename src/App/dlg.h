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
// ������ dialog content
const unsigned char *content_logo_1[] = { // English
	"Home", // english
	"Options", // english
	(unsigned char*)0,
};
const unsigned char *content_logo_2[] = { // Simplified Chinese
	" ", // S chinese
	"ѡ��", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_logo_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// ������ dialog struct
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
// ɨ�蹦�� dialog content
const unsigned char *content_scan_func_1[] = { // English
	" ", // english
	"Options", // english
	(unsigned char*)0,
};
const unsigned char *content_scan_func_2[] = { // Simplified Chinese
	"  ", // S chinese
	"ѡ��", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_scan_func_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// ɨ�蹦�� dialog struct
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
// ���ݴ��� dialog content
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
	"���ݴ��� ", // S chinese
	"        ѡ��     �˳�", // S chinese
	"1 ����", // S chinese
	"2 USB����", // S chinese
	"3 USB���⴮��", // S chinese
	"4 U��ģʽ", // S chinese
	"5 �ѻ��洢", // S chinese
	"6 ������Ʒ��Ϣ", // S chinese
	"7 �ָ�������", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_data_transfer_func_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// ���ݴ��� dialog struct
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
// ϵͳ���� dialog content
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
	"ϵͳ���� ", // S chinese
	"        ѡ��     �˳�", // S chinese
	"1 ����", // S chinese
	"2 ����", // S chinese
	"3 ������", // S chinese
	"4 ��ʱ����", // S chinese
	"5 ʱ�������", // S chinese
	"6 �ָ�Ĭ������", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_terminal_setting_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// ϵͳ���� dialog struct
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
// ϵͳ��Ϣ dialog content
const unsigned char *content_system_info_1[] = { // English
	"Info", // english
	"     Select       Esc", // english
	"1 Memery Info", // english
	"2 BlueTooth Info", // english
	"3 Firmware Version", // english
	(unsigned char*)0,
};
const unsigned char *content_system_info_2[] = { // Simplified Chinese
	"ϵͳ��Ϣ ", // S chinese
	"        ѡ��     �˳�", // S chinese
	"1 �鿴�ڴ�", // S chinese
	"2 ������Ϣ", // S chinese
	"3 �汾��Ϣ", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_system_info_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// ϵͳ��Ϣ dialog struct
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
// ϵͳ��Ϣ dialog content
const unsigned char *content_about_copyright_1[] = { // English
	(unsigned char*)0,
};
const unsigned char *content_about_copyright_2[] = { // Simplified Chinese
	(unsigned char*)0,
};
const unsigned char *content_about_copyright_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// ϵͳ��Ϣ dialog struct
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
// ɨ�蹦�ܵ�ѡ��˵� dialog content
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
	"        ѡ��     �˳�", // S chinese
	"1 ����¼��", // S chinese
	"2 ���ɨ��ʱ��", // S chinese
	"3 ���ɨ������", // S chinese
	"4 ��������", // S chinese
	"5 ���ݿ��ѯ", // S chinese
	"6 ��ֹ�ظ�����", // S chinese
	"7 ����ͳ��", // S chinese
	"8 ����ȷ��", // S chinese
	"9 ���ɨ���¼", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_scan_option_menu_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// ɨ�蹦�ܵ�ѡ��˵� dialog struct
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
// ɨ��ģʽ�˵� dialog content
const unsigned char *content_scan_mode_menu_1[] = { // English
	"Scan Mode", // english
	"      Select      Esc", // english
	"1 Good-read Off", // english
	"2 Momentary", // english
	"3 Continuous", // english
	(unsigned char*)0,
};
const unsigned char *content_scan_mode_menu_2[] = { // Simplified Chinese
	"ɨ��ģʽ ", // S chinese
	"        ѡ��     �˳�", // S chinese
	"1 ���ΰ�������", // S chinese
	"2 ��������", // S chinese
	"3 ����ɨ��", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_scan_mode_menu_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// ɨ��ģʽ�˵� dialog struct
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
// ����¼��ģʽ�˵� dialog content
const unsigned char *content_quantity_enter_menu_1[] = { // English
	"Quantity Enter", // english
	"      Select      Esc", // english
	"1 Disable", // english
	"2 Repeat Output ", // english
	"3 Output Quantity", // english
	(unsigned char*)0,
};
const unsigned char *content_quantity_enter_menu_2[] = { // Simplified Chinese
	"����¼��", // S chinese
	"         ѡ��    �˳�", // S chinese
	"1 �ر�", // S chinese
	"2 �ظ��������", // S chinese
	"3 �����������", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_quantity_enter_menu_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// ����¼��ģʽ�˵� dialog struct
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
// �ֶ���������Ĵ������ dialog content
const unsigned char *content_add_time_option_1[] = { // English
	"Add Time", // english
	"       Select     Esc", // english
	"1 Disbale", // english
	"2 Enable ", // english
	(unsigned char*)0,
};
const unsigned char *content_add_time_option_2[] = { // Simplified Chinese
	"���ʱ��", // S chinese
	"        ѡ��     �˳�", // S chinese
	"1 �ر�", // S chinese
	"2 ����", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_add_time_option_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// �ֶ���������Ĵ������ dialog struct
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
// �Ƿ���Ҫ������� dialog content
const unsigned char *content_add_date_option_1[] = { // English
	"Add Date", // english
	"       Select     Esc", // english
	"1 Disbale", // english
	"2 Enable ", // english
	(unsigned char*)0,
};
const unsigned char *content_add_date_option_2[] = { // Simplified Chinese
	"�������", // S chinese
	"        ѡ��     �˳�", // S chinese
	"1 �ر�", // S chinese
	"2 ����", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_add_date_option_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// �Ƿ���Ҫ������� dialog struct
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
// �ֶ���������Ĵ������ dialog content
const unsigned char *content_input_barcode_1[] = { // English
	(unsigned char*)0,
};
const unsigned char *content_input_barcode_2[] = { // Simplified Chinese
	(unsigned char*)0,
};
const unsigned char *content_input_barcode_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// �ֶ���������Ĵ������ editor
const TEditRes editres_input_barcode = {
	0, // x
	0, // y
	EDIT_ALPHA, // attrib
	20, // display max length
	30, // input max length
	0,
	(TEditRes*)0, // no next edit
};
// �ֶ���������Ĵ������ dialog struct
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
// �ֶ����뷢�������Ĵ������ dialog content
const unsigned char *content_input_send_num_1[] = { // English
	(unsigned char*)0,
};
const unsigned char *content_input_send_num_2[] = { // Simplified Chinese
	(unsigned char*)0,
};
const unsigned char *content_input_send_num_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// �ֶ����뷢�������Ĵ������ editor
const TEditRes editres_input_send_num = {
	0, // x
	0, // y
	EDIT_NUM, // attrib
	6, // display max length
	6, // input max length
	0,
	(TEditRes*)0, // no next edit
};
// �ֶ����뷢�������Ĵ������ dialog struct
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
// ���÷ָ����Ĵ������ dialog content
const unsigned char *content_seperator_setting_1[] = { // English
	(unsigned char*)0,
};
const unsigned char *content_seperator_setting_2[] = { // Simplified Chinese
	(unsigned char*)0,
};
const unsigned char *content_seperator_setting_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// ���÷ָ����Ĵ������ editor
const TEditRes editres_seperator_setting = {
	0, // x
	0, // y
	EDIT_ALPHA, // attrib
	2, // display max length
	2, // input max length
	0,
	(TEditRes*)0, // no next edit
};
// ���÷ָ����Ĵ������ dialog struct
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
// ȷ���޸ķ��������Ĵ������ dialog content
const unsigned char *content_modify_num_ack_1[] = { // English
	" ", // english
	"Yes               No ", // english
	(unsigned char*)0,
};
const unsigned char *content_modify_num_ack_2[] = { // Simplified Chinese
	"  ", // S chinese
	"��                ��", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_modify_num_ack_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// ȷ���޸ķ��������Ĵ������ dialog struct
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
// �Ƿ���Ҫ�������ݿ��ѯģʽ�˵� dialog content
const unsigned char *content_database_query_option_1[] = { // English
	"Database Query", // english
	"     Select      Esc", // english
	"1 Disbale", // english
	"2 Enable ", // english
	(unsigned char*)0,
};
const unsigned char *content_database_query_option_2[] = { // Simplified Chinese
	"���ݿ��ѯ", // S chinese
	"        ѡ��    �˳�", // S chinese
	"1 �ر�", // S chinese
	"2 ����", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_database_query_option_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// �Ƿ���Ҫ�������ݿ��ѯģʽ�˵� dialog struct
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
// �Ƿ���Ҫ���������빦��ģʽ�˵� dialog content
const unsigned char *content_avoid_dunplication_option_1[] = { // English
	"Avoid Dunplicaton", // english
	"     Select      Esc", // english
	"1 Disbale", // english
	"2 Enable ", // english
	(unsigned char*)0,
};
const unsigned char *content_avoid_dunplication_option_2[] = { // Simplified Chinese
	"��ֹ�ظ�����", // S chinese
	"        ѡ��    �˳�", // S chinese
	"1 �ر�", // S chinese
	"2 ����", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_avoid_dunplication_option_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// �Ƿ���Ҫ���������빦��ģʽ�˵� dialog struct
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
// �Ƿ���Ҫ��������ͳ�ƹ���ģʽ�˵� dialog content
const unsigned char *content_barcode_statistic_option_1[] = { // English
	"Barcode Statistic", // english
	"     Select      Esc", // english
	"1 Disbale", // english
	"2 Enable ", // english
	(unsigned char*)0,
};
const unsigned char *content_barcode_statistic_option_2[] = { // Simplified Chinese
	"����ͳ��", // S chinese
	"        ѡ��    �˳�", // S chinese
	"1 �ر�", // S chinese
	"2 ����", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_barcode_statistic_option_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// �Ƿ���Ҫ��������ͳ�ƹ���ģʽ�˵� dialog struct
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
// �Ƿ���Ҫ����ȷ��ģʽ�˵� dialog content
const unsigned char *content_transfer_confirm_option_1[] = { // English
	"Transfer Confirm", // english
	"     Select      Esc", // english
	"1 Disbale", // english
	"2 Enable ", // english
	(unsigned char*)0,
};
const unsigned char *content_transfer_confirm_option_2[] = { // Simplified Chinese
	"����ȷ��", // S chinese
	"        ѡ��    �˳�", // S chinese
	"1 �ر�", // S chinese
	"2 ����", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_transfer_confirm_option_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// �Ƿ���Ҫ����ȷ��ģʽ�˵� dialog struct
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
// �������ò˵� dialog content
const unsigned char *content_language_option_1[] = { // English
	"Language", // english
	"      Select      Esc", // english
	"1 English", // english
	"2 Simply Chinese", // english
	(unsigned char*)0,
};
const unsigned char *content_language_option_2[] = { // Simplified Chinese
	"����", // S chinese
	"        ѡ��     �˳�", // S chinese
	"1 Ӣ��", // S chinese
	"2 ��������", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_language_option_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// �������ò˵� dialog struct
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
// ���������ò˵� dialog content
const unsigned char *content_beeper_vol_option_1[] = { // English
	"Key Beeper", // english
	"      Select      Esc", // english
	"1 Disable", // english
	"2 Enable", // english
	(unsigned char*)0,
};
const unsigned char *content_beeper_vol_option_2[] = { // Simplified Chinese
	"������", // S chinese
	"        ѡ��     �˳�", // S chinese
	"1 �ر�", // S chinese
	"2 ����", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_beeper_vol_option_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// ���������ò˵� dialog struct
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
// ��ʱ���ò˵� dialog content
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
	"��ʱ����", // S chinese
	"        ѡ��     �˳�", // S chinese
	"1 �ر�", // S chinese
	"2 30��", // S chinese
	"3 60��", // S chinese
	"4 2����", // S chinese
	"5 5����", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_timeout_option_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// ��ʱ���ò˵� dialog struct
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
// ��������ģʽѡ�� dialog content
const unsigned char *content_transfer_bluetooth_option_1[] = { // English
	"BlueTooth", // english
	"      Select      Esc", // english
	"1 Offline Disable ", // english
	"2 Offline Enable ", // english
	(unsigned char*)0,
};
const unsigned char *content_transfer_bluetooth_option_2[] = { // Simplified Chinese
	"����ģʽ", // S chinese
	"        ѡ��     �˳�", // S chinese
	"1 �ѻ��洢�ر�", // S chinese
	"2 �ѻ��洢����", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_transfer_bluetooth_option_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// ��������ģʽѡ�� dialog struct
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
// U�̴���ģʽѡ�� dialog content
const unsigned char *content_transfer_u_disk_option_1[] = { // English
	"U Disk", // english
	"      Select      Esc", // english
	"1 Create new file", // english
	"2 File list", // english
	"3 Format", // english
	(unsigned char*)0,
};
const unsigned char *content_transfer_u_disk_option_2[] = { // Simplified Chinese
	"U��", // S chinese
	"        ѡ��     �˳�", // S chinese
	"1 �������ļ�", // S chinese
	"2 �ļ��б�", // S chinese
	"3 ��ʽ��", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_transfer_u_disk_option_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// U�̴���ģʽѡ�� dialog struct
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
// �ѻ��洢����ģʽѡ�� dialog content
const unsigned char *content_transfer_batch_option_1[] = { // English
	"Offline Data", // english
	"      Select      Esc", // english
	"1 Send offline data", // english
	"2 Interface", // english
	"3 Batch Data Delete", // english
	(unsigned char*)0,
};
const unsigned char *content_transfer_batch_option_2[] = { // Simplified Chinese
	"�ѻ����ݴ���", // S chinese
	"        ѡ��     �˳�", // S chinese
	"1 �����ѻ�����", // S chinese
	"2 ͨ�Žӿ�", // S chinese
	"3 ����ѻ�����", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_transfer_batch_option_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// �ѻ��洢����ģʽѡ�� dialog struct
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
// ������Ʒ��Ϣ dialog content
const unsigned char *content_import_database_1[] = { // English
	"Import Database", // english
	"                  Esc", // english
	(unsigned char*)0,
};
const unsigned char *content_import_database_2[] = { // Simplified Chinese
	"�������ݿ�", // S chinese
	"                 �˳�", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_import_database_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// ������Ʒ��Ϣ dialog struct
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
// �������� dialog content
const unsigned char *content_barcode_option_1[] = { // English
	"Barcode Setting", // english
	"       Select     Esc", // english
	"1 Decoder", // english
	"2 Invalid offset", // english
	(unsigned char*)0,
};
const unsigned char *content_barcode_option_2[] = { // Simplified Chinese
	"��������", // S chinese
	"       ѡ��      �˳�", // S chinese
	"1 ������", // S chinese
	"2 ��Чλ����", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_barcode_option_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// �������� dialog struct
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
// ʱ������ dialog content
const unsigned char *content_time_date_setting_1[] = { // English
	"Time/Date", // english
	"Input             Esc", // english
	"Set Date/Time:", // english
	" ", // english
	"(YYYYMMDDHHMMSS)", // english
	(unsigned char*)0,
};
const unsigned char *content_time_date_setting_2[] = { // Simplified Chinese
	"ʱ������", // S chinese
	"����             �˳�", // S chinese
	"��������/ʱ��:", // S chinese
	" ", // S chinese
	"(YYYYMMDDHHMMSS)", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_time_date_setting_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// ʱ������ editor
const TEditRes editres_time_date_setting = {
	0, // x
	26, // y
	EDIT_NUM | EDIT_UNDERLINE, // attrib
	14, // display max length
	14, // input max length
	0,
	(TEditRes*)0, // no next edit
};
// ʱ������ dialog struct
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
// �ָ�Ĭ������ dialog content
const unsigned char *content_default_setting_1[] = { // English
	"Default Setting", // english
	"Cancle          Enter", // english
	(unsigned char*)0,
};
const unsigned char *content_default_setting_2[] = { // Simplified Chinese
	"�ָ�Ĭ������", // S chinese
	"ȡ��             ȷ��", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_default_setting_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// �ָ�Ĭ������ dialog struct
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
// ����ϵͳ���� dialog content
const unsigned char *content_set_system_password_1[] = { // English
	"System Password", // english
	"                  Esc", // english
	(unsigned char*)0,
};
const unsigned char *content_set_system_password_2[] = { // Simplified Chinese
	"����ϵͳ����", // S chinese
	"                 �˳�", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_set_system_password_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// ����ϵͳ���� dialog struct
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
// ���������� dialog content
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
	"����������", // S chinese
	"       ��/��     �˳�", // S chinese
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
// ���������� dialog struct
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
// ��Чλ������ dialog content
const unsigned char *content_invalid_offset_setting_1[] = { // English
	"Invalid Offset", // english
	"Input            Esc", // english
	"Input BeginOff:", // english
	"Input End Off:", // english
	(unsigned char*)0,
};
const unsigned char *content_invalid_offset_setting_2[] = { // Simplified Chinese
	"��Чλ��", // S chinese
	"����             �˳�", // S chinese
	"������ʼ��Чλ:", // S chinese
	"���������Чλ:", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_invalid_offset_setting_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// ��Чλ������ editor1
const TEditRes editres_invalid_offset_setting1 = {
	90, // x
	26, // y
	EDIT_NUM, // attrib
	2, // display max length
	2, // input max length
	0,
	(TEditRes*)0, // no next edit
};
// ��Чλ������ editor
const TEditRes editres_invalid_offset_setting = {
	90, // x
	13, // y
	EDIT_NUM, // attrib
	2, // display max length
	2, // input max length
	0,
	(TEditRes*)&editres_invalid_offset_setting1, // next is edit1
};
// ��Чλ������ dialog struct
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
// �ѻ����ݴ��� dialog content
const unsigned char *content_batch_data_send_1[] = { // English
	"Batch Trans", // english
	"                  Esc", // english
	(unsigned char*)0,
};
const unsigned char *content_batch_data_send_2[] = { // Simplified Chinese
	"�ѻ����ݴ���", // S chinese
	"                 ����", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_batch_data_send_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// �ѻ����ݴ��� dialog struct
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
// ���ݴ��� dialog content
const unsigned char *content_batch_transfer_mode_1[] = { // English
	"Batch Trans Mode", // english
	"         Select   Esc", // english
	"1 BlueTooth", // english
	"2 USB Keyboard", // english
	"3 USB Virtual COM", // english
	(unsigned char*)0,
};
const unsigned char *content_batch_transfer_mode_2[] = { // Simplified Chinese
	"�ѻ�����ͨ�Žӿ� ", // S chinese
	"        ѡ��     �˳�", // S chinese
	"1 ����", // S chinese
	"2 USB����", // S chinese
	"3 USB���⴮��", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_batch_transfer_mode_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// ���ݴ��� dialog struct
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
// �ѻ�������� dialog content
const unsigned char *content_batch_data_clear_1[] = { // English
	"Batch clear", // english
	"Cancle          Enter", // english
	(unsigned char*)0,
};
const unsigned char *content_batch_data_clear_2[] = { // Simplified Chinese
	"��¼���", // S chinese
	"ȡ��             ȷ��", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_batch_data_clear_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// �ѻ�������� dialog struct
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
// ���ɨ������ dialog content
const unsigned char *content_clear_scan_record_1[] = { // English
	"Record clear", // english
	"Cancle          Enter", // english
	(unsigned char*)0,
};
const unsigned char *content_clear_scan_record_2[] = { // Simplified Chinese
	"��¼���", // S chinese
	"ȡ��             ȷ��", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_clear_scan_record_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// ���ɨ������ dialog struct
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
// �鿴�ڴ� dialog content
const unsigned char *content_view_memery_1[] = { // English
	"Memory", // english
	"                  Esc", // english
	(unsigned char*)0,
};
const unsigned char *content_view_memery_2[] = { // Simplified Chinese
	"�ڴ���Ϣ", // S chinese
	"                 ����", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_view_memery_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// �鿴�ڴ� dialog struct
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
// �����汾��Ϣ dialog content
const unsigned char *content_bluetooth_info_1[] = { // English
	"About Bluetooth", // english
	"                  Esc", // english
	(unsigned char*)0,
};
const unsigned char *content_bluetooth_info_2[] = { // Simplified Chinese
	"��������", // S chinese
	"                 ����", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_bluetooth_info_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// �����汾��Ϣ dialog struct
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
// �̼��汾 dialog content
const unsigned char *content_firmware_info_1[] = { // English
	"Version", // english
	"                  Esc", // english
	(unsigned char*)0,
};
const unsigned char *content_firmware_info_2[] = { // Simplified Chinese
	"�汾��Ϣ ", // S chinese
	"                 ����", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_firmware_info_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// �̼��汾 dialog struct
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
// �������ļ� dialog content
const unsigned char *content_create_new_file_1[] = { // English
	"New File", // english
	"Input  Creat   Cancle", // english
	"Input file name:", // english
	" ", // english
	"Only Creat Txt File", // english
	(unsigned char*)0,
};
const unsigned char *content_create_new_file_2[] = { // Simplified Chinese
	"�����ļ�", // S chinese
	"����    ȷ��     ȡ��", // S chinese
	"�������ļ���:", // S chinese
	" ", // S chinese
	"Ĭ�Ͻ���txt�ļ�", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_create_new_file_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// �������ļ� editor
const TEditRes editres_create_new_file = {
	0, // x
	26, // y
	EDIT_ALPHA, // attrib
	8, // display max length
	8, // input max length
	0,
	(TEditRes*)0, // no next edit
};
// �������ļ� dialog struct
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
// �ļ��б� dialog content
const unsigned char *content_file_list_1[] = { // English
	"File List", // english
	"Options  Select  Esc ", // english
	(unsigned char*)0,
};
const unsigned char *content_file_list_2[] = { // Simplified Chinese
	"�ļ��б�", // S chinese
	"ѡ��    ѡ��     ����", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_file_list_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// �ļ��б� dialog struct
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
// u�̸�ʽ�� dialog content
const unsigned char *content_u_disk_format_1[] = { // English
	"Format", // english
	"Yes                No", // english
	(unsigned char*)0,
};
const unsigned char *content_u_disk_format_2[] = { // Simplified Chinese
	"��ʽ��", // S chinese
	"��                ��", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_u_disk_format_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// u�̸�ʽ�� dialog struct
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
// �ļ�����ѡ�� dialog content
const unsigned char *content_file_options_menu_1[] = { // English
	"Options", // english
	"                 Esc", // english
	"1 Clear file", // english
	"2 Delete file", // english
	(unsigned char*)0,
};
const unsigned char *content_file_options_menu_2[] = { // Simplified Chinese
	"ѡ��", // S chinese
	"                ����", // S chinese
	"1 ����ļ� ", // S chinese
	"2 ɾ���ļ�", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_file_options_menu_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// �ļ�����ѡ�� dialog struct
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
// ����ļ� dialog content
const unsigned char *content_clear_file_ack_1[] = { // English
	"Clear file", // english
	"Yes               No ", // english
	(unsigned char*)0,
};
const unsigned char *content_clear_file_ack_2[] = { // Simplified Chinese
	"����ļ�", // S chinese
	"��                ��", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_clear_file_ack_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// ����ļ� dialog struct
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
// ɾ���ļ� dialog content
const unsigned char *content_delete_file_ack_1[] = { // English
	"Delete file", // english
	"Yes               No ", // english
	(unsigned char*)0,
};
const unsigned char *content_delete_file_ack_2[] = { // Simplified Chinese
	"ɾ���ļ�", // S chinese
	"��                ��", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_delete_file_ack_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// ɾ���ļ� dialog struct
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
// ���Բ˵� dialog content
const unsigned char *content_test_menu_1[] = { // English
	(unsigned char*)0,
};
const unsigned char *content_test_menu_2[] = { // Simplified Chinese
	" ", // S chinese
	"����", // S chinese
	"1.��������", // S chinese
	"2.�ϻ�����", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_test_menu_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// ���Բ˵� dialog struct
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
// �ϻ����Բ˵� dialog content
const unsigned char *content_age_test_menu_1[] = { // English
	(unsigned char*)0,
};
const unsigned char *content_age_test_menu_2[] = { // Simplified Chinese
	" ", // S chinese
	"����", // S chinese
	"1.ɨ���ϻ�����", // S chinese
	"2.�����ϻ�����", // S chinese
	"3.ɨ�贫�����", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_age_test_menu_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// �ϻ����Բ˵� dialog struct
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
// ������Բ˵� dialog content
const unsigned char *content_single_test_menu_1[] = { // English
	(unsigned char*)0,
};
const unsigned char *content_single_test_menu_2[] = { // Simplified Chinese
	" ", // S chinese
	"����", // S chinese
	"1.��ʾ����", // S chinese
	"2.���̲���", // S chinese
	"3.ɨ�����", // S chinese
	"4.��������", // S chinese
	"5.USB����", // S chinese
	"6.RTC����", // S chinese
	"7.��ص�ѹ���", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_single_test_menu_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// ������Բ˵� dialog struct
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
// ������Ʋ��� dialog content
const unsigned char *content_test_lcd_display_1[] = { // English
	(unsigned char*)0,
};
const unsigned char *content_test_lcd_display_2[] = { // Simplified Chinese
	" ", // S chinese
	"                ȷ��", // S chinese
	"����SCAN��֮�󽫿�ʼ", // S chinese
	"     ����ʾ���", // S chinese
	"                    ", // S chinese
	"�ٴΰ���SCAN��������", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_test_lcd_display_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// ������Ʋ��� dialog struct
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
// �������� dialog content
const unsigned char *content_test_keypad_1[] = { // English
	(unsigned char*)0,
};
const unsigned char *content_test_keypad_2[] = { // Simplified Chinese
	" ", // S chinese
	"                ȷ��", // S chinese
	"������Ӧ�ļ�����Ӧ��", // S chinese
	"�����ͻ�����Ļ����ʧ", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_test_keypad_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// �������� dialog struct
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
// ɨ��ͷ���� dialog content
const unsigned char *content_test_scaner_init_1[] = { // English
	(unsigned char*)0,
};
const unsigned char *content_test_scaner_init_2[] = { // Simplified Chinese
	" ", // S chinese
	"                ȷ��", // S chinese
	"����SCAN������ɨ��ͷ", // S chinese
	" ", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_test_scaner_init_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// ɨ��ͷ���� dialog struct
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
// usb���Ӳ��� dialog content
const unsigned char *content_test_usb_1[] = { // English
	(unsigned char*)0,
};
const unsigned char *content_test_usb_2[] = { // Simplified Chinese
	" ", // S chinese
	"                ȷ��", // S chinese
	"����SCAN������USB", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_test_usb_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// usb���Ӳ��� dialog struct
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
// ����RTC���� dialog content
const unsigned char *content_test_rtc_1[] = { // English
	(unsigned char*)0,
};
const unsigned char *content_test_rtc_2[] = { // Simplified Chinese
	(unsigned char*)0,
};
const unsigned char *content_test_rtc_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// ����RTC���� dialog struct
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
// ����AD������·���� dialog content
const unsigned char *content_test_AD_1[] = { // English
	(unsigned char*)0,
};
const unsigned char *content_test_AD_2[] = { // Simplified Chinese
	(unsigned char*)0,
};
const unsigned char *content_test_AD_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// ����AD������·���� dialog struct
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
// ����ģ����� dialog content
const unsigned char *content_test_bluetooth_1[] = { // English
	(unsigned char*)0,
};
const unsigned char *content_test_bluetooth_2[] = { // Simplified Chinese
	" ", // S chinese
	"                ȷ��", // S chinese
	"����SCAN����������", // S chinese
	"   ", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_test_bluetooth_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// ����ģ����� dialog struct
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
// ������� dialog content
const unsigned char *content_test_complete_1[] = { // English
	(unsigned char*)0,
};
const unsigned char *content_test_complete_2[] = { // Simplified Chinese
	(unsigned char*)0,
};
const unsigned char *content_test_complete_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// ������� dialog struct
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
// ��ʵ�ֽ��� dialog content
const unsigned char *content_todo_1[] = { // English
	"Todo", // english
	"Esc", // english
	(unsigned char*)0,
};
const unsigned char *content_todo_2[] = { // Simplified Chinese
	"��ʵ�� ", // S chinese
	"����", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_todo_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// ��ʵ�ֽ��� dialog struct
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
	// ������
	{ DLG_logo, (TDialogRes*)&dlgres_logo, (DialogCallback)dlgproc_logo, (TJumpList*)&jump_logo[0], "logo" }, // "������"
	// ɨ�蹦��
	{ DLG_scan_func, (TDialogRes*)&dlgres_scan_func, (DialogCallback)dlgproc_scan_func, (TJumpList*)&jump_scan_func[0], "scan_func" }, // "ɨ�蹦��"
	// ���ݴ���
	{ DLG_data_transfer_func, (TDialogRes*)&dlgres_data_transfer_func, (DialogCallback)dlgproc_data_transfer_func, (TJumpList*)&jump_data_transfer_func[0], "data_transfer_func" }, // "���ݴ���"
	// ϵͳ����
	{ DLG_terminal_setting, (TDialogRes*)&dlgres_terminal_setting, (DialogCallback)dlgproc_terminal_setting, (TJumpList*)&jump_terminal_setting[0], "terminal_setting" }, // "ϵͳ����"
	// ϵͳ��Ϣ
	{ DLG_system_info, (TDialogRes*)&dlgres_system_info, (DialogCallback)dlgproc_system_info, (TJumpList*)&jump_system_info[0], "system_info" }, // "ϵͳ��Ϣ"
	// ϵͳ��Ϣ
	{ DLG_about_copyright, (TDialogRes*)&dlgres_about_copyright, (DialogCallback)dlgproc_about_copyright, (TJumpList*)&jump_about_copyright[0], "about_copyright" }, // "ϵͳ��Ϣ"
	// ɨ�蹦�ܵ�ѡ��˵�
	{ DLG_scan_option_menu, (TDialogRes*)&dlgres_scan_option_menu, (DialogCallback)dlgproc_scan_option_menu, (TJumpList*)&jump_scan_option_menu[0], "scan_option_menu" }, // "ɨ�蹦�ܵ�ѡ��˵�"
	// ɨ��ģʽ�˵�
	{ DLG_scan_mode_menu, (TDialogRes*)&dlgres_scan_mode_menu, (DialogCallback)dlgproc_scan_mode_menu, (TJumpList*)&jump_scan_mode_menu[0], "scan_mode_menu" }, // "ɨ��ģʽ�˵�"
	// ����¼��ģʽ�˵�
	{ DLG_quantity_enter_menu, (TDialogRes*)&dlgres_quantity_enter_menu, (DialogCallback)dlgproc_quantity_enter_menu, (TJumpList*)&jump_quantity_enter_menu[0], "quantity_enter_menu" }, // "����¼��ģʽ�˵�"
	// �ֶ���������Ĵ������
	{ DLG_add_time_option, (TDialogRes*)&dlgres_add_time_option, (DialogCallback)dlgproc_add_time_option, (TJumpList*)&jump_add_time_option[0], "add_time_option" }, // "�ֶ���������Ĵ������"
	// �Ƿ���Ҫ�������
	{ DLG_add_date_option, (TDialogRes*)&dlgres_add_date_option, (DialogCallback)dlgproc_add_date_option, (TJumpList*)&jump_add_date_option[0], "add_date_option" }, // "�Ƿ���Ҫ�������"
	// �ֶ���������Ĵ������
	{ DLG_input_barcode, (TDialogRes*)&dlgres_input_barcode, (DialogCallback)dlgproc_input_barcode, (TJumpList*)&jump_input_barcode[0], "input_barcode" }, // "�ֶ���������Ĵ������"
	// �ֶ����뷢�������Ĵ������
	{ DLG_input_send_num, (TDialogRes*)&dlgres_input_send_num, (DialogCallback)dlgproc_input_send_num, (TJumpList*)&jump_input_send_num[0], "input_send_num" }, // "�ֶ����뷢�������Ĵ������"
	// ���÷ָ����Ĵ������
	{ DLG_seperator_setting, (TDialogRes*)&dlgres_seperator_setting, (DialogCallback)dlgproc_seperator_setting, (TJumpList*)&jump_seperator_setting[0], "seperator_setting" }, // "���÷ָ����Ĵ������"
	// ȷ���޸ķ��������Ĵ������
	{ DLG_modify_num_ack, (TDialogRes*)&dlgres_modify_num_ack, (DialogCallback)dlgproc_modify_num_ack, (TJumpList*)&jump_modify_num_ack[0], "modify_num_ack" }, // "ȷ���޸ķ��������Ĵ������"
	// �Ƿ���Ҫ�������ݿ��ѯģʽ�˵�
	{ DLG_database_query_option, (TDialogRes*)&dlgres_database_query_option, (DialogCallback)dlgproc_database_query_option, (TJumpList*)&jump_database_query_option[0], "database_query_option" }, // "�Ƿ���Ҫ�������ݿ��ѯģʽ�˵�"
	// �Ƿ���Ҫ���������빦��ģʽ�˵�
	{ DLG_avoid_dunplication_option, (TDialogRes*)&dlgres_avoid_dunplication_option, (DialogCallback)dlgproc_avoid_dunplication_option, (TJumpList*)&jump_avoid_dunplication_option[0], "avoid_dunplication_option" }, // "�Ƿ���Ҫ���������빦��ģʽ�˵�"
	// �Ƿ���Ҫ��������ͳ�ƹ���ģʽ�˵�
	{ DLG_barcode_statistic_option, (TDialogRes*)&dlgres_barcode_statistic_option, (DialogCallback)dlgproc_barcode_statistic_option, (TJumpList*)&jump_barcode_statistic_option[0], "barcode_statistic_option" }, // "�Ƿ���Ҫ��������ͳ�ƹ���ģʽ�˵�"
	// �Ƿ���Ҫ����ȷ��ģʽ�˵�
	{ DLG_transfer_confirm_option, (TDialogRes*)&dlgres_transfer_confirm_option, (DialogCallback)dlgproc_transfer_confirm_option, (TJumpList*)&jump_transfer_confirm_option[0], "transfer_confirm_option" }, // "�Ƿ���Ҫ����ȷ��ģʽ�˵�"
	// �������ò˵�
	{ DLG_language_option, (TDialogRes*)&dlgres_language_option, (DialogCallback)dlgproc_language_option, (TJumpList*)&jump_language_option[0], "language_option" }, // "�������ò˵�"
	// ���������ò˵�
	{ DLG_beeper_vol_option, (TDialogRes*)&dlgres_beeper_vol_option, (DialogCallback)dlgproc_beeper_vol_option, (TJumpList*)&jump_beeper_vol_option[0], "beeper_vol_option" }, // "���������ò˵�"
	// ��ʱ���ò˵�
	{ DLG_timeout_option, (TDialogRes*)&dlgres_timeout_option, (DialogCallback)dlgproc_timeout_option, (TJumpList*)&jump_timeout_option[0], "timeout_option" }, // "��ʱ���ò˵�"
	// ��������ģʽѡ��
	{ DLG_transfer_bluetooth_option, (TDialogRes*)&dlgres_transfer_bluetooth_option, (DialogCallback)dlgproc_transfer_bluetooth_option, (TJumpList*)&jump_transfer_bluetooth_option[0], "transfer_bluetooth_option" }, // "��������ģʽѡ��"
	// U�̴���ģʽѡ��
	{ DLG_transfer_u_disk_option, (TDialogRes*)&dlgres_transfer_u_disk_option, (DialogCallback)dlgproc_transfer_u_disk_option, (TJumpList*)&jump_transfer_u_disk_option[0], "transfer_u_disk_option" }, // "U�̴���ģʽѡ��"
	// �ѻ��洢����ģʽѡ��
	{ DLG_transfer_batch_option, (TDialogRes*)&dlgres_transfer_batch_option, (DialogCallback)dlgproc_transfer_batch_option, (TJumpList*)&jump_transfer_batch_option[0], "transfer_batch_option" }, // "�ѻ��洢����ģʽѡ��"
	// ������Ʒ��Ϣ
	{ DLG_import_database, (TDialogRes*)&dlgres_import_database, (DialogCallback)dlgproc_import_database, (TJumpList*)&jump_import_database[0], "import_database" }, // "������Ʒ��Ϣ"
	// ��������
	{ DLG_barcode_option, (TDialogRes*)&dlgres_barcode_option, (DialogCallback)dlgproc_barcode_option, (TJumpList*)&jump_barcode_option[0], "barcode_option" }, // "��������"
	// ʱ������
	{ DLG_time_date_setting, (TDialogRes*)&dlgres_time_date_setting, (DialogCallback)dlgproc_time_date_setting, (TJumpList*)&jump_time_date_setting[0], "time_date_setting" }, // "ʱ������"
	// �ָ�Ĭ������
	{ DLG_default_setting, (TDialogRes*)&dlgres_default_setting, (DialogCallback)dlgproc_default_setting, (TJumpList*)&jump_default_setting[0], "default_setting" }, // "�ָ�Ĭ������"
	// ����ϵͳ����
	{ DLG_set_system_password, (TDialogRes*)&dlgres_set_system_password, (DialogCallback)dlgproc_set_system_password, (TJumpList*)&jump_set_system_password[0], "set_system_password" }, // "����ϵͳ����"
	// ����������
	{ DLG_decoder_setting, (TDialogRes*)&dlgres_decoder_setting, (DialogCallback)dlgproc_decoder_setting, (TJumpList*)&jump_decoder_setting[0], "decoder_setting" }, // "����������"
	// ��Чλ������
	{ DLG_invalid_offset_setting, (TDialogRes*)&dlgres_invalid_offset_setting, (DialogCallback)dlgproc_invalid_offset_setting, (TJumpList*)&jump_invalid_offset_setting[0], "invalid_offset_setting" }, // "��Чλ������"
	// �ѻ����ݴ���
	{ DLG_batch_data_send, (TDialogRes*)&dlgres_batch_data_send, (DialogCallback)dlgproc_batch_data_send, (TJumpList*)&jump_batch_data_send[0], "batch_data_send" }, // "�ѻ����ݴ���"
	// ���ݴ���
	{ DLG_batch_transfer_mode, (TDialogRes*)&dlgres_batch_transfer_mode, (DialogCallback)dlgproc_batch_transfer_mode, (TJumpList*)&jump_batch_transfer_mode[0], "batch_transfer_mode" }, // "���ݴ���"
	// �ѻ��������
	{ DLG_batch_data_clear, (TDialogRes*)&dlgres_batch_data_clear, (DialogCallback)dlgproc_batch_data_clear, (TJumpList*)&jump_batch_data_clear[0], "batch_data_clear" }, // "�ѻ��������"
	// ���ɨ������
	{ DLG_clear_scan_record, (TDialogRes*)&dlgres_clear_scan_record, (DialogCallback)dlgproc_clear_scan_record, (TJumpList*)&jump_clear_scan_record[0], "clear_scan_record" }, // "���ɨ������"
	// �鿴�ڴ�
	{ DLG_view_memery, (TDialogRes*)&dlgres_view_memery, (DialogCallback)dlgproc_view_memery, (TJumpList*)&jump_view_memery[0], "view_memery" }, // "�鿴�ڴ�"
	// �����汾��Ϣ
	{ DLG_bluetooth_info, (TDialogRes*)&dlgres_bluetooth_info, (DialogCallback)dlgproc_bluetooth_info, (TJumpList*)&jump_bluetooth_info[0], "bluetooth_info" }, // "�����汾��Ϣ"
	// �̼��汾
	{ DLG_firmware_info, (TDialogRes*)&dlgres_firmware_info, (DialogCallback)dlgproc_firmware_info, (TJumpList*)&jump_firmware_info[0], "firmware_info" }, // "�̼��汾"
	// �������ļ�
	{ DLG_create_new_file, (TDialogRes*)&dlgres_create_new_file, (DialogCallback)dlgproc_create_new_file, (TJumpList*)&jump_create_new_file[0], "create_new_file" }, // "�������ļ�"
	// �ļ��б�
	{ DLG_file_list, (TDialogRes*)&dlgres_file_list, (DialogCallback)dlgproc_file_list, (TJumpList*)&jump_file_list[0], "file_list" }, // "�ļ��б�"
	// u�̸�ʽ��
	{ DLG_u_disk_format, (TDialogRes*)&dlgres_u_disk_format, (DialogCallback)dlgproc_u_disk_format, (TJumpList*)&jump_u_disk_format[0], "u_disk_format" }, // "u�̸�ʽ��"
	// �ļ�����ѡ��
	{ DLG_file_options_menu, (TDialogRes*)&dlgres_file_options_menu, (DialogCallback)dlgproc_file_options_menu, (TJumpList*)&jump_file_options_menu[0], "file_options_menu" }, // "�ļ�����ѡ��"
	// ����ļ�
	{ DLG_clear_file_ack, (TDialogRes*)&dlgres_clear_file_ack, (DialogCallback)dlgproc_clear_file_ack, (TJumpList*)&jump_clear_file_ack[0], "clear_file_ack" }, // "����ļ�"
	// ɾ���ļ�
	{ DLG_delete_file_ack, (TDialogRes*)&dlgres_delete_file_ack, (DialogCallback)dlgproc_delete_file_ack, (TJumpList*)&jump_delete_file_ack[0], "delete_file_ack" }, // "ɾ���ļ�"
	// ���Բ˵�
	{ DLG_test_menu, (TDialogRes*)&dlgres_test_menu, (DialogCallback)dlgproc_test_menu, (TJumpList*)&jump_test_menu[0], "test_menu" }, // "���Բ˵�"
	// �ϻ����Բ˵�
	{ DLG_age_test_menu, (TDialogRes*)&dlgres_age_test_menu, (DialogCallback)dlgproc_age_test_menu, (TJumpList*)&jump_age_test_menu[0], "age_test_menu" }, // "�ϻ����Բ˵�"
	// ������Բ˵�
	{ DLG_single_test_menu, (TDialogRes*)&dlgres_single_test_menu, (DialogCallback)dlgproc_single_test_menu, (TJumpList*)&jump_single_test_menu[0], "single_test_menu" }, // "������Բ˵�"
	// ������Ʋ���
	{ DLG_test_lcd_display, (TDialogRes*)&dlgres_test_lcd_display, (DialogCallback)dlgproc_test_lcd_display, (TJumpList*)&jump_test_lcd_display[0], "test_lcd_display" }, // "������Ʋ���"
	// ��������
	{ DLG_test_keypad, (TDialogRes*)&dlgres_test_keypad, (DialogCallback)dlgproc_test_keypad, (TJumpList*)&jump_test_keypad[0], "test_keypad" }, // "��������"
	// ɨ��ͷ����
	{ DLG_test_scaner_init, (TDialogRes*)&dlgres_test_scaner_init, (DialogCallback)dlgproc_test_scaner_init, (TJumpList*)&jump_test_scaner_init[0], "test_scaner_init" }, // "ɨ��ͷ����"
	// usb���Ӳ���
	{ DLG_test_usb, (TDialogRes*)&dlgres_test_usb, (DialogCallback)dlgproc_test_usb, (TJumpList*)&jump_test_usb[0], "test_usb" }, // "usb���Ӳ���"
	// ����RTC����
	{ DLG_test_rtc, (TDialogRes*)&dlgres_test_rtc, (DialogCallback)dlgproc_test_rtc, (TJumpList*)&jump_test_rtc[0], "test_rtc" }, // "����RTC����"
	// ����AD������·����
	{ DLG_test_AD, (TDialogRes*)&dlgres_test_AD, (DialogCallback)dlgproc_test_AD, (TJumpList*)&jump_test_AD[0], "test_AD" }, // "����AD������·����"
	// ����ģ�����
	{ DLG_test_bluetooth, (TDialogRes*)&dlgres_test_bluetooth, (DialogCallback)dlgproc_test_bluetooth, (TJumpList*)&jump_test_bluetooth[0], "test_bluetooth" }, // "����ģ�����"
	// �������
	{ DLG_test_complete, (TDialogRes*)&dlgres_test_complete, (DialogCallback)dlgproc_test_complete, (TJumpList*)&jump_test_complete[0], "test_complete" }, // "�������"
	// ��ʵ�ֽ���
	{ DLG_todo, (TDialogRes*)&dlgres_todo, (DialogCallback)0, (TJumpList*)&jump_todo[0], "todo" }, // "��ʵ�ֽ���"
};
