#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include "clientBridge.h"

void write_message(int fd, unsigned long command, char *message)
{
	if (ioctl(fd, command, message) == -1)
	{
		perror("Write message error at ioctl");
	}
}

void read_message(int fd, unsigned long command, char *message)
{
	if (ioctl(fd, command, message) == -1)
	{
		perror("Read message error at ioctl");
	}
}

void read_message_param(int fd, unsigned long command, int *value)
{
	if (ioctl(fd, command, value) == -1)
	{
		perror("Read message param error at ioctl");
	}
	else
	{
		printf("Copy the messsage from the kernel\n");
	}
}

void write_int(int fd, unsigned long command, int *value)
{
	if (ioctl(fd, command, value) == -1)
	{
		perror("Write int error at ioctl");
	}
}

void read_int(int fd, unsigned long command, int *value)
{
	if (ioctl(fd, command, value) == -1)
	{
		perror("Read int error at ioctl");
	}
	else
	{
		printf("Copy the int from the kernel\n");
	}
}

int send_empty_command(int fd, unsigned long command)
{
	int returned_value = ioctl(fd, command);
	if (returned_value == -1)
	{
		perror("Send command error at ioctl");
	}
	else
	{
		return returned_value;
		printf("Command OK to the kernel\n");
	}
}

void write_several_messages(int fd)
{
	write_message(fd, BRIDGE_W_S, "Message 1");
	write_message(fd, BRIDGE_W_S, "Message 2");
	write_message(fd, BRIDGE_W_S, "Message 3");
}

void read_all_messages(int fd)
{
	char message[100];
	while (send_empty_command(fd, BRIDGE_STATE_S) > 0)
	{
		read_message(fd, BRIDGE_R_L, message);
		printf("%s\n", message);
	}
}

void read_all_messages_from_stack(int fd)
{
	char message[100];
	while (send_empty_command(fd, BRIDGE_STATE_S) > 0)
	{
		read_message(fd, BRIDGE_R_S, message);
		printf("%s\n", message);
	}
}

// Punto 1
void reverse_lines_from_file(int fd)
{
	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	fp = fopen("input.txt", "r");
	if (fp == NULL)
		exit(EXIT_FAILURE);

	send_empty_command(fd, BRIDGE_CREATE_S);				// Create new stack
	printf("CURRENT LIST:\n");
	while ((read = getline(&line, &len, fp)) != -1)
	{
		write_message(fd, BRIDGE_W_S, line);					// Add element to stack
		printf("%s", line);
	}

	fclose(fp);
	if (line)
		free(line);

	printf("\nNEW LIST:\n");
	read_all_messages_from_stack(fd);
	send_empty_command(fd, BRIDGE_DESTROY_S);			// Destroy stack after usage
}

// Punto 2
void randomize_lines_from_file(int fd)
{
	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	fp = fopen("input.txt", "r");
	if (fp == NULL)
		exit(EXIT_FAILURE);

	send_empty_command(fd, BRIDGE_CREATE_L);				// Create new List
	printf("CURRENT LIST:\n");
	while ((read = getline(&line, &len, fp)) != -1)
	{
		write_message(fd, BRIDGE_W_L, line);					// Add element to List
		printf("%s", line);
	}

	fclose(fp);
	if (line)
		free(line);

	send_empty_command(fd, BRIDGE_RANDOMIZE_L);			// Randomizes kernel List

	printf("\nNEW LIST:\n");
	read_all_messages(fd);
	send_empty_command(fd, BRIDGE_DESTROY_L);			// Destroy stack after usage
}

// Punto 3
int verify_closing_brackets(int fd)
{
	FILE *fp;
	char *ch;
	fp = fopen("code_input.txt", "r");
	if (fp == NULL)
		exit(EXIT_FAILURE);

	send_empty_command(fd, BRIDGE_CREATE_S);				// Create new stack
	

	while ((ch = fgetc(fp)) != EOF)
	{
		printf("in: %c\n", ch);
		if (ch == '(')
		{
			char charIn[100] = "(";
			write_message(fd, BRIDGE_W_S, charIn);
		}
		else if (ch == '{')
		{
			char charIn[100] = "{";
			write_message(fd, BRIDGE_W_S, charIn);
		}
		else if (ch == ')')
		{
			if (send_empty_command(fd, BRIDGE_STATE_S) > 0)
			{
				char charOut[100];
				read_message(fd, BRIDGE_R_S, charOut);
				if (charOut[0] != '(')
				{
					return 0;
				}
			}
			else
			{
				return 0;
			}
		}
		else if (ch == '}')
		{
			if (send_empty_command(fd, BRIDGE_STATE_S) > 0)
			{
				char charOut[100];
				read_message(fd, BRIDGE_R_S, charOut);

				if (charOut[0] != '{')
				{
					return 0;
				}
			}
			else
			{
				return 0;
			}
		}
	}

	if (send_empty_command(fd, BRIDGE_STATE_S) > 0)
	{
		return 0;
	}
	else
	{
		printf("Parenthesis and Brackets are Balanced\n");
		return 1;
	}

	send_empty_command(fd, BRIDGE_DESTROY_S);			// Destroy stack after usage

	fclose(fp);
}

