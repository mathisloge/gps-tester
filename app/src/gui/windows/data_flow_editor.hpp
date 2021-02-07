#pragma once
#include <vector>
#include <ImNodes.h>
#include "base_window.hpp"
#include "../../protocols/protocol_loader.hpp"
#include "../data_flow/data_flow_graph.hpp"

namespace gui
{
    class DataFlowEditor : public BaseWindow
    {
        static constexpr const char* kDndTarget = "DND_DATAFLOW";
    public:
        explicit DataFlowEditor(const std::string &win_name, protocol::ProtocolLoader &proto_loader);
    private:
        void drawContent() override;
        void drawProtocols();
        void handleDnD();
        void refreshProtocols();

    private:
        protocol::ProtocolLoader &proto_loader_;
        std::vector<std::string> protocol_names_;
        df::DataFlowGraph data_flow_graph_;
    };
} // namespace gui