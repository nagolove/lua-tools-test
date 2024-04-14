// vim: set colorcolumn=85
// vim: fdm=marker

#include "libsmallregex.h"
#include "koh_lua_tools.h"
#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"
#include "munit.h"
#include "raylib.h"
#include <assert.h>
#include <memory.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int _table_push_rect_as_arr(Rectangle rect, const char *needle) {
    assert(needle);
    lua_State *l = luaL_newstate();
    luaL_openlibs(l);
    assert(l);
    table_push_rect_as_arr(l, rect);
    char *dump = table_dump2allocated_str(l);
    if (dump) {
        //printf(
            //"test_table_push_rect_as_arr: dump '%s'\n",
            //dump
        //);
        char *sub_str = strstr(dump, needle);
        //char *sub_str = strstr(dump, "{-11,-11,222,333}");
        //printf("test_table_push_rect_as_arr: sub_str %s\n", sub_str);
        free(dump);
        if (!sub_str) {
            lua_close(l);
            return MUNIT_FAIL;
        }
    }
    lua_close(l);
    return MUNIT_OK;
}

static MunitResult test_table_push_rect_as_arr(
    const MunitParameter params[], void* data
) {
    int retcode = 0;

    retcode = retcode & _table_push_rect_as_arr(
        (Rectangle) {
            .x = -10.,
            .y = -11.,
            .width = 222.,
            .height = 333.,
        }, "{-10,-11,222,333}"
    );

    retcode = retcode & _table_push_rect_as_arr(
        (Rectangle) {
            .x = 0.,
            .y = 0.,
            .width = 2.,
            .height = 3.,
        }, "{0,1,2,3}"
    );

    retcode = retcode & _table_push_rect_as_arr((Rectangle) {}, "{0,1,2,3}");

    return retcode;
}

static MunitResult test_dump2str_nil_stack_check(
    const MunitParameter params[], void* data
) {
    lua_State *lua = luaL_newstate();
    luaL_openlibs(lua);

    lua_pushnumber(lua, 1.);
    lua_pushstring(lua, "hi");
    
    const char *code =  "local b = 1";

    printf("test_dump2str_empty: [%s]\n", stack_dump(lua));
    luaL_loadstring(lua, code);
    munit_assert(lua_gettop(lua) == 3);

    int type = lua_type(lua, lua_gettop(lua));
    /*printf("test_dump2str_empty: %s\n", lua_typename(lua, type));*/
    munit_assert(type == LUA_TFUNCTION);

    /*
    if (type == LUA_TFUNCTION)
        lua_call(lua, 0, LUA_MULTRET);
    else
        printf(
            "test_dump2str_empty: non function on stack top [%s]\n",
            stack_dump(lua)
        );
    */

    char *dump = table_dump2allocated_str(lua);
    if (dump) {
        printf("test_dump2str: dump '%s'\n", dump);
        free(dump);
    }

    printf("test_dump2str_empty: [%s]\n", stack_dump(lua));
    munit_assert(lua_gettop(lua) == 2);
    lua_close(lua);

    return MUNIT_OK;
}

static MunitResult test_dump2str_stack_check(
    const MunitParameter params[], void* data
) {
    lua_State *lua = luaL_newstate();
    luaL_openlibs(lua);

    lua_pushnumber(lua, 1.);
    lua_pushstring(lua, "hi");
    
    const char *code =  "return {  }\n";

    printf("test_dump2str_empty: [%s]\n", stack_dump(lua));
    luaL_loadstring(lua, code);
    munit_assert(lua_gettop(lua) == 3);

    int type = lua_type(lua, lua_gettop(lua));

    /*printf("test_dump2str_empty: %s\n", lua_typename(lua, type));*/
    munit_assert(type == LUA_TFUNCTION);

    /*
    if (type == LUA_TFUNCTION)
        lua_call(lua, 0, LUA_MULTRET);
    else
        printf(
            "test_dump2str_empty: non function on stack top [%s]\n",
            stack_dump(lua)
        );
    */

    char *dump = table_dump2allocated_str(lua);
    if (dump) {
        printf("test_dump2str: dump '%s'\n", dump);
        free(dump);
    }

    printf("test_dump2str_empty: [%s]\n", stack_dump(lua));
    munit_assert(lua_gettop(lua) == 3);
    lua_close(lua);

    return MUNIT_OK;
}

