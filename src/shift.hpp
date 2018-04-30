#ifndef SHIFT
#define SHIFT

#include<memory>

#include"game_move.hpp"
#include"abstract_dispatcher.hpp"
#include"token.hpp"

namespace rbg_parser{

class shift : public game_move{
        //int x;
        //int y;
        token edge_name;
        shift(void)=default;
    public:
        shift(const token& edge_name);
        ~shift(void)override=default;
        shift(shift&&)=default;
        shift(const shift&)=default;
        shift& operator=(const shift&)=default;
        shift& operator=(shift&&)=default;
        std::unique_ptr<game_move> simplify(void)override{return std::unique_ptr<game_move>(new shift(std::move(*this)));};
        void accept(abstract_dispatcher& dispatcher)const override;
        uint priority(void)const override{return 4;};
        std::string to_rbg(uint)const override;
        std::string to_rbg()const override;
        std::unique_ptr<game_move> flatten(void)override;
        //int get_x(void)const;
        //int get_y(void)const;
        const token& get_content(void)const;
};

}

#endif