void generate_prior_q(int fd)
{
	write_message(fd, BRIDGE_W_LOW_PRIOR_Q, "Message 1");
	printf("Message 1, low prior\n");
	write_message(fd, BRIDGE_W_HIGH_PRIOR_Q, "Message 2");
	printf("Message 2, high prior\n");
	write_message(fd, BRIDGE_W_MIDDLE_PRIOR_Q, "Message 3");
	printf("Message 3, mid prior\n");
	write_message(fd, BRIDGE_W_LOW_PRIOR_Q, "Message 4");
	printf("Message 4, low prior\n");
	write_message(fd, BRIDGE_W_HIGH_PRIOR_Q, "Message 5");
	printf("Message 5, high prior\n");
		write_message(fd, BRIDGE_W_MIDDLE_PRIOR_Q, "Message 6");
	printf("Message 6, mid prior\n");
}

// Punto 4 - CAUSA SEGMENTATION FAULT EN SBIN, NO CORRER
void prior_queue(int fd) 
{
	send_empty_command(fd, BRIDGE_CREATE_L);				// Create new List

	printf("\nCURRENT QUEUE:\n");
	generate_prior_q(fd);
	read_all_messages(fd);

	send_empty_command(fd, BRIDGE_DESTROY_L);				// Create new List

}

// Punto 5 - Se encuentra solo en bridge.c, se usa frecuentemente

void generate_list(int fd)
{
	write_message(fd, BRIDGE_W_L, "Message 1");
	printf("Message 1\n");
	write_message(fd, BRIDGE_W_L, "Message 2");
	printf("Message 2\n");
	write_message(fd, BRIDGE_W_L, "Message 3");
	printf("Message 3\n");
	write_message(fd, BRIDGE_W_L, "Message 4");
	printf("Message 4\n");
	write_message(fd, BRIDGE_W_L, "Message 5");
	printf("Message 5\n");
}

void generate_alphabetical_list(int fd)
{
	write_message(fd, BRIDGE_W_L, "Geodude");
	printf("Geodude\n");
	write_message(fd, BRIDGE_W_L, "Clefairy");
	printf("Clefairy\n");
	write_message(fd, BRIDGE_W_L, "Squirtle");
	printf("Squirtle\n");
	write_message(fd, BRIDGE_W_L, "Charizard");
	printf("Charizard\n");
	write_message(fd, BRIDGE_W_L, "Bellsprout");
	printf("Bellsprout\n");
}

// Punto 6
void invert_list(int fd)
{
	send_empty_command(fd, BRIDGE_CREATE_L);				// Create new List

	printf("\nCURRENT LIST:\n");
	generate_list(fd);

	send_empty_command(fd, BRIDGE_INVERT_L);
	printf("\nNEW LIST:\n");
	read_all_messages(fd);

	send_empty_command(fd, BRIDGE_DESTROY_L);				// Create new List

}

// Punto 7
void concatenate_lists(int fd)
{
	send_empty_command(fd, BRIDGE_CREATE_L);				// Create new List

	send_empty_command(fd, BRIDGE_CONCAT_L);

	send_empty_command(fd, BRIDGE_DESTROY_L);				// Create new List

}

// Punto 8
void rotate_right_n_times(int fd)
{
	send_empty_command(fd, BRIDGE_CREATE_L);				// Create new List

	printf("\nCURRENT LIST:\n");
	generate_list(fd);
	int n = 3;
	int i;
	for (i = 0; i < n; i++)
	{
		write_int(fd, BRIDGE_ROTATE_L, &n);
	}

	printf("\nNEW LIST:\n");
	read_all_messages(fd);

	send_empty_command(fd, BRIDGE_DESTROY_L);				// Create new List

}

// Punto 9
void clean_list(int fd)
{
	send_empty_command(fd, BRIDGE_CREATE_L);				// Create new List

	printf("\nCURRENT LIST:\n");
	generate_list(fd);
	generate_list(fd);

	send_empty_command(fd, BRIDGE_CLEAN_L);

	printf("\nNEW LIST:\n");
	read_all_messages(fd);

	send_empty_command(fd, BRIDGE_DESTROY_L);				// Create new List

}

