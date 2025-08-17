#include <cstdint>
#include <list>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <type_traits>
#include <vector>
#define JODA_SETTINGS_EXPAND(x) x
#define JODA_SETTINGS_GET_MACRO(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, \
                                _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48,  \
                                _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, NAME, ...)                          \
  NAME
#define JODA_SETTINGS_PASTE(...)                                                                                                                \
  JODA_SETTINGS_EXPAND(JODA_SETTINGS_GET_MACRO(                                                                                                 \
      __VA_ARGS__, JODA_SETTINGS_PASTE64, JODA_SETTINGS_PASTE63, JODA_SETTINGS_PASTE62, JODA_SETTINGS_PASTE61, JODA_SETTINGS_PASTE60,           \
      JODA_SETTINGS_PASTE59, JODA_SETTINGS_PASTE58, JODA_SETTINGS_PASTE57, JODA_SETTINGS_PASTE56, JODA_SETTINGS_PASTE55, JODA_SETTINGS_PASTE54, \
      JODA_SETTINGS_PASTE53, JODA_SETTINGS_PASTE52, JODA_SETTINGS_PASTE51, JODA_SETTINGS_PASTE50, JODA_SETTINGS_PASTE49, JODA_SETTINGS_PASTE48, \
      JODA_SETTINGS_PASTE47, JODA_SETTINGS_PASTE46, JODA_SETTINGS_PASTE45, JODA_SETTINGS_PASTE44, JODA_SETTINGS_PASTE43, JODA_SETTINGS_PASTE42, \
      JODA_SETTINGS_PASTE41, JODA_SETTINGS_PASTE40, JODA_SETTINGS_PASTE39, JODA_SETTINGS_PASTE38, JODA_SETTINGS_PASTE37, JODA_SETTINGS_PASTE36, \
      JODA_SETTINGS_PASTE35, JODA_SETTINGS_PASTE34, JODA_SETTINGS_PASTE33, JODA_SETTINGS_PASTE32, JODA_SETTINGS_PASTE31, JODA_SETTINGS_PASTE30, \
      JODA_SETTINGS_PASTE29, JODA_SETTINGS_PASTE28, JODA_SETTINGS_PASTE27, JODA_SETTINGS_PASTE26, JODA_SETTINGS_PASTE25, JODA_SETTINGS_PASTE24, \
      JODA_SETTINGS_PASTE23, JODA_SETTINGS_PASTE22, JODA_SETTINGS_PASTE21, JODA_SETTINGS_PASTE20, JODA_SETTINGS_PASTE19, JODA_SETTINGS_PASTE18, \
      JODA_SETTINGS_PASTE17, JODA_SETTINGS_PASTE16, JODA_SETTINGS_PASTE15, JODA_SETTINGS_PASTE14, JODA_SETTINGS_PASTE13, JODA_SETTINGS_PASTE12, \
      JODA_SETTINGS_PASTE11, JODA_SETTINGS_PASTE10, JODA_SETTINGS_PASTE9, JODA_SETTINGS_PASTE8, JODA_SETTINGS_PASTE7, JODA_SETTINGS_PASTE6,     \
      JODA_SETTINGS_PASTE5, JODA_SETTINGS_PASTE4, JODA_SETTINGS_PASTE3, JODA_SETTINGS_PASTE2, JODA_SETTINGS_PASTE1)(__VA_ARGS__))
