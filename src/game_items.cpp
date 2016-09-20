#include<cassert>

#include "game_items.hpp"
#include"types.hpp"

game_items::game_items(void):
macros(),
game_segment(nullptr),
board_segment(nullptr),
player_segments(),
order_segment(nullptr),
goal_segments(),
next_item_context_order(0){
}

game_items::game_items(game_items&& src):
macros(std::move(src.macros)),
game_segment(src.game_segment),
board_segment(src.board_segment),
player_segments(std::move(src.player_segments)),
order_segment(src.order_segment),
goal_segments(std::move(src.goal_segments)),
next_item_context_order(src.next_item_context_order){
    src.game_segment = src.board_segment = src.order_segment = nullptr;
}

game_items& game_items::operator=(game_items&& src){
    if(this != &src){
        macro_bank temp(std::move(src.macros));
        src.macros = std::move(macros);
        macros = std::move(temp);
        std::swap(game_segment,src.game_segment);
        std::swap(board_segment,src.board_segment);
        std::swap(player_segments,src.player_segments);
        std::swap(order_segment,src.order_segment);
        std::swap(goal_segments,src.goal_segments);
        next_item_context_order = src.next_item_context_order;
    }
    return *this;
}

game_items::~game_items(void){
    delete game_segment;
    delete board_segment;
    delete order_segment;
}

uint game_items::input_macro(const std::vector<token>& input,uint current_token,messages_container& msg)throw(message){
    if(current_token >= input.size() || input[current_token].get_type() == hash)
        throw msg.build_message(input[current_token-1].get_position(),"Empty \'def\' directive");
    if(input[current_token].get_type() != identifier)
        throw msg.build_message(input[current_token].get_position(),"Expected identifier, encountered \'"+input[current_token].to_string()+"\'");
    token name = input[current_token];
    std::vector<token> args;
    current_token = parse_arguments(input,current_token+1,args,msg);
    if((args.size()>0 && macros.is_zero_arity_macro(name,next_item_context_order)) || (args.size()==0 && macros.is_non_zero_arity_macro(name,next_item_context_order)))
        throw msg.build_message(name.get_position(),"Cannot define two macros with the same name with zero and non zero arities");
    if(macros.is_macro_name_and_arity(name,args.size(),next_item_context_order))
        msg.add_message(name.get_position(),"Macro \'"+name.to_string()+"\' with arity "+std::to_string(args.size())+" already defined, overwriting previous definition");
    if(current_token >= input.size() || input[current_token].get_type() == hash)
        throw msg.build_message(input[current_token-1].get_position(),"Unexpected end of macro definition");
    if(input[current_token].get_type() != equal)
        throw msg.build_message(input[current_token].get_position(),"Expected \'=\' token, encountered \'"+input[current_token].to_string()+"\'");
    uint begin = current_token+1;
    uint end = current_token = reach_end_of_directive(input,begin);
    macro m = macro(std::move(args),&input,begin,end,next_item_context_order++);
    if(m.is_empty())
        msg.add_message(name.get_position(),"This macro doesn't have body");
    macros.add_macro(std::move(name),std::move(m));
    return current_token;
}

uint game_items::input_game(const std::vector<token>& input,uint current_token,messages_container& msg)throw(message){
    uint begin = current_token;
    uint end = current_token = reach_end_of_directive(input,current_token);
    slice* s = new slice(&input,begin,end,next_item_context_order++);
    if(s->is_empty())
        msg.add_message(input[begin-1].get_position(),"Empty \'game\' directive");
    game_segment = s;
    return end;
}

uint game_items::input_board(const std::vector<token>& input,uint current_token,messages_container& msg)throw(message){
    uint begin = current_token;
    uint end = current_token = reach_end_of_directive(input,current_token);
    slice* s = new slice(&input,begin,end,next_item_context_order++);
    if(s->is_empty())
        msg.add_message(input[begin-1].get_position(),"Empty \'board\' directive");
    board_segment = s;
    return end;
}

uint game_items::input_player(const std::vector<token>& input,uint current_token,messages_container& msg)throw(message){
    if(current_token >= input.size() || input[current_token].get_type() == hash)
        throw msg.build_message(input[current_token-1].get_position(),"Empty \'player\' directive");
    if(input[current_token].get_type() != identifier)
        throw msg.build_message(input[current_token].get_position(),"Expected identifier, encountered \'"+input[current_token].to_string()+"\'");
    if(player_segments.count(input[current_token]))
        msg.add_message(input[current_token].get_position(),"Player +\'"+input[current_token].to_string()+"\' already defined, overwriting previous definition");
    token name = input[current_token];
    uint begin = current_token+1;
    uint end = current_token = reach_end_of_directive(input,begin);
    slice s = slice(&input,begin,end,next_item_context_order++);
    if(s.is_empty())
        msg.add_message(input[begin-1].get_position(),"Empty \'player\' body");
    player_segments.insert(std::make_pair(std::move(name),std::move(s)));
    return end;
}

