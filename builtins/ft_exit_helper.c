/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_exit_helper.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibennaje <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/10 13:33:40 by ibennaje          #+#    #+#             */
/*   Updated: 2025/05/10 13:33:43 by ibennaje         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	ft_isnumber(char *str)
{
	int	i;

	i = 0;
	if (!str || !*str)
		return (0);
	if (str[i] == '+' || str[i] == '-')
		i++;
	if (!str[i])
		return (0);
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}

static int	ft_strtoll_check_sign(const char *str, int *i, int *sign)
{
	*sign = 1;
	if (str[*i] == '+' || str[*i] == '-')
	{
		if (str[*i] == '-')
			*sign = -1;
		(*i)++;
	}
	if (!str[*i])
		return (0);
	return (1);
}

static int	check_overflow(int sign, unsigned long long num, int digit)
{
	if (sign == 1 && num > (long long unsigned int)(LLONG_MAX - digit) / 10)
		return (0);
	if (sign == -1 && num > ((unsigned long long)LLONG_MAX + 1 - digit) / 10)
		return (0);
	return (1);
}

static int	process_digits(const char *str, int *i, int sign, long long *result)
{
	unsigned long long	num;
	int					digit;

	num = 0;
	while (str[*i] >= '0' && str[*i] <= '9')
	{
		digit = str[*i] - '0';
		if (!check_overflow(sign, num, digit))
			return (0);
		num = num * 10 + digit;
		(*i)++;
	}
	if (str[*i] != '\0')
		return (0);
	*result = (sign == 1) ? (long long)num : -(long long)num;
	return (1);
}

int	ft_strtoll(const char *str, long long *result)
{
	int	sign;
	int	i;

	i = 0;
	*result = 0;
	if (!ft_strtoll_check_sign(str, &i, &sign))
		return (0);
	return (process_digits(str, &i, sign, result));
}
