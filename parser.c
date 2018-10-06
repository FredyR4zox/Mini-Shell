COMMAND* invalid_command(COMMAND* commlist, char* msg){
	fprintf(stderr, "ERROR: %s\n", msg);
	
	free_commlist(commlist);
	
	return NULL;
}

COMMAND* parse(char* line){
	COMMAND* head_com,* com;
	char* line_com,* first,* second,* third,* in,* out;
	int len;
	
	inputfile = NULL;
	outputfile = NULL;
	background_exec = 0;

	if(line[0] == '|' || line[strlen(line)-1] == '|')
		return invalid_command(NULL, "'|' is the first or the last character of the command");

	head_com = NULL;
	line_com = strtok(line, "|");
	
	while(line_com){
		if(line_com[strlen(line_com)+1] == '|')
			return invalid_command(head_com, "two '|' characters in a row");

		if(head_com){
			com->next = malloc(sizeof(COMMAND));
			com = com->next;
		}
		else{
			head_com = malloc(sizeof(COMMAND));
			com = head_com;
		}

		com->cmd = line_com;
		line_com = strtok(NULL, "|");
	}

	com->next = NULL;
	com = head_com;

	while(com){
		line_com = com->cmd;

		if(line_com[0] == '&')
			return invalid_command(head_com, "'&'' is the first character of the command");

		len = strlen(line_com);
		first = strtok(line_com, "&");

		if(strtok(NULL, " "))
			return invalid_command(head_com, "'&' is not the last character of the command");

		if(len!=strlen(line_com))
			background_exec = 1;

		if(line_com[strlen(line_com)-1] == '<' || line_com[strlen(line_com)-1] == '>')
			return invalid_command(NULL, "'<' or '>' is the last character of the command");

		if(line_com[0] == '<'){
			if(line_com[1] == '<' || line_com[1] == '>')
				return invalid_command(NULL, "'<<' or '<>' sequence in the command");

			in = first = strtok(line_com, "<");

			if(strtok(NULL, ""))
				return invalid_command(NULL, "two '<' characters in the command");

			strtok(first, ">");

			if(first[strlen(first)+1] == '>')
				return invalid_command(head_com, "two '>' characters in a row");

			out = second = strtok(NULL, ">");

			if(strtok(NULL, ""))
				return invalid_command(NULL, "two '>' characters in the command");

			third = NULL;
		}
		else if(line_com[0] == '>'){
			if(line_com[1] == '<' || line_com[1] == '>')
				return invalid_command(NULL, "'><' or '>>' sequence in the command");

			out = first = strtok(line_com, ">");

			if(strtok(NULL, ""))
				return invalid_command(NULL, "two '>' characters in the command");

			strtok(first, "<");

			if(first[strlen(first)+1] == '<')
				return invalid_command(head_com, "two '<' characters in a row");

			in = second = strtok(NULL, "<");

			if(strtok(NULL, ""))
				return invalid_command(NULL, "two '<' characters in the command");

			third = NULL;
		}
		else{
			first = strtok(line_com, "<");

			if(first[strlen(first)+1] == '<' || first[strlen(first)+1] == '>')
				return invalid_command(NULL, "'<<' or '<>' sequence in the command");

			in = strtok(NULL, "<");

			if(in && strtok(NULL, ""))
				return invalid_command(NULL, "two '<' characters in the command");

			strtok(first, ">");

			if(first[strlen(first)+1] == '<' || first[strlen( first)+1] == '>')
				return invalid_command(NULL, "'><' or '>>' sequence in the command");

			out = strtok(NULL, ">");

			if(out && strtok(NULL, ""))
				return invalid_command(NULL, "two '>' characters in the command");

			if(out){
				strtok(in, ">");

				if(strtok(NULL, ""))
					return invalid_command(NULL, "two '>' characters in the command");

				second = out;
				third = in;
			}
			else if(in){
				strtok(in, ">");

				if(in[strlen(in)+1] == '>')
					return invalid_command(head_com, "two '>' characters in a row");

				out = strtok(NULL, ">");

				if(strtok(NULL, ""))
					return invalid_command(NULL, "two '>' characters in the command");

				second = in;
				third = out;
			}
			else
				second = third = NULL;
		}
		if(in && com != head_com)
			return invalid_command(head_com, "'<' is not in the first command");

		if(out && com->next)
			return invalid_command(head_com, "'>' is not in the last command");

		if(background_exec == 1 && com->next)
			return invalid_command(head_com,"'&' is not in the last command");

		com->argc = 0;

		if((com->argv[com->argc] = strtok(first, " ")) == NULL)
			return invalid_command(head_com, "missing arguments");

		if(in == first)
			inputfile = com->argv[com->argc];
		else if(out == first)
			outputfile = com->argv[com->argc];
		else
			com->argc++;

		while((com->argv[com->argc] = strtok(NULL, " "))!=NULL)
			com->argc++;

		if(second){
			if((com->argv[com->argc] = strtok(second, " ")) == NULL)
				return invalid_command(head_com, "missing arguments");

			if(in == second)
				inputfile = com->argv[com->argc];
			else if(out == second)
				outputfile = com->argv[com->argc];
			else
				com->argc++;

			while((com->argv[com->argc] = strtok(NULL, " "))!=NULL)
				com->argc++;
		}
		if(third){
			if((com->argv[com->argc] = strtok(third, " ")) == NULL)
				return invalid_command(head_com, "missing arguments");

			if(in == third)
				inputfile = com->argv[com->argc];
			else if(out == third)
				outputfile = com->argv[com->argc];
			else
				com->argc++;

			while((com->argv[com->argc] = strtok(NULL, " "))!=NULL)
				com->argc++;
		}
		com->argv[com->argc] = NULL;
		com->cmd = com->argv[0];
		com = com->next;
	}
	return head_com;
}