uint game_items::input_order(const std::vector<token>& input,uint current_token,messages_container& msg)throw(message){
    uint begin = current_token;
    uint end = current_token = reach_end_of_directive(input,current_token);
    slice* s = new slice(&input,begin,end,next_item_context_order++);
    if(s->is_empty())
        msg.add_message(input[begin-1].get_position(),"Empty \'order\' directive");
    order_segment = s;
    return end;
}

uint game_items::input_goal(const std::vector<token>& input,uint current_token,messages_container& msg)throw(message){
    if(current_token >= input.size() || input[current_token].get_type() == hash)
        throw msg.build_message(input[current_token-1].get_position(),"Empty \'goal\' directive");
    if(input[current_token].get_type() != identifier)
        throw msg.build_message(input[current_token].get_position(),"Expected identifier, encountered \'"+input[current_token].to_string()+"\'");
    if(goal_segments.count(input[current_token]))
        msg.add_message(input[current_token].get_position(),"Goal +\'"+input[current_token].to_string()+"\' already defined, overwriting previous definition");
    token name = input[current_token];
    uint begin = current_token+1;
    uint end = current_token = reach_end_of_directive(input,begin);
    slice s = slice(&input,begin,end,next_item_context_order++);
    if(s.is_empty())
        msg.add_message(input[begin-1].get_position(),"Empty \'goal\' body");
    goal_segments.insert(std::make_pair(std::move(name),std::move(s)));
    return end;
}

slice_iterator game_items::get_game_segment(void)const{
    assert(game_segment != nullptr);
    return slice_iterator(*game_segment);
}

const macro_bank& game_items::get_macros(void)const{
    return macros;
}

uint reach_end_of_directive(const std::vector<token>& input,uint current_token){
    while(current_token < input.size() && input[current_token].get_type() != hash)
        ++current_token;
    return current_token;
}

uint parse_arguments(const std::vector<token>& input,uint current_token,std::vector<token>& args,messages_container& msg)throw(message){
    if(input[current_token].get_type() != left_round_bracket || current_token >= input.size())
        return current_token;
    while(true){
        ++current_token;
        if(current_token >= input.size() || input[current_token].get_type() == hash)
            throw msg.build_message(input[current_token-1].get_position(),"Unexpected end of macro arguments list");
        if(input[current_token].get_type() != identifier)
            throw msg.build_message(input[current_token].get_position(),"Expected identifier, encountered \'"+input[current_token].to_string()+"\'");
        args.push_back(input[current_token++]);
        if(current_token >= input.size() || input[current_token].get_type() == hash)
            throw msg.build_message(input[current_token-1].get_position(),"Unexpected end of macro arguments list");
        if(input[current_token].get_type() == right_round_bracket)
            return current_token+1;
        if(input[current_token].get_type() != comma)
            throw msg.build_message(input[current_token].get_position(),"Expected \',\' or \'(\' token, encounered \'"+input[current_token].to_string()+"\'");
    }
}

game_items input_tokens(const std::vector<token>& input,messages_container& msg)throw(message){
    game_items result;
    if(input.empty())
        throw msg.build_message("No tokens in file");
    uint current_token = 0;
    if(input[current_token].get_type() != hash)
        throw msg.build_message(input[current_token].get_position(),"Expected \'#\' token at beginning of file, encountered \'"+input[current_token].to_string()+"\'");
    while(current_token < input.size()){
        ++current_token;
        if(current_token < input.size()){
            if(input[current_token].get_type() == def)
                current_token = result.input_macro(input,current_token+1,msg);
            else if(input[current_token].get_type() == game)
                current_token = result.input_game(input,current_token+1,msg);
            else if(input[current_token].get_type() == board)
                current_token = result.input_board(input,current_token+1,msg);
            else if(input[current_token].get_type() == player)
                current_token = result.input_player(input,current_token+1,msg);
            else if(input[current_token].get_type() == order)
                current_token = result.input_order(input,current_token+1,msg);
            else if(input[current_token].get_type() == goal)
                current_token = result.input_goal(input,current_token+1,msg);
            else
                throw msg.build_message(input[current_token].get_position(),"Expected \'def\', \'game\', \'board\', \'player\', \'order\' or \'goal\' token, encountered \'"+input[current_token].to_string()+"\'");
        }
    }
    if(result.game_segment == nullptr)
        msg.add_message("No \'game\' directive");
    if(result.board_segment == nullptr)
        msg.add_message("No \'board\' directive");
    if(result.order_segment == nullptr)
        msg.add_message("No \'order\' directive");
    return result;
}
