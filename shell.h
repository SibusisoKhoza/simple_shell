#ifndef SHELL_H
#define SHELL_H
#include <unistd.h>
#include <stddef.h>

int linum(int add);
char *get_prog_name(char *name);
void my_error(char *command, int status, char *extra);
list_s *insert_node_at_index(list_s **head, unsigned int idx, char *ptr);
int delete_node_at_index(list_s **head, unsigned int index);
char **arrayify(list_s *);
list_s *listify(char **);
void signal_handler(int sig);
void __free_list(list_t *head);
void __free_list_full(list_t *head);
list_t *__get_node_at_index(list_t *head, unsigned int index);
list_t *__insert_node_at_index(list_t **head, unsigned int idx, void *ptr);
int __delete_node_at_index(list_t **head, unsigned int index);
size_t list_len(list_s *h);
list_s *add_node(list_s **head, char *ptr);
list_s *add_node_end(list_s **head, char *ptr);
void free_list(list_s *head);
void free_list_full(list_s *head);
list_s *get_node_at_index(list_s *head, unsigned int index);
void free_double_array(char **);
char **get_env();
char **do_env(char *x, char *y);
char *get_full_command(char *path, char *command);
int setenv_builtin(char **tokens);
int unsetenv_builtin(char **tokens);
int _strcmp(char *s1, char *s2);
void main_loop(char *filename);
size_t __list_len(list_t *h);
list_t *__add_node(list_t **head, void *ptr);
list_t *__add_node_end(list_t **head, void *ptr);
int _strlen(char *s);
int word_count(char *str, char *delim);
char *_strcat(char *s1, char *s2);
char *_strcpy(char *dest, char *src);
int _atoi(char *s);
int sizeof_command(char **tokens);
int _isdigit(int c);
int has_newline(char *input);
void shiftbuffer(char *input, int newline_index, int filled);
char *_itoa(int num);
char *_reverse(char *tr, int n);
int env_builtin(void);
char **get_path();
char *get_env_val(char *name);
char *find_path(char **path, char *command);
int cd_builtin(char **tokens);
char **get_builtins();
char *prep_execve(char *token);
char *read_textfile(char *filename);
int execute(char **tokens);
int exec_nb(char **tokens);
int search_ops(char **tokens);
int exec_builtin(char **tokens, int bcase);
int check_access(char *comm, char *token);
char *_memset(char *s, char b, int n);
void *do_mem(size_t size, void *ptr);
void do_exit(int fd, char *msg, int code);
ssize_t else_handle_input(char *lineptr, int stream, char *input, int filled);
ssize_t _getline(char *lineptr, int stream);
char **_strtok(char *str, char *delim);

/**
 * struct list_p2 - singly linked list
 * @ptr: a malloced string
 * @next: points to the next node
 *
 * Description: singly linked list node structure
 */
typedef struct list_p2
{
	char *ptr;
	struct list_p2 *next;
} list_s;

/**
 * struct list_p - singly linked list
 * @ptr: a malloced address
 * @next: points to the next node
 *
 * Description: singly linked list node structure
 */
typedef struct list_p
{
	void *ptr;
	struct list_p *next;
} list_t;

#endif /*SHELL_H*/
