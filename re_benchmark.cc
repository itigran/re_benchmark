#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <getopt.h>
#include <stdio.h>
#include <time.h>
#include <re2/re2.h>
#include <pcre.h>
#include <regex.h>

using namespace std;

#define DBG_BREAK  __asm__ __volatile__ ("int $0x03\n")

#define TICK(X) clock_t X = clock()
#define TOCK(X) printf("%-10s: %f sec.\n", (#X), (double)(clock() - (X)) / CLOCKS_PER_SEC)

#define OPTID_REGEXFILE  	0
#define OPTID_TEXTFILE  	1
#define OPTID_TESTCOUNT  	2
#define OPTID_COMPILED  	3
#define OPTID_HELP	7
static  struct option long_options[] = {
/* name       arg     flag  val
 *        0-no arg 
 *        1-req arg
 *        2-opt arg
 */
  {"regex-file",    1     , 0   , OPTID_REGEXFILE},
  {"text-file",     1     , 0   , OPTID_TEXTFILE},
  {"test-count",    1     , 0   , OPTID_TESTCOUNT},
  {"compiled",      0     , 0   , OPTID_COMPILED},
  {"help"     ,     0     , 0   , OPTID_HELP},

  {0, 0, 0, 0}
};
struct options_help_t
{
  int opt_id;
  const char* help_str;
} options_help[]=
{
  {OPTID_REGEXFILE, "File containg regex to test. One regex per line." },
  {OPTID_TEXTFILE,  "A text file to run regex tests against" },
  {OPTID_HELP,    "this stuff." }
};

void usage()
{
  char message[] = "\n"
"This is a small regex benchmarking program.\n"
"Possible options are:\n"
;
  int i=0;
  printf("%s",message);

  while(long_options[i].name)
  {
    printf("--%s\n",long_options[i].name);
    i++;
  }
  printf("\nOption details:\n");
  i=0;
  while(long_options[i].name)
  {
    printf("--%s %s\n",long_options[i].name,long_options[i].has_arg?"<arg>":"");
    if(long_options[i].has_arg)
      printf("\twhere <arg> is %s\n",options_help[i].help_str);
    else
      printf("\t%s\n",options_help[i].help_str);

    i++;
  }
  exit(0);
}

/******************************************************************************
*                         GLIBC library tests                                 *
******************************************************************************/
static int run_glibc_tests(size_t count, const char** patterns,const char **text)
{
    for(size_t t=0;t < count; t++) {
        for(size_t p=0; patterns[p];p++){
            for( size_t l=0; text[l]; l++) {
                regex_t preg;
                regcomp(&preg, patterns[p], 1);
                regexec(&preg, text[l], strlen(text[l]), NULL, 0);
            }
        }
    }
}

static int run_glibc_comp_tests(size_t count, const char** patterns,const char **text)
{
    for(size_t p=0; patterns[p];p++){
        regex_t preg;
        regcomp(&preg, patterns[p], 1);
        for(size_t t=0;t < count; t++) {
            for( size_t l=0; text[l]; l++) {
                regexec(&preg, text[l], strlen(text[l]), NULL, 0);
            }
        }
    }
}

/******************************************************************************
*                           RE2 library tests                                 *
******************************************************************************/
static int run_re2_tests(size_t count, const char** patterns,const char **text)
{
    for(size_t t=0;t < count; t++) {
        for(size_t p=0; patterns[p];p++){
            for( size_t l=0; text[l]; l++) {
                RE2::FullMatch(text[l], RE2(patterns[p]));
            }
        }
    }
}

static int run_re2_comp_tests(size_t count, const char** patterns,const char **text)
{
    for(size_t p=0; patterns[p];p++){
        RE2 pattern(patterns[p]);
        for(size_t t=0;t < count; t++) {
            for( size_t l=0; text[l]; l++) {
                RE2::FullMatch(text[l], pattern);
            }
        }
    }
}

