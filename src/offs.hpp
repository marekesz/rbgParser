#ifndef OFFS
#define OFFS

#include<cassert>

#include"game_move.hpp"
#include"token.hpp"
#include"sum.hpp"
#include"message.hpp"
#include"slice_iterator.hpp"
#include"parser_helpers.hpp"
#include"declarations.hpp"

class off : public game_move{
        token off_piece;
        off(token&& off_piece);
        off(void)=default;
    public:
        ~off(void)override=default;
        off(off&&)=default;
        off(const off&)=default;
        off& operator=(const off&)=default;
        off& operator=(off&&)=default;
        std::unique_ptr<pure_game_move> transform_into_pure(void)override{assert(false);};
        friend parser_result<sum> parse_offs(slice_iterator& it, const declarations& decls, messages_container& msg)throw(message);
        friend parser_result<sum>;
};

parser_result<sum> parse_offs(slice_iterator& it, const declarations& decls, messages_container& msg)throw(message);

#endif
