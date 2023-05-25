#include "shell.h"
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

/**
 * __list_len - size of a list_t
 * @h: struct list_t, first node
 * Return: number of nodes
 */
size_t __list_len(list_t *h)
{
	int i = 0;

	for (i = 0; h; i++, h = h->next)
		;
	return (i);
}


/**
 * __add_node - adds a new node at the beginning of a list_t list
 * @head: struct list_t, first node
 * @ptr: the pointer data
 * Return: new list_t
 */
list_t *__add_node(list_t **head, void *ptr)
{
	list_t *new = NULL;

	new = malloc(sizeof(list_t));
	if (!new)
	{
		return (NULL);
	}
	new->ptr = ptr;
	new->next = *head;

	*head = new;
	return (new);
}

/**
 * __add_node_end - adds a new node at the end of a list_t list
 * @head: struct list_t, node
 * @ptr: the pointer data
 * Return: new list_t
 */
list_t *__add_node_end(list_t **head, void *ptr)
{
	list_t *new = NULL, *last = NULL;

	new = malloc(sizeof(list_t));
	if (!new)
	{
		return (NULL);
	}

	new->ptr = ptr;
	new->next = NULL;

	if (*head == NULL)
	{
		*head = new;
		return (new);
	}

	for (last = *head; last->next != NULL; last = last->next)
		;

	last->next = new;
	return (new);
}

/**
 * __free_list - frees list_t
 * @head: linked list , list_t
 */
void __free_list(list_t *head)
{
	list_t *hold = NULL;

	while (head != NULL)
	{
		hold = head;
		head = head->next;
		free(hold);
	}
}

/**
 * __free_list_full - frees list_t and all contained pointers
 * @head: linked list , list_t
 */
void __free_list_full(list_t *head)
{
	list_t *hold = NULL;

	while (head != NULL)
	{
		hold = head;
		head = head->next;
		/* printf("EXITMEMORY: %s\n", (char *)(hold->ptr)); */
		free(hold->ptr);
		free(hold);
	}
}

/**
 * __get_node_at_index - return nth node of a linked list
 * @head: pointer to the start of linked list
 * @index: index
 * Return: the nth node
 */
list_t *__get_node_at_index(list_t *head, unsigned int index)
{
	unsigned int i = 0;

	for (i = 0; head; i++, head = (*head).next)
		if (i == index)
			return (head);
	return (NULL);
}

/**
 * __insert_node_at_index - inserts a new node at a given position
 * @head: pointer to the beginning of linked list
 * @idx: index where to insert
 * @ptr: value for ptr
 * Return: address of new node or NULL if fails
 */
list_t *__insert_node_at_index(list_t **head, unsigned int idx, void *ptr)
{
	list_t *new = NULL, *hold = *head;
	unsigned int i = 0;

	if (!idx)
	{
		new = malloc(sizeof(list_t));
		if (!new)
			return (NULL);

		(*new).ptr = ptr;
		(*new).next = *head;

		*head = new;

		return (*head);
	}

	while (hold)
	{
		if (i == (idx - 1))
		{
			new = malloc(sizeof(list_t));
			if (!new)
				return (NULL);
			(*new).ptr = ptr;
			(*new).next = (*hold).next;
			(*hold).next = new;
			return (new);
		}
		hold = (*hold).next;
		i++;
	}
	return (NULL);
}

/**
 * __delete_node_at_index - deletes a node at a given position
 * @head: pointer to the beginning of linked list
 * @index: index to be deleted
 * Return: 1 if succesful -1 if fails
 */
int __delete_node_at_index(list_t **head, unsigned int index)
{
	list_t *hold = NULL, *delete = NULL;
	unsigned int i = 0, action = 0;

	if (!*head)
		return (-1);
	for (i = 0, hold = *head; hold && index; i++, hold = (*hold).next)
		if (i == (index - 1))
		{
			action = 1;
			break;
		}

	if (action)
	{
		delete = (*hold).next;
		(*hold).next = (*delete).next;
		free(delete);
		return (1);
	}
	else if (!index && (**head).next)
	{
		delete = *head;
		*head = (*delete).next;
		free(delete);
		return (1);
	}
	else if (!index && *head)
	{
		*head = NULL;
		free(*head);
		return (1);
	}
	return (-1);
}

/**
 * _getchar - get a character
 * Return: the int value of the character or EOF
 */
int _getchar(void)
{
	char c;

	return (read(0, &c, 1) == 1 ? (unsigned char) c : EOF);
}

/**
 * else_handle_input - if buffer does not include \n or EOF
 * @lineptr: the buffer to put the data in
 * @stream: the stream to read from
 * @input: buffer
 * @filled: size of buffer
 * Return: _getline function
 */