/******************************************************************************
*                         PCRE library tests                                  *
******************************************************************************/
static int run_pcre_tests(size_t count, const char ** patterns, const char **text)
{
    for(size_t t=0;t < count; t++) {
        for(size_t p=0; patterns[p];p++){
            for( size_t l=0; text[l]; l++) {
                pcre *reCompiled;
                pcre_extra *pcreExtra;
                int pcreExecRet;
                const char *pcreErrorStr = NULL;
                int pcreErrorOffset; 
                reCompiled = pcre_compile(patterns[p], 0, &pcreErrorStr, &pcreErrorOffset, NULL);

                /* OPTIONS (second argument) (||'ed together) can be:
                   PCRE_ANCHORED       -- Like adding ^ at start of pattern.
                   PCRE_CASELESS       -- Like m//i
                   PCRE_DOLLAR_ENDONLY -- Make $ match end of string regardless of \n's
                                              No Perl equivalent.
                   PCRE_DOTALL         -- Makes . match newlins too.  Like m//s
                   PCRE_EXTENDED       -- Like m//x
                   PCRE_EXTRA          -- 
                   PCRE_MULTILINE      -- Like m//m
                   PCRE_UNGREEDY       -- Set quantifiers to be ungreedy.  Individual quantifiers
                                              may be set to be greedy if they are followed by "?".
                   PCRE_UTF8           -- Work with UTF8 strings.
                */
                // pcre_compile returns NULL on error, and sets pcreErrorOffset & pcreErrorStr
                if(reCompiled == NULL) {
                    return -1;
                }

                //pcreExtra = pcre_study(reCompiled, 0, &pcreErrorStr);

                /* pcre_study() returns NULL for both errors and when it can not optimize the regex.  The last argument is how one checks for
                    errors (it is NULL if everything works, and points to an error string otherwise. */
                 if(pcreErrorStr != NULL) {
                    printf("ERROR: Could not study '%s': %s\n", patterns[p], pcreErrorStr);
                    return -1;
                 }

                 pcreExecRet = pcre_exec(reCompiled,
                                         pcreExtra,
                                         text[l], 
                                         strlen(text[l]),  // length of string
                                         0,                // Start looking at this point
                                         0,                // OPTIONS
                                         NULL,
                                         0);               // Length of subStrVec

                    /* pcre_exec OPTIONS (||'ed together) can be:
                       PCRE_ANCHORED -- can be turned on at this time.
                       PCRE_NOTBOL
                       PCRE_NOTEOL
                       PCRE_NOTEMPTY */

                  // Free up the regular expression.
                  pcre_free(reCompiled);
            }
        }
    }
}

static int run_pcre_comp_tests(size_t count, const char ** patterns, const char **text)
{
    for(size_t p=0; patterns[p];p++){
        pcre *reCompiled = NULL;
        pcre_extra *pcreExtra = NULL;
        int pcreExecRet;
        const char *pcreErrorStr = NULL;
        int pcreErrorOffset; 
        reCompiled = pcre_compile(patterns[p], 0, &pcreErrorStr, &pcreErrorOffset, NULL);
        if(reCompiled == NULL) {
            return -1;
        }
        pcreExtra = pcre_study(reCompiled, 0, &pcreErrorStr);

         if(pcreErrorStr != NULL) {
            printf("ERROR: Could not study '%s': %s\n", patterns[p], pcreErrorStr);
            return -1;
         }
        for(size_t t=0;t < count; t++) {
            for( size_t l=0; text[l]; l++) {
                 pcreExecRet = pcre_exec(reCompiled,
                                         pcreExtra,
                                         text[l], 
                                         strlen(text[l]),  // length of string
                                         0,                // Start looking at this point
                                         0,                // OPTIONS
                                         NULL,
                                         0);               // Length of subStrVec

            }
        }
        pcre_free(reCompiled);
    }
}


