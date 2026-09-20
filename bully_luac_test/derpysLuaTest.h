/* DERPY'S SCRIPT LOADER: LUA API

	this header is a combination of lua.h and lauxlib.h
	but functions with known addresses are defined to be called through pointers
	this means you *do not* have to link against any Lua libraries to use Lua functions

	some functions aren't in-game though, so they're still just declared like normal
	you'll need to link your own Lua libraries to be able to use those functions

	missing functions (you can't use these without linking your own libraries)
		luaL_addstring luaL_checkudata luaL_findstring luaL_getmetatable luaL_newmetatable
		luaL_prepbuffer luaL_typerror lua_cpcall lua_dofile lua_dump lua_equal
		lua_isuserdata lua_pushupvalues lua_tocfunction lua_version

	using the full Lua library (*not needed* unless you need a missing function)
		link against your own Lua 5.0.2 libraries configured to use float for numbers
		declare void luaC_collectgarbage(); in your code so you can patch it
		patch &luaC_collectgarbage to instantly do a near jump to 0x740F20

*/

#ifndef LUA_NUMBER
#define LUA_NUMBER float
#endif

/*
** $Id: lua.h,v 1.175b 2003/03/18 12:31:39 roberto Exp $
** Lua - An Extensible Extension Language
** Tecgraf: Computer Graphics Technology Group, PUC-Rio, Brazil
** http://www.lua.org	mailto:info@lua.org
** See Copyright Notice at the end of this file
*/


#ifndef lua_h
#define lua_h

#include <stdarg.h>
#include <stddef.h>


#define LUA_VERSION	"Lua 5.0.2"
#define LUA_COPYRIGHT	"Copyright (C) 1994-2004 Tecgraf, PUC-Rio"
#define LUA_AUTHORS 	"R. Ierusalimschy, L. H. de Figueiredo & W. Celes"



/* option for multiple returns in `lua_pcall' and `lua_call' */
#define LUA_MULTRET	(-1)


/*
** pseudo-indices
*/
#define LUA_REGISTRYINDEX	(-10000)
#define LUA_GLOBALSINDEX	(-10001)
#define lua_upvalueindex(i)	(LUA_GLOBALSINDEX-(i))


/* error codes for `lua_load' and `lua_pcall' */
#define LUA_ERRRUN	1
#define LUA_ERRFILE	2
#define LUA_ERRSYNTAX	3
#define LUA_ERRMEM	4
#define LUA_ERRERR	5


typedef struct lua_State lua_State;

typedef int (*lua_CFunction) (lua_State* L);


/*
** functions that read/write blocks when loading/dumping Lua chunks
*/
typedef const char* (*lua_Chunkreader) (lua_State* L, void* ud, size_t* sz);

typedef int (*lua_Chunkwriter) (lua_State* L, const void* p,
	size_t sz, void* ud);


/*
** basic types
*/
#define LUA_TNONE	(-1)

#define LUA_TNIL	0
#define LUA_TBOOLEAN	1
#define LUA_TLIGHTUSERDATA	2
#define LUA_TNUMBER	3
#define LUA_TSTRING	4
#define LUA_TTABLE	5
#define LUA_TFUNCTION	6
#define LUA_TUSERDATA	7
#define LUA_TTHREAD	8


/* minimum Lua stack available to a C function */
#define LUA_MINSTACK	20


/*
** generic extra include file
*/
#ifdef LUA_USER_H
#include LUA_USER_H
#endif


/* type of numbers in Lua */
#ifndef LUA_NUMBER
typedef double lua_Number;
#else
typedef LUA_NUMBER lua_Number;
#endif


/* mark for all API functions */
#ifndef LUA_API
#define LUA_API		extern
#endif


/*
** state manipulation
*/
#define lua_open (*(lua_State*(__cdecl*)(void))0x742010)
#define lua_close (*(void(__cdecl*)(lua_State *L))0x7420B0)
#define lua_newthread (*(lua_State*(__cdecl*)(lua_State *L))0x73AE60)

#define lua_atpanic (*(lua_CFunction(__cdecl*)(lua_State *L,lua_CFunction panicf))0x73AE40)


/*
** basic stack manipulation
*/
#define lua_gettop (*(int(__cdecl*)(lua_State *L))0x73AEA0)
#define lua_settop (*(void(__cdecl*)(lua_State *L,int idx))0x73AEB0)
#define lua_pushvalue (*(void(__cdecl*)(lua_State *L,int idx))0x73AFE0)
#define lua_remove (*(void(__cdecl*)(lua_State *L,int idx))0x73AF00)
#define lua_insert (*(void(__cdecl*)(lua_State *L,int idx))0x73AF40)
#define lua_replace (*(void(__cdecl*)(lua_State *L,int idx))0x73AF90)
#define lua_checkstack (*(int(__cdecl*)(lua_State *L,int sz))0x73AD90)

