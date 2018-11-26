/* search.c
 *   by Alex Chadwick
 * 
 * Copyright (C) 2014, Alex Chadwick
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Modified by aplumafreak500 for Luma's CT Pack Launcher
 */

/* This file should ideally avoid Wii specific methods so unit testing can be
 * conducted elsewhere. */
 
#include "search.h"

#include <dirent.h>
#include <errno.h>
#include <ogc/lwp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "apploader/apploader.h"
#include "library/dolphin_os.h"
#include "library/event.h"
#include "search/fsm.h"
#include "search/symbol.h"
#include "main.h"
#include "threads.h"

#include "../../brainslug-modules/bslug-symbols.h"

typedef struct {
    void *address;
    bool search_fail;
} search_symbol_global_t;
typedef struct {
    void *address;
    const char *name;
} search_module_symbol_t;

search_symbol_global_t *search_symbol_globals;

#define SEARCH_MODULE_SYMBOLS_CAPACITY_DEFAULT 128

search_module_symbol_t *search_module_symbols;

size_t search_module_symbols_count = 0;
size_t search_module_symbols_capacity = 0;
size_t search_module_symbols_sorted = 0;

event_t search_event_complete;

bool search_has_error;
bool search_has_info;

static fsm_t *search_fsm = NULL;

static void *search_symbol__start;

static void *Search_Main(void *arg);
static void Search_SymbolsLoad(void);
static void Search_Load(const char *xml, const char *name);
static bool Search_BuildFSM(void);
static void Search_SymbolMatch(symbol_index_t symbol, uint8_t *addr);
static int Search_ModuleSymbolCompare(const void *left, const void *right);

bool Search_Init(void) {
    return Event_Init(&search_event_complete);
}

bool Search_RunBackground(void) {
    int ret;
    lwp_t thread;
    
    ret = LWP_CreateThread(
        &thread, &Search_Main,
        NULL, NULL, 0, THREAD_PRIO_IO);
        
    if (ret) {
        errno = ENOMEM;
        return false;
    }
    
    return true;
}

static void *Search_Main(void *arg) {
    Search_SymbolsLoad();
    
    if (symbol_count > 0) {
        symbol_index_t i;
        
        if (!Search_BuildFSM())
           goto exit_error;
        
        assert(search_fsm != NULL);
        
        search_symbol_globals =
            malloc(symbol_count * sizeof(*search_symbol_globals));
        
        for (i = 0; i < symbol_count; i++) {
            search_symbol_globals[i].address = NULL;
            search_symbol_globals[i].search_fail = false;
        }
        
        Event_Wait(&apploader_event_complete);
        
        if (apploader_app0_start != NULL) {
            assert(apploader_app0_end != NULL);
            assert(apploader_app0_end >= apploader_app0_start);
            
            FSM_Run(
                search_fsm, apploader_app0_start,
                apploader_app0_end - apploader_app0_start,
                &Search_SymbolMatch);
        }
        
        FSM_Free(search_fsm);
        search_fsm = NULL;
    }
    
    Event_Trigger(&search_event_complete);
    return NULL;
exit_error:
    printf("Search_Main: exit_error\n");
    search_has_error = true;
    Event_Trigger(&search_event_complete);
    return NULL;
}

#define xml_count 15

static const struct {
	const char name[32];
	const char *xml;
} xml_dict[xml_count+1] = {
	{
		.name = "cache.xml",
		.xml = cache_xml,
	},
	{
		.name = "ctype.xml",
		.xml = ctype_xml,
	},
	{
		.name = "dwc.xml",
		.xml = dwc_xml,
	},
	{
		.name = "GXFrameBuf.xml",
		.xml = GXFrameBuf_xml,
	},
	{
		.name = "GXTransform.xml",
		.xml = GXTransform_xml,
	},
	{
		.name = "hook.xml",
		.xml = hook_xml,
	},
	{
		.name = "ipc.xml",
		.xml = ipc_xml,
	},
	{
		.name = "md5.xml",
		.xml = md5_xml,
	},
	{
		.name = "OSMutex.xml",
		.xml = OSMutex_xml,
	},
	{
		.name = "OSThread.xml",
		.xml = OSThread_xml,
	},
	{
		.name = "OSTime.xml",
		.xml = OSTime_xml,
	},
	{
		.name = "OSUtf.xml",
		.xml = OSUtf_xml,
	},
	{
		.name = "stdio.xml",
		.xml = stdio_xml,
	},
	{
		.name = "string.xml",
		.xml = string_xml,
	},
	{
		.name = "vi.xml",
		.xml = vi_xml,
	},
	{
		.name = "End of list",
		.xml = symbols_end,
	}
};

