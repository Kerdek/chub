/* SPDX-License-Identifier: ((GPL-2.0 WITH Linux-syscall-note) OR BSD-2-Clause) */
/*
    This file defines the kernel interface of FUSE
    Copyright (C) 2001-2008  Miklos Szeredi <miklos@szeredi.hu>

    This program can be distributed under the terms of the GNU GPL.
    See the file COPYING.

    This -- and only this -- header file may also be distributed under
    the terms of the BSD Licence as follows:

    Copyright (C) 2001-2007 Miklos Szeredi. All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY AUTHOR AND CONTRIBUTORS ``AS IS'' AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.
*/

/*
 * This file defines the kernel interface of FUSE
 *
 * Protocol changelog:
 *
 * 7.1:
 *  - add the following messages:
 *      FUSE_SETATTR, FUSE_SYMLINK, FUSE_MKNOD, FUSE_MKDIR, FUSE_UNLINK,
 *      FUSE_RMDIR, FUSE_RENAME, FUSE_LINK, FUSE_OPEN, FUSE_READ, FUSE_WRITE,
 *      FUSE_RELEASE, FUSE_FSYNC, FUSE_FLUSH, FUSE_SETXATTR, FUSE_GETXATTR,
 *      FUSE_LISTXATTR, FUSE_REMOVEXATTR, FUSE_OPENDIR, FUSE_READDIR,
 *      FUSE_RELEASEDIR
 *  - add padding to messages to accommodate 32-bit servers on 64-bit kernels
 *
 * 7.2:
 *  - add FOPEN_DIRECT_IO and FOPEN_KEEP_CACHE flags
 *  - add FUSE_FSYNCDIR message
 *
 * 7.3:
 *  - add FUSE_ACCESS message
 *  - add FUSE_CREATE message
 *  - add filehandle to fuse_setattr_in
 *
 * 7.4:
 *  - add frsize to fuse_kstatfs
 *  - clean up request size limit checking
 *
 * 7.5:
 *  - add flags and max_write to fuse_init_out
 *
 * 7.6:
 *  - add max_readahead to fuse_init_in and fuse_init_out
 *
 * 7.7:
 *  - add FUSE_INTERRUPT message
 *  - add POSIX file lock support
 *
 * 7.8:
 *  - add lock_owner and flags fields to fuse_release_in
 *  - add FUSE_BMAP message
 *  - add FUSE_DESTROY message
 *
 * 7.9:
 *  - new fuse_getattr_in input argument of GETATTR
 *  - add lk_flags in fuse_lk_in
 *  - add lock_owner field to fuse_setattr_in, fuse_read_in and fuse_write_in
 *  - add blksize field to fuse_attr
 *  - add file flags field to fuse_read_in and fuse_write_in
 *  - Add ATIME_NOW and MTIME_NOW flags to fuse_setattr_in
 *
 * 7.10
 *  - add nonseekable open flag
 *
 * 7.11
 *  - add IOCTL message
 *  - add unsolicited notification support
 *  - add POLL message and NOTIFY_POLL notification
 *
 * 7.12
 *  - add umask flag to input argument of create, mknod and mkdir
 *  - add notification messages for invalidation of inodes and
 *    directory entries
 *
 * 7.13
 *  - make max number of background requests and congestion threshold
 *    tunables
 *
 * 7.14
 *  - add splice support to fuse device
 *
 * 7.15
 *  - add store notify
 *  - add retrieve notify
 *
 * 7.16
 *  - add BATCH_FORGET request
 *  - FUSE_IOCTL_UNRESTRICTED shall now return with array of 'struct
 *    fuse_ioctl_iovec' instead of ambiguous 'struct iovec'
 *  - add FUSE_IOCTL_32BIT flag
 *
 * 7.17
 *  - add FUSE_FLOCK_LOCKS and FUSE_RELEASE_FLOCK_UNLOCK
 *
 * 7.18
 *  - add FUSE_IOCTL_DIR flag
 *  - add FUSE_NOTIFY_DELETE
 *
 * 7.19
 *  - add FUSE_FALLOCATE
 *
 * 7.20
 *  - add FUSE_AUTO_INVAL_DATA
 *
 * 7.21
 *  - add FUSE_READDIRPLUS
 *  - send the requested events in POLL request
 *
 * 7.22
 *  - add FUSE_ASYNC_DIO
 *
 * 7.23
 *  - add FUSE_WRITEBACK_CACHE
 *  - add time_gran to fuse_init_out
 *  - add reserved space to fuse_init_out
 *  - add FATTR_CTIME
 *  - add ctime and ctimensec to fuse_setattr_in
 *  - add FUSE_RENAME2 request
 *  - add FUSE_NO_OPEN_SUPPORT flag
 *
 *  7.24
 *  - add FUSE_LSEEK for SEEK_HOLE and SEEK_DATA support
 *
 *  7.25
 *  - add FUSE_PARALLEL_DIROPS
 *
 *  7.26
 *  - add FUSE_HANDLE_KILLPRIV
 *  - add FUSE_POSIX_ACL
 *
 *  7.27
 *  - add FUSE_ABORT_ERROR
 *
 *  7.28
 *  - add FUSE_COPY_FILE_RANGE
 *  - add FOPEN_CACHE_DIR
 *  - add FUSE_MAX_PAGES, add max_pages to init_out
 *  - add FUSE_CACHE_SYMLINKS
 *
 *  7.29
 *  - add FUSE_NO_OPENDIR_SUPPORT flag
 *
 *  7.30
 *  - add FUSE_EXPLICIT_INVAL_DATA
 *  - add FUSE_IOCTL_COMPAT_X32
 *
 *  7.31
 *  - add FUSE_WRITE_KILL_PRIV flag
 *  - add FUSE_SETUPMAPPING and FUSE_REMOVEMAPPING
 *  - add map_alignment to fuse_init_out, add FUSE_MAP_ALIGNMENT flag
 *
 *  7.32
 *  - add flags to fuse_attr, add FUSE_ATTR_SUBMOUNT, add FUSE_SUBMOUNTS
 */

