#ifndef OPTIONS
#define OPTIONS

#include<string>
#include<exception>

#include"types.hpp"

class wrong_argument_error : public std::exception{
        std::string description;
    public:
        wrong_argument_error(void);
        wrong_argument_error(const std::string& description);
        virtual ~wrong_argument_error(void);

        virtual const char* what(void)const noexcept;
};

class options{
        bool just_verify : 1;
        bool show_warnings : 1;
        bool warnings_as_errors : 1;
        bool unfold_simple_powers : 1;
        uint to_stage : 2; // 0->preprocessor, 1->semisteps, 2->full translation
        std::string output_name;
    public:
        options(void);
        options(uint number_of_args, const char** args)throw(wrong_argument_error);

        bool verifying(void)const;
        bool showing_warnings(void)const;
        bool escalating_warnings(void)const;
        bool unfolding_simples(void)const;
        bool just_preprocess(void)const;
        bool just_semisteps(void)const;
        const std::string& output_file(void)const;
};

#endif
