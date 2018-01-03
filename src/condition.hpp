#ifndef CONDITION
#define CONDITION

#include<memory>
#include<vector>

namespace rbg_parser{

class abstract_dispatcher;
// interface
class condition{
    public:
        virtual ~condition(void)=default;
        virtual void accept(abstract_dispatcher &dispatcher)const=0;
        virtual std::unique_ptr<condition> simplify(void)=0;
        virtual uint priority(void)const=0; // being of higher priority containg lower ones requires surrounding them with brackets
        virtual std::string to_rbg(uint indent)const=0;
        virtual std::string to_rbg()const=0;
        virtual std::unique_ptr<condition> flatten(void)=0;
        virtual void gather_conjunction_elements(std::vector<std::unique_ptr<condition>>& elements){elements.push_back(flatten());};
        virtual void gather_alterantive_elements(std::vector<std::unique_ptr<condition>>& elements){elements.push_back(flatten());};
};

}

#endif
