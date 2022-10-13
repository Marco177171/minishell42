/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: masebast <masebast@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/01 17:12:49 by masebast          #+#    #+#             */
/*   Updated: 2022/10/13 19:36:55 by masebast         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_recognize_command(t_command *command_struct, int pipe_index, char **envp)
{
	if (ft_strcmp("echo", command_struct->word_matrix[0]) == 0)
		return (ft_echo(command_struct, pipe_index));
	else if (ft_strcmp("cd", command_struct->word_matrix[0]) == 0)
		return (ft_cd(command_struct));
	else if (ft_strcmp("pwd", command_struct->word_matrix[0]) == 0)
		return (ft_pwd());
	else if (ft_strcmp("export", command_struct->word_matrix[0]) == 0)
		return (ft_export(command_struct, envp));
	else if (ft_strcmp("unset", command_struct->word_matrix[0]) == 0)
		return (ft_unset(command_struct, envp));
	else if (ft_strcmp("env", command_struct->word_matrix[0]) == 0)
		return (ft_env(command_struct, envp));
	else if (ft_strcmp("exit", command_struct->word_matrix[0]) == 0)
	{
		ft_exit(command_struct);
		return (0);
	}
	else
		ft_other_commands(command_struct, envp);
	return (0);
}

void	ft_remove_quotes(char *command)
{
	int index;
	int res_index;
	char *result;

	index = 0;
	res_index = 0;
	if (ft_check_quote(command) == 1)
	{
		while (command[index])
			index++;
		result = malloc(sizeof(char) * index);
		index = 0;
		while (command[index])
		{
			if (command[index] == '\"')
			{
				index++;
				while (command[index] != '\"')
				{
					result[res_index] = command[index];
					res_index++;
					index++;
				}
				index++;
			}
			if (command[index] == '\'')
			{
				index++;
				while (command[index] != '\'')
				{
					result[res_index] = command[index];
					res_index++;
					index++;
				}
				index++;
			}
			else
			{
				result[res_index] = command[index];
				res_index++;
				index++;
			}
		}
		result[res_index] = '\0';
		free (command);
		command = malloc(sizeof(char) * res_index + 1);
		res_index = 0;
		while (result[res_index])
		{
			command[res_index] = result[res_index];
			res_index++;
		}
		command[res_index] = '\0';
		free(result);
		return ;
	}
	else
		return ;
}

void	ft_init_struct(t_command *command_struct)
{
	char *value;

	value = ft_itoa(ft_atoi(getenv("SHLVL")) + 1);
	command_struct->current_shell_level = ft_strjoin("SHLVL=", value);
	free(value);
	command_struct->write_fd = 1;
	command_struct->pipes[0] = 0;
	command_struct->pipes[1] = 1;
	command_struct->total_pipes = 0;
	g_exit_status = malloc(sizeof(int) * 1);
}

int	ft_check_syntax(char *command)
{
	int index;
	int flag;

	index = 0;
	flag = 0;
	while (command[index])
	{
		if (command[index] == '|')
		{
			flag = 1;
			index++;
			if (command[index] == '\0')
				break ;
			else
			{
				while (command[index])
				{
					if (command[index] == '|')
					{
						write(2, "minishell: syntax error near unexpected token `", 47);
						write(2, &command[index], 1);
						write(2, "'\n", 2);
						return (1);
					}
					else if (command[index] == ' ')
						index++;
					else if (command[index] != ' ' || command[index] != '|')
					{
						flag = 0;
						break ;
					}
					else if (command[index] == '\0')
						break ;
				}
			}
		}
		index++;
	}
	if (flag == 1 && (command[index - 1] == '|' || command[index - 1] == ' '))
	{
		write(2, "minishell: syntax error near unexpected token `", 47);
		write(2, &command[index - 1], 1);
		write(2, "'\n", 2);
		return (1);
	}
	return (0);
}

void	ft_execute_cycle(t_command *command_struct, char **envp)
{
	command_struct->command_string = readline("minishell$ ");
	if (command_struct->command_string[0] != '\0')
	{
		add_history(command_struct->command_string);
		if (ft_check_syntax(command_struct->command_string) == 1)
		{
			free(command_struct->command_string);
			return ;
		}
		command_struct->total_pipes = ft_count_pipes(command_struct->command_string);
		command_struct->pipe_matrix = ft_split(command_struct->command_string, '|');
		if (command_struct->total_pipes > 1)
			ft_manage_pipes(command_struct, envp);
		else
		{
			command_struct->word_matrix = ft_split(command_struct->pipe_matrix[0], ' ');
			ft_remove_quotes(command_struct->word_matrix[0]);
			ft_recognize_command(command_struct, 0, envp);
			ft_free_matrix(command_struct->word_matrix);
			ft_free_matrix(command_struct->pipe_matrix);
			free(command_struct->command_string);
		}
	}
	else
		free(command_struct->command_string);
}

int	main(int ac, char **av, char **envp)
{
	t_command	command_struct;

	av = NULL;
	if (ac == 1)
	{
		ft_init_struct(&command_struct);
		ft_modify_var(command_struct.current_shell_level, envp);
		while (TRUE)
		{
			ft_manage_signals();
			ft_execute_cycle(&command_struct, envp);
		}
	}
	else
		ft_error();
	return (FALSE);
}
