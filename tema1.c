#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_LINE_SIZE 300

struct Dir;
struct File;

typedef struct Dir {
	char *name;
	struct Dir* parent;
	struct File* head_children_files;
	struct Dir* head_children_dirs;
	struct Dir* next;
} Dir;

typedef struct File {
	char *name;
	struct Dir* parent;
	struct File* next;
} File;

File *alloc_file_node(char *name, Dir *parent) {
	File *file = malloc(sizeof(File));
	file->name = malloc(sizeof(char) * (strlen(name) + 1));
	
	strcpy(file->name, name);
	file->parent = parent;
	file->next = NULL;
	return file;
}

Dir *alloc_dir_node(char *name, Dir *parent) {
	Dir *directory = malloc(sizeof(Dir));
	directory->name = malloc(sizeof(char) * (strlen(name) + 1));
	strcpy(directory->name, name);
	directory->parent = parent;
	directory->head_children_files = NULL;
	directory->head_children_dirs = NULL;
	directory->next = NULL;
	return directory;
}

void free_file(File *file) {
	free(file->name);
	free(file);
}

void free_listOf_files(File *head) {
	while(head != NULL) {
		File *new_head = head->next;
		free_file(head);
		head = new_head;
	}
}

void del_sub_dir(Dir *head) {
	while(head != NULL) {
		Dir *new_head = head->next;
		Dir *sub_dir = head->head_children_dirs;
		while(sub_dir != NULL) {
			Dir *new_sub_head = sub_dir->next;
			del_sub_dir(sub_dir);
			sub_dir = new_sub_head;
		}
		free_listOf_files(head->head_children_files);
		free(head->name);
		free(head);
		head = new_head;
	}
}

void free_dir(Dir *head) {
	while(head->parent != NULL) {
		head = head->parent;
	}
	while(head != NULL) {
		Dir *new_head = head->next;
		del_sub_dir(head->head_children_dirs);
		free_listOf_files(head->head_children_files);
		free(head->name);
		free(head);
		head = new_head;
	}
}


void touch(Dir* parent, char* name) {
	File *new_file = alloc_file_node(name, parent);
	if(parent->head_children_files == NULL) {	
		parent->head_children_files = new_file;
	}
	else {
		if(!strcmp(parent->head_children_files->name, name)) {
			printf("File already exists\n");
			free_file(new_file);
			return;
		}
		File *temp_file_list = parent->head_children_files;
		while(temp_file_list->next != NULL) {
			if(!strcmp(temp_file_list->next->name, name)) {
				printf("File already exists\n");
				free_file(new_file);
				return;
			}
			temp_file_list = temp_file_list->next;
		}
		temp_file_list->next = new_file;
	}
}

void mkdir(Dir* parent, char* name) {
	Dir *new_dir = alloc_dir_node(name, parent);
	if(parent->head_children_dirs == NULL) {
		parent->head_children_dirs = new_dir;
	}
	else {
		if(!strcmp(parent->head_children_dirs->name, name)) {
			printf("Directory already exists\n");
			free(new_dir->name);
			free(new_dir);
			return;
		}
		Dir *temp_dir_list = parent->head_children_dirs;
		while(temp_dir_list->next != NULL) {
			if(!strcmp(temp_dir_list->next->name, name)) {
				printf("Directory already exists\n");
				free(new_dir->name);
				free(new_dir);
				return;
			}
			temp_dir_list = temp_dir_list->next;
		}
		temp_dir_list->next = new_dir;
	}
}

void ls(Dir* parent) {
	Dir *temp_dir = parent->head_children_dirs;
	File *temp_file = parent->head_children_files; 
	while(temp_dir != NULL) {
		printf("%s\n", temp_dir->name);
		temp_dir = temp_dir->next;
	}
	while(temp_file != NULL) {
		printf("%s\n", temp_file->name);
		temp_file = temp_file->next;
	}
}

void rm(Dir* parent, char* name) {
	if(parent->head_children_files == NULL) {
		printf("Could not find the file\n");
		return;
	}
	int deleted = 0;
	if(!strcmp(parent->head_children_files->name, name)) {
		deleted = 1;
		File *new_head = parent->head_children_files->next;
		free_file(parent->head_children_files);
		parent->head_children_files = new_head;
	}
	else {
		File *temp_file_list = parent->head_children_files;
		File *prev = parent->head_children_files;
		while(temp_file_list != NULL) {
			if(!strcmp(temp_file_list->name, name)) {
				deleted = 1;
				prev->next = temp_file_list->next;
				free_file(temp_file_list);
				return;
			}
			prev = temp_file_list;
			temp_file_list = temp_file_list->next;
		}
	}
	if(deleted == 0) {
		printf("Could not find the file\n");
	}
}

