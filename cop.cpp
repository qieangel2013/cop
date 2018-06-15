/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2018 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:zqf   qieangel2013                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */
#include "./lib/co_routine.h"
#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include <stack>
#include <netinet/in.h>
#include <sys/un.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
extern "C" {
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_cop.h"
}
/* If you declare any globals in php_cop.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(cop)
*/

/* True global resources - no need for thread safety here */
using namespace std;
static int le_cop;
ZEND_DECLARE_MODULE_GLOBALS(cop)
struct task_t
{
        stCoRoutine_t *co;
        zval *callback;
        zval *arg;
        zval retval;
        long fd;
};

static stack<task_t*> g_readwrite;
static stack<task_t*> g_data;

ZEND_BEGIN_ARG_INFO_EX(cop_create_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0,callback)
    ZEND_ARG_INFO(0,arg)
ZEND_END_ARG_INFO()
/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("cop.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_cop_globals, cop_globals)
    STD_PHP_INI_ENTRY("cop.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_cop_globals, cop_globals)
PHP_INI_END()
*/
/* }}} */

/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_cop_compiled(string arg)
   Return a string to confirm that the module is compiled in */
 static void php_cop_init_globals(zend_cop_globals *cop_globals)
{
    cop_globals->counter = 0;
}

void *cop_routine(void *arg){
	co_enable_hook_sys(); 
	task_t *co = (task_t*)arg;
    if (call_user_function_ex(EG(function_table), NULL, co->callback,&(co->retval),1,co->arg,0,NULL) != SUCCESS)
    {
    	php_error_docref(NULL TSRMLS_CC, E_WARNING, "参数不正确!!!");
    	co->fd=-1;
    }
    if( -1 == co->fd )
    {
      co_yield_ct();
    }
}
/*
void *cop_routine(void *arg){
	co_enable_hook_sys(); 
	char buffer[]={'x','y','z'};
	FILE *fp = fopen("/server/cop/da.txt","a+");
	fwrite(buffer,sizeof(buffer),1,fp);
	task_t *co = (task_t*)arg;
	char buf[ 1024 * 16 ];
	for(;;)
    {
		if( -1 == co->fd )
		{
			g_readwrite.push( co );
			co_yield_ct();
			continue;
	    }
	    int fd = co->fd;
		co->fd = -1;
		for(;;)
		{
			struct pollfd pf = { 0 };
			pf.fd = fd;
			pf.events = (POLLIN|POLLERR|POLLHUP);
			co_poll( co_get_epoll_ct(),&pf,1,1000);
			int ret = read( fd,buf,sizeof(buf) );
			if( ret > 0 )
			{
				
				if (call_user_function_ex(EG(function_table), NULL, co->callback,&(co->retval),1,co->arg,0,NULL) != SUCCESS)
    			{
    				php_error_docref(NULL TSRMLS_CC, E_WARNING, "参数不正确!!!");
    			}
    			
			}
			if( ret <= 0 )
			{
				// accept_routine->SetNonBlock(fd) cause EAGAIN, we should continue
				if (errno == EAGAIN)
					continue;
				//close( fd );
				break;
			}
		}
	}
	fclose(fp);
}

void *cop_accept_routine( void * )
{
	co_enable_hook_sys();
	for(;;)
	{
		if( g_readwrite.empty() )
		{
			struct pollfd pf = { 0 };
			pf.fd = -1;
			poll( &pf,1,1000);
			continue;
		}
		if( g_data.empty() )
		{
			continue;
		}
		task_t *rco = g_data.top();
		int fd = rco->fd;
		if( fd < 0 )
		{
			struct pollfd pf = { 0 };
			pf.fd = -1;
			pf.events = (POLLIN|POLLERR|POLLHUP);
			co_poll( co_get_epoll_ct(),&pf,1,1000 );
			continue;
		}
		g_data.pop();
		if( g_readwrite.empty() )
		{
			continue;
		}
		task_t *co = g_readwrite.top();
		co->fd = fd;
		g_readwrite.pop();
		co_resume( co->co );
	}
}

*/

PHP_FUNCTION(cop_create)
{
	zval *callback;
	zval *arg = NULL;
	task_t *co;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|z",&callback,&arg) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "参数不正确!!!");
        RETURN_FALSE;
	}
	COP_G(counter)++;
    task_t * task = (task_t*)calloc( 1,sizeof(task_t) );
    task->fd = COP_G(counter);
    task->callback=callback;
    task->arg=arg;
    co_create(&(task->co),NULL,cop_routine,task);
    co_resume(task->co);
   RETURN_TRUE;
}
/*
PHP_FUNCTION(cop_create)
{
	zval *callback;
	zval *arg = NULL;
	task_t *co;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "z|z",&callback,&arg) == FAILURE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "参数不正确!!!");
        RETURN_FALSE;
	}
	COP_G(counter)++;
    task_t * task = (task_t*)calloc( 1,sizeof(task_t) );
    task->fd =COP_G(counter);
    task->callback=callback;
    task->arg=arg;
    g_data.push(task);
    printf("test:%d!\n",g_data.size());
    //co_resume(task->co);
   RETURN_TRUE;
}
*/
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and
   unfold functions in source code. See the corresponding marks just before
   function definition, where the functions purpose is also documented. Please
   follow this convention for the convenience of others editing your code.
*/


/* {{{ php_cop_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_cop_init_globals(zend_cop_globals *cop_globals)
{
	cop_globals->global_value = 0;
	cop_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(cop)
{
	/* If you have INI entries, uncomment these lines
	REGISTER_INI_ENTRIES();
	*/
/*
	for (int i = 0; i < 10; ++i)
	{
		task_t * task = (task_t*)calloc( 1,sizeof(task_t) );
		task->fd=-1;
		co_create(&(task->co),NULL,cop_routine,task);
    	co_resume(task->co);
	}
	stCoRoutine_t *accept_co = NULL;
	co_create( &accept_co,NULL,cop_accept_routine,0 );
	co_resume( accept_co );

	co_eventloop( co_get_epoll_ct(),0,0 );*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(cop)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(cop)
{
#if defined(COMPILE_DL_COP) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(cop)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(cop)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "cop support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ cop_functions[]
 *
 * Every user visible function must have an entry in cop_functions[].
 */
const zend_function_entry cop_functions[] = {
	PHP_FE(cop_create,	NULL)		
	PHP_FE_END	/* Must be the last line in cop_functions[] */
};
/* }}} */

/* {{{ cop_module_entry
 */
zend_module_entry cop_module_entry = {
	STANDARD_MODULE_HEADER,
	"cop",
	cop_functions,
	PHP_MINIT(cop),
	PHP_MSHUTDOWN(cop),
	PHP_RINIT(cop),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(cop),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(cop),
	PHP_COP_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_COP
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
BEGIN_EXTERN_C()
ZEND_GET_MODULE(cop)
END_EXTERN_C()
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