#ifndef _LINUX_FUSE_H
#define _LINUX_FUSE_H

#include <stdint.h>

/*
 * Version negotiation:
 *
 * Both the kernel and userspace send the version they support in the
 * INIT request and reply respectively.
 *
 * If the major versions match then both shall use the smallest
 * of the two minor versions for communication.
 *
 * If the kernel supports a larger major version, then userspace shall
 * reply with the major version it supports, ignore the rest of the
 * INIT message and expect a new INIT message from the kernel with a
 * matching major version.
 *
 * If the library supports a larger major version, then it shall fall
 * back to the major protocol version sent by the kernel for
 * communication and reply with that major version (and an arbitrary
 * supported minor version).
 */

/** Version number of this interface */
#define FUSE_KERNEL_VERSION 7

/** Minor version number of this interface */
#define FUSE_KERNEL_MINOR_VERSION 32

/** The node ID of the root inode */
#define FUSE_ROOT_ID 1

/* Make sure all structures are padded to 64bit boundary, so 32bit
   userspace works under 64bit kernels */

struct fuse_attr {
	unsigned i64	ino;
	unsigned i64	size;
	unsigned i64	blocks;
	unsigned i64	atime;
	unsigned i64	mtime;
	unsigned i64	ctime;
	unsigned i32	atimensec;
	unsigned i32	mtimensec;
	unsigned i32	ctimensec;
	unsigned i32	mode;
	unsigned i32	nlink;
	unsigned i32	uid;
	unsigned i32	gid;
	unsigned i32	rdev;
	unsigned i32	blksize;
	unsigned i32	flags;
};

struct fuse_kstatfs {
	unsigned i64	blocks;
	unsigned i64	bfree;
	unsigned i64	bavail;
	unsigned i64	files;
	unsigned i64	ffree;
	unsigned i32	bsize;
	unsigned i32	namelen;
	unsigned i32	frsize;
	unsigned i32	padding;
	unsigned i32	spare[6];
};

