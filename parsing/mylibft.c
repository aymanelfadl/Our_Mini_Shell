#include <minishell.h>

char *my_strnstr(const char *haystack, const char *needle, size_t len)
{
	size_t i;
	size_t j;

	if (!haystack && len == 0)
		return (NULL);
	if (!needle[0])
		return ((char *)haystack);
	if (ft_strlen(needle) > ft_strlen(haystack))
		return (NULL);
	i = 0;
	while (haystack[i] && i < len)
	{
		if (haystack[i] == needle[i - i])
		{
			j = i + 1;
			while ((haystack[j] == needle[j - i]) && needle[j - i] && j < len)
				j++;
			if ((!needle[j - i]) && commas_ops_check((char *)&haystack[j]))
				return ((char *)&haystack[i]);
		}
		i++;
	}
	return (NULL);
}

static int incleded_in_char(char s, char *target)
{
	while (*target)
	{
		if (*target == s)
			return (1);
		target++;
	}
	return (0);
}

char *my_strchr(char *s, char *target)
{
	if (!s)
		return (NULL);
	while (*s)
	{
		if (incleded_in_char(*s, target))
			return (s);
		s++;
	}
	return (NULL);
}

int ft_strcmp(char *s1, char *s2)
{
	while (*s1 && *s2 && *s1 == *s2)
	{
		s1++;
		s2++;
	}
	return (*s1 - *s2);
}