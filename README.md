# Smart_house_mini_iot

### Chủ đề: Smart Home
### Môn học: Thí nghiệm 
### Giảng viên: Bùi Kiên
### Thành viên nhóm:
Nguyễn Minh Đức,
Nguyễn Nam Khánh,
An Đức Mạnh

### I. MỤC TIÊU DỰ ÁN
    Mục tiêu chính của dự án là xây dựng một hệ thống nhà thông minh tích hợp, giúp gia chủ dễ dàng kiểm soát và giám sát tình trạng ngôi nhà từ xa thông qua ứng dụng Telegram. Các chức năng chính bao gồm:
	•	Đo nhiệt độ và độ ẩm trong nhà, hiển thị thông tin qua Telegram.
	•	Cảnh báo cháy và rò rỉ khí gas qua Telegram.
	•	Phát hiện mưa, tự động đóng/mở mái che tùy vào điều kiện thời tiết.
	•	Đóng/mở cửa bằng điện thoại qua Telegram.
	•	Cảnh báo nếu cửa mở quá lâu.

### II. THÀNH PHẦN HỆ THỐNG
	1.	Phần cứng:
	•	ESP32: Vi điều khiển chính.
	•	Cảm biến nhiệt độ và độ ẩm (DHT11 hoặc DHT22).
	•	Cảm biến khí gas (MQ-2).
	•	Cảm biến lửa (Flame sensor).
	•	Cảm biến mưa.
	•	Cảm biến độ ẩm đất (để mở rộng tính năng trong tương lai).
	•	Công tắc từ (Reed Switch) để phát hiện trạng thái cửa.
	•	Động cơ Servo: Điều khiển mái che và cửa.
	•	Relay: Điều khiển các thiết bị như máy bơm nước hoặc còi báo động.
	•	Buzzer: Còi báo động khi phát hiện cháy hoặc khí gas.
	2.	Phần mềm:
	•	Arduino IDE: Viết và nạp chương trình cho ESP32.
	•	Telegram Bot API: Gửi và nhận tin nhắn điều khiển từ điện thoại.

### III. NGUYÊN LÝ HOẠT ĐỘNG
	1.	Đo nhiệt độ và độ ẩm trong nhà:
	•	Cảm biến DHT11/DHT22 được kết nối với ESP32 để đo nhiệt độ và độ ẩm.
	•	Dữ liệu được gửi đến điện thoại qua Telegram khi người dùng yêu cầu bằng lệnh “/temp”.
	2.	Cảnh báo cháy và rò rỉ khí gas:
	•	Cảm biến khí gas MQ-2 phát hiện nồng độ khí độc hại vượt ngưỡng cho phép.
	•	Cảm biến lửa phát hiện ngọn lửa hoặc nhiệt độ cao bất thường.
	•	Khi phát hiện sự cố, ESP32 gửi thông báo cảnh báo qua Telegram và kích hoạt còi báo động.
	3.	Phát hiện mưa và tự động điều chỉnh mái che:
	•	Khi cảm biến mưa phát hiện có mưa, hệ thống sẽ tự động đóng mái che bằng động cơ Servo.
	•	Khi trời nắng, mái che sẽ tự động mở lại.
	•	Trạng thái thời tiết được thông báo qua Telegram.
	4.	Đóng/mở cửa bằng điện thoại:
	•	Người dùng gửi lệnh “/mocua” hoặc “/dongcua” qua Telegram để điều khiển động cơ Servo mở hoặc đóng cửa.
	5.	Cảnh báo cửa mở quá lâu:
	•	Cảm biến công tắc từ phát hiện trạng thái cửa (đóng/mở).
	•	Nếu cửa mở quá lâu (hơn 15 giây), hệ thống gửi cảnh báo qua Telegram.

### IV. KẾT QUẢ THỰC HIỆN
	•	Hệ thống hoạt động ổn định, cho phép giám sát và điều khiển từ xa qua Telegram.
	•	Tự động hóa các chức năng cơ bản giúp tăng cường sự tiện nghi và an toàn cho ngôi nhà.
	•	Các cảnh báo được gửi kịp thời, giảm thiểu rủi ro từ các sự cố như cháy, rò rỉ khí gas hay mưa bất ngờ.

### V. ƯU ĐIỂM VÀ HẠN CHẾ
	1.	Ưu điểm:
	•	Dễ dàng cài đặt và vận hành.
	•	Tích hợp nhiều chức năng trong một hệ thống duy nhất.
	•	Chi phí phần cứng thấp, phù hợp với người dùng cá nhân.
	2.	Hạn chế:
	•	Phụ thuộc vào kết nối WiFi, nếu mất mạng sẽ không hoạt động.
	•	Cần tối ưu hơn về tiêu thụ năng lượng của ESP32 và các cảm biến.

### VI. ĐỊNH HƯỚNG PHÁT TRIỂN
	•	Mở rộng tính năng kiểm soát bằng giọng nói qua trợ lý ảo (Google Assistant, Alexa).
	•	Tích hợp thêm camera để giám sát hình ảnh.
	•	Tăng cường bảo mật thông qua mã hóa giao tiếp giữa ESP32 và Telegram.

### VII. KẾT LUẬN

Dự án “Nhà thông minh” không chỉ mang lại sự tiện lợi trong việc điều khiển và giám sát mà còn nâng cao mức độ an toàn cho ngôi nhà. Với những cải tiến trong tương lai, hệ thống sẽ trở thành một trợ lý đáng tin cậy cho cuộc sống hiện đại.