#define lua_xmove (*(void(__cdecl*)(lua_State *from,lua_State *to,int n))0x73ADF0)


/*
** access functions (stack -> C)
*/

#define lua_isnumber (*(int(__cdecl*)(lua_State *L,int idx))0x73B0C0)
#define lua_isstring (*(int(__cdecl*)(lua_State *L,int idx))0x73B110)
#define lua_iscfunction (*(int(__cdecl*)(lua_State *L,int idx))0x73B080)
LUA_API int lua_isuserdata(lua_State* L, int idx);
#define lua_type (*(int(__cdecl*)(lua_State *L,int idx))0x73B030)
#define lua_typename (*(const char*(__cdecl*)(lua_State *L,int tp))0x73B060)

LUA_API int lua_equal(lua_State* L, int idx1, int idx2);
#define lua_rawequal (*(int(__cdecl*)(lua_State *L,int idx1,int idx2))0x73B150)
#define lua_lessthan (*(int(__cdecl*)(lua_State *L,int idx1,int idx2))0x73B1B0)

#define lua_tonumber (*(lua_Number(__cdecl*)(lua_State *L,int idx))0x73B220)
#define lua_toboolean (*(int(__cdecl*)(lua_State *L,int idx))0x73B270)
#define lua_tostring (*(const char*(__cdecl*)(lua_State *L,int idx))0x73B2B0)
#define lua_strlen (*(size_t(__cdecl*)(lua_State *L,int idx))0x73B320)
LUA_API lua_CFunction lua_tocfunction(lua_State* L, int idx);
#define lua_touserdata (*(void*(__cdecl*)(lua_State *L,int idx))0x73B380)
#define lua_tothread (*(lua_State*(__cdecl*)(lua_State *L,int idx))0x73B3D0)
#define lua_topointer (*(const void*(__cdecl*)(lua_State *L,int idx))0x73B410)


/*
** push functions (C -> stack)
*/
#define lua_pushnil (*(void(__cdecl*)(lua_State *L))0x73B480)
#define lua_pushnumber (*(void(__cdecl*)(lua_State *L,lua_Number n))0x73B4A0)
#define lua_pushlstring (*(void(__cdecl*)(lua_State *L,const char *s,size_t l))0x73B4C0)
#define lua_pushstring (*(void(__cdecl*)(lua_State *L,const char *s))0x73B500)
#define lua_pushvfstring (*(const char*(__cdecl*)(lua_State *L,const char *fmt,va_list argp))0x73B570)
#define lua_pushfstring (*(const char*(__cdecl*)(lua_State *L,const char *fmt,...))0x73B5A0)
#define lua_pushcclosure (*(void(__cdecl*)(lua_State *L,lua_CFunction fn,int n))0x73B5D0)
#define lua_pushboolean (*(void(__cdecl*)(lua_State *L,int b))0x73B650)
#define lua_pushlightuserdata (*(void(__cdecl*)(lua_State *L,void *p))0x73B670)


/*
** get functions (Lua -> stack)
*/
#define lua_gettable (*(void(__cdecl*)(lua_State *L,int idx))0x73B690)
#define lua_rawget (*(void(__cdecl*)(lua_State *L,int idx))0x73B6E0)
#define lua_rawgeti (*(void(__cdecl*)(lua_State *L,int idx,int n))0x73B730)
#define lua_newtable (*(void(__cdecl*)(lua_State *L))0x73B780)
#define lua_newuserdata (*(void*(__cdecl*)(lua_State *L,size_t sz))0x73BC70)
#define lua_getmetatable (*(int(__cdecl*)(lua_State *L,int objindex))0x73B7C0)
#define lua_getfenv (*(void(__cdecl*)(lua_State *L,int idx))0x73B830)


/*
** set functions (stack -> Lua)
*/
#define lua_settable (*(void(__cdecl*)(lua_State *L,int idx))0x73B890)
#define lua_rawset (*(void(__cdecl*)(lua_State *L,int idx))0x73B8D0)
#define lua_rawseti (*(void(__cdecl*)(lua_State *L,int idx,int n))0x73B920)
#define lua_setmetatable (*(int(__cdecl*)(lua_State *L,int objindex))0x73B970)
#define lua_setfenv (*(int(__cdecl*)(lua_State *L,int idx))0x73B9D0)


