#ifndef SHIFT
#define SHIFT

#include<memory>

#include"pure_game_move.hpp"
#include"message.hpp"
#include"slice_iterator.hpp"
#include"parser_helpers.hpp"
#include"abstract_dispatcher.hpp"

class shift : public pure_game_move{
        int x;
        int y;
        shift(int x,int y);
        shift(void)=default;
    public:
        ~shift(void)override=default;
        shift(shift&&)=default;
        shift(const shift&)=default;
        shift& operator=(const shift&)=default;
        shift& operator=(shift&&)=default;
        std::unique_ptr<pure_game_move> transform_into_pure(void)override{return std::unique_ptr<pure_game_move>(new shift(*this));};
        void accept(abstract_dispatcher& dispatcher)override;
        int get_x(void)const;
        int get_y(void)const;
        friend parser_result<shift> parse_shift(slice_iterator& it, messages_container& msg)throw(message);
        friend parser_result<shift>;
};

parser_result<shift> parse_shift(slice_iterator& it, messages_container& msg)throw(message);

#endif
