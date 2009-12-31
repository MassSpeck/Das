/* das.c
 *
 * A totally overdesigned method of printing character
 * buffers to the screen.
 *
 * In theory, this code can be used to implement an 
 * ascii style game that runs in a command line. 
 * Specifically, this would be the 'rendering engine'.
 * It does not handle user input, sounds (bleeps) or
 * what have you.
 */

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

#define HEIGHT 21
#define WIDTH  78


void* xmalloc(size_t nbytes)
{
	void* mem = malloc(nbytes);
	if (!mem) {
		perror("xmalloc");
		exit(EXIT_FAILURE);
	}
	return mem;
}


typedef void SCType;
#define SCType_Fields size_t ref_cnt; void (*dealloc)(SCType*)

typedef
struct _SCType {
	SCType_Fields;
} _SCType;


SCType* SCType_init(SCType* type, void (*dealloc)(SCType*))
{
	_SCType* self = (_SCType*)type;
	if (self) {
		self->ref_cnt = 1;
		self->dealloc = dealloc;
	}
	return self;
}


void SCType_retain(SCType* type)
{
	_SCType* self = (_SCType*)type;
	self->ref_cnt++;
}


void SCType_release(SCType* type)
{
	_SCType* self = (_SCType*)type;
	if (!self) return;
	if (self->ref_cnt > 0)
		self->ref_cnt--;
	if (self->ref_cnt == 0)
		self->dealloc(self);
}


typedef
struct _ScreenBuffer {
	SCType_Fields;
	char buffer[HEIGHT][WIDTH];
	int  height;
	int  width;
} ScreenBuffer;


ScreenBuffer* ScreenBuffer_alloc()
{
	ScreenBuffer* sb = 
		(ScreenBuffer*)xmalloc(sizeof(ScreenBuffer));
	if (sb != NULL)
		memset(sb, 0, sizeof(*sb));

	return sb;
}


void ScreenBuffer_dealloc(SCType* self)
{
	free(self);
}


ScreenBuffer* ScreenBuffer_init(ScreenBuffer* self)
{
	int i, j;
	const char vert_bar  = '|';
	const char horiz_bar = '_';
	const char blank     = ' ';

	if (self == NULL) return self;

	SCType_init(self, ScreenBuffer_dealloc);

	self->height = HEIGHT;
	self->width  = WIDTH;

	for (i = 0; i < self->height; i++) {
		for (j = 0; j < self->width; j++) {
			if (i == 0 || i == self->height - 1)
				self->buffer[i][j] = horiz_bar;
			else if (j == 0 || j == self->width - 1)
				self->buffer[i][j] = vert_bar;
			else
				self->buffer[i][j] = blank;
		}
	}

	return self;
}


void ScreenBuffer_write(ScreenBuffer* self, FILE* out)
{
	int i, j;
	assert(self != NULL);

	for (i = 0; i < self->height; i++) {
		for (j = 0; j < self->width; j++)
			fputc(self->buffer[i][j], out);
		fputc('\n', out);
	}
}


void ScreenBuffer_set(ScreenBuffer* self, int x, int y, char c)
{
	if (0 <= x && x < self->width
		&& 0 <= y && y < self->height)
		self->buffer[y][x] = c;
}


int main(int argc, char* argv[])
{
	ScreenBuffer* sb = ScreenBuffer_init(ScreenBuffer_alloc());
	assert(sb != NULL);

	/* Draw a simple and cheezy cross-hair */
	ScreenBuffer_set(sb, WIDTH/2-2, HEIGHT/2, '-');
	ScreenBuffer_set(sb, WIDTH/2-1, HEIGHT/2, '-');
	ScreenBuffer_set(sb, WIDTH/2, HEIGHT/2, 'x');
	ScreenBuffer_set(sb, WIDTH/2+1, HEIGHT/2, '-');
	ScreenBuffer_set(sb, WIDTH/2+2, HEIGHT/2, '-');
	ScreenBuffer_set(sb, WIDTH/2, HEIGHT/2-1, '|');
	ScreenBuffer_set(sb, WIDTH/2, HEIGHT/2+1, '|');
	
	printf("Welcome to das...\n");
	ScreenBuffer_write(sb, stdout);
	printf("All done!\n");

	SCType_release(sb);

	return 0;
}
