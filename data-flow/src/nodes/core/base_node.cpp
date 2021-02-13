#include "data-flow/nodes/core/base_node.hpp"
#include <imgui.h>
#include <imnodes.h>
#include "json.hpp"

using json = nlohmann::json;
namespace dt::df
{

    class BaseNode::Impl
    {
    public:
        explicit Impl(const NodeId id,
                      const NodeKey &key,
                      const std::string &title,
                      Slots &&inputs,
                      Slots &&outputs)
            : id_{id},
              key_{key},
              title_{title},
              inputs_{std::move(inputs)},
              outputs_{std::move(outputs)}
        {
        }

        void to_json(nlohmann::json &j) const
        {
            j["id"] = id_;
            j["key"] = key_;
            j["title"] = title_;
            json input_slots = json::array();
            json output_slots = json::array();
            for (const auto &slot : inputs_)
            {
                json slot_val;
                slot->to_json(slot_val);
                input_slots.emplace_back(std::move(slot_val));
            }
            for (const auto &slot : outputs_)
            {
                json slot_val;
                slot->to_json(slot_val);
                output_slots.emplace_back(std::move(slot_val));
            }
            j["inputs"] = std::move(input_slots);
            j["outputs"] = std::move(output_slots);

            const auto position = imnodes::GetNodeEditorSpacePos(id_);
            j["position"] = {{"x", position.x}, {"y", position.y}};
        }

    private:
        const NodeId id_;
        const NodeKey key_;
        const std::string title_;
        const Slots inputs_;
        const Slots outputs_;

        friend BaseNode;
    };

    BaseNode::BaseNode(const NodeId id,
                       const NodeKey &key,
                       const std::string &title,
                       Slots &&inputs,
                       Slots &&outputs)
        : impl_{new BaseNode::Impl{id, key, title, std::forward<Slots>(inputs), std::forward<Slots>(outputs)}}
    {
    }

    const NodeKey &BaseNode::key() const
    {
        return impl_->key_;
    }

    void BaseNode::render()
    {
        imnodes::BeginNode(impl_->id_);
        imnodes::BeginNodeTitleBar();
        ImGui::TextUnformatted(impl_->title_.c_str());
        imnodes::EndNodeTitleBar();

        for (auto &slot : impl_->inputs_)
        {
            imnodes::BeginInputAttribute(slot->id());
            slot->render();
            imnodes::EndInputAttribute();
        }
        
        renderCustomContent();

        for (auto &slot : impl_->outputs_)
        {
            imnodes::BeginOutputAttribute(slot->id());
            slot->render();
            imnodes::EndOutputAttribute();
        }
        imnodes::EndNode();
    }

    void BaseNode::renderCustomContent()
    {
    }

    NodeId BaseNode::id() const
    {
        return impl_->id_;
    }

    const Slots &BaseNode::inputs() const
    {
        return impl_->inputs_;
    }
    const Slots &BaseNode::outputs() const
    {
        return impl_->outputs_;
    }

    SlotPtr BaseNode::inputs(const SlotId id) const
    {
        auto slot_it = std::find_if(impl_->inputs_.begin(), impl_->inputs_.end(), [id](const auto &slot) {
            return slot->id() == id;
        });

        if (slot_it == impl_->inputs_.end())
            return nullptr;
        return *slot_it;
    }
    SlotPtr BaseNode::outputs(const SlotId id) const
    {
        auto slot_it = std::find_if(impl_->outputs_.begin(), impl_->outputs_.end(), [id](const auto &slot) {
            return slot->id() == id;
        });

        if (slot_it == impl_->outputs_.end())
            return nullptr;
        return *slot_it;
    }

    void BaseNode::to_json(nlohmann::json &j) const
    {
        impl_->to_json(j);
    }

    BaseNode::~BaseNode()
    {
        delete impl_;
    }

} // namespace dt::df