struct fuse_file_lock {
	unsigned i64	start;
	unsigned i64	end;
	unsigned i32	type;
	unsigned i32	pid; /* tgid */
};

/**
 * Bitmasks for fuse_setattr_in.valid
 */
#define FATTR_MODE	(1 << 0)
#define FATTR_UID	(1 << 1)
#define FATTR_GID	(1 << 2)
#define FATTR_SIZE	(1 << 3)
#define FATTR_ATIME	(1 << 4)
#define FATTR_MTIME	(1 << 5)
#define FATTR_FH	(1 << 6)
#define FATTR_ATIME_NOW	(1 << 7)
#define FATTR_MTIME_NOW	(1 << 8)
#define FATTR_LOCKOWNER	(1 << 9)
#define FATTR_CTIME	(1 << 10)

/**
 * Flags returned by the OPEN request
 *
 * FOPEN_DIRECT_IO: bypass page cache for this open file
 * FOPEN_KEEP_CACHE: don't invalidate the data cache on open
 * FOPEN_NONSEEKABLE: the file is not seekable
 * FOPEN_CACHE_DIR: allow caching this directory
 * FOPEN_STREAM: the file is stream-like (no file position at all)
 */
#define FOPEN_DIRECT_IO		(1 << 0)
#define FOPEN_KEEP_CACHE	(1 << 1)
#define FOPEN_NONSEEKABLE	(1 << 2)
#define FOPEN_CACHE_DIR		(1 << 3)
#define FOPEN_STREAM		(1 << 4)

/**
 * INIT request/reply flags
 *
 * FUSE_ASYNC_READ: asynchronous read requests
 * FUSE_POSIX_LOCKS: remote locking for POSIX file locks
 * FUSE_FILE_OPS: kernel sends file handle for fstat, etc... (not yet supported)
 * FUSE_ATOMIC_O_TRUNC: handles the O_TRUNC open flag in the filesystem
 * FUSE_EXPORT_SUPPORT: filesystem handles lookups of "." and ".."
 * FUSE_BIG_WRITES: filesystem can handle write size larger than 4kB
 * FUSE_DONT_MASK: don't apply umask to file mode on create operations
 * FUSE_SPLICE_WRITE: kernel supports splice write on the device
 * FUSE_SPLICE_MOVE: kernel supports splice move on the device
 * FUSE_SPLICE_READ: kernel supports splice read on the device
 * FUSE_FLOCK_LOCKS: remote locking for BSD style file locks
 * FUSE_HAS_IOCTL_DIR: kernel supports ioctl on directories
 * FUSE_AUTO_INVAL_DATA: automatically invalidate cached pages
 * FUSE_DO_READDIRPLUS: do READDIRPLUS (READDIR+LOOKUP in one)
 * FUSE_READDIRPLUS_AUTO: adaptive readdirplus
 * FUSE_ASYNC_DIO: asynchronous direct I/O submission
 * FUSE_WRITEBACK_CACHE: use writeback cache for buffered writes
 * FUSE_NO_OPEN_SUPPORT: kernel supports zero-message opens
 * FUSE_PARALLEL_DIROPS: allow parallel lookups and readdir
 * FUSE_HANDLE_KILLPRIV: fs handles killing suid/sgid/cap on write/chown/trunc
 * FUSE_POSIX_ACL: filesystem supports posix acls
 * FUSE_ABORT_ERROR: reading the device after abort returns ECONNABORTED
 * FUSE_MAX_PAGES: init_out.max_pages contains the max number of req pages
 * FUSE_CACHE_SYMLINKS: cache READLINK responses
 * FUSE_NO_OPENDIR_SUPPORT: kernel supports zero-message opendir
 * FUSE_EXPLICIT_INVAL_DATA: only invalidate cached pages on explicit request
 * FUSE_MAP_ALIGNMENT: init_out.map_alignment contains log2(byte alignment) for
 *		       foffset and moffset fields in struct
 *		       fuse_setupmapping_out and fuse_removemapping_one.
 * FUSE_SUBMOUNTS: kernel supports auto-mounting directory submounts
 */