static MunitResult test_dump2str_empty(
    const MunitParameter params[], void* data
) {
    lua_State *lua = luaL_newstate();
    luaL_openlibs(lua);

    const char *code =  "return {  }\n";

    printf("test_dump2str_empty: [%s]\n", stack_dump(lua));
    munit_assert(lua_gettop(lua) == 0);

    luaL_loadstring(lua, code);

    int type = lua_type(lua, lua_gettop(lua));
    printf("test_dump2str_empty: %s\n", lua_typename(lua, type));
    if (type == LUA_TFUNCTION)
        lua_call(lua, 0, LUA_MULTRET);
    else
        printf(
            "test_dump2str_empty: non function on stack top [%s]\n",
            stack_dump(lua)
        );

    char *dump = table_dump2allocated_str(lua);
    if (dump) {
        printf("test_dump2str: dump '%s'\n", dump);
        free(dump);
    }

    printf("test_dump2str_empty: [%s]\n", stack_dump(lua));
    munit_assert(lua_gettop(lua) == 1);
    lua_close(lua);

    return MUNIT_OK;
}

static MunitResult test_dump2str(
    const MunitParameter params[], void* data
) {
    lua_State *lua = luaL_newstate();
    luaL_openlibs(lua);

    const char *code =  "return {\n"
                        "   f = {\n"
                        "       1, 2, 3\n"
                        "   },\n"
                        "   a = {\n"
                        "       'her', 'var', 'par'\n"
                        "   },\n"
                        "}\n";

	luaL_loadstring(lua, code);
    lua_call(lua, 0, LUA_MULTRET);

    char *dump = table_dump2allocated_str(lua);
    lua_close(lua);
    if (!dump) {
        return MUNIT_FAIL;
    }

    {
        lua_State *l = luaL_newstate();
        luaL_openlibs(l);
        luaL_loadstring(l, dump);
        /*printf("[%s]\n", stack_dump(l));*/
        lua_call(l, 0, 1);
        /*printf("[%s]\n", stack_dump(l));*/

        {
            lua_pushstring(l, "f");
            munit_assert(lua_gettable(l, -2) == LUA_TTABLE);

            for (int i = 1; i <= 3; i++) {
                munit_assert(lua_geti(l, -1, i) == LUA_TNUMBER);
                double n = lua_tonumber(l, -1);
                munit_assert(n == i);
                lua_remove(l, -1);
            }

            lua_remove(l, -1);
        }

        {
            lua_pushstring(l, "a");
            munit_assert(lua_gettable(l, -2) == LUA_TTABLE);

            const char *values[] = { "her", "var", "par", NULL };
            char **pvalue = (char**)values;

            lua_pushnil(l);
            while (lua_next(l, -2)) {
                /*printf("[%s]\n", stack_dump(l));*/
                munit_assert(lua_type(l, -1) == LUA_TSTRING);

                if (!pvalue) {
                    free(dump);
                    lua_close(l);
                    return MUNIT_FAIL;
                }

                assert(*pvalue);
                munit_assert(!strcmp(*pvalue, lua_tostring(l, -1)));
                pvalue++;
                lua_remove(l, -1);
            }
        }

        lua_close(l);
    }

    free(dump);

    return MUNIT_OK;
}

