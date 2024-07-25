Chương trình chạy trên hệ điều hành Windows, sử dụng thư viện winsock2.h
Tài liệu tham khảo: https://learn.microsoft.com/en-us/windows/win32/winsock/finished-server-and-client-code

Trao đổi dữ liệu giữa 2 máy A và B, máy A gửi dữ liệu, máy B nhận dữ liệu.

B1:
Gọi ipconfig để lấy địa chỉ của máy B

B2: Trên máy B
Gọi:
main.exe ReceiveData –out <location_store_file>
hoặc
main.exe ReceiveData
Khi này <location_store_file> là chính thư mục chứa main.exe "./"

Lưu ý:
 Có thể nhập đường dẫn tới tệp có hoặc không có ngằm trong cặp dấu ngoặc kép.
 Nếu đường dẫn có dấu cách thì phải đặt trong cặp dấu ngoặc kép.
 Đường dẫn có thể chưa tồn tại trước đó vì chương trình có thể khởi tạo.

vd: main.exe ReceiveData –out "luu chu"
hoặc: main.exe ReceiveData –out luuchu

B3: Trên máy A
Gọi:
main.exe SendData <destination_address>
<destination_address> là địa chỉ ip của máy B được lấy từ B1
Nếu thành công sẽ có dòng thông báo "Is conneting <destination_address>" ở máy A
và "Is conneting" ở máy B

B4: Trên máy A chọn 1 trong 2 kiểu gửi
Gửi đoạn văn: SendText <text> End
Sau khi gửi xong sẽ hiện "End send text" ở máy A
Lưu ý:
 Cần nhập "End" ở cuối dòng rồi nhấn enter để kết thúc gửi đoạn văn

Gửi file: SendFile <path> <buffer-size>
Sau khi gửi xong sẽ hiện "End send file" ở máy A
Lưu ý:
 Có thể nhập đường dẫn tới file có hoặc không có ngằm trong cặp dấu ngoặc kép.
 Nếu đường dẫn có dấu cách thì phải đặt trong cặp dấu ngoặc kép.
 Đường dẫn có thể chưa tồn tại trước đó vì chương trình có thể khởi tạo.