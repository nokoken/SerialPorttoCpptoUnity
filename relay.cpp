#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <termios.h>
#define PORT 12345

// シリアルポートを開き初期化
int setup_serial(const std::string &port_path)
{
    int fd = open(port_path.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    // ポート開くのが失敗した時
    if (fd < 0)
    {
        std::cerr << "Failed to open port: " << port_path << std::endl;
        return -1;
    }

    struct termios tty;
    if (tcgetattr(fd, &tty) != 0)
    {
        std::cerr << "Error from tcgetattr" << std::endl;
        close(fd);
        return -1;
    }

    // 通信設定：115200bps, 8N1, rawモード
    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8-bit chars
    tty.c_iflag &= ~IGNBRK;                     // ignore break signal
    tty.c_lflag = 0;                            // no signaling chars, no echo, no canonical
    tty.c_oflag = 0;                            // no remapping, no delays
    tty.c_cc[VMIN] = 1;                         // read blocks until 1 char is available
    tty.c_cc[VTIME] = 1;                        // 0.1 sec read timeout

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl
    tty.c_cflag |= (CLOCAL | CREAD);        // ignore modem controls, enable reading
    tty.c_cflag &= ~(PARENB | PARODD);      // no parity
    tty.c_cflag &= ~CSTOPB;                 // 1 stop bit
    tty.c_cflag &= ~CRTSCTS;                // no hardware flow control

    if (tcsetattr(fd, TCSANOW, &tty) != 0)
    {
        std::cerr << "Error from tcsetattr" << std::endl;
        close(fd);
        return -1;
    }

    return fd;
}

// シリアルポートから送られてきたデータを読み込む
std::string read_serial_line(const std::string &port_path)
{
    int fd = open(port_path.c_str(), O_RDONLY | O_NOCTTY | O_NONBLOCK);
    if (fd < 0)
    {
        std::cerr << "Failed to open serial port!" << std::endl;
        return "";
    }

    // termios設定（非カノニカルモードなども必要だが省略）
    char buf[256];
    int n = read(fd, buf, sizeof(buf) - 1);
    close(fd);

    if (n > 0)
    {
        buf[n] = '\0';
        for (int i = 0; i < n; i++)
            if (buf[i] == char(32))
            {
                buf[i] = 0;
            }
        return std::string(buf);
    }

    return "";

    // std::ifstream serial(port_path);
    // std::string line;
    // if (serial.is_open())
    // {
    //     std::getline(serial, line);
    //     std::cout << "tmp string" << std::endl;
    // }
    // return line;
}

// Unity側へTCP方式で通信を行う
void tcp_server_loop(const std::string &port_path)
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 1);

    std::cout << "Wait for Unity connection..." << std::endl;
    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    std::cout << "Connected to Unity." << std::endl;
    while (true)
    {
        std::string line = read_serial_line(port_path);
        if (!line.empty())
        {
            std::cout << "line = " << line << std::endl;
            send(new_socket, line.c_str(), line.length(), 0);
            send(new_socket, "\n", 1, 0);
        }
        usleep(500000); // 500ms
    }
}

int main()
{
    std::string serial_port = "/dev/tty.usbmodem102";
    setup_serial(serial_port);
    tcp_server_loop(serial_port);
    return 0;
}