#define JODA_SETTINGS_PASTE2(func, v1) func(v1)
#define JODA_SETTINGS_PASTE3(func, v1, v2) JODA_SETTINGS_PASTE2(func, v1) JODA_SETTINGS_PASTE2(func, v2)
#define JODA_SETTINGS_PASTE4(func, v1, v2, v3) JODA_SETTINGS_PASTE2(func, v1) JODA_SETTINGS_PASTE3(func, v2, v3)
#define JODA_SETTINGS_PASTE5(func, v1, v2, v3, v4) JODA_SETTINGS_PASTE2(func, v1) JODA_SETTINGS_PASTE4(func, v2, v3, v4)
#define JODA_SETTINGS_PASTE6(func, v1, v2, v3, v4, v5) JODA_SETTINGS_PASTE2(func, v1) JODA_SETTINGS_PASTE5(func, v2, v3, v4, v5)
#define JODA_SETTINGS_PASTE7(func, v1, v2, v3, v4, v5, v6) JODA_SETTINGS_PASTE2(func, v1) JODA_SETTINGS_PASTE6(func, v2, v3, v4, v5, v6)
#define JODA_SETTINGS_PASTE8(func, v1, v2, v3, v4, v5, v6, v7) JODA_SETTINGS_PASTE2(func, v1) JODA_SETTINGS_PASTE7(func, v2, v3, v4, v5, v6, v7)
#define JODA_SETTINGS_PASTE9(func, v1, v2, v3, v4, v5, v6, v7, v8) \
  JODA_SETTINGS_PASTE2(func, v1) JODA_SETTINGS_PASTE8(func, v2, v3, v4, v5, v6, v7, v8)
#define JODA_SETTINGS_PASTE10(func, v1, v2, v3, v4, v5, v6, v7, v8, v9) \
  JODA_SETTINGS_PASTE2(func, v1) JODA_SETTINGS_PASTE9(func, v2, v3, v4, v5, v6, v7, v8, v9)
#define JODA_SETTINGS_PASTE11(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10) \
  JODA_SETTINGS_PASTE2(func, v1) JODA_SETTINGS_PASTE10(func, v2, v3, v4, v5, v6, v7, v8, v9, v10)
#define JODA_SETTINGS_PASTE12(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11) \
  JODA_SETTINGS_PASTE2(func, v1) JODA_SETTINGS_PASTE11(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11)
#define JODA_SETTINGS_PASTE13(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12) \
  JODA_SETTINGS_PASTE2(func, v1) JODA_SETTINGS_PASTE12(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12)
#define JODA_SETTINGS_PASTE14(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13) \
  JODA_SETTINGS_PASTE2(func, v1) JODA_SETTINGS_PASTE13(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13)
#define JODA_SETTINGS_PASTE15(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14) \
  JODA_SETTINGS_PASTE2(func, v1) JODA_SETTINGS_PASTE14(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14)
#define JODA_SETTINGS_PASTE16(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15) \
  JODA_SETTINGS_PASTE2(func, v1)                                                                      \
  JODA_SETTINGS_PASTE15(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15)
#define JODA_SETTINGS_PASTE17(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16) \
  JODA_SETTINGS_PASTE2(func, v1)                                                                           \
  JODA_SETTINGS_PASTE16(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16)
#define JODA_SETTINGS_PASTE18(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17) \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                \
  JODA_SETTINGS_PASTE17(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17)
#define JODA_SETTINGS_PASTE19(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18) \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                     \
  JODA_SETTINGS_PASTE18(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18)
#define JODA_SETTINGS_PASTE20(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19) \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                          \
  JODA_SETTINGS_PASTE19(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19)
#define JODA_SETTINGS_PASTE21(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20) \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                               \
  JODA_SETTINGS_PASTE20(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20)
#define JODA_SETTINGS_PASTE22(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21) \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                    \
  JODA_SETTINGS_PASTE21(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21)
#define JODA_SETTINGS_PASTE23(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22) \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                         \
  JODA_SETTINGS_PASTE22(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22)
#define JODA_SETTINGS_PASTE24(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23) \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                              \
  JODA_SETTINGS_PASTE23(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23)
#define JODA_SETTINGS_PASTE25(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24) \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                   \
  JODA_SETTINGS_PASTE24(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24)
#define JODA_SETTINGS_PASTE26(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, \
                              v25)                                                                                                                 \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                   \
  JODA_SETTINGS_PASTE25(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25)
#define JODA_SETTINGS_PASTE27(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, \
                              v25, v26)                                                                                                            \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                   \
  JODA_SETTINGS_PASTE26(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26)
