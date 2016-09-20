#ifndef GAME_ITEMS
#define GAME_ITEMS

#include<map>
#include<vector>

#include"macro_bank.hpp"
#include"token.hpp"
#include"message.hpp"
#include"slice_iterator.hpp"

class game_items{
        macro_bank macros;
        slice* game_segment;
        slice* board_segment;
        std::map<token,slice> player_segments;
        slice* order_segment;
        std::map<token,slice> goal_segments;
        uint next_item_context_order;
        game_items(void);
    public:
        game_items(const game_items&)=delete;
        game_items(game_items&& src);
        game_items& operator=(const game_items&)=delete;
        game_items& operator=(game_items&& src);
        ~game_items(void);

        friend game_items input_tokens(const std::vector<token>& input,messages_container& msg)throw(message);
        uint input_macro(const std::vector<token>& input,uint current_token,messages_container& msg)throw(message);
        uint input_game(const std::vector<token>& input,uint current_token,messages_container& msg)throw(message);
        uint input_board(const std::vector<token>& input,uint current_token,messages_container& msg)throw(message);
        uint input_player(const std::vector<token>& input,uint current_token,messages_container& msg)throw(message);
        uint input_order(const std::vector<token>& input,uint current_token,messages_container& msg)throw(message);
        uint input_goal(const std::vector<token>& input,uint current_token,messages_container& msg)throw(message);

        slice_iterator get_game_segment(void)const;
        const macro_bank& get_macros(void)const;
};

uint reach_end_of_directive(const std::vector<token>& input,uint current_token);
uint parse_arguments(const std::vector<token>& input,uint current_token,std::vector<token>& args,messages_container& msg)throw(message);
game_items input_tokens(const std::vector<token>& input,messages_container& msg)throw(message);

#endif
