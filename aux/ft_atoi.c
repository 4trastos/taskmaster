#include "taskmaster.h"
#include "ft_printf.h"

static int	ft_count(const char *str, int i)
{
	int	num;
	int	result;

	result = 0;
	while (str[i] >= '0' && str[i] <= '9')
	{
		num = str[i] - '0';
		result = result * 10 + num;
		i++;
	}
	return (result);
}

int	ft_atoi(const char *str)
{
	int	i;
	int	sign;
	int	result;

	i = 0;
	sign = 1;
	result = 0;
	while (str[i] == ' ' || str[i] == '\t' || str[i] == '\n'
		|| str[i] == '\r' || str[i] == '\f' || str[i] == '\v')
	{
		i++;
	}
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
		{
			sign = -1;
		}
		i++;
	}
	result = ft_count(str, i);
	return (sign * result);
}