#define JODA_SETTINGS_PASTE28(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, \
                              v25, v26, v27)                                                                                                       \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                   \
  JODA_SETTINGS_PASTE27(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, \
                        v27)
#define JODA_SETTINGS_PASTE29(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, \
                              v25, v26, v27, v28)                                                                                                  \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                   \
  JODA_SETTINGS_PASTE28(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, \
                        v27, v28)
#define JODA_SETTINGS_PASTE30(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, \
                              v25, v26, v27, v28, v29)                                                                                             \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                   \
  JODA_SETTINGS_PASTE29(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, \
                        v27, v28, v29)
#define JODA_SETTINGS_PASTE31(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, \
                              v25, v26, v27, v28, v29, v30)                                                                                        \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                   \
  JODA_SETTINGS_PASTE30(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, \
                        v27, v28, v29, v30)
#define JODA_SETTINGS_PASTE32(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, \
                              v25, v26, v27, v28, v29, v30, v31)                                                                                   \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                   \
  JODA_SETTINGS_PASTE31(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, \
                        v27, v28, v29, v30, v31)
#define JODA_SETTINGS_PASTE33(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, \
                              v25, v26, v27, v28, v29, v30, v31, v32)                                                                              \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                   \
  JODA_SETTINGS_PASTE32(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, \
                        v27, v28, v29, v30, v31, v32)
#define JODA_SETTINGS_PASTE34(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, \
                              v25, v26, v27, v28, v29, v30, v31, v32, v33)                                                                         \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                   \
  JODA_SETTINGS_PASTE33(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, \
                        v27, v28, v29, v30, v31, v32, v33)
#define JODA_SETTINGS_PASTE35(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, \
                              v25, v26, v27, v28, v29, v30, v31, v32, v33, v34)                                                                    \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                   \
  JODA_SETTINGS_PASTE34(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, \
                        v27, v28, v29, v30, v31, v32, v33, v34)
#define JODA_SETTINGS_PASTE36(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, \
                              v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35)                                                               \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                   \
  JODA_SETTINGS_PASTE35(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, \
                        v27, v28, v29, v30, v31, v32, v33, v34, v35)
#define JODA_SETTINGS_PASTE37(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, \
                              v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36)                                                          \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                   \
  JODA_SETTINGS_PASTE36(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, \
                        v27, v28, v29, v30, v31, v32, v33, v34, v35, v36)
#define JODA_SETTINGS_PASTE38(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, \
                              v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37)                                                     \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                   \
  JODA_SETTINGS_PASTE37(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, \
                        v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37)
#define JODA_SETTINGS_PASTE39(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, \
                              v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38)                                                \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                   \
  JODA_SETTINGS_PASTE38(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, \
                        v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38)
#define JODA_SETTINGS_PASTE40(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, \
                              v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39)                                           \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                   \
  JODA_SETTINGS_PASTE39(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, \
                        v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39)
#define JODA_SETTINGS_PASTE41(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, \
                              v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40)                                      \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                   \
  JODA_SETTINGS_PASTE40(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, \
                        v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40)
#define JODA_SETTINGS_PASTE42(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, \
                              v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41)                                 \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                   \
  JODA_SETTINGS_PASTE41(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, \
                        v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41)
#define JODA_SETTINGS_PASTE43(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, \
                              v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42)                            \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                   \
  JODA_SETTINGS_PASTE42(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, \
                        v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42)
#define JODA_SETTINGS_PASTE44(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, \
                              v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43)                       \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                   \
  JODA_SETTINGS_PASTE43(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, \
                        v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43)
#define JODA_SETTINGS_PASTE45(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, \
                              v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44)                  \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                   \
  JODA_SETTINGS_PASTE44(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, \
                        v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44)
#define JODA_SETTINGS_PASTE46(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, \
                              v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45)             \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                   \
  JODA_SETTINGS_PASTE45(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, \
                        v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45)