void rmdir(Dir* parent, char* name) {
	if(parent->head_children_dirs == NULL) {
		printf("Could not find the dir\n");
		return;
	}
	int deleted = 0;
	if(!strcmp(parent->head_children_dirs->name, name)) {
		deleted = 1;
		Dir *new_head = parent->head_children_dirs->next;
		del_sub_dir(parent->head_children_dirs->head_children_dirs);
		free_listOf_files(parent->head_children_dirs->head_children_files);
		free(parent->head_children_dirs->name);
		free(parent->head_children_dirs);
		parent->head_children_dirs = new_head;
	}
	else {
		Dir *temp_dir_list = parent->head_children_dirs;
		Dir *prev = parent->head_children_dirs;
		while(temp_dir_list != NULL) {
			if(!strcmp(temp_dir_list->name, name)) {
				deleted = 1;
				prev->next = temp_dir_list->next;
				del_sub_dir(temp_dir_list->head_children_dirs);
				free_listOf_files(temp_dir_list->head_children_files);
				free(temp_dir_list->name);
				free(temp_dir_list);
				return;
			}
			prev = temp_dir_list;
			temp_dir_list = temp_dir_list->next;
		}
	}
	if(deleted == 0) {
		printf("Could not find the dir\n");
	}
}

void cd(Dir** target, char *name) {
	if(!strcmp(name, "..")) {
		if(!strcmp((*target)->name, "home")) {
			return;
		}
		*target = (*target)->parent;
		return;

	}
	Dir *temp_dir = (*target)->head_children_dirs;
	while(temp_dir != NULL) {
		if(!strcmp(temp_dir->name, name)) {
			*target = temp_dir;
			return;
		}
		temp_dir = temp_dir->next;
	}
	printf("No directories found!\n");
}

void reverse(Dir **head) {
	Dir *prev = NULL;
	Dir *current = *head;
	Dir *next = NULL;
	while(current != NULL) {
		next = current->next;
		current->next = prev;
		prev = current;
		current = next;
	}
	*head = prev;
}

int get_size(Dir * target) {
	Dir *temp_dir = target;
	int len = 0;
	while(temp_dir != NULL) {
		len += (strlen(temp_dir->name) + 1);
		temp_dir = temp_dir->parent;
	}
	return len;
}

char *pwd (Dir* target) {
	Dir *temp_dir = target;
	int len = get_size(temp_dir);
	char *string = malloc(sizeof(char) * (len + 1));
	char *string_helper = malloc(sizeof(char) * (len + 1));
	string_helper[0] = '\0';
	string[0] = '\0';
	while(temp_dir != NULL) {
		strcpy(string_helper, "/");
		strcat(string_helper, temp_dir->name);
		strcat(string_helper, string);
		strcpy(string, string_helper);
		temp_dir = temp_dir->parent;
	}
	free(string_helper);
	return string;
}

void stop (Dir* target) {
	free_dir(target);
}

void tree (Dir* target, int level) {
	int i;
	Dir *temp_dir = target->head_children_dirs;
	File *temp_file = target->head_children_files; 

	while(temp_dir != NULL) {
		for(i = 0; i < level; i++) {
			printf("    ");
		}
		printf("%s\n", temp_dir->name);
		if(temp_dir->head_children_dirs != NULL) {
			tree(temp_dir, level + 1);
		}
		temp_dir = temp_dir->next;
	}

	while(temp_file != NULL) {
		for(i = 0; i < level; i++) {
			printf("    ");
		}
		printf("%s\n", temp_file->name);
		temp_file = temp_file->next;
	}
}

int check_if_exists(Dir *parent, char *newname) {
	Dir *temp_dir = parent->head_children_dirs;
	File *temp_file = parent->head_children_files;
	int found = 0;
	while(temp_dir != NULL) {
		if(!strcmp(temp_dir->name, newname)) {
			found = 1;
			return found;
		}
		temp_dir = temp_dir->next;
	}
	while(temp_file != NULL) {
		if(!strcmp(temp_file->name, newname)) {
			found = 1;
			return found;
		}
		temp_file = temp_file->next;
	}
	return found;
}


