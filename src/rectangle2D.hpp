#ifndef RECTANGLE2D
#define RECTANGLE2D

#include"token.hpp"
#include"slice_iterator.hpp"
#include"graph_builder.hpp"
#include"parser_helpers.hpp"
#include"graph.hpp"

namespace rbg_parser{

class declarations;
class messages_container;
class message;

class rectangle2D : public graph_builder{
        token up;
        token down;
        token left;
        token right;
        std::vector<std::vector<token>> starting_pieces;
        bool add_line_if_aligned(std::vector<token>&& line);
        void parse_edge_argument(token rectangle2D::*direction, declarations& decl, slice_iterator& it, messages_container& msg)throw(message);
        bool parse_boardline(declarations& decl, slice_iterator& it, messages_container& msg)throw(message);
        std::tuple<token,token,edges> transform_square(uint line_number,uint column_number)const;
        uint compute_index(uint line_number, uint column_number)const;
    public:
        rectangle2D(void)=default;
        rectangle2D(const rectangle2D&)=delete;
        rectangle2D& operator=(const rectangle2D&)=delete;
        rectangle2D(rectangle2D&&)=default;
        rectangle2D& operator=(rectangle2D&&)=default;
        ~rectangle2D(void)override=default;
        graph build_graph(messages_container&)const throw(message) override;
        friend parser_result<std::unique_ptr<graph_builder>> parse_rectangle2D(declarations& decl, slice_iterator& it, messages_container& msg)throw(message);
};

parser_result<std::unique_ptr<graph_builder>> parse_rectangle2D(declarations& decl, slice_iterator& it, messages_container& msg)throw(message);

}

#endif