#define JODA_SETTINGS_PASTE47(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, \
                              v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46)        \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                   \
  JODA_SETTINGS_PASTE46(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, \
                        v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46)
#define JODA_SETTINGS_PASTE48(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, \
                              v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47)   \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                   \
  JODA_SETTINGS_PASTE47(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, \
                        v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47)
#define JODA_SETTINGS_PASTE49(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, \
                              v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47,   \
                              v48)                                                                                                                 \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                   \
  JODA_SETTINGS_PASTE48(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, \
                        v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48)
#define JODA_SETTINGS_PASTE50(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, \
                              v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47,   \
                              v48, v49)                                                                                                            \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                   \
  JODA_SETTINGS_PASTE49(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, \
                        v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49)
#define JODA_SETTINGS_PASTE51(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, \
                              v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47,   \
                              v48, v49, v50)                                                                                                       \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                   \
  JODA_SETTINGS_PASTE50(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, \
                        v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50)
#define JODA_SETTINGS_PASTE52(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, \
                              v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47,   \
                              v48, v49, v50, v51)                                                                                                  \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                   \
  JODA_SETTINGS_PASTE51(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26, \
                        v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51)
#define JODA_SETTINGS_PASTE53(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24,   \
                              v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47,     \
                              v48, v49, v50, v51, v52)                                                                                               \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                     \
  JODA_SETTINGS_PASTE52(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26,   \
                        v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, \
                        v52)
#define JODA_SETTINGS_PASTE54(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24,   \
                              v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47,     \
                              v48, v49, v50, v51, v52, v53)                                                                                          \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                     \
  JODA_SETTINGS_PASTE53(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26,   \
                        v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, \
                        v52, v53)
#define JODA_SETTINGS_PASTE55(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24,   \
                              v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47,     \
                              v48, v49, v50, v51, v52, v53, v54)                                                                                     \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                     \
  JODA_SETTINGS_PASTE54(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26,   \
                        v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, \
                        v52, v53, v54)
#define JODA_SETTINGS_PASTE56(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24,   \
                              v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47,     \
                              v48, v49, v50, v51, v52, v53, v54, v55)                                                                                \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                     \
  JODA_SETTINGS_PASTE55(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26,   \
                        v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, \
                        v52, v53, v54, v55)
#define JODA_SETTINGS_PASTE57(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24,   \
                              v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47,     \
                              v48, v49, v50, v51, v52, v53, v54, v55, v56)                                                                           \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                     \
  JODA_SETTINGS_PASTE56(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26,   \
                        v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, \
                        v52, v53, v54, v55, v56)
#define JODA_SETTINGS_PASTE58(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24,   \
                              v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47,     \
                              v48, v49, v50, v51, v52, v53, v54, v55, v56, v57)                                                                      \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                     \
  JODA_SETTINGS_PASTE57(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26,   \
                        v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, \
                        v52, v53, v54, v55, v56, v57)
#define JODA_SETTINGS_PASTE59(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24,   \
                              v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47,     \
                              v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58)                                                                 \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                     \
  JODA_SETTINGS_PASTE58(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26,   \
                        v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, \
                        v52, v53, v54, v55, v56, v57, v58)
#define JODA_SETTINGS_PASTE60(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24,   \
                              v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47,     \
                              v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59)                                                            \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                     \
  JODA_SETTINGS_PASTE59(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26,   \
                        v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, \
                        v52, v53, v54, v55, v56, v57, v58, v59)
#define JODA_SETTINGS_PASTE61(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24,   \
                              v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47,     \
                              v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60)                                                       \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                     \
  JODA_SETTINGS_PASTE60(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26,   \
                        v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, \
                        v52, v53, v54, v55, v56, v57, v58, v59, v60)
#define JODA_SETTINGS_PASTE62(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24,   \
                              v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47,     \
                              v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60, v61)                                                  \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                     \
  JODA_SETTINGS_PASTE61(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26,   \
                        v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, \
                        v52, v53, v54, v55, v56, v57, v58, v59, v60, v61)
