#pragma once
#include "connection_win.hpp"
#include "../../connection/tcp_connection.hpp"

namespace gui
{
    class TcpConnectionWin : public ConnectionWin
    {
    public:
        explicit TcpConnectionWin(const std::shared_ptr<connection::Tcp> &connection, DeviceConnection &device_connection);

    private:
        void drawConnectionOverview() override;
        void drawConnectionRawInput() override;
        void drawConnectionDetails() override;
        void drawConnectionSettings() override;

    private:
        std::shared_ptr<connection::Tcp> connection_;
        connection::TcpOptions options_;
        std::string tx_buffer_;
        bool append_new_line_;
    };
} // namespace gui
