#include "raw_mem_viewer.hpp"
#include "../imgui_commons.hpp"
namespace gui
{
    RawMemViewer::RawMemViewer()
        : auto_scroll_{true}
    {
        data_.reserve(10000);
        mem_edit_.ReadOnly = true;
    }

    void RawMemViewer::addData(std::span<uint8_t> data)
    {
        data_.insert(data_.end(), data.begin(), data.end());
    }

    void RawMemViewer::draw()
    {
        if (ImGui::Button("Clear"))
        {
            data_.clear();
        }
        ImGui::SameLine();
        ImGui::Checkbox("Autoscroll", &auto_scroll_);
        ImGui::PushFont(gFonts["Monospace"]);
        if (auto_scroll_)
        {
            mem_edit_.GotoAddr = data_.size() == 0 ? 0 : data_.size() - 1;
        }
        mem_edit_.DrawContents(data_.size() == 0 ? nullptr : &data_[0], data_.size());
        ImGui::PopFont();
    }
} // namespace gui