ssize_t else_handle_input(char *lineptr, int stream, char *input, int filled)
{
	int red;
	char tmp;

	/* if the buffer is full then read until \n or EOF */
	if (filled == 4096)
	{
		/* should always fill buffer with \n or EOF at end*/
		red = 1;
		while (red && tmp != '\n')
		{
			tmp = 0;
			red = read(stream, &tmp, 1);
		}
		input[4095] = '\n';
		return (_getline(lineptr, stream));
	}
	/* if the buffer isn't full, then fill it and try again. */
	else
	{
		red = read(stream, input + filled, 4096 - filled);
		/* ctrl D was pressed if red is less */
		if (red < (4096 - filled))
			input[filled + red] = '\n';
		filled = filled + red + 1;
		return (_getline(lineptr, stream));
	}
}

/**
 * _getline - reads a number of chars from stdin
 * @lineptr: the buffer to put the data in
 * @stream: the stream to read from
 * Return: the number of bytes read
 */
ssize_t _getline(char *lineptr, int stream)
{
	static char input[4096];
	static int filled;
	int newline_index = -1, i = 0, red = 0;
	ssize_t ret = 0;

	/* if the buffer is empty, fill it */
	if (!filled)
	{
		while ((red = read(stream, input, 4096)) < 0)
		{
			perror("Read Error\n");
			return (-1);
		}
		filled = red;
		if (!red)
			return (0);
	}

	/* if the buffer contains \n or EOF */
	newline_index = has_newline(input);
	if (newline_index != -1)
	{
		for (i = 0; i <= newline_index; i++)
			lineptr[i] = input[i];
		ret = newline_index;
		if (input[ret] == '\n')
			ret = ret + 1;
		/* Shift any remaining chars to the left */
		shiftbuffer(input, newline_index + 1, filled);
		filled = filled - ret;
		return (ret);
	}
	/* if the buffer doesn't contain \n or EOF */
	else
		else_handle_input(lineptr, stream, input, filled);
	return (-1);
}

/**
  * get_builtins - list of builtin commands
  * Return: double pointer holding list of commands
  */
char **get_builtins()
{
	char **builtins;

	builtins = do_mem(sizeof(char *) * 6, NULL);

	builtins[0] = "exit";
	builtins[1] = "cd";
	builtins[2] = "env";
	builtins[3] = "setenv";
	builtins[4] = "unsetenv";
	builtins[5] = NULL;

	return (builtins);
}

/**
 * env_builtin - Lists environment
 *  Return: 0 if successfull
 */
int env_builtin(void)
{
	char **env;
	int i = 0, len = 0;

	env = get_env();
	while (env[i])
	{
		len = _strlen(env[i]);
		write(STDOUT_FILENO, env[i], len);
		write(STDOUT_FILENO, "\n", 1);
		i = i + 1;
	}
	free_double_array(env);

	return (0);
}

/**
 * setenv_builtin - Set an environment variable
 * @tokens: the tokens for the command
 * Return: the return code
 */
int setenv_builtin(char **tokens)
{
	char *ret = NULL;

	if (!tokens[1] || !tokens[2])
	{
		my_error(tokens[0], 3001, NULL);
		return (1);
	}
	do_env(NULL, tokens[1]);

	ret = do_mem(_strlen(tokens[1]) + _strlen(tokens[2]) + 2, NULL);
	_strcat(ret, tokens[1]);
	_strcat(ret, "=");
	_strcat(ret, tokens[2]);
	do_env(ret, NULL);

	return (0);
}

/**
 * unsetenv_builtin - Unset an environment variable
 * @tokens: the tokens for the command
 * Return: the return code
 */
int unsetenv_builtin(char **tokens)
{

	if (!tokens[1])
	{
		my_error(tokens[0], 3002, NULL);
		return (1);
	}
	do_env(NULL, tokens[1]);

	return (0);
}


/**
 * cd_builtin - executes cd function, changes directory
 * @tokens: command input into prompt tokenized
 * Return: the exit status
 */
int cd_builtin(char **tokens)
{
	char *HOME = NULL, *templd;
	static char *lastdir;

	(void)HOME;
	if (!lastdir)
		lastdir = do_mem(100, NULL);
	templd = do_mem(100, NULL);
	if (tokens[1] && _strcmp(tokens[1], "-") == 0)
	{
		/* go to previous directory */
		getcwd(templd, 100);
		chdir(lastdir);
		write(STDOUT_FILENO, lastdir, _strlen(lastdir));
		write(STDOUT_FILENO, "\n", 1);
		lastdir = templd;
	}
	else if (tokens[1])
	{
		/* change directory to tokens[1] */
		getcwd(lastdir, 100);
		chdir(tokens[1]);
	}
	else
	{
		/* change to home directory */
		getcwd(lastdir, 100);
		chdir(get_env_val("HOME"));
	}
	return (0);
}

