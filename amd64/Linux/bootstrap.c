/* Copyright (C) 2016 Jeremiah Orians
 * This file is part of M2-Planet.
 *
 * M2-Planet is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * M2-Planet is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with M2-Planet.  If not, see <http://www.gnu.org/licenses/>.
 */

// CONSTANT stdin 0
// CONSTANT stdout 1
// CONSTANT stderr 2
// CONSTANT EOF 0xFFFFFFFF
// CONSTANT NULL 0
// CONSTANT EXIT_FAILURE 1
// CONSTANT EXIT_SUCCESS 0
// CONSTANT TRUE 1
// CONSTANT FALSE 0


int fgetc(FILE* f)
{
	asm("LOAD_EFFECTIVE_ADDRESS_rdi %8"
	    "LOAD_INTEGER_rdi"
	    "LOAD_IMMEDIATE_rax %0"
	    "PUSH_RAX"
	    "LOAD_EFFECTIVE_ADDRESS_rsi %0"
	    "LOAD_IMMEDIATE_rdx %1"
	    "SYSCALL"
	    "LOAD_IMMEDIATE_rbx %0"
	    "CMP"
	    "POP_RAX"
	    "JUMP_NE %FUNCTION_fgetc_Done"
	    "LOAD_IMMEDIATE_rax %-1"
	    ":FUNCTION_fgetc_Done");
}

void fputc(char s, FILE* f)
{
	asm("LOAD_IMMEDIATE_rax %1"
	    "LOAD_EFFECTIVE_ADDRESS_rdi %8"
	    "LOAD_INTEGER_rdi"
	    "LOAD_EFFECTIVE_ADDRESS_rsi %16"
	    "LOAD_IMMEDIATE_rdx %1"
	    "SYSCALL");
}

void fputs(char* s, FILE* f)
{
	while(0 != s[0])
	{
		fputc(s[0], f);
		s = s + 1;
	}
}

FILE* open(char* name, int flag, int mode)
{
	asm("LOAD_EFFECTIVE_ADDRESS_rdi %24"
	    "LOAD_INTEGER_rdi"
	    "LOAD_EFFECTIVE_ADDRESS_rsi %16"
	    "LOAD_INTEGER_rsi"
	    "LOAD_EFFECTIVE_ADDRESS_rdx %8"
	    "LOAD_INTEGER_rdx"
	    "LOAD_IMMEDIATE_rax %2"
	    "SYSCALL");
}

FILE* fopen(char* filename, char* mode)
{
	FILE* f;
	if('w' == mode[0])
	{ /* 577 is O_WRONLY|O_CREAT|O_TRUNC, 384 is 600 in octal */
		f = open(filename, 577 , 384);
	}
	else
	{ /* Everything else is a read */
		f = open(filename, 0, 0);
	}

	/* Negative numbers are error codes */
	if(0 > f)
	{
		return 0;
	}
	return f;
}

int close(int fd)
{
	asm("LOAD_EFFECTIVE_ADDRESS_rdi %8"
	    "LOAD_INTEGER_rdi"
	    "LOAD_IMMEDIATE_rax %3"
	    "SYSCALL");
}

int fclose(FILE* stream)
{
	int error = close(stream);
	return error;
}

int brk(void *addr)
{
	asm("LOAD_RSP_IMMEDIATE_into_rax %8"
	    "PUSH_RAX"
	    "LOAD_IMMEDIATE_rax %12"
	    "POP_RBX"
	    "COPY_rbx_to_rdi"
	    "SYSCALL");
}

long _malloc_ptr;
long _brk_ptr;

void* malloc(int size)
{
	if(NULL == _brk_ptr)
	{
		_brk_ptr = brk(0);
		_malloc_ptr = _brk_ptr;
	}

	if(_brk_ptr < _malloc_ptr + size)
	{
		_brk_ptr = brk(_malloc_ptr + size);
		if(-1 == _brk_ptr) return 0;
	}

	long old_malloc = _malloc_ptr;
	_malloc_ptr = _malloc_ptr + size;
	return old_malloc;
}

void* memset(void* ptr, int value, int num)
{
	char* s;
	for(s = ptr; 0 < num; num = num - 1)
	{
		s[0] = value;
		s = s + 1;
	}
}

void* calloc(int count, int size)
{
	void* ret = malloc(count * size);
	if(NULL == ret) return NULL;
	memset(ret, 0, (count * size));
	return ret;
}

void free(void* l)
{
	return;
}

void exit(int value)
{
	asm("POP_RBX"
	    "POP_RDI"
	    "LOAD_IMMEDIATE_rax %0x3C"
	    "SYSCALL");
}