/*
** `load' and `call' functions (load and run Lua code)
*/
#define lua_call (*(void(__cdecl*)(lua_State *L,int nargs,int nresults))0x73BA20)
#define lua_pcall (*(int(__cdecl*)(lua_State *L,int nargs,int nresults,int errfunc))0x73BA70)
LUA_API int lua_cpcall(lua_State* L, lua_CFunction func, void* ud);
#define lua_load (*(int(__cdecl*)(lua_State *L,lua_Chunkreader reader,void *dt,const char *chunkname))0x73BAE0)

LUA_API int lua_dump(lua_State* L, lua_Chunkwriter writer, void* data);


/*
** coroutine functions
*/
#define lua_yield (*(int(__cdecl*)(lua_State *L,int nresults))0x73FEF0)
#define lua_resume (*(int(__cdecl*)(lua_State *L,int narg))0x740110)

/*
** garbage-collection functions
*/
#define lua_getgcthreshold (*(int(__cdecl*)(lua_State *L))0x73BB30)
#define lua_getgccount (*(int(__cdecl*)(lua_State *L))0x73BB40)
#define lua_setgcthreshold (*(void(__cdecl*)(lua_State *L,int newthreshold))0x73BB50)

/*
** miscellaneous functions
*/

LUA_API const char* lua_version(void);

#define lua_error (*(int(__cdecl*)(lua_State *L))0x73BB90)

#define lua_next (*(int(__cdecl*)(lua_State *L,int idx))0x73BBA0)

#define lua_concat (*(void(__cdecl*)(lua_State *L,int n))0x73BBF0)



/*
** ===============================================================
** some useful macros
** ===============================================================
*/

#define lua_boxpointer(L,u) \
	(*(void **)(lua_newuserdata(L, sizeof(void *))) = (u))

#define lua_unboxpointer(L,i)	(*(void **)(lua_touserdata(L, i)))

#define lua_pop(L,n)		lua_settop(L, -(n)-1)

#define lua_register(L,n,f) \
	(lua_pushstring(L, n), \
	 lua_pushcfunction(L, f), \
	 lua_settable(L, LUA_GLOBALSINDEX))

#define lua_pushcfunction(L,f)	lua_pushcclosure(L, f, 0)

#define lua_isfunction(L,n)	(lua_type(L,n) == LUA_TFUNCTION)
#define lua_istable(L,n)	(lua_type(L,n) == LUA_TTABLE)
#define lua_islightuserdata(L,n)	(lua_type(L,n) == LUA_TLIGHTUSERDATA)
#define lua_isnil(L,n)		(lua_type(L,n) == LUA_TNIL)
#define lua_isboolean(L,n)	(lua_type(L,n) == LUA_TBOOLEAN)
#define lua_isnone(L,n)		(lua_type(L,n) == LUA_TNONE)
#define lua_isnoneornil(L, n)	(lua_type(L,n) <= 0)

#define lua_pushliteral(L, s)	\
	lua_pushlstring(L, "" s, (sizeof(s)/sizeof(char))-1)



/*
** compatibility macros and functions
*/


LUA_API int lua_pushupvalues(lua_State* L);

#define lua_getregistry(L)	lua_pushvalue(L, LUA_REGISTRYINDEX)
#define lua_setglobal(L,s)	\
   (lua_pushstring(L, s), lua_insert(L, -2), lua_settable(L, LUA_GLOBALSINDEX))

#define lua_getglobal(L,s)	\
		(lua_pushstring(L, s), lua_gettable(L, LUA_GLOBALSINDEX))


/* compatibility with ref system */

/* pre-defined references */
#define LUA_NOREF	(-2)
#define LUA_REFNIL	(-1)

#define lua_ref(L,lock)	((lock) ? luaL_ref(L, LUA_REGISTRYINDEX) : \
      (lua_pushstring(L, "unlocked references are obsolete"), lua_error(L), 0))

#define lua_unref(L,ref)	luaL_unref(L, LUA_REGISTRYINDEX, (ref))

#define lua_getref(L,ref)	lua_rawgeti(L, LUA_REGISTRYINDEX, ref)



/*
** {======================================================================
** useful definitions for Lua kernel and libraries
** =======================================================================
*/

/* formats for Lua numbers */
#ifndef LUA_NUMBER_SCAN
#define LUA_NUMBER_SCAN		"%lf"
#endif