char *get_env_val(char *);

/**
 * get_path - get the path in a double char pointer
 * Return: the double char pointer of the path
 */
char **get_path()
{
	char *temp = NULL;
	char **ret = NULL;

	temp = get_env_val("PATH");
	ret = _strtok(temp, ":");
	do_mem(0, temp);
	return (ret);
}

/**
 * get_env_val - get the value of an env variable
 * @name: the name of the variable to get the value of
 * Return: the string pointer to where the value part of the variable starts
 */
char *get_env_val(char *name)
{
	int i = 0, j = 0;
	char **env = NULL;
	char *res = NULL;

	env = get_env();
	while (env[i])
	{
		j = 0;
		while (env[i][j] && name[j])
		{
			if (env[i][j] != name[j])
				break;
			j++;
		}
		/* j only counts until null byte if name matches */
		if (name[j] == '\0' && env[i][j] == '=')
		{
			if (!env[i][j + 1])
				return (NULL);
			res = do_mem((_strlen((env[i]) + j + 1) + 1), NULL);
			_strcpy(res, ((env[i]) + j + 1));
			free_double_array(env);
			return (res);
		}
		i++;
	}
	free_double_array(env);
	return (NULL);
}

/**
  * find_path - finds if a command exists in a path and returns the path
  * @path: paths to search
  * @command: command to search for
  * Return: path where command is
  */
char *find_path(char **path, char *command)
{
	/* pointer for directory entry  & opendir returns a pointer of DIR type*/
	struct dirent *de = NULL;
	DIR *dr = NULL;
	int i = 0;

	if (!command || !path)
		return (NULL);
	for (i = 0; path[i]; i++)
	{
		dr = opendir(path[i]);
		/* opendir returns NULL if couldn't open directory */
		if (!dr)
		{
			write(STDOUT_FILENO, "Could not open directory\n", 25);
			closedir(dr);
		}
		else
		{
			while ((de = readdir(dr)) != NULL)
			{
				if (_strcmp((*de).d_name, command) == 0)
				{
					closedir(dr);
					return (path[i]);
				}
			}
			closedir(dr);
		}
	}
	return (NULL);
}

/**
 * get_env - get current environment as a malloc'd, NULL-terminating char**
 * Return: the environment as a char**
 */
char **get_env(void)
{
	return (do_env(NULL, NULL));
}

/**
 * get_full_command - get the command with the correct path prepended
 * @path: all of the possible paths
 * @command: the base command
 * Return: the correct path + command (leave command alone if already done)
 */
char *get_full_command(char *path, char *command)
{
	int i = 0, j = 0;
	char *res = NULL;
	char **tempsplit = NULL;

	/* if command has any / in it, then return command */
	tempsplit = _strtok(command, "/");
	if (tempsplit && tempsplit[0] && tempsplit[1])
	{
		free_double_array(tempsplit);
		return (command);
	}
	free_double_array(tempsplit);

	/* else, concat the path with the command and a slash */
	i = _strlen(path);
	j = _strlen(command);
	res = do_mem(sizeof(char) * (i + j + 1 + 1), NULL);
	_strcat(res, path);
	_strcat(res, "/");
	_strcat(res, command);
	return (res);
}

/**
 * do_env - Get the env, or add a var, or delete a var
 * @add: a variable to add
 * @delete: a variable to delete
 * Return: the current environment after any changes
 */
char **do_env(char *add, char *delete)
{
	static list_s *my_env;
	char *tmp = NULL;
	int len = 0, i = 0, j = 0;

	if (!my_env)
	{
		my_env = listify((char **)add);
		return (NULL);
	}
	if (add)
		add_node_end(&my_env, add);
	else if (delete)
	{
		len = list_len(my_env);
		for (i = 0; i < len; i++)
		{
			j = 0;
			tmp = get_node_at_index(my_env, i)->ptr;
			while (delete && tmp && delete[j] && tmp[j] != '=')
			{
				if (delete[j] != tmp[j])
					break;
				j++;
			}
			if (!(delete[j]) && tmp[j] == '=')
			{
				delete_node_at_index(&my_env, i);
				break;
			}
		}
	}
	return (arrayify(my_env));
}

/**
  * exec_builtin - execute function for builtins
  * @tokens: STDIN tokenized
  * @bcase: which builtin to execute
  * Return: 0 if succesful 1 if it fails
  */
