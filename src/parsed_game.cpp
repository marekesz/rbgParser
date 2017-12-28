#include"parsed_game.hpp"

parsed_game::parsed_game(
std::string&& name,
game_board&& brd,
std::unique_ptr<game_move> moves):
name(std::move(name)),
brd(std::move(brd)),
moves(std::move(moves)){
}

const game_board& parsed_game::get_board(void)const{
    return brd;
}

const game_move* parsed_game::get_moves(void)const{
    return moves.get();
}