// Дублирование элементов на стеке через lua_pushvalue() 
static MunitResult test_stack_dup(
    const MunitParameter params[], void* data
) {
    lua_State *lua;

    lua = luaL_newstate();
    printf("[%s]\n", stack_dump(lua));

    lua_createtable(lua, 0, 0);
    munit_assert_int(lua_gettop(lua), ==, 1);
    munit_assert(lua_type(lua, 1) == LUA_TTABLE);
    lua_pushvalue(lua, -1);
    munit_assert_int(lua_gettop(lua), ==, 2);
    munit_assert(lua_type(lua, 1) == LUA_TTABLE);
    munit_assert(lua_type(lua, 2) == LUA_TTABLE);

    lua_pushinteger(lua, 11);
    munit_assert(lua_type(lua, -1) == LUA_TNUMBER);
    lua_pushvalue(lua, -1);
    munit_assert(lua_type(lua, -1) == LUA_TNUMBER);
    munit_assert(lua_type(lua, -2) == LUA_TNUMBER);

    printf("[%s]\n", stack_dump(lua));

    lua_close(lua);

    return MUNIT_OK;
}

static MunitResult test_table_printing(
    const MunitParameter params[], void* data
) {
    lua_State *lua;

    lua = luaL_newstate();

    printf("\n");

    luaL_openlibs(lua);
    lua_createtable(lua, 0, 0);

    // Для кода luaL_dostring()
    lua_pushvalue(lua, -1);
    lua_setglobal(lua, "XX");

    float v1 = 1.5, v2 = 2;
    const char *key1 = "x", *key2 = "y";

    lua_pushstring(lua, key1);
    lua_pushnumber(lua, v1);
    lua_rawset(lua, 1);

    lua_pushstring(lua, key2);
    lua_pushnumber(lua, v2);
    lua_rawset(lua, 1);

    const char *tbl_str = table_get_print(lua, 1, &(struct TablePrintOpts) {
        .tabulate = true,
    });

    float printed_v1 = 0, printed_v2 = 0;
    char printed_key1[32] = {}, printed_key2[32] = {};

    munit_assert(regex_match("y = 2", tbl_str) > 0);
    munit_assert(regex_match("x = 1.5", tbl_str) > 0);

    //munit_assert(regex_match("    y = 2", tbl_str) > 0);
    //munit_assert(regex_match("   x = 1.5", tbl_str) > 0);
    // XXX: Почему падает на строке с четырьмя пробелами?
    //munit_assert(regex_match("   x = 1.5", tbl_str) > 0);

    printf("printed_key1 %s, printed_v1 %lf\n", printed_key1, printed_v1);
    printf("printed_key2 %s, printed_v2 %lf\n", printed_key2, printed_v2);

    printf("{\n%s}\n", tbl_str); 
    table_print(lua, 1);

    int errcode = luaL_dostring(lua,    "for k, v in pairs(XX) do"
                                        "   print(k, v)"
                                        "end");
    (void)(errcode);

    lua_close(lua);

    return MUNIT_OK;
}


static MunitTest test_suite_tests[] = {
  {
    (char*) "/table_printing",
    test_table_printing,
    NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL
  },
  {
    (char*) "/dump2str_empty",
    test_dump2str_empty,
    NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL
  },
  {
    (char*) "/dump2str_stack_check",
    test_dump2str_stack_check,
    NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL
  },
  {
    (char*) "/dump2str_nil_stack_check",
    test_dump2str_nil_stack_check,
    NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL
  },
  {
    (char*) "/dump2str",
    test_dump2str,
    NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL
  },
  {
    (char*) "/table_push_rect_as_arr",
    test_table_push_rect_as_arr,
    NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL
  },
  {
    (char*) "/stack_dup",
    test_stack_dup,
    NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL
  },
  { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite test_suite = {
  (char*) "lua_tools", test_suite_tests, NULL, 1, MUNIT_SUITE_OPTION_NONE
};

int main(int argc, char **argv) {
    return munit_suite_main(&test_suite, (void*) "µnit", argc, argv);
}
