/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: achacon- <achacon-@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/10 14:15:56 by achacon-          #+#    #+#             */
/*   Updated: 2024/05/02 13:24:41 by achacon-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

char	*get_path_line(char **env)
{
	char	*path_line;
	int		i;

	i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], "PATH=", 5) == 0)
			path_line = env[i];
		i++;
	}
	path_line = path_line + 5;
	return (path_line);
}

char	*get_path(char *command, char **env)
{
	char	*path_line;
	char	**path_array;
	char	*path_final;
	int		i;

	i = 0;
	if (command[0] == '/')
		return (command);
	path_line = get_path_line(env);
	path_array = ft_split(path_line, ':');
	while (path_array[i])
	{
		path_final = ft_strjoin(path_array[i], "/");
		path_final = ft_strjoin(path_final, command);
		if (access(path_final, F_OK) == 0)
			break ;
		i++;
	}
	free_array(path_array);
	return (path_final);
}

void	child(t_store store, char **env)
{
	store.in_fd = open(store.in_name, O_RDONLY);
	if (store.in_fd == -1)
	{
		free_array(store.array_cmd1);
		free_array(store.array_cmd2);
		close_pipes(store);
		ft_putstr_fd("Input file error\n", 2);
		exit(1);
	}
	dup2(store.in_fd, STDIN_FILENO);
	dup2(store.pipe[1], STDOUT_FILENO);
	close(store.pipe[0]);
	if (execve(get_path(store.array_cmd1[0], env), store.array_cmd1, env) == -1)
	{
		free_array(store.array_cmd1);
		free_array(store.array_cmd2);
		close_pipes(store);
		ft_putstr_fd("Command 1 not found\n", 2);
		exit(1);
	}
	exit(0);
}

void	parent(t_store store, char **env)
{
	wait(NULL);
	store.out_fd = open(store.out_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (store.out_fd == -1)
	{
		free_array(store.array_cmd1);
		free_array(store.array_cmd2);
		close_pipes(store);
		ft_putstr_fd("Open function error\n", 2);
		exit(1);
	}
	dup2(store.pipe[0], STDIN_FILENO);
	close(store.pipe[1]);
	dup2(store.out_fd, STDOUT_FILENO);
	if (execve(get_path(store.array_cmd2[0], env), store.array_cmd2, env) == -1)
	{
		free_array(store.array_cmd1);
		free_array(store.array_cmd2);
		close_pipes(store);
		ft_putstr_fd("Command 2 not found\n", 2);
		exit(1);
	}
	exit(0);
}