#ifndef LUA_NUMBER_FMT
#define LUA_NUMBER_FMT		"%.14g"
#endif

/* }====================================================================== */


/*
** {======================================================================
** Debug API
** =======================================================================
*/


/*
** Event codes
*/
#define LUA_HOOKCALL	0
#define LUA_HOOKRET	1
#define LUA_HOOKLINE	2
#define LUA_HOOKCOUNT	3
#define LUA_HOOKTAILRET 4


/*
** Event masks
*/
#define LUA_MASKCALL	(1 << LUA_HOOKCALL)
#define LUA_MASKRET	(1 << LUA_HOOKRET)
#define LUA_MASKLINE	(1 << LUA_HOOKLINE)
#define LUA_MASKCOUNT	(1 << LUA_HOOKCOUNT)

typedef struct lua_Debug lua_Debug;  /* activation record */

typedef void (*lua_Hook) (lua_State* L, lua_Debug* ar);


#define lua_getstack (*(int(__cdecl*)(lua_State *L,int level,lua_Debug *ar))0x73E820)
#define lua_getinfo (*(int(__cdecl*)(lua_State *L,const char *what,lua_Debug *ar))0x73F370)
#define lua_getlocal (*(const char*(__cdecl*)(lua_State *L,const lua_Debug *ar,int n))0x73E8A0)
#define lua_setlocal (*(const char*(__cdecl*)(lua_State *L,const lua_Debug *ar,int n))0x73E920)
#define lua_getupvalue (*(const char*(__cdecl*)(lua_State *L,int funcindex,int n))0x73BCB0)
#define lua_setupvalue (*(const char*(__cdecl*)(lua_State *L,int funcindex,int n))0x73BD50)

#define lua_sethook (*(int(__cdecl*)(lua_State *L,lua_Hook func,int mask,int count))0x73E7B0)
#define lua_gethook (*(lua_Hook(__cdecl*)(lua_State *L))0x73E7F0)
#define lua_gethookmask (*(int(__cdecl*)(lua_State *L))0x73E800)
#define lua_gethookcount (*(int(__cdecl*)(lua_State *L))0x73E810)


#define LUA_IDSIZE	60

struct lua_Debug {
	int event;
	const char* name;	/* (n) */
	const char* namewhat;	/* (n) `global', `local', `field', `method' */
	const char* what;	/* (S) `Lua', `C', `main', `tail' */
	const char* source;	/* (S) */
	int currentline;	/* (l) */
	int nups;		/* (u) number of upvalues */
	int linedefined;	/* (S) */
	char short_src[LUA_IDSIZE]; /* (S) */
	/* private part */
	int i_ci;  /* active function */
};

/* }====================================================================== */


/******************************************************************************
* Copyright (C) 1994-2004 Tecgraf, PUC-Rio.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/


#endif

/*
** $Id: lauxlib.h,v 1.60 2003/04/03 13:35:34 roberto Exp $
** Auxiliary functions for building Lua libraries
** See Copyright Notice in lua.h
*/


#ifndef lauxlib_h
#define lauxlib_h


#include <stddef.h>
#include <stdio.h>

/* #include "lua.h" */


#ifndef LUALIB_API
#define LUALIB_API	LUA_API
#endif



typedef struct luaL_reg {
	const char* name;
	lua_CFunction func;
} luaL_reg;


#define luaL_openlib (*(void(__cdecl*)(lua_State *L,const char *libname,const luaL_reg *l,int nup))0x73BF80)
#define luaL_getmetafield (*(int(__cdecl*)(lua_State *L,int obj,const char *e))0x73BEC0)
#define luaL_callmeta (*(int(__cdecl*)(lua_State *L,int obj,const char *e))0x73BF20)
LUALIB_API int luaL_typerror(lua_State* L, int narg, const char* tname);
#define luaL_argerror (*(int(__cdecl*)(lua_State *L,int numarg,const char *extramsg))0x73C920)
#define luaL_checklstring (*(const char*(__cdecl*)(lua_State *L,int numArg,size_t *l))0x73CA40)
#define luaL_optlstring (*(const char*(__cdecl*)(lua_State *L,int numArg,const char *def,size_t *l))0x73CAB0)
#define luaL_checknumber (*(lua_Number(__cdecl*)(lua_State *L,int numArg))0x73CB10)
#define luaL_optnumber (*(lua_Number(__cdecl*)(lua_State *L,int nArg,lua_Number def))0x73CB90)

