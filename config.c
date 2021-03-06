#include "config.h"

int configure_server(config_parameter_t* dest)
{
	int config_fd;
	char buf[CONFIG_PARAMETER_NUM*50];
	char default_cfg[] = DEFAULT_CONFIG_STRING;
	int length;
	char ch = 0;
	int i = 0, j = 0, k = 0, pos = 0;
	char* cfg_data[CONFIG_PARAMETER_NUM];
	char cfg_param_list[CONFIG_PARAMETER_NUM][CONFIG_PARAMETER_MAX_LENGTH] = CONFIG_PARAMETER_LIST;
	char tmp[CONFIG_PARAMETER_MAX_LENGTH];
	char cfg_param_value[CONFIG_PARAMETER_NUM][CONFIG_PARAMETER_MAX_LENGTH];
	int value_flag = 0;

	/* 	open config file, if doesn't exist create it */ 
	config_fd = open("./config.cfg", O_RDONLY);
	if( config_fd < 0)
	{
		perror("Missing config.cfg");
		config_fd = creat("./config.cfg", 0644);
		if(config_fd < 0)
		{
			perror("Couldn't create config.cfg");
			return 1;
		}	

		if(write(config_fd, default_cfg, strlen(default_cfg)) != strlen(default_cfg))
		{
			perror("Couldn't write default config parameters.");
			return 1;
		}
		else
		{
			close(config_fd);
			config_fd = open("./config.cfg", O_RDONLY);
			if(config_fd < 0)
			{
				perror("Couldn't create config.cfg");
				return 1;
			}	
		}
	}

	/* read config file */
	length = read(config_fd, buf, sizeof(buf) - 1);
	if(length < 0)
	{
		perror("config_read");
		return 1;
	}

	/* get separete lines from config file */
	buf[length] = '\0';
	while(j != CONFIG_PARAMETER_NUM)
	{
		while(ch != '\n')
		{
			ch = buf[i + pos];
			i++;
		}
		cfg_data[j] = (char*) malloc(i);
		cfg_data[j][i-1] = '\0';
		i = i-2; 
		if(cfg_data[j] == NULL)
		{
			perror("Memory error.");
			return 1;
		}
		for(k=i; k>=0; k--)
		{
			cfg_data[j][k] = buf[pos + k];
		}
		pos = pos + i + 2;
		i = 0;
		j++;
		ch = 0;
	}
	i = j = pos = 0;
	ch = 0;

	/* get parameters values */
	for(i=0; i<CONFIG_PARAMETER_NUM; i++)
	{
		value_flag = 0;
		for(j=0; j<strlen(cfg_data[i])+1; j++)
		{
			if(!value_flag)
			{
				tmp[j] = cfg_data[i][j];
				if(tmp[j] == '=')
				{
					tmp[j] = '\0';
					if(!strcmp(tmp, cfg_param_list[i]))
					{
						/* known config paramter, value starts */
						value_flag = 1;
						pos = j;
					}
					else
					{
						perror("Unknown config parameter.");
						return -1;	
					}
				}
			}
			else
			{
				tmp[j-pos-1] = cfg_data[i][j];
				/* read till the end of string */
				if(tmp[j-pos-1] == '\0')
				{
					/* find out is paramter numeric or string */
					if(strstr(cfg_param_list[i], "NUM"))
					{
						switch(i){
							case 0:
								dest->localhost = strtoimax(tmp, NULL, 10);
								break;
							case 2: 
								dest->max_conn = strtoimax(tmp, NULL, 10);
							break;
							case 3:
								dest->ss_php = strtoimax(tmp, NULL, 10);
							break;
							case 4:
								dest->log = strtoimax(tmp, NULL, 10);
							break;
							default:
								perror("Config switch");
							break;
						};
					}
					else
					{
						if( (strcpy((char*)(&cfg_param_value[i]), (char*)tmp) == NULL) )
						{
							perror("String copy error");
						}
					}
				}
			}
		}
	}

	/* free dynamic resources */
	for(i=0; i<CONFIG_PARAMETER_NUM; i++)
	{
		free(cfg_data[i]);
	}
	/* close opened files */
	close(config_fd);
	strcpy((char*)dest->port, (char*)(&cfg_param_value[1]));
	return 0;
}