int exec_builtin(char **tokens, int bcase)
{
	int exit = 0;
	int i = 0;

	switch (bcase)
	{
	case 1:
	{
		if (tokens[1])
		{
			for (; tokens[1][i]; i++)
				if (!_isdigit(tokens[1][i]))
				{
					do_exit(2, "numeric arguments only\n", exit);
				}
			exit = _atoi(tokens[1]);
		}
		do_exit(2, "", exit);
		break;
	}
	case 2:
		return (cd_builtin(tokens));
	case 3:
		return (env_builtin());
	case 4:
		return (setenv_builtin(tokens));
	case 5:
		return (unsetenv_builtin(tokens));
	}
	return (0);
}

/**
 * check_access - checks if path exists or if permission exists for command
 * @comm: path to command
 * @token: command
 * Return: exit condition
 */
int check_access(char *comm, char *token)
{
	int accessCode = 0;
	/* check if path exists */
	accessCode = access(comm, F_OK);
	if (accessCode)
	{
		/* not found */
		my_error(token, 2, NULL);
		return (2);
	}
	/* check if path is exucatable */
	accessCode = access(comm, X_OK);
	if (accessCode)
	{
		/* Permission denied */
		my_error(token, 126, NULL);
		return (126);
	}
	return (0);
}

/**
 * prep_execve - preps cmd by checking current path and then the PATH for cmd
 * @token: commmand to check
 * Return: command preped for execve
 */
char *prep_execve(char *token)
{
	char **envVars = NULL;
	char *comm = NULL;
	char *cwd = NULL;
	char *path = NULL;
	int accessCode = 0;

	cwd = do_mem(100, NULL);
	comm = get_full_command(cwd, token);
	accessCode = access(comm, F_OK);
	if (accessCode)
	{
		envVars = get_path();
		path = find_path(envVars, token);
		comm = get_full_command(path, token);
		free_double_array(envVars);
	}
	return (comm);
}
/**
  * exec_nb - execute function for non builtins
  * @tokens: STDIN tokenized
  * Return: the exit status of the program, 0 if successful
  */