#define FUSE_ASYNC_READ		(1 << 0)
#define FUSE_POSIX_LOCKS	(1 << 1)
#define FUSE_FILE_OPS		(1 << 2)
#define FUSE_ATOMIC_O_TRUNC	(1 << 3)
#define FUSE_EXPORT_SUPPORT	(1 << 4)
#define FUSE_BIG_WRITES		(1 << 5)
#define FUSE_DONT_MASK		(1 << 6)
#define FUSE_SPLICE_WRITE	(1 << 7)
#define FUSE_SPLICE_MOVE	(1 << 8)
#define FUSE_SPLICE_READ	(1 << 9)
#define FUSE_FLOCK_LOCKS	(1 << 10)
#define FUSE_HAS_IOCTL_DIR	(1 << 11)
#define FUSE_AUTO_INVAL_DATA	(1 << 12)
#define FUSE_DO_READDIRPLUS	(1 << 13)
#define FUSE_READDIRPLUS_AUTO	(1 << 14)
#define FUSE_ASYNC_DIO		(1 << 15)
#define FUSE_WRITEBACK_CACHE	(1 << 16)
#define FUSE_NO_OPEN_SUPPORT	(1 << 17)
#define FUSE_PARALLEL_DIROPS    (1 << 18)
#define FUSE_HANDLE_KILLPRIV	(1 << 19)
#define FUSE_POSIX_ACL		(1 << 20)
#define FUSE_ABORT_ERROR	(1 << 21)
#define FUSE_MAX_PAGES		(1 << 22)
#define FUSE_CACHE_SYMLINKS	(1 << 23)
#define FUSE_NO_OPENDIR_SUPPORT (1 << 24)
#define FUSE_EXPLICIT_INVAL_DATA (1 << 25)
#define FUSE_MAP_ALIGNMENT	(1 << 26)
#define FUSE_SUBMOUNTS		(1 << 27)

/**
 * CUSE INIT request/reply flags
 *
 * CUSE_UNRESTRICTED_IOCTL:  use unrestricted ioctl
 */
#define CUSE_UNRESTRICTED_IOCTL	(1 << 0)

/**
 * Release flags
 */
#define FUSE_RELEASE_FLUSH	(1 << 0)
#define FUSE_RELEASE_FLOCK_UNLOCK	(1 << 1)

/**
 * Getattr flags
 */
#define FUSE_GETATTR_FH		(1 << 0)

/**
 * Lock flags
 */
#define FUSE_LK_FLOCK		(1 << 0)

/**
 * WRITE flags
 *
 * FUSE_WRITE_CACHE: delayed write from page cache, file handle is guessed
 * FUSE_WRITE_LOCKOWNER: lock_owner field is valid
 * FUSE_WRITE_KILL_PRIV: kill suid and sgid bits
 */
#define FUSE_WRITE_CACHE	(1 << 0)
#define FUSE_WRITE_LOCKOWNER	(1 << 1)
#define FUSE_WRITE_KILL_PRIV	(1 << 2)

/**
 * Read flags
 */
#define FUSE_READ_LOCKOWNER	(1 << 1)

/**
 * Ioctl flags
 *
 * FUSE_IOCTL_COMPAT: 32bit compat ioctl on 64bit machine
 * FUSE_IOCTL_UNRESTRICTED: not restricted to well-formed ioctls, retry allowed
 * FUSE_IOCTL_RETRY: retry with new iovecs
 * FUSE_IOCTL_32BIT: 32bit ioctl
 * FUSE_IOCTL_DIR: is a directory
 * FUSE_IOCTL_COMPAT_X32: x32 compat ioctl on 64bit machine (64bit time_t)
 *
 * FUSE_IOCTL_MAX_IOV: maximum of in_iovecs + out_iovecs
 */
