# TickLabChallenge
* Đồng hồ 3 chức năng
* Main source: "Prj_TickLabChallenge_update.ino"
# Done (updated: 3/12/2022)
 - Normal mode 				                 	
 - Set time mode 				               
 - Alarm mode (set)			               
 - Alarm mode (alarm)			             
 - Count up mode 
 - Count down mode (set)
 - Count down mode (count)	
 - Đếm lên hiển thị sec:msec (00:00 -> 99:99)
 - Tích hợp thêm chuông báo (cho alarm và countDown mode) 
 - Tạch module từng chức năng
 - Hiển thị bằng cách quét led
# Bug
 - Function: dec_2_bin (Fixed)
# Done (updated: 05/01/2023)
 - Chuyển sang ngắt trên Timer
   + Timer 2: Count time + Alarm
   + Timer 1: Button (Capture event Interrupt)
   + Timer 0: Led Sweeping 
 - Thay đổi sơ đồ nguyên lý mạch đồng hồ 3 chức năng (file TickLabchallenge.pdsprj)
# Bug
 - Debounce (Fixed)
 - CountingUp mode chỉ đếm đến 40s (Fixed)
# Updating
 
