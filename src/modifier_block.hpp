#ifndef MODIFIER_BLOCK
#define MODIFIER_BLOCK

#include"concatenation.hpp"

namespace rbg_parser{

class modifier_block : public concatenation{
    public:
        modifier_block(std::vector<std::unique_ptr<game_move>>&& content);
        void accept(abstract_dispatcher& dispatcher)const override;
        bool modifies(void)const override{return true;};
        straightness_result compute_k_straightness(void)const override;
};

}

#endif
