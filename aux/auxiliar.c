#include "taskmaster.h"
#include "ft_printf.h"

char* ft_substr(char *s, int start, int len)
{
	size_t	i;
	char	*sb;

	if ((size_t)start >= ft_strlen(s))
		return (ft_strdup(""));
	if ((size_t)len > ft_strlen(s) - start)
		len = ft_strlen(s) - start;
	sb = (char *)malloc(len + 1);
	if (sb == NULL)
		return (NULL);
	i = 0;
	while (i < (size_t)len)
	{
		sb[i] = s[start + i];
		i++;
	}
	sb[i] = '\0';
	return (sb);
}

int	ft_strcmp(const char *s1, const char *s2) {
	int	i;

	i = 0;
	while (s1[i] && s1[i] == s2[i])
		i++;
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

bool ft_atob(const char *str) {
    return (str && ft_strcmp(str, "true") == 0) ? true : false;
}