// Punto 10
void get_highest_string(int fd)
{

	send_empty_command(fd, BRIDGE_CREATE_L);				// Create new List


	printf("\nCURRENT LIST:\n");
	generate_alphabetical_list(fd);

	char message[100];
	read_message(fd, BRIDGE_GREATER_VAL_L, message);

	printf("\nHIGHEST VALUE: %s\n", message);

	send_empty_command(fd, BRIDGE_DESTROY_L);				// Create new List

}

int main(int argc, char *argv[])
{

	const char *file_name = "/dev/bridge"; // used by ioctl
	int fd;

	fd = open(file_name, O_RDWR);
	if (fd == -1)
	{
		perror("Bridge ioctl file open");
		return 2;
	}

	int option = atoi(argv[1]);
	switch (option)
	{
	case 1:
		reverse_lines_from_file(fd);
		break;

	case 2:
		randomize_lines_from_file(fd);
		break;

	case 3:
		printf("%d\n", verify_closing_brackets(fd));
		break;

	case 4:
		prior_queue(fd);
		break;

	case 5:
		send_empty_command(fd, BRIDGE_DESTROY_L);
		break;

	case 6:
		invert_list(fd);
		break;

	case 7:
		concatenate_lists(fd);
		break;

	case 8:
		rotate_right_n_times(fd);
		break;

	case 9:
		clean_list(fd);
		break;

	case 10:
		get_highest_string(fd);
		break;

	default:
		printf("Unvalid option\n");
	}

	// char message[] = "mensajeEspecifico";
	// int value = 10;

	// send_empty_command(fd, BRIDGE_CREATE_Q);                    //Create a queue
	// write_message(fd, BRIDGE_W_HIGH_PRIOR_Q, message);		//Send message with high priority
	// write_message(fd, BRIDGE_W_MIDDLE_PRIOR_Q, message);	//Send message with middle priority
	// write_message(fd, BRIDGE_W_LOW_PRIOR_Q, message);		//Send message with low priority
	// read_message(fd, BRIDGE_R_HIGH_PRIOR_Q, message);		//Read a message with high priority
	// read_message(fd, BRIDGE_R_MIDDLE_PRIOR_Q, message);		//Read a message with middle priority
	// read_message(fd, BRIDGE_R_LOW_PRIOR_Q, message);		//Read a message with low priority
	// send_empty_command(fd, BRIDGE_STATE_Q);			//Get an int indicating the state of the queue
	// send_empty_command(fd, BRIDGE_DESTROY_Q);			//Destroy a queue completely releasing memory (IMPORTANT!!)

	// send_empty_command(fd, BRIDGE_CREATE_S);                   	//Create a stack
	// write_message(fd, BRIDGE_W_S, "Message WS");		//Write a message in the stack
	// read_message(fd, BRIDGE_R_S, message);			//Read a message from the stack
	// send_empty_command(fd, BRIDGE_STATE_S);			//Get an int indicating the statte of the stack
	// send_empty_command(fd, BRIDGE_DESTROY_S);			//Destroy a stack completely releasing the memory (IMPORTANT!!)

	// send_empty_command(fd, BRIDGE_CREATE_L);                    //Create a list
	// write_message(fd, BRIDGE_W_L, message);			//Write a message in the last position of a list
	// read_message(fd, BRIDGE_R_L, message);			//Read a message of the last position of a list
	// send_empty_command(fd, BRIDGE_INVERT_L);			//Invert the order of the elements of a list
	// write_int(fd, BRIDGE_ROTATE_L, &value);			//Rotate a list value position to the right
	// send_empty_command(fd, BRIDGE_CLEAN_L);			//Eliminate repeated elements in a list
	// read_message(fd, BRIDGE_GREATER_VAL_L, message);		//Find the greater value of a list of strings
	// printf("Value obtained: %s\n", message);
	// send_empty_command(fd, BRIDGE_END_L);			//End a list (to be able to create another list) (Module limited to at most three list)
	// send_empty_command(fd, BRIDGE_CONCAT_L);			//Concatenate two previous list in a third new one
	// send_empty_command(fd, BRIDGE_STATE_L);			//Get an int indicating the state of a list
	// send_empty_command(fd, BRIDGE_DESTROY_L);			//Destroy all the list of the module releasing memory (IMPORTANT!!)
	//  write_several_messages(fd);
	//  read_all_messages(fd);
	//  close (fd);
	return 0;
}