/******************************************************************************
*                             strstr tests                                    *
******************************************************************************/
static int run_strstr_tests(size_t count, const char** patterns,const char **text)
{
    for(size_t t=0;t < count; t++) {
        for(size_t p=0; patterns[p];p++){
            for( size_t l=0; text[l]; l++) {
                strstr(patterns[p], text[l]);
            }
        }
    }
}
static int run_strstr_flush_tests(size_t count, const char** patterns,const char **text)
{
    for(size_t t=0;t < count; t++) {
        for(size_t p=0; patterns[p];p++){
            for( size_t l=0; text[l]; l++) {
                strstr(patterns[p], text[l]);
            }
        }
    }
}

static char ** read_file(const char * filename)
{
    FILE * f = NULL;
    uint32_t file_len = 0;
    char * content = NULL;
    uint32_t lines = 0,i;
    char **arr = NULL;
    if( !filename )
        return NULL;
    f = fopen(filename,"r");
    if( !f )
        return NULL;

    fseek(f,0,SEEK_END);
    file_len = ftell(f);
    fseek(f,0,SEEK_SET);
    if(file_len){
        content = (char*)calloc(1,file_len+1);
        if(content) {
            if( file_len != fread(content, 1, file_len, f)){
                free(content);
                return NULL;
            }
            char *ptr = (char*)content;
            char * tmp = NULL;
            while(tmp = strsep(&ptr,"\n")) {
               lines++; 
            }
            arr = (char **)calloc(lines+1,sizeof(char*));
            if(arr) {
                i = 0;
                for(tmp = content;*tmp;tmp+=strlen(tmp)+1)
                    arr[i++] = tmp;
            }
        }
    }
    return arr;
}
int main(int argc, char **argv)
{
    const char ** regexs = NULL;
    const char ** texts = NULL;
    char c;
	int option_index;
    const char *regex_file = NULL;
    const char *text_file = NULL;
    size_t test_count = 1;
    bool compiled = false;
 
    if(argc<2)
    {
        usage();
        exit(0);
    }
    while(1)
    {
        c=getopt_long (argc, argv, "",long_options, &option_index);
        if (c == -1)
            break;
        switch (c)
        {
            case OPTID_REGEXFILE:
				if (optarg)
					regex_file = strdup(optarg);
	        break;
            case OPTID_TEXTFILE:
				if (optarg)
					text_file = strdup(optarg);
	        break;
            case OPTID_TESTCOUNT:
                test_count = (size_t)atoi(optarg);
            break;
            case OPTID_COMPILED:
                compiled = true;
            break;
            default:
                usage();
                exit(1);
        }
    }
    if(!regex_file || ! text_file) {
        printf("Missing regex/text file, exiting!");
        exit(2);
    }
    regexs = (const char **)read_file(regex_file);
    texts = (const char **)read_file(text_file);
    size_t r = 0;
    size_t l = 0;
    for(r=0;regexs[r];r++);
    size_t size = 0;
    for(l=0;texts[l];l++)
        size+=strlen(texts[l]);
    printf("Running benchmark with:\n");
    printf("\tcount      : %lu\n",test_count);
    printf("\tregex      : %lu\n",r);
    printf("\ttext       : %lu(KB), %lu lines\n",size / 1024, l);
    printf("\tregex runs : %lu\n", r * l);
    if(!regexs || !texts) {
        printf("Failed to read regex/text file(s).\n");
        exit(3);
    }
    {
        TICK(STRSTR);
        if(compiled)
            run_strstr_tests(test_count,regexs,texts);
        else 
            run_strstr_tests(test_count,regexs,texts);
        TOCK(STRSTR);
    }
    {
        TICK(PCRE);
        if(compiled)
            run_pcre_comp_tests(test_count, regexs, texts);
        else
            run_pcre_tests(test_count, regexs, texts);
        TOCK(PCRE);
    }
    {
        TICK(GLIBC);
        if(compiled)
            run_glibc_comp_tests(test_count, regexs, texts);
        else
            run_glibc_tests(test_count, regexs, texts);
        TOCK(GLIBC);
    }
    {
        TICK(RE2);
        if(compiled)
            run_re2_comp_tests(test_count,regexs,texts);
        else
            run_re2_tests(test_count,regexs,texts);
        TOCK(RE2);
    }
}

