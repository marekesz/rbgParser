#ifndef PARSED_GAME
#define PARSED_GAME

#include<string>
#include<memory>

#include"declarations.hpp"
#include"game_board.hpp"
#include"game_move.hpp"
#include"graph.hpp"

namespace rbg_parser{

class parsed_game{
        std::string name;
        declarations decl;
//        game_board brd;
        graph g;
        std::unique_ptr<game_move> moves;
        uint bound_val;
        int straightness;
    public:
        parsed_game(
            std::string&& name,
            declarations&& decl,
//            game_board&& brd,
            graph&& g,
            std::unique_ptr<game_move> moves,
            uint bound_val);
        const declarations& get_declarations(void)const;
        const graph& get_board(void)const;
        const game_move* get_moves(void)const;
        const std::string& get_name(void)const;
        uint get_bound(void)const;
        int get_straightness(void)const;
        std::string to_rbg(bool pretty=false)const;
};

}

#endif