#define JODA_SETTINGS_PASTE63(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24,   \
                              v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47,     \
                              v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60, v61, v62)                                             \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                     \
  JODA_SETTINGS_PASTE62(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26,   \
                        v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, \
                        v52, v53, v54, v55, v56, v57, v58, v59, v60, v61, v62)
#define JODA_SETTINGS_PASTE64(func, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24,   \
                              v25, v26, v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47,     \
                              v48, v49, v50, v51, v52, v53, v54, v55, v56, v57, v58, v59, v60, v61, v62, v63)                                        \
  JODA_SETTINGS_PASTE2(func, v1)                                                                                                                     \
  JODA_SETTINGS_PASTE63(func, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15, v16, v17, v18, v19, v20, v21, v22, v23, v24, v25, v26,   \
                        v27, v28, v29, v30, v31, v32, v33, v34, v35, v36, v37, v38, v39, v40, v41, v42, v43, v44, v45, v46, v47, v48, v49, v50, v51, \
                        v52, v53, v54, v55, v56, v57, v58, v59, v60, v61, v62, v63)

// #define JODA_SETTINGS_TO(v1) JODA_SETTINGS_j[#v1] = JODA_SETTINGS_t.v1;

template <typename T>
concept IsPrimitive_t = std::same_as<T, int> || std::same_as<T, uint32_t> || std::same_as<T, uint16_t> || std::same_as<T, float> ||
                        std::same_as<T, bool> || std::is_enum<T>::value || std::same_as<T, std::string> || std::same_as<T, unsigned char>;

template <typename T>
concept IsBaseSetting_t = !(std::same_as<T, int> || std::same_as<T, uint32_t> || std::same_as<T, uint16_t> || std::same_as<T, float> ||
                            std::same_as<T, bool> || std::is_enum<T>::value || std::same_as<T, std::string> || std::same_as<T, unsigned char>);

template <IsBaseSetting_t T>
inline void toLog(const T &v1, auto &settingsParserLog)
{
  v1.getErrorLogRecursive(settingsParserLog);
}

template <IsPrimitive_t T>
inline void toLog(const std::optional<T> &v1, auto & /*settingsParserLog*/)
{
}

template <IsBaseSetting_t T>
inline void toLog(const std::optional<T> &v1, auto &settingsParserLog)
{
  if(v1.has_value()) {
    v1.value().getErrorLogRecursive(settingsParserLog);
  }
}

template <IsPrimitive_t T>
inline void toLog(const T & /*v1*/, auto & /*settingsParserLog*/)
{
}

template <IsBaseSetting_t T>
inline void toLog(const std::set<T> &v1, auto &settingsParserLog)
{
  for(const auto &e : v1) {
    e.getErrorLogRecursive(settingsParserLog);
  }
}

template <IsPrimitive_t T>
inline void toLog(const std::set<T> & /*v1*/, auto & /*settingsParserLog*/)
{
}

template <IsBaseSetting_t T>
inline void toLog(const std::list<T> &v1, auto &settingsParserLog)
{
  for(const auto &e : v1) {
    e.getErrorLogRecursive(settingsParserLog);
  }
}

template <IsPrimitive_t T>
inline void toLog(const std::list<T> & /*v1*/, auto & /*settingsParserLog*/)
{
}

template <IsBaseSetting_t T>
inline void toLog(const std::vector<T> &v1, auto &settingsParserLog)
{
  for(const auto &e : v1) {
    e.getErrorLogRecursive(settingsParserLog);
  }
}

template <IsPrimitive_t T>
inline void toLog(const std::vector<T> & /*v1*/, auto & /*settingsParserLog*/)
{
}

inline void toLog(const std::vector<std::vector<int32_t>> & /*v1*/, auto & /*settingsParserLog*/)
{
}

#define JODA_SETTINGS_TO(v1) toLog(v1, settingsParserLog);
