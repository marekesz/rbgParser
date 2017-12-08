#include"offs.hpp"

off::off(token&& off_piece):
off_piece(std::move(off_piece)){
}

parser_result<sum> parse_offs(slice_iterator& it, const declarations& decls, messages_container& msg)throw(message){
    parsing_context_string_guard g(&it, "Unexpected end of input while parsing offs");
    auto set_result = parse_sequence(it,"legal offs",decls.get_legal_pieces(),true,msg);
    if(!set_result.is_success())
        return failure<sum>();
    else{
        sum result;
        std::vector<token> temp_token_set(set_result.move_value());
        while(!temp_token_set.empty()){
            result.add_move(std::unique_ptr<game_move>(new off(std::move(temp_token_set.back()))));
            temp_token_set.pop_back();
        }
        return success(std::move(result));
    }
}