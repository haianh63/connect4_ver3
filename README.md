# AI Connect 4

## Giới thiệu

AI Connect 4 là một dự án phát triển trí tuệ nhân tạo để chơi trò chơi Connect 4. Dự án sử dụng thuật toán **Minimax** với **cắt tỉa Alpha-Beta** để tìm nước đi tối ưu. Hàm **heuristic** được thiết kế kết hợp nhiều yếu tố đánh giá, bao gồm mối đe dọa (threat) từ các chuỗi 2 hoặc 3 quân liên tiếp và ưu tiên các vị trí trung tâm trên bàn cờ. Để tối ưu hóa tốc độ tính toán, dự án sử dụng **bitboard** để biểu diễn trạng thái bàn cờ và được triển khai bằng **C++**, với giao tiếp tới **Python** thông qua binding để tận dụng tốc độ tính toán của C++ trong môi trường Python.

## Các tính năng chính

- **Thuật toán Minimax với cắt tỉa Alpha-Beta**: Đảm bảo tìm nước đi tối ưu trong thời gian ngắn hơn so với Minimax thông thường.
- **Hàm heuristic thông minh**:
  - Đánh giá dựa trên mối đe dọa từ các chuỗi 2 hoặc 3 quân liên tiếp (threat detection).
  - Ưu tiên các vị trí trung tâm để kiểm soát bàn cờ.
- **Bitboard**: Biểu diễn bàn cờ dưới dạng bit để tối ưu hóa hiệu suất tính toán.
- **Tích hợp C++ và Python**: Các hàm tính toán lõi được viết bằng C++ và gọi từ Python thông qua binding 
