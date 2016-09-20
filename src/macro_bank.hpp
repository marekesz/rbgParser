#ifndef MACRO_BANK
#define MACRO_BANK

#include<vector>
#include<map>

#include"types.hpp"
#include"token.hpp"
#include"slice.hpp"
#include"message.hpp"

class macro{
        std::vector<token> arguments;
        const std::vector<token>* data;
        uint begin;
        uint end;
        uint context_order;
    public:
        macro(std::vector<token>&& arguments, const std::vector<token>* data, uint begin, uint end, uint context_order);
        macro(const macro& src);
        macro(macro&& src);
        macro& operator=(const macro&)=delete;
        macro& operator=(macro&&)=delete;
        ~macro(void);

        slice turn_into_slice(const std::vector<slice>& passed_values)const;
        uint get_arity(void)const;
        uint get_context_order(void)const;
        bool is_empty(void)const;
};

class macro_bank{
        std::map<token,std::map<uint,macro>> content;
    public:
        macro_bank(void);
        void add_macro(const token& name,macro&& m);
        slice get_macro_slice(const token& name,const std::vector<slice>& passed_values)const;
        bool could_be_macro_name(const token& name)const;
        bool is_zero_arity_macro(const token& name, uint context_order)const;
        bool is_non_zero_arity_macro(const token& name, uint context_order)const;
        bool is_macro_name_and_arity(const token& name, uint arity, uint context_order)const;
};

#endif
