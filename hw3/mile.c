#include "mile.h"

MILE* mopen(const char* name, char* mode) {
	int fd;
	int flags = FLAGS(mode);
	if((fd = open(name, flags)) != -1) {
		MILE* m = (MILE*)malloc(sizeof(MILE*));
		m->_fd = fd;
		if(!flags) {
			m->_mode = 'r';
		} else {
			m->_mode = 'w';
		}
		m->_buf = (void*)malloc(MAX_SIZE);
		m->_pos = 0;
		m->_end = 0;
		return m;
	}	
	PRINTERR("Error: low level open returned -1\n");
	return NULL;
}

void _readIn(MILE* m) {
	if((m->_end = read(m->_fd, m->_buf, MAX_SIZE)) == -1) {
		PRINTERR("Error: low level read returned -1\n");
	}
	m->_pos = 0;
	return;
}

int _copyOut(MILE* m, void* into, int howmuch) {
	int dataleft;
	if((dataleft = (m->_end - m->_pos)) < howmuch) {
		howmuch = dataleft;
	}
	memcpy(into, m->_buf + m->_pos, howmuch);
	m->_pos += howmuch;
	return howmuch;
}

int mread(void* b, int len, MILE* m) {
	if(m->_mode != 'r') {
		PRINTERR("Error: that mile pointer has not been opened for reading\n");
		return -1;
	}
	if(!m->_end) {
		_readIn(m);		
	}
	int numread = 0;
	int dataleft = m->_end - m->_pos;
	if(dataleft && len > MAX_SIZE) {
		numread += _copyOut(m, b, dataleft);
		len -= numread;
		_readIn(m);		
	}
	for(int i = 0; i < (len / MAX_SIZE); i++) {
		numread += _copyOut(m, b + numread, MAX_SIZE);
		_readIn(m);	
	}
	return (numread += _copyOut(m, b + numread, len % MAX_SIZE));
}

void _writeOut(MILE* m) {
	if(write(m->_fd, m->_buf, m->_pos) == -1) {
		PRINTERR("Error: low level write returned -1\n");
	}	
	m->_pos = 0;
	return;
}

int _copyIn(MILE* m, void* from, int howmuch) {
	memcpy(m->_buf + m->_pos, from, howmuch);
	m->_pos += howmuch;
	return howmuch;
}

int mwrite(void* b, int len, MILE* m) {
	if(m->_mode != 'w') {
		PRINTERR("Error: that mile pointer has not been opened for writing or appending\n");
		return -1;
	}
	if((MAX_SIZE - m->_pos) < len) {
		_writeOut(m);
	}
	int numwritten = 0;
	for(int i = 0; i < (len / MAX_SIZE); i++) {
		numwritten += _copyIn(m, b + numwritten, MAX_SIZE);
		_writeOut(m);
	}
	return (numwritten += _copyIn(m, b + numwritten, len % MAX_SIZE));
}

int mclose(MILE* m) {
	if(m->_mode == 'w' && m->_pos) {
		_writeOut(m);
	}
	int ret = close(m->_fd);
	free(m->_buf);
	free(m);
	return ret;
}