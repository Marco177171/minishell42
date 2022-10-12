/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_other_commands.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masebast <masebast@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/09 17:29:07 by masebast          #+#    #+#             */
/*   Updated: 2022/10/12 17:27:29 by masebast         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_execute_sub_process(t_command *command_struct, char **envp)
{
	if (execve(command_struct->word_matrix[0], command_struct->word_matrix, envp) != 0)
	{
		write(2, "minishell: ", 11);
		write(2, command_struct->word_matrix[0], ft_strlen(command_struct->word_matrix[0]));
		write(2, ": No such file or directory\n", 28);
		return (1);
	}
	return (0);
}

void	ft_child(t_command *command_struct, char **envp)
{
	int		index;
	char	*path;
	char	**mypath;
	
	path = getenv("PATH");
	mypath = ft_split(path, ':');
	index = -1;
	*g_exit_status = 0;
	while (mypath[++index])
		mypath[index] = ft_strjoin(mypath[index], "/");
	index = -1;
	while (mypath[++index])
	{
		path = ft_strjoin(mypath[index], command_struct->word_matrix[0]);
		if (access(path, R_OK) == 0)
			*g_exit_status = execve(path, command_struct->word_matrix, envp);
		else if (strncmp(command_struct->word_matrix[0], "./", 2) == 0)
		{
			*g_exit_status = ft_execute_sub_process(command_struct, envp);
			exit(*g_exit_status);
		}
		else
			*g_exit_status = execve(command_struct->word_matrix[0], command_struct->word_matrix, envp);
		free(path);
	}
	if (*g_exit_status != 0)
	{
		write(2, "minishell: ", 11);
		write(2, command_struct->word_matrix[0], ft_strlen(command_struct->word_matrix[0]));
		write(2, ": command not found\n", 20);
	}
	exit(*g_exit_status);
}

int	ft_other_commands(t_command *command_struct, char **envp)
{
	int		status;
	pid_t	pid;

	status = 0;
	pid = fork();
	if (pid == 0)
		ft_child(command_struct, envp);
	else
	{
		waitpid(pid, &status, 0);
		if (status != 0)
			*g_exit_status = 127;
	}
	return (1);
}
