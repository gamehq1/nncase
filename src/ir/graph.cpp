#include <ir/graph.h>
#include <ir/visitor.h>
#include <unordered_set>

using namespace nncase;
using namespace nncase::ir;

void graph::assign_names()
{
    std::unordered_set<std::string_view> names;

    for (auto &&node : nodes_)
    {
        size_t i = 0;
        while (node->name().empty() || names.contains(node->name()))
            node->name(std::string(node_opcode_names(node->runtime_opcode())) + "_" + std::to_string(i++));
        names.emplace(node->name());
    }
}

void graph::collect()
{
    std::unordered_set<node *> used_nodes;

    auto visitor = make_relay_ir_visitor([&](node &node) {
        used_nodes.emplace(&node);
    });
    visitor.visit(*this);

    auto end = std::remove_if(std::begin(nodes_), std::end(nodes_), [&](auto &node) {
        if (!used_nodes.contains(node.get()))
        {
            for (auto &in : node->inputs())
                in.clear_connection();
            for (auto &out : node->outputs())
                out.clear_connections();
            return true;
        }

        return false;
    });
    nodes_.erase(end, std::end(nodes_));
}