void mv(Dir* parent, char *oldname, char *newname) {
	Dir *temp_dir = parent->head_children_dirs;
	Dir *prev = parent->head_children_dirs;
	Dir *prev_real = parent->head_children_dirs;
	File *temp_file = parent->head_children_files;
	File *prev_file = parent->head_children_files;
	File *prev_real_file = parent->head_children_files;
	int old_found = 0;
	int head_node = 1;
	

	while(temp_dir != NULL) {
		if(!strcmp(temp_dir->name, oldname)) {
			old_found = 1;
			if(check_if_exists(parent, newname) == 0) {
			 	Dir *new_dir = alloc_dir_node(newname, parent);
			 	new_dir->head_children_files = temp_dir->head_children_files;
			 	new_dir->head_children_dirs = temp_dir->head_children_dirs;
			  	while(prev->next != NULL) {
			 		prev = prev->next;
			  	}
				prev->next = new_dir;

				if(head_node == 1) {
					Dir *new_head = parent->head_children_dirs->next;
					free(temp_dir->name);
					free(temp_dir);
					parent->head_children_dirs = new_head;
				}
				else {
					prev_real->next = temp_dir->next;
					free(temp_dir->name);
					free(temp_dir);
				}
				return;
			}
			else {
				printf("File/Director already exists\n");
			 	return;
			}
		}
		head_node = 0;
		prev_real = temp_dir;
		prev = temp_dir;
		temp_dir = temp_dir->next;
	}

	while(temp_file != NULL) {
		if(!strcmp(temp_file->name, oldname)) {
			old_found = 1;
			if(check_if_exists(parent, newname) == 0) {
				File *new_file = alloc_file_node(newname, parent);
				while(prev_file->next != NULL) {
					prev_file = prev_file->next;
				}
				prev_file->next = new_file;
				if(head_node == 1) {
					File *new_head = parent->head_children_files->next;
					free(temp_file->name);
					free(temp_file);
					parent->head_children_files = new_head;
				}
				else {
					prev_real_file->next = temp_file->next;
					free(temp_file->name);
					free(temp_file);
				}
				return;
			}
			else {
				printf("File/Director already exists\n");
				return;
			}
		}
		head_node = 0;
		prev_real_file = temp_file;
		prev_file = temp_file;
		temp_file = temp_file->next;
	}
	if(old_found == 0) {
		printf("File/Director not found\n");
	}
}


int main () {
	char *line = malloc(sizeof(char) * MAX_INPUT_LINE_SIZE);
	char *command;
	char *name;
	char *old_name;
	int nivel = 0;
	Dir *directory = alloc_dir_node("home", NULL);
	do
	{
		/*
		Summary:
			Reads from stdin a string and breaks it down into command and in
			case it needs into a name.
		*/
		fgets(line, MAX_INPUT_LINE_SIZE,stdin);
		command = strtok(line, " ");
		command[strcspn(command, "\n")] = '\0';
		
		if(!strcmp(command, "touch")) {
			name = strtok(NULL, " ");
			name[strcspn(name, "\n")] = '\0';
			touch(directory, name);
		}
		if(!strcmp(command, "mkdir")) {
			name = strtok(NULL, " ");
			name[strcspn(name, "\n")] = '\0';
			mkdir(directory, name);
		}
		if(!strcmp(command, "rm")) {
			name = strtok(NULL, " ");
			name[strcspn(name, "\n")] = '\0';
			rm(directory, name);
		}
		if(!strcmp(command, "rmdir")) {
			name = strtok(NULL, " ");
			name[strcspn(name, "\n")] = '\0';
			rmdir(directory, name);
		}
		if(!strcmp(command, "ls")) {
			ls(directory);
		}
		if(!strcmp(command, "pwd")) {
			char *str = pwd(directory);
			printf("%s\n",str);
			free(str);
		}
		if(!strcmp(command, "cd")) {
			name = strtok(NULL, " ");
			name[strcspn(name, "\n")] = '\0';
			cd(&directory, name);
		}
		if(!strcmp(command, "tree")) {
			tree(directory, 0);
		}

		if(!strcmp(command, "mv")) {
			old_name = strtok(NULL, " ");
			old_name[strcspn(old_name, "\n")] = '\0';
			name = strtok(NULL, " ");
			name[strcspn(name, "\n")] = '\0';
			mv(directory, old_name, name);
		}
	} while (strcmp(command, "stop") != 0);
	

	free(line);
	stop(directory);
	return 0;
}
