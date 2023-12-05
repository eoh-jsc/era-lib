#ifndef INC_ERA_STRUCT_HPP_
#define INC_ERA_STRUCT_HPP_

namespace eras {
    template <class Arg1, class Arg2, class Result> 
    struct binary_function {
        using first_argument_type = Arg1;
        using second_argument_type = Arg2;
        using result_type = Result;
    };

    template <class Arg, class Result>
    struct unary_function {
        using argument_type = Arg;
        using result_type = Result;
    };
};

#endif /* INC_ERA_STRUCT_HPP_ */