int exec_nb(char **tokens)
{
	char *comm = NULL;
	pid_t cpid, wid;
	int status = 0, accessCode = 0;

	comm = prep_execve(tokens[0]);
	while ((accessCode = check_access(comm, tokens[0])))
		return (accessCode);
	/* fork and exec */
	cpid = fork();
	/* Fork failed - exits with error message and exit code */
	if (cpid == -1)
		do_exit(2, "Fork failed", EXIT_FAILURE);
	if (!cpid)/* child */
	{
		execve(comm, tokens, (char * const *)get_env());
		perror("");
		do_exit(2, "Couldn't exec", EXIT_FAILURE);
	}
	else/* parent */
	{
		do {
			wid = waitpid(cpid, &status, WUNTRACED);
			if (wid == -1)
			{
				perror("waitpid");
				do_exit(STDERR_FILENO, "", EXIT_FAILURE);
			}
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
		do_mem(0, comm);
	}
	return (status);
}

/**
  * search_ops - search for ;, &&, || operators
  * @tokens: tokens from std input
  * Return: 0 if none, 1 if ';', 2 if '&&' 3 if '||'
  */
int search_ops(char **tokens)
{
	int i = 0;

	if (!tokens)
	{
		return (0);
	}
	for (i = 0; tokens[i]; i++)
	{
		/* check for ;, &&, || */
		if (tokens[i][0] == ';')
			return (1);
		if (tokens[i][0] == '&' && tokens[i][1] && tokens[i][1] == '&')
			return (2);
		if (tokens[i][0] == '|' && tokens[i][1] && tokens[i][1] == '|')
			return (3);
	}
	return (0);
}

/**
 * exec_single - execute a single command
 * @tokens: the tokens for a single command
 * Return: int return value of the exec
 */
int exec_single(char **tokens)
{
	char **builtins = NULL;
	int works = 1, i = 0, checkBuiltIn = 0;

	if (!tokens || !tokens[0])
		return (0);
	builtins = get_builtins();
	/* check if its a builtin */
	for (i = 0; builtins[i]; i++)
		if (_strcmp(builtins[i], tokens[0]) == 0)
		{
			checkBuiltIn = i + 1;
			break;
		}

	if (checkBuiltIn && tokens)
		works = exec_builtin(tokens, checkBuiltIn);
	else if (tokens)
		works = exec_nb(tokens);
	return (works);
}

/**
 * get_next_commands - gets the tokens after the first && || or ;
 * @tokens: the tokens for the current command segment
 * Return: the tokens after the first && || ;, or NULL
 */
char **get_next_commands(char **tokens)
{
	char **extokens = NULL;
	int commandSize = 0, retSize = 0, i = 0;

	if (!tokens || !(tokens[0]))
		return (NULL);
	commandSize = sizeof_command(tokens);
	if (!(tokens[commandSize]) || !(tokens[commandSize + 1]))
		return (NULL);
	while (tokens[commandSize + 1 + retSize])
		retSize++;

	extokens = do_mem(sizeof(char *) * (retSize + 1), NULL);
	for (i = 0; i < retSize; i++)
	{
		extokens[i] = do_mem(_strlen(tokens[commandSize + 1 + i]) + 1, NULL);
		_strcpy(extokens[i], tokens[commandSize + 1 + i]);
	}
	extokens[i] = NULL;
	return (extokens);
}

/**
 * get_current_command - gets the command before && || or ;
 * @tokens: the tokens for the current command segment
 * Return: the command up until the first occurence of && || ; or NULL
 */
char **get_current_command(char **tokens)
{
	char **extokens = NULL;
	int commandSize = 0, i = 0;

	if (!tokens || !(tokens[0]))
		return (NULL);
	/* split tokens into separate commands if ;,&&,|| exist */
	commandSize = sizeof_command(tokens);
	if (!commandSize)
		return (NULL);
	extokens = do_mem(sizeof(char *) * (commandSize + 1), NULL);
	for (i = 0; i < commandSize; i++)
	{
		extokens[i] = do_mem(_strlen(tokens[i]) + 1, NULL);

		_strcpy(extokens[i], tokens[i]);
	}
	extokens[i] = NULL;

	return (extokens);
}

/**
  * execute - main execute function
  * @tokens: STDIN tokenized
  * Return: int if succesful
  */
int execute(char **tokens)
{
	int works = 0, op = 0;
	char **head = NULL;
	char **tail = NULL;

	op = search_ops(tokens);
	head = get_current_command(tokens);
	tail = get_next_commands(tokens);

	while (1)
	{
		if (!head || !(head[0]))
			return (works);

		works = exec_single(head);

		if (op == 3 && !works)
			return (works);
		if (op == 2 && works)
			return (works);

		op = search_ops(tail);
		head = get_current_command(tail);
		tail = get_next_commands(tail);

	}
	return (1);
}

/**
  * read_textfile - reads a file and returns string of what it read
  * @filename: name of file to read
  * Return: string of what it read
  */

char *read_textfile(char *filename)
{
	char *buff;
	int of;
	ssize_t lRead;

	if (!filename)
		return (NULL);
	/* allocate memory for content of file */
	buff = do_mem(4096, NULL);
	/* open file */
	of = open(filename, O_RDONLY);
	if (of == -1)
	{
		do_mem(0, buff);
		do_exit(2, _strcat("Can't open ", filename), 127);
	}

	/* read file up to the size of the buffer 4096 */
	lRead = read(of, buff, 4096);
	if (lRead < 1)
	{
		do_mem(0, buff);
		close(of);
		return (NULL);
	}

	/* check if close fails? */
	close(of);

	return (buff);
}

/**
 * list_len - size of a list_s
 * @h: struct list_s, first node
 * Return: number of nodes
 */
size_t list_len(list_s *h)
{
	int i = 0;

	for (i = 0; h; i++, h = h->next)
		;
	return (i);
}


/**
 * add_node - adds a new node at the beginning of a list_s list
 * @head: struct list_s, first node
 * @ptr: the pointer data
 * Return: new list_s
 */
list_s *add_node(list_s **head, char *ptr)
{
	list_s *new = NULL;

	new = do_mem(sizeof(list_s), NULL);
	if (!new)
	{
		return (NULL);
	}
	new->ptr = ptr;
	new->next = *head;

	*head = new;
	return (new);
}

/**
 * add_node_end - adds a new node at the end of a list_t list
 * @head: struct list_s, node
 * @ptr: the pointer data
 * Return: new list_s
 */
list_s *add_node_end(list_s **head, char *ptr)
{
	list_s *new = NULL, *last = NULL;

	new = do_mem(sizeof(list_s), NULL);
	if (!new)
	{
		return (NULL);
	}

	new->ptr = ptr;
	new->next = NULL;

	if (*head == NULL)
	{
		*head = new;
		return (new);
	}

	for (last = *head; last->next != NULL; last = last->next)
		;

	last->next = new;
	return (new);
}

/**
 * free_list - frees list_s
 * @head: linked list , list_s
 */
void free_list(list_s *head)
{
	list_s *hold = NULL;

	while (head != NULL)
	{
		hold = head;
		head = head->next;
		do_mem(0, hold);
	}
}

/**
 * free_list_full - frees list_s and all contained pointers
 * @head: linked list , list_s
 */
void free_list_full(list_s *head)
{
	list_s *hold = NULL;

	while (head != NULL)
	{
		hold = head;
		head = head->next;
		do_mem(0, hold->ptr);
		do_mem(0, hold);
	}
}

/**
 * get_node_at_index - return nth node of a linked list
 * @head: pointer to the start of linked list
 * @index: index
 * Return: the nth node
 */
list_s *get_node_at_index(list_s *head, unsigned int index)
{
	unsigned int i = 0;

	for (i = 0; head; i++, head = (*head).next)
		if (i == index)
			return (head);
	return (NULL);
}

/**
 * insert_node_at_index - inserts a new node at a given position
 * @head: pointer to the beginning of linked list
 * @idx: index where to insert
 * @ptr: value for ptr
 * Return: address of new node or NULL if fails
 */
list_s *insert_node_at_index(list_s **head, unsigned int idx, char *ptr)
{
	list_s *new = NULL, *hold = *head;
	unsigned int i = 0;

	if (!idx)
	{
		new = do_mem(sizeof(list_s), NULL);
		if (!new)
			return (NULL);

		(*new).ptr = ptr;
		(*new).next = *head;

		*head = new;

		return (*head);
	}

	while (hold)
	{
		if (i == (idx - 1))
		{
			new = do_mem(sizeof(list_s), NULL);
			if (!new)
				return (NULL);
			(*new).ptr = ptr;
			(*new).next = (*hold).next;
			(*hold).next = new;
			return (new);
		}
		hold = (*hold).next;
		i++;
	}
	return (NULL);
}

/**
 * delete_node_at_index - deletes a node at a given position
 * @head: pointer to the beginning of linked list
 * @index: index to be deleted
 * Return: 1 if succesful -1 if fails
 */
int delete_node_at_index(list_s **head, unsigned int index)
{
	list_s *hold = NULL, *delete = NULL;
	unsigned int i = 0, action = 0;

	if (!*head)
		return (-1);
	for (i = 0, hold = *head; hold && index; i++, hold = (*hold).next)
		if (i == (index - 1))
		{
			action = 1;
			break;
		}

	if (action)
	{
		delete = (*hold).next;
		(*hold).next = (*delete).next;
		do_mem(0, delete);
		return (1);
	}
	else if (!index && (**head).next)
	{
		delete = *head;
		*head = (*delete).next;
		do_mem(0, delete);
		return (1);
	}
	else if (!index && *head)
	{
		*head = NULL;
		do_mem(0, *head);
		return (1);
	}
	return (-1);
}

/**
 * arrayify - copy a char * linked list into a char **
 * @head: the head of the linked list
 * Return: a malloc'd char **
 */
char **arrayify(list_s *head)
{
	char **ret = NULL;
	char *tmp = NULL;
	char *tmp2 = NULL;
	int len = 0, i = 0;

	len = list_len(head);
	ret = do_mem(sizeof(char *) * (len + 1), NULL);
	for (i = 0; i < len; i++)
	{
		tmp2 = get_node_at_index(head, i)->ptr;
		tmp = do_mem(sizeof(char) * (_strlen(tmp2) + 1), NULL);
		tmp = _strcat(tmp, tmp2);
		ret[i] = tmp;
	}
	ret[i] = NULL;
	return (ret);
}

/**
 * listify - copy a char ** array into a malloc'd char * linked list
 * @arr: the char **
 * Return: a malloc'd char * linked list
 */
list_s *listify(char **arr)
{
	list_s *ret = NULL;
	char *tmp = NULL;
	int i = 0;

	while (arr[i])
	{
		tmp = do_mem(sizeof(char) * (_strlen(arr[i]) + 1), NULL);
		tmp = _strcat(tmp, arr[i]);
		add_node_end(&ret, tmp);
		i++;
	}
	return (ret);
}


/**
 * free_double_array - free a double char pointer
 * @list: the double char pointer
 */
void free_double_array(char **list)
{
	int i = 0;

	for (i = 0; list[i]; i++)
		do_mem(0, list[i]);
	do_mem(0, list);
}

/**
 * _strcmp - compares two strings
 * @s1: the first string
 * @s2: the second string
 * Return: a negative num if s1 < s2, positive if s1 > s2 or zero if s1 == s2
 */
int _strcmp(char *s1, char *s2)
{
	char *s11;
	char *s22;

	s11 = s1;
	s22 = s2;

	if (!s1 && !s2)
		return (0);

	if (!s1 || !s2)
		return (-1);

	while (*s11 == *s22)
	{
		if (*s11 == '\0')
			break;
		if (*s22 == '\0')
			break;
		s11 = s11 + 1;
		s22 = s22 + 1;
	}

	if (*s11 < *s22)
		return (*s11 - *s22);
	if (*s11 > *s22)
		return (*s11 - *s22);

	return (0);
}

/**
 * _strlen - get the length of a string
 * @s: the string
 * Return: the number of characters in the string minus the null char
 */
int _strlen(char *s)
{
	int len;

	len = 0;
	while (s && s[len] != '\0')
		len++;
	return (len);
}

/**
  * word_count - counts the number of words in a string seperated by a delim
  * @str: string to count the words
  * @delim: separator between words
  * Return: number of words
  */
int word_count(char *str, char *delim)
{
	int i, check = 0, count = 0;

	for (i = 0; str && str[i]; i++)
	{
		if ((str[i] == delim[0]) && check)
		{
			count++;
			check = 0;
		}
		else if (str[i] != delim[0])
			check = 1;
	}
	if (str && str[0])
		count++;

	return (count);
}

/**
 * _strcat - Concats two strings
 * @dest: The destination
 * @src: The source
 *
 * Return: The destination string
 */
char *_strcat(char *dest, char *src)
{
	char *write;
	char *s2;

	if (!src)
		return (dest);
	if (!dest)
		return (src);
	write = dest;
	s2 = src;
	for (; *write != '\0'; write++)
	{
	}
	for (; *s2 != '\0'; s2++)
	{
		*write = *s2;
		write++;
	}
	*write = '\0';
	return (dest);
}

/**
  * _strcpy - copy src into dest
  * @dest: destination for copied string
  * @src: string to copy
  * Return: copied string
  */
char *_strcpy(char *dest, char *src)
{
	int i;

	for (i = 0; i <= _strlen(src); i++)
	{
		dest[i] = src[i];
	}

	return (dest);
}

/**
  * _atoi - converst a string to an integer
  * @s: string to be converted
  * Return: integers in a string
  */

int _atoi(char *s)
{
	int num, len, numcount, mult, x, neg;

	len = 0;
	x = 0;
	numcount = 0;
	mult = 1;
	neg = 1;
	num = 0;

	while (s[len] != '\0')
	{
		if (s[len] >= '0' && s[len] <= '9')
		{
			numcount++;
			if (!(s[len + 1] >= '0' && s[len + 1] <= '9'))
				break;
		}
		len++;
	}
	for (; numcount > 1; numcount--)
		mult *= 10;
	for (; x <= len; x++)
	{
		if (s[x] == '-')
			neg *= -1;
		else if (s[x] <= '9' && s[x] >= '0')
		{
			num += (s[x] - '0') * mult * neg;
			mult /= 10;
		}
	}
	return (num);
}

/**
  * sizeof_command - returns the size of command, breaks if it hits &&, ||, ;
  * @tokens: standard input tokenized
  * Return: the size of the command
  */
int sizeof_command(char **tokens)
{
	int i = 0;

	if (!tokens)
		return (0);
	for (i = 0; tokens[i]; i++)
	{
		if (tokens[i][0] == '&' && tokens[i][1] == '&')
			break;
		if (tokens[i][0] == '|' && tokens[i][1] == '|')
			break;
		if (tokens[i][0] == ';')
			break;
	}
	return (i);
}

/**
 * _isdigit - check for a digit (0 through 9)
 * @c: digit checked
 * Return: 1 if c is a digit returns 0 otherwise
 */

int _isdigit(int c)
{
	if (c >= '0' && c <= '9')
		return (1);

	return (0);
}

/**
 * has_newline - checks for \n or EOF and returns the index
 * @input: input read from read function in getline
 * Return: the size of command when it reaches \n or EOF
 */
int has_newline(char *input)
{
	int i;

	for (i = 0; input && input[i]; i++)
	{
		/*searches for \n, new line */
		if (input[i] == '\n')
			return (i);

		/* check if EOF */
		/*
		*if (input[1] == '-1')
		*	return (i);
		*/
	}

	return (i);
}

/**
 * shiftbuffer - shifts the buffer to the next command after \n
 *  @input: input from standard input
 *  @newline_index: where there is a new line or EOF
 *  @filled: size filled to
 */
void shiftbuffer(char *input, int newline_index, int filled)
{
	/* might have to make input a double pointer */
	/* shift and place back to beggining */
	int i = newline_index;
	int j = 0;

	for (; i < filled; i++, j++)
	{
		/* copy */
		input[j] = input[i];
	}
	/* fills remainder with '\0' */
	for (; j < 4096; j++)
	{
		input[j] = '\0';
	}
}

/**
 * _reverse - reverses the content of a string
 * @str: string to reverse
 * @n: lengeth of string
 * Return: reversed string
 */

char *_reverse(char *str, int n)
{
	char *begin = str;
	char *end;
	char hold;

	end = str + n - 1;
	for (; begin < end; begin++, end--)
	{
		hold = *end;
		*end = *begin;
		*begin = hold;
	}
	return (str);
}

/**
 * _itoa - converts an integer base 10 to a string
 * @num: integer to convert
 * Return: integer in string format
 */

char *_itoa(int num)
{
	int i = 0, neg = 0, cnum = num, len, remainder = 0;
	char *str;

	for (len = 0; cnum; cnum /= 10)
		;

	/* check if num is 0 */
	if (num == 0)
	{
		str = do_mem(2 * sizeof(char), NULL);
		str[i++] = '0';
		str[i] = '\0';
		return (str);
	}
	/* check if negative number */
	if (num < 0)
	{
		neg = 1;
		num = -num;
		len++;
	}
	/* malloc for size of string */
	str = do_mem((len + 1) * sizeof(char), NULL);
	/* handle individual numbers */
	while (num != 0)
	{
		remainder = num % 10;
		str[i++] = remainder + '0';
		num = num / 10;
	}
	/* Add negative sign if negatice */
	if (neg)
		str[i++] = '-';
	/* add null bite */
	str[i] = '\0';
	/* reverse string */
	_reverse(str, i);

	return (str);
}

/**
 * _memset - memset function
 * @s: start point of string to change
 * @b: value that will replace
 * @n: number of bytes to change
 * Return: changed pointer
 */

char *_memset(char *s, char b, int n)
{
	char *p = s;

	for (; n != 0; p++, n--)
		*p = b;
	return (s);
}

/**
 * _free - free a double char pointer
 * @list: the double char pointer
 * @count: the number of single char pointers to free
 */
void _free(char **list, int count)
{
	for (; count >= 0; count--)
		do_mem(0, list[count]);
	do_mem(0, list);
}

/**
 * _strtok - split a string into a double char pointer
 * @str: the string to split
 * @delim: any characters to split the string by
 * Return: the double char pointer
 */
char **_strtok(char *str, char *delim)
{
	int i = 0, j = 0, d = 0, len = 0, count = 0, check = 0;
	char **list = NULL;

	/* get count of words, if no words return NULL */
	while (!(count = word_count(str, delim)))
		return (NULL);
	list = do_mem((count + 1) * sizeof(char *), NULL);
	if (!list)
		return (NULL);
	/* tokenize str to individual words inside a double pointer*/
	for (i = 0, len = 0, count = 0; str[i] || len;)
	{
		for (d = 0, check = 0; delim[d]; d++)
		{
			if (((str[i] == delim[d]) || (!str[i])))
			{
				check += 1;
				if (len)
				{
					list[count] = do_mem(sizeof(char) * (len + 1), NULL);
					if (!list[count])
					{
						_free(list, count);
						return (NULL);
					}
					for (j = 0; len; len--, j++)
						list[count][j] = str[i - len];
					list[count][j] = '\0';
					count++;
				}
			}

		}
		if (!check)
			len++;
		if (str[i])
			i++;
	}
	list[count] = NULL;
	return (list);
}

/**
 * do_mem - malloc, free, or free all with a static record
 * @size: the size to malloc if non-0
 * @ptr: the pointer to free if non-NULL
 * Return: the malloc'd pointer if malloc, otherwise the result of free
 */
void *do_mem(size_t size, void *ptr)
{
	static list_t *all;
	void *ret = NULL;
	int i = 0, len = 0;

	if (size)
	{
		ret = malloc(size);
		if (!ret)
			do_exit(2, "malloc failed", EXIT_FAILURE);
		for (i = 0; (unsigned int)i < size; i++)
			((char *)ret)[i] = 0;
		__add_node_end(&all, ret);
		return (ret);
	}
	else if (ptr)
	{
		len = __list_len(all);
		for (i = 0; i < len; i++)
		{
			if (__get_node_at_index(all, i)->ptr == ptr)
			{
				__delete_node_at_index(&all, i);
				break;
			}
		}
		free(ptr);
		return (NULL);
	}
	else
	{
		/* If neither size nor ptr, then nuke everything. */
		__free_list_full(all);
		return (NULL);
	}
	return (NULL);
}

/**
 * do_exit - custom exit with error message, code, and automatic memory cleanup
 * @fd: the file descriptor to write the message
 * @msg: the message to print
 * @code: the numerical exit code
 */
void do_exit(int fd, char *msg, int code)
{
	if (*msg)
	{
		/* print message */
		write(fd, msg, _strlen(msg));
	}
	/* nuke extra memory */
	do_mem(0, NULL);

	/* exit with code */
	exit(code);
}