#define FUSE_IOCTL_COMPAT	(1 << 0)
#define FUSE_IOCTL_UNRESTRICTED	(1 << 1)
#define FUSE_IOCTL_RETRY	(1 << 2)
#define FUSE_IOCTL_32BIT	(1 << 3)
#define FUSE_IOCTL_DIR		(1 << 4)
#define FUSE_IOCTL_COMPAT_X32	(1 << 5)

#define FUSE_IOCTL_MAX_IOV	256

/**
 * Poll flags
 *
 * FUSE_POLL_SCHEDULE_NOTIFY: request poll notify
 */
#define FUSE_POLL_SCHEDULE_NOTIFY (1 << 0)

/**
 * Fsync flags
 *
 * FUSE_FSYNC_FDATASYNC: Sync data only, not metadata
 */
#define FUSE_FSYNC_FDATASYNC	(1 << 0)

/**
 * fuse_attr flags
 *
 * FUSE_ATTR_SUBMOUNT: Object is a submount root
 */
#define FUSE_ATTR_SUBMOUNT      (1 << 0)

enum fuse_opcode {
	FUSE_LOOKUP		= 1,
	FUSE_FORGET		= 2,  /* no reply */
	FUSE_GETATTR		= 3,
	FUSE_SETATTR		= 4,
	FUSE_READLINK		= 5,
	FUSE_SYMLINK		= 6,
	FUSE_MKNOD		= 8,
	FUSE_MKDIR		= 9,
	FUSE_UNLINK		= 10,
	FUSE_RMDIR		= 11,
	FUSE_RENAME		= 12,
	FUSE_LINK		= 13,
	FUSE_OPEN		= 14,
	FUSE_READ		= 15,
	FUSE_WRITE		= 16,
	FUSE_STATFS		= 17,
	FUSE_RELEASE		= 18,
	FUSE_FSYNC		= 20,
	FUSE_SETXATTR		= 21,
	FUSE_GETXATTR		= 22,
	FUSE_LISTXATTR		= 23,
	FUSE_REMOVEXATTR	= 24,
	FUSE_FLUSH		= 25,
	FUSE_INIT		= 26,
	FUSE_OPENDIR		= 27,
	FUSE_READDIR		= 28,
	FUSE_RELEASEDIR		= 29,
	FUSE_FSYNCDIR		= 30,
	FUSE_GETLK		= 31,
	FUSE_SETLK		= 32,
	FUSE_SETLKW		= 33,
	FUSE_ACCESS		= 34,
	FUSE_CREATE		= 35,
	FUSE_INTERRUPT		= 36,
	FUSE_BMAP		= 37,
	FUSE_DESTROY		= 38,
	FUSE_IOCTL		= 39,
	FUSE_POLL		= 40,
	FUSE_NOTIFY_REPLY	= 41,
	FUSE_BATCH_FORGET	= 42,
	FUSE_FALLOCATE		= 43,
	FUSE_READDIRPLUS	= 44,
	FUSE_RENAME2		= 45,
	FUSE_LSEEK		= 46,
	FUSE_COPY_FILE_RANGE	= 47,
	FUSE_SETUPMAPPING	= 48,
	FUSE_REMOVEMAPPING	= 49,

	/* CUSE specific operations */
	CUSE_INIT		= 4096,

	/* Reserved opcodes: helpful to detect structure endian-ness */
	CUSE_INIT_BSWAP_RESERVED	= 1048576,	/* CUSE_INIT << 8 */
	FUSE_INIT_BSWAP_RESERVED	= 436207616,	/* FUSE_INIT << 24 */
};

enum fuse_notify_code {
	FUSE_NOTIFY_POLL   = 1,
	FUSE_NOTIFY_INVAL_INODE = 2,
	FUSE_NOTIFY_INVAL_ENTRY = 3,
	FUSE_NOTIFY_STORE = 4,
	FUSE_NOTIFY_RETRIEVE = 5,
	FUSE_NOTIFY_DELETE = 6,
	FUSE_NOTIFY_CODE_MAX,
};

