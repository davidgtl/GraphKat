//
// Created by david on 22/12/2019.
//

#ifndef GRAPHKAT_MACRO_SHENANIGANS_H
#define GRAPHKAT_MACRO_SHENANIGANS_H

#define DCAT(a, b, c) CAT(a, CAT(b,c))
#define CAT(a, ...) PRIMITIVE_CAT(a, __VA_ARGS__)
#define PRIMITIVE_CAT(a, ...) a ## __VA_ARGS__

//TODO: extend number of arguments
#define ELEVENTH_ARGUMENT(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, ...) a11
#define COUNT_ARGUMENTS(...) ELEVENTH_ARGUMENT(dummy, ## __VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define ApplyMacro(M, A, ...) CAT(ApplyMacro_,COUNT_ARGUMENTS(__VA_ARGS__))(M, A, __VA_ARGS__)
#define ApplyMacro_0(M, A, ...)
#define ApplyMacro_1(M, A, arg, ...) M(A, arg)ApplyMacro_0(M, A, __VA_ARGS__)
#define ApplyMacro_2(M, A, arg, ...) M(A, arg),ApplyMacro_1(M, A, __VA_ARGS__)
#define ApplyMacro_3(M, A, arg, ...) M(A, arg),ApplyMacro_2(M, A, __VA_ARGS__)
#define ApplyMacro_4(M, A, arg, ...) M(A, arg),ApplyMacro_3(M, A, __VA_ARGS__)

#endif //GRAPHKAT_MACRO_SHENANIGANS_H
