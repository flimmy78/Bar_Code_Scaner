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
#define DLG_scan_func	1
#define DLG_terminal_setting	(DLG_scan_func+1)
#define DLG_system_info	(DLG_terminal_setting+1)
#define DLG_about_copyright	(DLG_system_info+1)
#define DLG_scan_option_menu	(DLG_about_copyright+1)
#define DLG_basical_barcode_setting	(DLG_scan_option_menu+1)
#define DLG_input_barcode_ext	(DLG_basical_barcode_setting+1)
#define DLG_input_barcode	(DLG_input_barcode_ext+1)
#define DLG_language_option	(DLG_input_barcode+1)
#define DLG_beeper_vol_option	(DLG_language_option+1)
#define DLG_timeout_option	(DLG_beeper_vol_option+1)
#define DLG_time_date_setting	(DLG_timeout_option+1)
#define DLG_default_setting	(DLG_time_date_setting+1)
#define DLG_view_memery	(DLG_default_setting+1)
#define DLG_fw_update	(DLG_view_memery+1)
#define DLG_firmware_info	(DLG_fw_update+1)
#define DLG_test_menu	(DLG_firmware_info+1)
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
#define DLG_MAX	28



// function prototypes
unsigned char dlgproc_scan_func(unsigned char type,unsigned char *key);
unsigned char dlgproc_terminal_setting(unsigned char type,unsigned char *key);
unsigned char dlgproc_system_info(unsigned char type,unsigned char *key);
unsigned char dlgproc_about_copyright(unsigned char type,unsigned char *key);
unsigned char dlgproc_scan_option_menu(unsigned char type,unsigned char *key);
unsigned char dlgproc_basical_barcode_setting(unsigned char type,unsigned char *key);
unsigned char dlgproc_input_barcode_ext(unsigned char type,unsigned char *key);
unsigned char dlgproc_input_barcode(unsigned char type,unsigned char *key);
unsigned char dlgproc_language_option(unsigned char type,unsigned char *key);
unsigned char dlgproc_beeper_vol_option(unsigned char type,unsigned char *key);
unsigned char dlgproc_timeout_option(unsigned char type,unsigned char *key);
unsigned char dlgproc_time_date_setting(unsigned char type,unsigned char *key);
unsigned char dlgproc_default_setting(unsigned char type,unsigned char *key);
unsigned char dlgproc_view_memery(unsigned char type,unsigned char *key);
unsigned char dlgproc_fw_update(unsigned char type,unsigned char *key);
unsigned char dlgproc_firmware_info(unsigned char type,unsigned char *key);
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
// ɨ�蹦�� dialog content
const unsigned char *content_scan_func_1[] = { // English
	" ", // english
	"Options SCAN", // english
	(unsigned char*)0,
};
const unsigned char *content_scan_func_2[] = { // Simplified Chinese
	"  ", // S chinese
	"ѡ��    SCAN", // S chinese
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
	{KEY_LEFT_SHOTCUT,	DLG_scan_option_menu	},
	{KEY_EXT_1,	DLG_basical_barcode_setting	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// ϵͳ���� dialog content
const unsigned char *content_terminal_setting_1[] = { // English
	"Terminal Setting", // english
	"       Select     Esc", // english
	"1 Language", // english
	"2 Key Volume", // english
	"3 Timeout Setting", // english
	"4 DateTime", // english
	"5 Default setting", // english
	(unsigned char*)0,
};
const unsigned char *content_terminal_setting_2[] = { // Simplified Chinese
	"ϵͳ���� ", // S chinese
	"        ѡ��     �˳�", // S chinese
	"1 ����", // S chinese
	"2 ������", // S chinese
	"3 ��ʱ����", // S chinese
	"4 ʱ�������", // S chinese
	"5 �ָ�Ĭ������", // S chinese
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
	{KEY_ESC,	DLG_scan_option_menu	},
	{KEY_EXT_1,	DLG_language_option	},
	{KEY_EXT_2,	DLG_beeper_vol_option	},
	{KEY_EXT_3,	DLG_timeout_option	},
	{KEY_EXT_4,	DLG_time_date_setting	},
	{KEY_EXT_5,	DLG_default_setting	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// ϵͳ��Ϣ dialog content
const unsigned char *content_system_info_1[] = { // English
	"Info", // english
	"     Select       Esc", // english
	"1 Memery Info", // english
	"2 Firmware Version", // english
	(unsigned char*)0,
};
const unsigned char *content_system_info_2[] = { // Simplified Chinese
	"ϵͳ��Ϣ ", // S chinese
	"        ѡ��     �˳�", // S chinese
	"1 �鿴�ڴ�", // S chinese
	"2 �汾��Ϣ", // S chinese
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
	{KEY_ESC,	DLG_scan_option_menu	},
	{KEY_EXT_1,	DLG_view_memery	},
	{KEY_EXT_2,	DLG_firmware_info	},
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
	"1 Basical barcode ", // english
	"2 system setting", // english
	"3 system info", // english
	(unsigned char*)0,
};
const unsigned char *content_scan_option_menu_2[] = { // Simplified Chinese
	"  ", // S chinese
	"        ѡ��     �˳�", // S chinese
	"1 �ֶ���������", // S chinese
	"2 ���ò�������", // S chinese
	"3 ϵͳ����", // S chinese
	"4 ϵͳ��Ϣ", // S chinese
	"5 �̼�����", // S chinese
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
	{KEY_EXT_1,	DLG_input_barcode_ext	},
	{KEY_EXT_2,	DLG_basical_barcode_setting	},
	{KEY_EXT_3,	DLG_terminal_setting	},
	{KEY_EXT_4,	DLG_system_info	},
	{KEY_EXT_5,	DLG_fw_update	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// ���ò������� dialog content
const unsigned char *content_basical_barcode_setting_1[] = { // English
	"Basical ", // english
	"Input    SCAN", // english
	(unsigned char*)0,
};
const unsigned char *content_basical_barcode_setting_2[] = { // Simplified Chinese
	"��������  ", // S chinese
	"�ֶ�����  SCAN", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_basical_barcode_setting_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// ���ò������� dialog struct
const TDialogRes dlgres_basical_barcode_setting = {
	DLG_basical_barcode_setting, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_basical_barcode_setting_1, // text content english
	(unsigned char**)content_basical_barcode_setting_2, // text content chinese S 
	(unsigned char**)content_basical_barcode_setting_3, // text content chinese T 
};
// basical_barcode_setting jump list
const TJumpList jump_basical_barcode_setting[] = {
	{KEY_ESC,	DLG_scan_option_menu	},
	{KEY_LEFT_SHOTCUT,	DLG_input_barcode	},
	{0, 0},
};
// ---------------------------------------------------------------------------------------------
// �ֶ���������Ĵ������ dialog content
const unsigned char *content_input_barcode_ext_1[] = { // English
	(unsigned char*)0,
};
const unsigned char *content_input_barcode_ext_2[] = { // Simplified Chinese
	(unsigned char*)0,
};
const unsigned char *content_input_barcode_ext_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// �ֶ���������Ĵ������ editor
const TEditRes editres_input_barcode_ext = {
	0, // x
	0, // y
	EDIT_ALPHA, // attrib
	20, // display max length
	30, // input max length
	0,
	(TEditRes*)0, // no next edit
};
// �ֶ���������Ĵ������ dialog struct
const TDialogRes dlgres_input_barcode_ext = {
	DLG_input_barcode_ext, // self id 
	STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)&editres_input_barcode_ext, // editor for this dialog
	(unsigned char**)content_input_barcode_ext_1, // text content english
	(unsigned char**)content_input_barcode_ext_2, // text content chinese S 
	(unsigned char**)content_input_barcode_ext_3, // text content chinese T 
};
// input_barcode_ext jump list
const TJumpList jump_input_barcode_ext[] = {
	{KEY_ESC,	DLG_scan_func	},
	{KEY_EXT_1,	DLG_scan_func	},
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
	{KEY_ESC,	DLG_basical_barcode_setting	},
	{KEY_EXT_1,	DLG_basical_barcode_setting	},
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
// �̼����� dialog content
const unsigned char *content_fw_update_1[] = { // English
	"Update", // english
	"                  Esc", // english
	(unsigned char*)0,
};
const unsigned char *content_fw_update_2[] = { // Simplified Chinese
	"�̼����� ", // S chinese
	"                 ����", // S chinese
	(unsigned char*)0,
};
const unsigned char *content_fw_update_3[] = { // Traditional Chinese 
	(unsigned char*)0,
};
// �̼����� dialog struct
const TDialogRes dlgres_fw_update = {
	DLG_fw_update, // self id 
	STYLE_TITLE | STYLE_SHORTCUT, // style 
	0, // dialog exit by manual
	(TEditRes*)0, // no editor in this dialog
	(unsigned char**)content_fw_update_1, // text content english
	(unsigned char**)content_fw_update_2, // text content chinese S 
	(unsigned char**)content_fw_update_3, // text content chinese T 
};
// fw_update jump list
const TJumpList jump_fw_update[] = {
	{KEY_ESC,	DLG_scan_option_menu	},
	{KEY_RIGHT_SHOTCUT,	DLG_scan_option_menu	},
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
	{KEY_ESC,	DLG_scan_option_menu	},
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
	{KEY_ESC,	DLG_scan_option_menu	},
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
	{KEY_ESC,	DLG_scan_option_menu	},
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
	{KEY_ENTER,	DLG_scan_option_menu	},
	{0, 0},
};
// --------------------------------------------------
// dialog action list
const TActionList dlg_actionlist[] ={
	// ɨ�蹦��
	{ DLG_scan_func, (TDialogRes*)&dlgres_scan_func, (DialogCallback)dlgproc_scan_func, (TJumpList*)&jump_scan_func[0], "scan_func" }, // "ɨ�蹦��"
	// ϵͳ����
	{ DLG_terminal_setting, (TDialogRes*)&dlgres_terminal_setting, (DialogCallback)dlgproc_terminal_setting, (TJumpList*)&jump_terminal_setting[0], "terminal_setting" }, // "ϵͳ����"
	// ϵͳ��Ϣ
	{ DLG_system_info, (TDialogRes*)&dlgres_system_info, (DialogCallback)dlgproc_system_info, (TJumpList*)&jump_system_info[0], "system_info" }, // "ϵͳ��Ϣ"
	// ϵͳ��Ϣ
	{ DLG_about_copyright, (TDialogRes*)&dlgres_about_copyright, (DialogCallback)dlgproc_about_copyright, (TJumpList*)&jump_about_copyright[0], "about_copyright" }, // "ϵͳ��Ϣ"
	// ɨ�蹦�ܵ�ѡ��˵�
	{ DLG_scan_option_menu, (TDialogRes*)&dlgres_scan_option_menu, (DialogCallback)dlgproc_scan_option_menu, (TJumpList*)&jump_scan_option_menu[0], "scan_option_menu" }, // "ɨ�蹦�ܵ�ѡ��˵�"
	// ���ò�������
	{ DLG_basical_barcode_setting, (TDialogRes*)&dlgres_basical_barcode_setting, (DialogCallback)dlgproc_basical_barcode_setting, (TJumpList*)&jump_basical_barcode_setting[0], "basical_barcode_setting" }, // "���ò�������"
	// �ֶ���������Ĵ������
	{ DLG_input_barcode_ext, (TDialogRes*)&dlgres_input_barcode_ext, (DialogCallback)dlgproc_input_barcode_ext, (TJumpList*)&jump_input_barcode_ext[0], "input_barcode_ext" }, // "�ֶ���������Ĵ������"
	// �ֶ���������Ĵ������
	{ DLG_input_barcode, (TDialogRes*)&dlgres_input_barcode, (DialogCallback)dlgproc_input_barcode, (TJumpList*)&jump_input_barcode[0], "input_barcode" }, // "�ֶ���������Ĵ������"
	// �������ò˵�
	{ DLG_language_option, (TDialogRes*)&dlgres_language_option, (DialogCallback)dlgproc_language_option, (TJumpList*)&jump_language_option[0], "language_option" }, // "�������ò˵�"
	// ���������ò˵�
	{ DLG_beeper_vol_option, (TDialogRes*)&dlgres_beeper_vol_option, (DialogCallback)dlgproc_beeper_vol_option, (TJumpList*)&jump_beeper_vol_option[0], "beeper_vol_option" }, // "���������ò˵�"
	// ��ʱ���ò˵�
	{ DLG_timeout_option, (TDialogRes*)&dlgres_timeout_option, (DialogCallback)dlgproc_timeout_option, (TJumpList*)&jump_timeout_option[0], "timeout_option" }, // "��ʱ���ò˵�"
	// ʱ������
	{ DLG_time_date_setting, (TDialogRes*)&dlgres_time_date_setting, (DialogCallback)dlgproc_time_date_setting, (TJumpList*)&jump_time_date_setting[0], "time_date_setting" }, // "ʱ������"
	// �ָ�Ĭ������
	{ DLG_default_setting, (TDialogRes*)&dlgres_default_setting, (DialogCallback)dlgproc_default_setting, (TJumpList*)&jump_default_setting[0], "default_setting" }, // "�ָ�Ĭ������"
	// �鿴�ڴ�
	{ DLG_view_memery, (TDialogRes*)&dlgres_view_memery, (DialogCallback)dlgproc_view_memery, (TJumpList*)&jump_view_memery[0], "view_memery" }, // "�鿴�ڴ�"
	// �̼�����
	{ DLG_fw_update, (TDialogRes*)&dlgres_fw_update, (DialogCallback)dlgproc_fw_update, (TJumpList*)&jump_fw_update[0], "fw_update" }, // "�̼�����"
	// �̼��汾
	{ DLG_firmware_info, (TDialogRes*)&dlgres_firmware_info, (DialogCallback)dlgproc_firmware_info, (TJumpList*)&jump_firmware_info[0], "firmware_info" }, // "�̼��汾"
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
