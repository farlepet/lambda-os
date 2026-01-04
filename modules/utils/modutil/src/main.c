#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#include <lambda-json.h>

static void _print_help(void);
static int  _cmd_help(int, char **);
static int  _cmd_common(int, char **);

static const struct {
    const char *name;
    int (*func)(int, char **);
    const char *desc;
    const char *help;
} _commands[] = {
    { "help",  _cmd_help,   "List commands, or get help on specific command",
      "Usage: modtool help <command>" },
    { "check", NULL,        "Checks validity of JSON module configuration",
      "Usage: modtool check <module.json>" },
    { "deps",  _cmd_common, "List module dependencies, one per line",
      "Usage: modtool deps <module.json>" },
    { "cgen",  _cmd_common, "Generate C source for MODULE_HEADER",
      "Usage: modtool check <module.json> <output>\n"
      "  If <output> not supplied, output is printed to stdout." }
};
#define N_COMMANDS (sizeof(_commands) / sizeof(_commands[0]))


int main(int argc, char **argv, char **envp) {
    (void)envp;
    
    if(argc < 2) {
        _print_help();
        return 1;
    }

    for(size_t i = 0; i < N_COMMANDS; i++) {
        if(!strcmp(argv[1], _commands[i].name)) {
            if(_commands[i].func) {
                return _commands[i].func(argc, argv);
            } else {
                fprintf(stderr, "FATAL: Command entry for `%s` does not have a valid function pointer!\n", argv[1]);
                return 1;
            }
        }
    }

    fprintf(stderr, "Command `%s` invalid!\n", argv[1]);

    return 1;
}

static void _print_help(void) {
    fprintf(stderr, "Usage: modutil <command>\n\n"
                    "Available commands:\n");
    
    for(size_t i = 0; i < N_COMMANDS; i++) {
        fprintf(stderr, "  %s: %s\n", _commands[i].name, _commands[i].desc);
    }
}

static int _cmd_help(int argc, char **argv) {
    if(argc == 2) {
        _print_help();
        return 0;
    } else if(argc == 3) {
        for(size_t i = 0; i < N_COMMANDS; i++) {
            if(!strcmp(argv[2], _commands[i].name)) {
                fprintf(stderr, "%s: %s\n\n%s\n", _commands[i].name, _commands[i].desc, _commands[i].help);
                return 0;
            }
        }
        return 1;
    } else {
        fprintf(stderr, "Too many arguments to help command\n");
        return 1;
    }
}

static int _common_deps(ljson_t *json) {
    /* Looking for "requirements"[*] */
    if(json->root.type != LJSON_ITEMTYPE_MAP) {
        fprintf(stderr, "Root JSON object is not a map!\n");
        return 1;
    }

    ljson_item_t *_item;

    if(!(_item = ljson_map_search_type(json->root.map, "requirements", LJSON_ITEMTYPE_ARRAY))) {
        fprintf(stderr, "Could not find 'requirements' array!\n");
        return 1;
    }
    ljson_array_t *reqarr = _item->array;

    for(size_t i = 0; i < reqarr->count; i++) {
        if(reqarr->items[i].type != LJSON_ITEMTYPE_STRING) {
            fprintf(stderr, "Non-string type in 'requirements' array!\n");
            return 1;
        }
        printf("%s.mod\n", reqarr->items[i].str);
    }

    return 0;
}

