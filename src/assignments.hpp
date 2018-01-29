#ifndef ASSIGNMENTS
#define ASSIGNMENTS

#include<cassert>

#include"game_move.hpp"
#include"concatenation.hpp"
#include"token.hpp"
#include"message.hpp"
#include"slice_iterator.hpp"
#include"parser_helpers.hpp"
#include"declarations.hpp"
#include"abstract_dispatcher.hpp"

namespace rbg_parser{

class assignment : public game_move{
        token left_side;
        token right_side;
        token kind_of_modification; // =, ++ or --; just a temporary solution
        bool lazy;
        assignment(token&& left_side, token&& right_side, token&& kind_of_modification, bool lazy=false);
        assignment(void)=default;
    public:
        ~assignment(void)override=default;
        assignment(assignment&&)=default;
        assignment(const assignment&)=default;
        assignment& operator=(const assignment&)=default;
        assignment& operator=(assignment&&)=default;
        void set_lazy(void)override{lazy=true;};
        std::unique_ptr<pure_game_move> transform_into_pure(void)override{assert(false);};
        std::unique_ptr<game_move> simplify(void)override{return std::unique_ptr<game_move>(new assignment(std::move(*this)));};
        void accept(abstract_dispatcher& dispatcher)const override;
        uint priority(void)const override{return 3;};
        std::string to_rbg(uint)const override;
        std::string to_rbg()const override;
        std::unique_ptr<game_move> flatten(void)override;
        void gather_concatenation_elements(
            std::vector<std::unique_ptr<game_move>>& elements,
            std::vector<std::unique_ptr<game_move>>& next_block_elements)override;
        straightness_result compute_k_straightness(void)const override;
        bool check_if_redundant(std::set<token>& assignments_so_far, bool&)const override;
        bool finalizer_elligible(void)const override;
        const token& get_left_side(void)const;
        const token& get_right_side(void)const;
        bool is_lazy(void)const;
        bool is_assignment(void)const;
        bool is_incrementation(void)const;
        bool is_decrementation(void)const;
        friend parser_result<assignment> parse_single_assignment(slice_iterator& it, const declarations& decls, messages_container& msg)throw(message);
        friend parser_result<assignment>;
        friend parser_result<concatenation> parse_assignments(slice_iterator& it, const declarations& decls, messages_container& msg)throw(message);
};

parser_result<concatenation> parse_assignments(slice_iterator& it, const declarations& decls, messages_container& msg)throw(message);

}

#endif