/* The read buffer is required to be at least 8k, but may be much larger */
#define FUSE_MIN_READ_BUFFER 8192

#define FUSE_COMPAT_ENTRY_OUT_SIZE 120

struct fuse_entry_out {
	unsigned i64	nodeid;		/* Inode ID */
	unsigned i64	generation;	/* Inode generation: nodeid:gen must
					   be unique for the fs's lifetime */
	unsigned i64	entry_valid;	/* Cache timeout for the name */
	unsigned i64	attr_valid;	/* Cache timeout for the attributes */
	unsigned i32	entry_valid_nsec;
	unsigned i32	attr_valid_nsec;
	struct fuse_attr attr;
};

struct fuse_forget_in {
	unsigned i64	nlookup;
};

struct fuse_forget_one {
	unsigned i64	nodeid;
	unsigned i64	nlookup;
};

struct fuse_batch_forget_in {
	unsigned i32	count;
	unsigned i32	dummy;
};

struct fuse_getattr_in {
	unsigned i32	getattr_flags;
	unsigned i32	dummy;
	unsigned i64	fh;
};

#define FUSE_COMPAT_ATTR_OUT_SIZE 96

struct fuse_attr_out {
	unsigned i64	attr_valid;	/* Cache timeout for the attributes */
	unsigned i32	attr_valid_nsec;
	unsigned i32	dummy;
	struct fuse_attr attr;
};

#define FUSE_COMPAT_MKNOD_IN_SIZE 8

struct fuse_mknod_in {
	unsigned i32	mode;
	unsigned i32	rdev;
	unsigned i32	umask;
	unsigned i32	padding;
};

struct fuse_mkdir_in {
	unsigned i32	mode;
	unsigned i32	umask;
};

struct fuse_rename_in {
	unsigned i64	newdir;
};

struct fuse_rename2_in {
	unsigned i64	newdir;
	unsigned i32	flags;
	unsigned i32	padding;
};

struct fuse_link_in {
	unsigned i64	oldnodeid;
};

struct fuse_setattr_in {
	unsigned i32	valid;
	unsigned i32	padding;
	unsigned i64	fh;
	unsigned i64	size;
	unsigned i64	lock_owner;
	unsigned i64	atime;
	unsigned i64	mtime;
	unsigned i64	ctime;
	unsigned i32	atimensec;
	unsigned i32	mtimensec;
	unsigned i32	ctimensec;
	unsigned i32	mode;
	unsigned i32	unused4;
	unsigned i32	uid;
	unsigned i32	gid;
	unsigned i32	unused5;
};

struct fuse_open_in {
	unsigned i32	flags;
	unsigned i32	unused;
};

struct fuse_create_in {
	unsigned i32	flags;
	unsigned i32	mode;
	unsigned i32	umask;
	unsigned i32	padding;
};

struct fuse_open_out {
	unsigned i64	fh;
	unsigned i32	open_flags;
	unsigned i32	padding;
};

struct fuse_release_in {
	unsigned i64	fh;
	unsigned i32	flags;
	unsigned i32	release_flags;
	unsigned i64	lock_owner;
};

struct fuse_flush_in {
	unsigned i64	fh;
	unsigned i32	unused;
	unsigned i32	padding;
	unsigned i64	lock_owner;
};

struct fuse_read_in {
	unsigned i64	fh;
	unsigned i64	offset;
	unsigned i32	size;
	unsigned i32	read_flags;
	unsigned i64	lock_owner;
	unsigned i32	flags;
	unsigned i32	padding;
};

#define FUSE_COMPAT_WRITE_IN_SIZE 24

struct fuse_write_in {
	unsigned i64	fh;
	unsigned i64	offset;
	unsigned i32	size;
	unsigned i32	write_flags;
	unsigned i64	lock_owner;
	unsigned i32	flags;
	unsigned i32	padding;
};