static int _common_cgen(ljson_t *json) {
    /* TODO: Allow writing to file */
    int fd = STDOUT_FILENO;

    /* TODO: Allow for some of these to be optional */

    const char    *ident   = NULL;
    const char    *name    = NULL;
    const char    *desc    = NULL;
    const char    *license = NULL;
    ljson_array_t *authors = NULL;
    ljson_array_t *reqs    = NULL;

    if(json->root.type != LJSON_ITEMTYPE_MAP) {
        fprintf(stderr, "Root JSON object is not a map!\n");
        return 1;
    }
    
    ljson_item_t *_item;

    if(!(_item = ljson_map_search_type(json->root.map, "metadata", LJSON_ITEMTYPE_MAP))) {
        fprintf(stderr, "Could not find 'metadata' map!\n");
        return 1;
    }
    ljson_map_t *metamap = _item->map;

    if(!(_item = ljson_map_search_type(metamap, "ident", LJSON_ITEMTYPE_STRING))) {
        fprintf(stderr, "Could not find identifier string!\n");
        return 1;
    }
    ident = _item->str;
    
    if(!(_item = ljson_map_search_type(metamap, "name", LJSON_ITEMTYPE_STRING))) {
        fprintf(stderr, "Could not find name string!\n");
        return 1;
    }
    name = _item->str;
    
    if(!(_item = ljson_map_search_type(metamap, "description", LJSON_ITEMTYPE_STRING))) {
        fprintf(stderr, "Could not find description string!\n");
        return 1;
    }
    desc = _item->str;
    
    if(!(_item = ljson_map_search_type(metamap, "license", LJSON_ITEMTYPE_STRING))) {
        fprintf(stderr, "Could not find license string!\n");
        return 1;
    }
    license = _item->str;
    
    if(!(_item = ljson_map_search_type(metamap, "authors", LJSON_ITEMTYPE_ARRAY))) {
        fprintf(stderr, "Could not find author array!\n");
        return 1;
    }
    authors = _item->array;
    
    if(!(_item = ljson_map_search_type(json->root.map, "requirements", LJSON_ITEMTYPE_ARRAY))) {
        fprintf(stderr, "Could not find identifier string!\n");
        return 1;
    }
    reqs = _item->array;
    

    dprintf(fd, "#include <lambda/mod/module.h>\n\n"
                "extern int mod_func(uint32_t, void *);\n\n"
                "MODULE_HEADER = {\n"
                "    .head_magic   = LAMBDA_MODULE_HEAD_MAGIC,\n"
                "    .head_version = LAMBDA_MODULE_HEAD_VERSION,\n"
                "    .kernel       = LAMBDA_VERSION,\n"
                "    .function     = &mod_func,\n"
                "    .metadata     = {\n");
    dprintf(fd, "        .ident       = \"%s\",\n"
                "        .name        = \"%s\",\n"
                "        .description = \"%s\",\n"
                "        .license     = \"%s\",\n"
                "        .authors     = (char *[]){\n",
            ident, name, desc, license);
    
    for(size_t i = 0; i < authors->count; i++) {
        if(authors->items[i].type != LJSON_ITEMTYPE_STRING) {
            fprintf(stderr, "Non-string type in 'requirements' array!\n");
            return 1;
        }
        dprintf(fd, "            \"%s\",\n", authors->items[i].str);
    }
    
    dprintf(fd, "            NULL\n"
                "        },\n"
                "        .requirements = (char *[]){\n");

    for(size_t i = 0; i < reqs->count; i++) {
        if(reqs->items[i].type != LJSON_ITEMTYPE_STRING) {
            fprintf(stderr, "Non-string type in 'requirements' array!\n");
            return 1;
        }
        dprintf(fd, "            \"%s\",\n", reqs->items[i].str);
    }

    dprintf(fd, "            NULL\n"
                "        }\n"
                "    }\n"
                "};\n");
    
    return 0;
}

static int _cmd_common(int argc, char **argv) {
    if(argc != 3) {
        fprintf(stderr, "Incorrect usage\n");
        return 1;
    }

    int fd = open(argv[2], O_RDONLY);
    if(fd < 0) {
        fprintf(stderr, "Could not open '%s': %s\n", argv[2], strerror(errno));
        return 1;
    }

    struct stat st;
    if(fstat(fd, &st) < 0) {
        fprintf(stderr, "Could not stat '%s': %s\n", argv[2], strerror(errno));
        close(fd);
        return 1;
    }
    size_t sz = (size_t)st.st_size;

    char *buff = (char *)malloc(sz + 1); /* +1 for null terminator */
    if(buff == NULL) {
        fprintf(stderr, "Could not allocate %lu bytes for JSON input: %s\n",
                        sz, strerror(errno));
        return 1;
    }

    if(read(fd, buff, sz) != (ssize_t)sz) {
        fprintf(stderr, "Could not read %lu bytes from `%s`\n", sz, argv[2]);
        return 1;
    }
    buff[sz] = '\0';

    ljson_t *json = ljson_parse(buff, 0);
    if(json == NULL) {
        fprintf(stderr, "Error while parsing JSON\n");
        free(buff);
        close(fd);
        return 1;
    }

    free(buff);
    close(fd);

    int ret = 1;
    if(!strcmp(argv[1], "deps")) {
        ret = _common_deps(json);
    } else if(!strcmp(argv[1], "cgen")) {
        ret = _common_cgen(json);
    } else if(!strcmp(argv[1], "check")) {
    } else {
        fprintf(stderr, "FATAL: Partially-handled command\n");
    }
    
    ljson_destroy(json);

    return ret;
}