static void Search_SymbolsLoad(void) {
	u32 i;
	for (i = 0; i < xml_count; i++) {
		Search_Load(xml_dict[i].xml, xml_dict[i].name);
	}
}

static void Search_Load(const char *xml, const char *name) {
    if (name == NULL) {
    	char tmp_path[17];
    	sprintf(tmp_path, "xml_mem_0x%08lx", (u32)xml);
    	name = tmp_path;
    }
    if (!Symbol_ParseString(xml, name)) {
        printf("Could not load symbol file %s.\n", name);
        search_has_info = true;
    }
}

static bool Search_BuildFSM(void) {
    bool result = false;
    symbol_index_t i;
    fsm_t *fsm_final = NULL;
    
    for (i = 0; i < symbol_count; i++) {
        fsm_t *fsm;

        fsm = FSM_Create(i);
        if (fsm == NULL)
            goto exit_error;
        
        if (fsm_final == NULL) {
            fsm_final = fsm;
        } else {
            fsm_t *fsm_merge;
            
            fsm_merge = FSM_Merge(fsm_final, fsm);
            if (fsm_merge == NULL) {
                FSM_Free(fsm);
                goto exit_error;
            }
            
            FSM_Free(fsm);
            FSM_Free(fsm_final);
            
            fsm_final = fsm_merge;
        }
    }
    
    /* store the final result and make sure we don't free it!! */
    search_fsm = fsm_final;
    fsm_final = NULL;
    
    result = true;
exit_error:
    if (fsm_final != NULL)
        FSM_Free(fsm_final);
    return result;
}

static void Search_SymbolMatch(symbol_index_t symbol, uint8_t *addr) {
    symbol_t *symbol_data;
    
    symbol_data = Symbol_GetSymbol(symbol);
    if (symbol_data == NULL)
        return;
    
    if (symbol_data->debugging) {
        printf("\t%p: found %s\n", addr, symbol_data->name);
        search_has_info = true;
    }
    
    if (search_symbol_globals[symbol].address == NULL) {
        search_symbol_globals[symbol].address = addr;
    } else {
        /* rarely, a symbol is included twice (ex strlen in RMCP). Prevent this
         * causing problems if there are no relocations. */
        if (symbol_data->mask != NULL &&
            symbol_data->data_size == symbol_data->size) {
            
            size_t i;
            
            for (i = 0; i < symbol_data->data_size; i++)
                if (symbol_data->mask[i] != 0xff) {
                    search_symbol_globals[symbol].search_fail = true;
                    break;
                }
        } else
            search_symbol_globals[symbol].search_fail = true;
    }
}