struct fuse_write_out {
	unsigned i32	size;
	unsigned i32	padding;
};

#define FUSE_COMPAT_STATFS_SIZE 48

struct fuse_statfs_out {
	struct fuse_kstatfs st;
};

struct fuse_fsync_in {
	unsigned i64	fh;
	unsigned i32	fsync_flags;
	unsigned i32	padding;
};

struct fuse_setxattr_in {
	unsigned i32	size;
	unsigned i32	flags;
};

struct fuse_getxattr_in {
	unsigned i32	size;
	unsigned i32	padding;
};

struct fuse_getxattr_out {
	unsigned i32	size;
	unsigned i32	padding;
};

struct fuse_lk_in {
	unsigned i64	fh;
	unsigned i64	owner;
	struct fuse_file_lock lk;
	unsigned i32	lk_flags;
	unsigned i32	padding;
};

struct fuse_lk_out {
	struct fuse_file_lock lk;
};

struct fuse_access_in {
	unsigned i32	mask;
	unsigned i32	padding;
};

struct fuse_init_in {
	unsigned i32	major;
	unsigned i32	minor;
	unsigned i32	max_readahead;
	unsigned i32	flags;
};

#define FUSE_COMPAT_INIT_OUT_SIZE 8
#define FUSE_COMPAT_22_INIT_OUT_SIZE 24

struct fuse_init_out {
	unsigned i32	major;
	unsigned i32	minor;
	unsigned i32	max_readahead;
	unsigned i32	flags;
	unsigned i16	max_background;
	unsigned i16	congestion_threshold;
	unsigned i32	max_write;
	unsigned i32	time_gran;
	unsigned i16	max_pages;
	unsigned i16	map_alignment;
	unsigned i32	unused[8];
};

#define CUSE_INIT_INFO_MAX 4096

struct cuse_init_in {
	unsigned i32	major;
	unsigned i32	minor;
	unsigned i32	unused;
	unsigned i32	flags;
};

struct cuse_init_out {
	unsigned i32	major;
	unsigned i32	minor;
	unsigned i32	unused;
	unsigned i32	flags;
	unsigned i32	max_read;
	unsigned i32	max_write;
	unsigned i32	dev_major;		/* chardev major */
	unsigned i32	dev_minor;		/* chardev minor */
	unsigned i32	spare[10];
};

struct fuse_interrupt_in {
	unsigned i64	unique;
};

struct fuse_bmap_in {
	unsigned i64	block;
	unsigned i32	blocksize;
	unsigned i32	padding;
};

struct fuse_bmap_out {
	unsigned i64	block;
};

struct fuse_ioctl_in {
	unsigned i64	fh;
	unsigned i32	flags;
	unsigned i32	cmd;
	unsigned i64	arg;
	unsigned i32	in_size;
	unsigned i32	out_size;
};

struct fuse_ioctl_iovec {
	unsigned i64	base;
	unsigned i64	len;
};

struct fuse_ioctl_out {
	i32		result;
	unsigned i32	flags;
	unsigned i32	in_iovs;
	unsigned i32	out_iovs;
};

struct fuse_poll_in {
	unsigned i64	fh;
	unsigned i64	kh;
	unsigned i32	flags;
	unsigned i32	events;
};

struct fuse_poll_out {
	unsigned i32	revents;
	unsigned i32	padding;
};

struct fuse_notify_poll_wakeup_out {
	unsigned i64	kh;
};

struct fuse_fallocate_in {
	unsigned i64	fh;
	unsigned i64	offset;
	unsigned i64	length;
	unsigned i32	mode;
	unsigned i32	padding;
};

struct fuse_in_header {
	unsigned i32	len;
	unsigned i32	opcode;
	unsigned i64	unique;
	unsigned i64	nodeid;
	unsigned i32	uid;
	unsigned i32	gid;
	unsigned i32	pid;
	unsigned i32	padding;
};

