#pragma once
#include "connectors.h"
#include "opcode.h"
#include <vector>
#include <xtl/xspan.hpp>

namespace nncase
{
namespace ir
{
#define DEFINE_NODE_OPCODE(value)                                    \
    static constexpr node_opcode opcode() noexcept { return value; } \
    node_opcode runtime_opcode() const noexcept override { return value; }

    class node
    {
    public:
        node() = default;
        node(node &) = delete;
        virtual ~node();

        const std::string &name() const noexcept { return name_; }

        template <class TArg, class... TArgs>
        void name(TArg arg, TArgs... args) { name_.assign(std::forward<TArg>(arg), std::forward<TArgs>(args)...); }

        xtl::span<input_connector> inputs() noexcept { return input_connectors_; }
        xtl::span<output_connector> outputs() noexcept { return output_connectors_; }

        input_connector &input_at(size_t index) { return input_connectors_.at(index); }
        output_connector &output_at(size_t index) { return output_connectors_.at(index); }

        virtual node_opcode runtime_opcode() const noexcept = 0;

    protected:
        template <class TName, class TShape>
        input_connector &add_input(TName &&name, datatype_t type, TShape &&shape)
        {
            return input_connectors_.emplace_back(*this, std::forward<TName>(name), type, std::forward<TShape>(shape));
        }

        template <class TName, class TShape>
        output_connector &add_output(TName &&name, datatype_t type, TShape &&shape, memory_type_t memory_type = mem_main)
        {
            return output_connectors_.emplace_back(*this, std::forward<TName>(name), type, std::forward<TShape>(shape), memory_type);
        }

    private:
        std::string name_;
        std::vector<input_connector> input_connectors_;
        std::vector<output_connector> output_connectors_;
    };
}
}