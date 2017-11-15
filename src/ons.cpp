#include"ons.hpp"

ons::ons(std::vector<token>&& legal_ons):
legal_ons(legal_ons.begin(),legal_ons.end()){
}

ons::ons(void):
legal_ons(){
}

parser_result<ons> parse_ons(slice_iterator& it, const std::set<token>& declared_pieces, messages_container& msg)throw(message){
    parsing_context_string_guard g(&it, "Unexpected end of input while parsing \'ons\' move");
    auto set_result = parse_sequence(it,"legal ons",declared_pieces,true,msg);
    if(!set_result.is_success())
        return failure<ons>();
    else
        return success(ons(set_result.move_value()));
}