#define luaL_checkstack (*(void(__cdecl*)(lua_State *L,int sz,const char *msg))0x73BE90)
#define luaL_checktype (*(void(__cdecl*)(lua_State *L,int narg,int t))0x73C9C0)
#define luaL_checkany (*(void(__cdecl*)(lua_State *L,int narg))0x73CA10)

LUALIB_API int luaL_newmetatable(lua_State* L, const char* tname);
LUALIB_API void luaL_getmetatable(lua_State* L, const char* tname);
LUALIB_API void* luaL_checkudata(lua_State* L, int ud, const char* tname);

#define luaL_where (*(void(__cdecl*)(lua_State *L,int lvl))0x73BDF0)
#define luaL_error (*(int(__cdecl*)(lua_State *L,const char *fmt,...))0x73BE60)

LUALIB_API int luaL_findstring(const char* st, const char* const lst[]);

#define luaL_ref (*(int(__cdecl*)(lua_State *L,int t))0x73C4B0)
#define luaL_unref (*(void(__cdecl*)(lua_State *L,int t,int ref))0x73C580)

#define luaL_getn (*(int(__cdecl*)(lua_State *L,int t))0x73C1C0)
#define luaL_setn (*(void(__cdecl*)(lua_State *L,int t,int n))0x73C0F0)

#define luaL_loadfile (*(int(__cdecl*)(lua_State *L,const char *filename))0x73C660)
#define luaL_loadbuffer (*(int(__cdecl*)(lua_State *L,const char *buff,size_t sz,const char *name))0x73C7F0)



/*
** ===============================================================
** some useful macros
** ===============================================================
*/

#define luaL_argcheck(L, cond,numarg,extramsg) if (!(cond)) \
                                               luaL_argerror(L, numarg,extramsg)
#define luaL_checkstring(L,n)	(luaL_checklstring(L, (n), NULL))
#define luaL_optstring(L,n,d)	(luaL_optlstring(L, (n), (d), NULL))
#define luaL_checkint(L,n)	((int)luaL_checknumber(L, n))
#define luaL_checklong(L,n)	((long)luaL_checknumber(L, n))
#define luaL_optint(L,n,d)	((int)luaL_optnumber(L, n,(lua_Number)(d)))
#define luaL_optlong(L,n,d)	((long)luaL_optnumber(L, n,(lua_Number)(d)))


/*
** {======================================================
** Generic Buffer manipulation
** =======================================================
*/


#ifndef LUAL_BUFFERSIZE
#define LUAL_BUFFERSIZE	  BUFSIZ
#endif


typedef struct luaL_Buffer {
	char* p;			/* current position in buffer */
	int lvl;  /* number of strings in the stack (level) */
	lua_State* L;
	char buffer[LUAL_BUFFERSIZE];
} luaL_Buffer;

#define luaL_putchar(B,c) \
  ((void)((B)->p < ((B)->buffer+LUAL_BUFFERSIZE) || luaL_prepbuffer(B)), \
   (*(B)->p++ = (char)(c)))

#define luaL_addsize(B,n)	((B)->p += (n))

#define luaL_buffinit (*(void(__cdecl*)(lua_State *L,luaL_Buffer *B))0x73C490)
LUALIB_API char* luaL_prepbuffer(luaL_Buffer* B);
#define luaL_addlstring (*(void(__cdecl*)(luaL_Buffer *B,const char *s,size_t l))0x73C360)
LUALIB_API void luaL_addstring(luaL_Buffer* B, const char* s);
#define luaL_addvalue (*(void(__cdecl*)(luaL_Buffer *B))0x73C400)
#define luaL_pushresult (*(void(__cdecl*)(luaL_Buffer *B))0x73C3C0)


/* }====================================================== */



/*
** Compatibility macros and functions
*/

LUALIB_API int lua_dofile(lua_State* L, const char* filename);
#define lua_dostring (*(int(__cdecl*)(lua_State *L,const char *str))0x73C8F0)
#define lua_dobuffer (*(int(__cdecl*)(lua_State *L,const char *buff,size_t sz,const char *n))0x73C890)


#define luaL_check_lstr 	luaL_checklstring
#define luaL_opt_lstr 	luaL_optlstring 
#define luaL_check_number 	luaL_checknumber 
#define luaL_opt_number	luaL_optnumber
#define luaL_arg_check	luaL_argcheck
#define luaL_check_string	luaL_checkstring
#define luaL_opt_string	luaL_optstring
#define luaL_check_int	luaL_checkint
#define luaL_check_long	luaL_checklong
#define luaL_opt_int	luaL_optint
#define luaL_opt_long	luaL_optlong


#endif