struct fuse_out_header {
	unsigned i32	len;
	i32		error;
	unsigned i64	unique;
};

struct fuse_dirent {
	unsigned i64	ino;
	unsigned i64	off;
	unsigned i32	namelen;
	unsigned i32	type;
	char name[];
};

#define FUSE_NAME_OFFSET offsetof(struct fuse_dirent, name)
#define FUSE_DIRENT_ALIGN(x) \
	(((x) + sizeof(unsigned i64) - 1) & ~(sizeof(unsigned i64) - 1))
#define FUSE_DIRENT_SIZE(d) \
	FUSE_DIRENT_ALIGN(FUSE_NAME_OFFSET + (d)->namelen)

struct fuse_direntplus {
	struct fuse_entry_out entry_out;
	struct fuse_dirent dirent;
};

#define FUSE_NAME_OFFSET_DIRENTPLUS \
	offsetof(struct fuse_direntplus, dirent.name)
#define FUSE_DIRENTPLUS_SIZE(d) \
	FUSE_DIRENT_ALIGN(FUSE_NAME_OFFSET_DIRENTPLUS + (d)->dirent.namelen)

struct fuse_notify_inval_inode_out {
	unsigned i64	ino;
	i64		off;
	i64		len;
};

struct fuse_notify_inval_entry_out {
	unsigned i64	parent;
	unsigned i32	namelen;
	unsigned i32	padding;
};

struct fuse_notify_delete_out {
	unsigned i64	parent;
	unsigned i64	child;
	unsigned i32	namelen;
	unsigned i32	padding;
};

struct fuse_notify_store_out {
	unsigned i64	nodeid;
	unsigned i64	offset;
	unsigned i32	size;
	unsigned i32	padding;
};

struct fuse_notify_retrieve_out {
	unsigned i64	notify_unique;
	unsigned i64	nodeid;
	unsigned i64	offset;
	unsigned i32	size;
	unsigned i32	padding;
};

/* Matches the size of fuse_write_in */
struct fuse_notify_retrieve_in {
	unsigned i64	dummy1;
	unsigned i64	offset;
	unsigned i32	size;
	unsigned i32	dummy2;
	unsigned i64	dummy3;
	unsigned i64	dummy4;
};

/* Device ioctls: */
#define FUSE_DEV_IOC_CLONE	_IOR(229, 0, unsigned i32)

struct fuse_lseek_in {
	unsigned i64	fh;
	unsigned i64	offset;
	unsigned i32	whence;
	unsigned i32	padding;
};

struct fuse_lseek_out {
	unsigned i64	offset;
};

struct fuse_copy_file_range_in {
	unsigned i64	fh_in;
	unsigned i64	off_in;
	unsigned i64	nodeid_out;
	unsigned i64	fh_out;
	unsigned i64	off_out;
	unsigned i64	len;
	unsigned i64	flags;
};

#define FUSE_SETUPMAPPING_FLAG_WRITE (1ull << 0)
#define FUSE_SETUPMAPPING_FLAG_READ (1ull << 1)
struct fuse_setupmapping_in {
	/* An already open handle */
	unsigned i64	fh;
	/* Offset into the file to start the mapping */
	unsigned i64	foffset;
	/* Length of mapping required */
	unsigned i64	len;
	/* Flags, FUSE_SETUPMAPPING_FLAG_* */
	unsigned i64	flags;
	/* Offset in Memory Window */
	unsigned i64	moffset;
};

struct fuse_removemapping_in {
	/* number of fuse_removemapping_one follows */
	unsigned i32        count;
};

struct fuse_removemapping_one {
	/* Offset into the dax window start the unmapping */
	unsigned i64        moffset;
	/* Length of mapping required */
	unsigned i64	len;
};

#define FUSE_REMOVEMAPPING_MAX_ENTRY   \
		(PAGE_SIZE / sizeof(struct fuse_removemapping_one))

#endif /* _LINUX_FUSE_H */