bool Search_SymbolAdd(const char *name, void *address) {
    size_t index;
    
    assert(name != NULL);
    
    if (search_module_symbols_count == search_module_symbols_capacity) {
        if (search_module_symbols_capacity == 0) {
            assert(search_module_symbols == NULL);
            
            search_module_symbols =
                malloc(sizeof(*search_module_symbols) *
                SEARCH_MODULE_SYMBOLS_CAPACITY_DEFAULT);
            if (search_module_symbols == NULL)
                return false;
            
            search_module_symbols_capacity = SEARCH_MODULE_SYMBOLS_CAPACITY_DEFAULT;
        } else {
            assert(search_module_symbols != NULL);
            void *alloc;
            
            alloc = realloc(
                search_module_symbols,
                sizeof(*search_module_symbols) * 2 *
                search_module_symbols_capacity);
            if (alloc == NULL)
                return false;
            
            search_module_symbols = alloc;
            search_module_symbols_capacity *= 2;
        }
    }
    
    assert(search_module_symbols != NULL);
    assert(search_module_symbols_count < search_module_symbols_capacity);
    
    index = search_module_symbols_count++;
    
    search_module_symbols[index].address = address;
    search_module_symbols[index].name = strdup(name);
    if (search_module_symbols[index].name == NULL) {
        search_module_symbols_count--;
        return false;
    }
    
    return true;
}
bool Search_SymbolReplace(const char *name, void *address) {
    symbol_alphabetical_index_t symbol_global;
    
    assert(name != NULL);
    
    if (strcmp(name, "_start") == 0) {
        search_symbol__start = address;
        return true;
    }
        
    /* The symbol search could in theory have multiple versions of a symbol,
     * for example if there are multiple versions of a method in the wild from
     * different versions of the library. Therefore, we can't just change the
     * value at Symbol_SearchSymbol, we must check the ones after it
     * sequentially too. */
    for (symbol_global = Symbol_SearchSymbol(name);
         symbol_global != SYMBOL_NULL && symbol_global < symbol_count;
         symbol_global++) {
         
        symbol_t *symbol = Symbol_GetSymbolAlphabetical(symbol_global);
        
        if (strcmp(symbol->name, name) != 0)
            break;
        
        if (search_symbol_globals[symbol->index].address != NULL &&
            search_symbol_globals[symbol->index].search_fail == false) {
            
            /* Duplicated symbol! */
            search_symbol_globals[symbol->index].address = address;
        }
    }
    if (symbol_global == SYMBOL_NULL)
        return false;
        
    return true;
}
void *Search_SymbolLookup(const char *name) {
    symbol_alphabetical_index_t symbol_global;
    search_module_symbol_t *symbol_module, key_module;
    void *result;
    
    assert(name != NULL);
    
    if (search_module_symbols_sorted != search_module_symbols_count) {
        qsort(
            search_module_symbols, search_module_symbols_count,
            sizeof(*search_module_symbols), &Search_ModuleSymbolCompare);
        search_module_symbols_sorted = search_module_symbols_count;
    }
    assert(search_module_symbols_sorted == search_module_symbols_count);
    
    key_module.name = name;
    
    symbol_module = bsearch(
        &key_module, search_module_symbols, search_module_symbols_count,
            sizeof(*search_module_symbols), &Search_ModuleSymbolCompare);
            
    if (symbol_module != NULL)
        return symbol_module->address;
    
    if (strcmp(name, "_start") == 0) {
        if (search_symbol__start != NULL)
            return search_symbol__start;
        return apploader_game_entry_fn;
    } else if (strcmp(name, "bslug_game_start") == 0) {
        return apploader_app0_start;
    } else if (strcmp(name, "bslug_game_end") == 0) {
        return apploader_app0_end;
    }
    
    result = NULL;
    
    /* The symbol search could in theory have multiple versions of a symbol,
     * for example if there are multiple versions of a method in the wild from
     * different versions of the library. Therefore, we can't just check the
     * value at Symbol_SearchSymbol, we must check the ones after it
     * sequentially too. If we have two symbols with the same name at different
     * addresses, we return NULL. */
    for (symbol_global = Symbol_SearchSymbol(name);
         symbol_global != SYMBOL_NULL && symbol_global < symbol_count;
         symbol_global++) {
         
        symbol_t *symbol = Symbol_GetSymbolAlphabetical(symbol_global);
        
        if (strcmp(symbol->name, name) != 0)
            break;
        
        if (search_symbol_globals[symbol->index].address != NULL &&
            search_symbol_globals[symbol->index].search_fail == false) {
            
            /* Duplicated symbol! */
            if (result != NULL &&
                result != search_symbol_globals[symbol->index].address) {
                
                printf(
                    "Warning: Duplicated symbol %s (%p, %p)\n",
                    symbol->name, result,
                    search_symbol_globals[symbol->index].address);
                search_has_info = true;
                return NULL;
            }
            
            result = search_symbol_globals[symbol->index].address;
        }
    }
    
    return result;
}

static int Search_ModuleSymbolCompare(const void *left, const void *right) {
    return strcmp(
        ((const search_module_symbol_t *)left)->name,
        ((const search_module_symbol_t *)right)->name);
}
